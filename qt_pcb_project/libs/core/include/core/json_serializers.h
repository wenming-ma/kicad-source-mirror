// QT_TRANSFORMATION_COMPLETED
#ifndef JSON_SERIALIZERS_H_
#define JSON_SERIALIZERS_H_

#include <json_common.h>
#include <QString>
#include <optional>

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail
{
// Custom char_traits for QChar to support JSON serialization
template <>
struct char_traits<QChar> : std::char_traits<char>
{
    using char_type = QChar;
    using int_type = uint32_t;      // Use same type as Qt's internal representation

    static int_type to_int_type( char_type c ) noexcept { return static_cast<int_type>( c.unicode() ); }

    static char_type to_char_type( int_type i ) noexcept { return QChar( static_cast<ushort>( i ) ); }

    static constexpr int_type eof() noexcept { return static_cast<int_type>( EOF ); }
};
} // namespace detail
NLOHMANN_JSON_NAMESPACE_END

namespace nlohmann
{
template <>
struct adl_serializer<QString>
{
    static void from_json( const json& j, QString& s )
    {
        s = QString::fromStdString( j.get<std::string>() );
    }

    static void to_json( json& j, const QString& s ) { j = s.toStdString(); }
};

template <typename T>
struct adl_serializer<std::optional<T>>
{
    static void from_json( const json& j, std::optional<T>& opt )
    {
        if( j.is_null() )
        {
            opt = std::nullopt;
        }
        else
        {
            opt = j.template get<T>();
        }
    }

    static void to_json( json& j, const std::optional<T>& opt )
    {
        if( opt.has_value() )
        {
            j = *opt;
        }
        else
        {
            j = nullptr;
        }
    }
};
} // namespace nlohmann


#endif // JSON_SERIALIZERS_H_