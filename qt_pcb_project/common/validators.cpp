
#include <string_utils.h>
#include <confirm.h>
#include <validators.h>
#include <template_fieldnames.h>

#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QValidator>
#include <QKeyEvent>
#include <QComboBox>
#include <QApplication>
#include <refdes_utils.h>


FOOTPRINT_NAME_VALIDATOR::FOOTPRINT_NAME_VALIDATOR( QString* aValue ) :
    QValidator()
{
    // This list of characters follows the string from footprint.cpp which, in turn mimics the
    // strings from lib_id.cpp
    m_illegalChars = "%$<>\t\n\r\"\\/:";
}

QValidator::State FOOTPRINT_NAME_VALIDATOR::validate( QString& aInput, int& aPos ) const
{
    for( int i = 0; i < aInput.length(); ++i )
    {
        if( m_illegalChars.contains( aInput[i] ) )
            return QValidator::Invalid;
    }
    return QValidator::Acceptable;
}


FILE_NAME_WITH_PATH_CHAR_VALIDATOR::FILE_NAME_WITH_PATH_CHAR_VALIDATOR( QString* aValue ) :
    QValidator()
{
    // The Windows (DOS) file system forbidden characters already include the forbidden
    // file name characters for both Posix and OSX systems.  The characters *?|"<> are
    // illegal and filtered by the validator, but /\: are valid (\ and : only on Windows.
    QString illegalChars = "*?|\"<>";
    
    for( int i = 0; i < illegalChars.size(); i++ )
    {
        QChar ch = illegalChars[i];
        if( ch == '/' )
            continue;

#if defined (__WINDOWS__)
        if( ch == '\\' || ch == ':' )
            continue;
#endif
        m_illegalChars += ch;
    }
}

QValidator::State FILE_NAME_WITH_PATH_CHAR_VALIDATOR::validate( QString& aInput, int& aPos ) const
{
    if( aInput.isEmpty() )
        return QValidator::Invalid;
        
    for( int i = 0; i < aInput.length(); ++i )
    {
        if( m_illegalChars.contains( aInput[i] ) )
            return QValidator::Invalid;
    }
    return QValidator::Acceptable;
}


ENV_VAR_NAME_VALIDATOR::ENV_VAR_NAME_VALIDATOR( QString* aValue ) :
    QValidator()
{
}


ENV_VAR_NAME_VALIDATOR::ENV_VAR_NAME_VALIDATOR( const ENV_VAR_NAME_VALIDATOR& val )
    : QValidator()
{
}


ENV_VAR_NAME_VALIDATOR::~ENV_VAR_NAME_VALIDATOR()
{
}


QValidator::State ENV_VAR_NAME_VALIDATOR::validate( QString& aInput, int& aPos ) const
{
    if( aInput.isEmpty() )
        return QValidator::Acceptable;

    // Convert to uppercase
    aInput = aInput.toUpper();

    for( int i = 0; i < aInput.length(); ++i )
    {
        QChar c = aInput[i];
        
        if( c == '_' )
        {
            // OK anywhere
            continue;
        }
        else if( c.isDigit() )
        {
            // not as first character
            if( i == 0 )
                return QValidator::Invalid;
        }
        else if( c.isLetter() )
        {
            // Only uppercase letters allowed
            if( c.isLower() )
            {
                aInput[i] = c.toUpper();
            }
        }
        else
        {
            return QValidator::Invalid;
        }
    }
    
    return QValidator::Acceptable;
}

void ENV_VAR_NAME_VALIDATOR::OnChar( QKeyEvent& aEvent )
{
    // This method is kept for compatibility but Qt validation happens in validate()
    aEvent.accept();
}


void ENV_VAR_NAME_VALIDATOR::OnTextChanged( const QString& text )
{
    // This method is kept for compatibility
    // Qt validation is handled in validate() method
}


REGEX_VALIDATOR::REGEX_VALIDATOR( const QString& aRegEx, QString* aValue ) :
    QValidator()
{
    compileRegEx( aRegEx, QRegularExpression::NoPatternOption );
}


REGEX_VALIDATOR::REGEX_VALIDATOR( const QString& aRegEx, QRegularExpression::PatternOptions aFlags, QString* aValue ) :
    QValidator()
{
    compileRegEx( aRegEx, aFlags );
}


REGEX_VALIDATOR::REGEX_VALIDATOR( const REGEX_VALIDATOR& aOther ) :
    QValidator(),
    m_regExString( aOther.m_regExString ),
    m_regExFlags( aOther.m_regExFlags ),
    m_regEx( aOther.m_regEx )
{
}


QValidator::State REGEX_VALIDATOR::validate( QString& aInput, int& aPos ) const
{
    QRegularExpressionMatch match = m_regEx.match( aInput );
    
    if( match.hasMatch() && match.capturedStart() == 0 && match.capturedLength() == aInput.length() )
    {
        return QValidator::Acceptable;
    }
    
    return QValidator::Invalid;
}


void REGEX_VALIDATOR::compileRegEx( const QString& aRegEx, QRegularExpression::PatternOptions aFlags )
{
    m_regEx.setPattern( aRegEx );
    m_regEx.setPatternOptions( aFlags );
    
    if( !m_regEx.isValid() )
    {
        throw std::runtime_error( "REGEX_VALIDATOR: Invalid regular expression: "
                + aRegEx.toStdString() );
    }

    m_regExString = aRegEx;
    m_regExFlags = aFlags;
}


NETNAME_VALIDATOR::NETNAME_VALIDATOR( QString *aVal ) :
         QValidator(),
         m_allowSpaces( false )
{
}


NETNAME_VALIDATOR::NETNAME_VALIDATOR( const NETNAME_VALIDATOR& aValidator ) :
        QValidator(),
        m_allowSpaces( aValidator.m_allowSpaces )
{
}


