// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_JSON_SETTINGS_INTERNALS_H
#define KICAD_JSON_SETTINGS_INTERNALS_H

#include <json_common.h>

class KICOMMON_API JSON_SETTINGS_INTERNALS : public nlohmann::json
{
    friend class JSON_SETTINGS;

public:
    JSON_SETTINGS_INTERNALS() :
            nlohmann::json()
    {}

    static nlohmann::json::json_pointer PointerFromString( std::string aPath );

    template<typename ValueType>
    void SetFromString( const std::string& aPath, ValueType aVal )
    {
        ( *this )[aPath] = std::move( aVal );
    }

    template<typename ValueType>
    ValueType Get( const std::string& aPath ) const
    {
        return at( PointerFromString( aPath ) ).get<ValueType>();
    }

    nlohmann::json& At( const std::string& aPath )
    {
        return at( PointerFromString( aPath ) );
    }

    nlohmann::json& operator[]( const nlohmann::json::json_pointer& aPointer )
    {
        return nlohmann::json::operator[]( aPointer );
    }

    nlohmann::json& operator[]( const std::string& aPath )
    {
        return nlohmann::json::operator[]( PointerFromString( aPath ) );
    }

    void CloneFrom( const JSON_SETTINGS_INTERNALS& aOther )
    {
        nlohmann::json::json_pointer root( "" );
        this->nlohmann::json::operator[]( root ) = aOther.nlohmann::json::operator[]( root );
    }

private:

    nlohmann::json m_original;
};

#endif // KICAD_JSON_SETTINGS_INTERNALS_H
