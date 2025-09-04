// QT_TRANSFORMATION_COMPLETED

#ifndef MAP_HELPERS_H_
#define MAP_HELPERS_H_

#include <optional>
#include <map>
#include <QString>


template <typename V>
inline std::optional<V> get_opt( const std::map<QString, V>& aMap, const QString& aKey )
{
    auto it = aMap.find( aKey );

    if( it == aMap.end() )
        return std::nullopt;

    return it->second;
}


template <typename V>
inline std::optional<V> get_opt( const std::map<QString, V>& aMap, const char* aKey )
{
    return get_opt( aMap, QString::fromUtf8( aKey ) );
}


template <typename K, typename V>
inline std::optional<V> get_opt( const std::map<K, V>& aMap, const K& aKey )
{
    auto it = aMap.find( aKey );

    if( it == aMap.end() )
        return std::nullopt;

    return it->second;
}


inline QString get_def( const std::map<QString, QString>& aMap, const char* aKey,
                         const char* aDefval = "" )
{
    typename std::map<QString, QString>::const_iterator it =
            aMap.find( QString::fromUtf8( aKey ) );
    if( it == aMap.end() )
    {
        return QString::fromUtf8( aDefval );
    }
    else
    {
        return it->second;
    }
}


inline QString get_def( const std::map<QString, QString>& aMap, const char* aKey,
                         const QString& aDefval = QString() )
{
    typename std::map<QString, QString>::const_iterator it =
            aMap.find( QString::fromUtf8( aKey ) );
    if( it == aMap.end() )
    {
        return aDefval;
    }
    else
    {
        return it->second;
    }
}


inline QString get_def( const std::map<QString, QString>& aMap, const QString& aKey,
                         const QString& aDefval = QString() )
{
    typename std::map<QString, QString>::const_iterator it = aMap.find( aKey );
    if( it == aMap.end() )
    {
        return aDefval;
    }
    else
    {
        return it->second;
    }
}


template <typename K, typename V>
inline V get_def( const std::map<K, V>& aMap, const K& aKey, const V& aDefval = V() )
{
    typename std::map<K, V>::const_iterator it = aMap.find( aKey );
    if( it == aMap.end() )
    {
        return aDefval;
    }
    else
    {
        return it->second;
    }
}


#endif // MAP_HELPERS_H_
