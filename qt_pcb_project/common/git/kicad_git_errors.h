#ifndef KICAD_GIT_ERRORS_H
#define KICAD_GIT_ERRORS_H

#include <vector>
#include <string>
#include <QString>

class KIGIT_ERRORS
{
public:

    KIGIT_ERRORS() = default;
    virtual ~KIGIT_ERRORS() = default;

    std::vector<std::string> GetErrorStrings() const
    {
        return m_errorStrings;
    }

    QString PeekErrorString() const
    {
        if( m_errorStrings.empty() )
        {
            return QString("No error");
        }
        else
            return QString::fromStdString(m_errorStrings.back());
    }

    QString GetErrorString()
    {
        if( m_errorStrings.empty() )
            return QString("No error");

        const std::string errorString = m_errorStrings.back();
        m_errorStrings.pop_back();
        return QString::fromStdString(errorString);
    }

    void AddErrorString( const QString aErrorString )
    {
        m_errorStrings.push_back( aErrorString.toStdString() );
    }

    void AddErrorString( const std::string aErrorString )
    {
        m_errorStrings.push_back( aErrorString );
    }

    void ClearErrorStrings()
    {
        m_errorStrings.clear();
    }

private:
    std::vector<std::string> m_errorStrings;
};

#endif // KICAD_GIT_ERRORS_H