NETNAME_VALIDATOR::NETNAME_VALIDATOR( bool aAllowSpaces ) :
        QValidator(),
        m_allowSpaces( aAllowSpaces )
{
}


QValidator::State NETNAME_VALIDATOR::validate( QString& aInput, int& aPos ) const
{
    QString errormsg = IsValid( aInput );

    if( !errormsg.isEmpty() )
        return QValidator::Invalid;

    return QValidator::Acceptable;
}


QString NETNAME_VALIDATOR::IsValid( const QString& str ) const
{
    if( str.contains( '\r' ) || str.contains( '\n' ) )
        return QObject::tr( "Signal names cannot contain CR or LF characters" );

    if( !m_allowSpaces && ( str.contains( ' ' ) || str.contains( '\t' ) ) )
        return QObject::tr( "Signal names cannot contain spaces" );

    return QString();
}


void KIUI::ValidatorTransferToWindowWithoutEvents( QValidator& aValidator )
{
    // Qt validators work differently - they don't transfer data to/from windows
    // This function is kept for compatibility but is essentially a no-op in Qt
}


FIELD_VALIDATOR::FIELD_VALIDATOR( int aFieldId, QString* aValue ) :
        QValidator(), m_fieldId( aFieldId )
{
    // Fields cannot contain carriage returns, line feeds, or tabs.
    m_excludes = "\r\n\t";

    // The reference and sheet name fields cannot contain spaces.
    if( aFieldId == REFERENCE_FIELD )
    {
        m_excludes += " ";
    }
    else if( m_fieldId == SHEETNAME_V )
    {
        m_excludes += "/";
    }

    // The reference, sheetname and sheetfilename fields cannot be empty.
    m_allowEmpty = !( aFieldId == REFERENCE_FIELD || aFieldId == SHEETNAME_V || aFieldId == SHEETFILENAME_V );
}


FIELD_VALIDATOR::FIELD_VALIDATOR( const FIELD_VALIDATOR& aValidator ) :
        QValidator(), m_fieldId( aValidator.m_fieldId ), m_excludes( aValidator.m_excludes ), m_allowEmpty( aValidator.m_allowEmpty )
{
}


QValidator::State FIELD_VALIDATOR::validate( QString& aInput, int& aPos ) const
{
    QString val = aInput;
    
    if( !DoValidate( val, nullptr ) )
        return QValidator::Invalid;
        
    return QValidator::Acceptable;
}


bool FIELD_VALIDATOR::DoValidate( const QString& aValue, QWidget* aParent )
{
    QString msg;

    if( !m_allowEmpty && aValue.isEmpty() )
        msg = QObject::tr( "The value of the field cannot be empty." );

    bool containsExcluded = false;
    for( const QChar& excludeChar : m_excludes )
    {
        if( aValue.contains( excludeChar ) )
        {
            containsExcluded = true;
            break;
        }
    }
    
    if( containsExcluded )
    {
        QStringList badCharsFound;

        for( const QChar& excludeChar : m_excludes )
        {
            if( aValue.contains( excludeChar ) )
            {
                if( excludeChar == '\r' )
                    badCharsFound.append( QObject::tr( "carriage return" ) );
                else if( excludeChar == '\n' )
                    badCharsFound.append( QObject::tr( "line feed" ) );
                else if( excludeChar == '\t' )
                    badCharsFound.append( QObject::tr( "tab" ) );
                else if( excludeChar == ' ' )
                    badCharsFound.append( QObject::tr( "space" ) );
                else
                    badCharsFound.append( QString( "'%1'" ).arg( excludeChar ) );
            }
        }

        QString badChars;

        for( int i = 0; i < badCharsFound.size(); i++ )
        {
            if( !badChars.isEmpty() )
            {
                if( badCharsFound.size() == 2 )
                {
                    badChars += QObject::tr( " or " );
                }
                else
                {
                    if( i < badCharsFound.size() - 2 )
                        badChars += QObject::tr( ", or " );
                    else
                        badChars += ", ";
                }
            }

            badChars += badCharsFound[i];
        }

        switch( m_fieldId )
        {
        case REFERENCE_FIELD:
            msg = QObject::tr( "The reference designator cannot contain %1 character(s)." ).arg( badChars );
            break;

        case VALUE_FIELD:
            msg = QObject::tr( "The value field cannot contain %1 character(s)." ).arg( badChars );
            break;

        case FOOTPRINT_FIELD:
            msg = QObject::tr( "The footprint field cannot contain %1 character(s)." ).arg( badChars );
            break;

        case DATASHEET_FIELD:
            msg = QObject::tr( "The datasheet field cannot contain %1 character(s)." ).arg( badChars );
            break;

        case SHEETNAME_V:
            msg = QObject::tr( "The sheet name cannot contain %1 character(s)." ).arg( badChars );
            break;

        case SHEETFILENAME_V:
            msg = QObject::tr( "The sheet filename cannot contain %1 character(s)." ).arg( badChars );
            break;

        default:
            msg = QObject::tr( "The field cannot contain %1 character(s)." ).arg( badChars );
            break;
        };
    }
    else if( m_fieldId == REFERENCE_FIELD && aValue.contains( "${" ) )
    {
        msg = QObject::tr( "The reference designator cannot contain text variable references" );
    }
    else if( m_fieldId == REFERENCE_FIELD && UTIL::GetRefDesPrefix( aValue ).isEmpty() )
    {
        msg = QObject::tr( "References must start with a letter." );
    }

    if( !msg.isEmpty() )
    {
        if( aParent )
        {
            QMessageBox::warning( aParent, QObject::tr( "Field Validation Error" ), msg );
        }

        return false;
    }

    return true;
}
