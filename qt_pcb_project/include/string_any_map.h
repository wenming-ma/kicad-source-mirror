// QT_TRANSFORMATION_COMPLETED

#ifndef STRING_ANY_MAP_H_
#define STRING_ANY_MAP_H_

#include <string>
#include <map>
#include <optional>

#include <QVariant>


class STRING_ANY_MAP : public std::map<std::string, QVariant>
{
    double m_iuScale;

public:

    STRING_ANY_MAP( double aIUScale = 1.0 ) : m_iuScale( aIUScale ) {}

    template <typename T>
    bool get_to( const std::string& aKey, T& aVar ) const
    {
        if( !contains( aKey ) )
            return false;

        aVar = at( aKey ).value<T>();
        return at( aKey ).canConvert<T>();
    }

    template <typename T>
    bool get_to_iu( const std::string& aKey, T& aVar ) const
    {
        if( !contains( aKey ) )
            return false;

        const QVariant& value = at( aKey );

        if( value.canConvert<double>() || value.canConvert<int>() || value.canConvert<long>()
            || value.canConvert<long long>() )
        {
            bool ok = false;
            double number = value.toDouble( &ok );

            if( !ok )
                return false;

            number *= m_iuScale;
            aVar = number;
        }
        else
        {
            if( !value.canConvert<T>() )
                return false;
            
            aVar = value.value<T>();
        }

        return true;
    }

    template <typename T>
    void set( const std::string& aKey, const T& aVar )
    {
        emplace( aKey, aVar );
    }

    template <typename T>
    void set_iu( const std::string& aKey, const T& aVar)
    {
        emplace( aKey, aVar / m_iuScale );
    }

    bool contains( const std::string& aKey ) const
    { //
        return find( aKey ) != end();
    }

    template <typename T>
    std::optional<T> get_opt( const std::string& aKey ) const
    {
        if( contains( aKey ) )
        {
            T val;

            if( !at( aKey ).canConvert<T>() )
                return std::nullopt;

            val = at( aKey ).value<T>();

            return val;
        }

        return std::nullopt;
    }
};


#endif // STRING_ANY_MAP_H_