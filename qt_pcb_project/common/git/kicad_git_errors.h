#ifndef KICAD_GIT_ERRORS_H
#define KICAD_GIT_ERRORS_H

#include <QVector>
#include <QString>

class KIGIT_ERRORS
{
public:

    KIGIT_ERRORS() = default;
    virtual ~KIGIT_ERRORS() = default;

    const QVector<QString>& GetErrorStrings() const
    {
        return m_errorStrings;
    }

    const QString& PeekErrorString() const
    {
        if( m_errorStrings.empty() )
        {
            static const QString noError = "No error";
            return noError;
        }
        else
            return m_errorStrings.back();
    }

    QString GetErrorString()
    {
        if( m_errorStrings.empty() )
            return "No error";

        const QString errorString( m_errorStrings.back() );
        m_errorStrings.pop_back();
        return errorString;
    }

    void AddErrorString( const QString aErrorString )
    {
        m_errorStrings.push_back( aErrorString );
    }

    void AddErrorString( const std::string aErrorString )
    {
        m_errorStrings.push_back( QString::fromStdString( aErrorString ) );
    }

    void ClearErrorStrings()
    {
        m_errorStrings.clear();
    }

private:
    QVector<QString> m_errorStrings;
};

#endif // KICAD_GIT_ERRORS_H