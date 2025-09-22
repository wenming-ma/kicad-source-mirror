#ifndef KICAD_GIT_ERRORS_H
#define KICAD_GIT_ERRORS_H

#include <vector>

#include <QString>

class KIGIT_ERRORS
{
public:

    KIGIT_ERRORS() = default;
    virtual ~KIGIT_ERRORS() = default;

    const std::vector<QString>& GetErrorStrings() const
    {
        return m_errorStrings;
    }

    const QString& PeekErrorString() const
    {
        if( m_errorStrings.empty() )
            return "No error";
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
        m_errorStrings.emplace_back( aErrorString );
    }

    void AddErrorString( const std::string aErrorString )
    {
        m_errorStrings.emplace_back( aErrorString );
    }

    void ClearErrorStrings()
    {
        m_errorStrings.clear();
    }

private:

    std::vector<QString> m_errorStrings;

};

#endif // KICAD_GIT_ERRORS_H
