
#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <memory>

#include <QValidator>
#include <QRegularExpression>
#include <QWidget>
#include <QString>
#include <QKeyEvent>

#include <lib_id.h>


#define FIELD_NAME  -1
#define FIELD_VALUE -2

#define SHEETNAME_V      100    // We can't use SHEETNAME and SHEETFILENAME because they
#define SHEETFILENAME_V  101    //   overlap with REFERENCE_FIELD and VALUE_FIELD
#define SHEETUSERFIELD_V 102

#define LABELUSERFIELD_V 200


class FOOTPRINT_NAME_VALIDATOR : public QValidator
{
    Q_OBJECT

public:
    FOOTPRINT_NAME_VALIDATOR( QString* aValue = nullptr );

    virtual QValidator::State validate( QString& aInput, int& aPos ) const override;
    
private:
    QString m_illegalChars;
};


class FILE_NAME_WITH_PATH_CHAR_VALIDATOR : public QValidator
{
    Q_OBJECT

public:
    FILE_NAME_WITH_PATH_CHAR_VALIDATOR( QString* aValue = nullptr );

    virtual QValidator::State validate( QString& aInput, int& aPos ) const override;
    
private:
    QString m_illegalChars;
};


class ENV_VAR_NAME_VALIDATOR : public QValidator
{
    Q_OBJECT

public:
    ENV_VAR_NAME_VALIDATOR( QString* aValue = nullptr );
    ENV_VAR_NAME_VALIDATOR( const ENV_VAR_NAME_VALIDATOR& val );

    virtual ~ENV_VAR_NAME_VALIDATOR();

    virtual QValidator::State validate( QString& aInput, int& aPos ) const override;

    void OnChar( QKeyEvent& event );

    void OnTextChanged( const QString& text );
};


class REGEX_VALIDATOR : public QValidator
{
    Q_OBJECT

public:
    REGEX_VALIDATOR( const QString& aRegEx, QString* aValue = nullptr );

    REGEX_VALIDATOR( const QString& aRegEx, QRegularExpression::PatternOptions aFlags, QString* aValue = nullptr );

    REGEX_VALIDATOR( const REGEX_VALIDATOR& aOther );

    virtual QValidator::State validate( QString& aInput, int& aPos ) const override;

    const QString& GetRegEx() const
    {
        return m_regExString;
    }

protected:
    void compileRegEx( const QString& aRegEx, QRegularExpression::PatternOptions aFlags );

    QString m_regExString;
    QRegularExpression::PatternOptions m_regExFlags;
    QRegularExpression m_regEx;
};

class NETNAME_VALIDATOR : public QValidator
{
    Q_OBJECT

public:
    NETNAME_VALIDATOR( QString* aVal = nullptr );

    NETNAME_VALIDATOR( bool aAllowSpaces );

    NETNAME_VALIDATOR( const NETNAME_VALIDATOR& aValidator );

    virtual QValidator::State validate( QString& aInput, int& aPos ) const override;

    QString IsValid( const QString& aVal ) const;

private:
    bool m_allowSpaces;
};


namespace KIUI
{

void ValidatorTransferToWindowWithoutEvents( QValidator& aValidator );

} // namespace KIUI


class FIELD_VALIDATOR : public QValidator
{
    Q_OBJECT

public:
    FIELD_VALIDATOR( int aFieldId, QString* aValue = nullptr );

    FIELD_VALIDATOR( const FIELD_VALIDATOR& aValidator );

    virtual QValidator::State validate( QString& aInput, int& aPos ) const override;

    bool DoValidate( const QString& aValue, QWidget* aParent ) const;

private:
    int m_fieldId;
    QString m_excludes;
    bool m_allowEmpty;
};


#endif  // #ifndef VALIDATORS_H
