// QT_TRANSFORMATION_COMPLETED - Mathematical utility functions for KiCad

#ifndef UTIL_H
#define UTIL_H

#include <config.h>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <typeinfo>
#include <type_traits>

// Math constants not defined on Windows
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef M_PI_2
#define M_PI_2 (M_PI / 2.0)
#endif

#ifndef M_PI_4
#define M_PI_4 (M_PI / 4.0)
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.4142135623730950488016887242097
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865475244008443621048
#endif

void kimathLogDebug( const char* aFormatString, ... );

void kimathLogOverflow( double v, const char* aTypeName );


// Suppress an annoying warning that the explicit rounding we do is not precise
#ifdef HAVE_WIMPLICIT_FLOAT_CONVERSION
    _Pragma( "GCC diagnostic push" ) \
    _Pragma( "GCC diagnostic ignored \"-Wimplicit-int-float-conversion\"" )
#endif


template <typename in_type = long long int, typename ret_type = int>
inline constexpr ret_type KiCheckedCast( in_type v )
{
    if constexpr( std::is_same_v<in_type, long long int> && std::is_same_v<ret_type, int> )
    {
        if( v > (std::numeric_limits<int>::max)() )
        {
            kimathLogOverflow( double( v ), typeid( int ).name() );

            return (std::numeric_limits<int>::max)();
        }
        else if( v < std::numeric_limits<int>::lowest() )
        {
            kimathLogOverflow( double( v ), typeid( int ).name() );

            return std::numeric_limits<int>::lowest();
        }

        return int( v );
    }
    else
    {
        return v;
    }
}


template <typename fp_type, typename ret_type = int>
constexpr ret_type KiROUND( fp_type v, bool aQuiet = false )
{
    using max_ret = long long int;
    fp_type ret = v < 0 ? v - 0.5 : v + 0.5;

    if( ret > (std::numeric_limits<ret_type>::max)() )
    {
        if( !aQuiet )
        {
            kimathLogOverflow( double( v ), typeid( ret_type ).name() );
        }

        return (std::numeric_limits<ret_type>::max)() - 1;
    }
    else if( ret < std::numeric_limits<ret_type>::lowest() )
    {
        if( !aQuiet )
        {
            kimathLogOverflow( double( v ), typeid( ret_type ).name() );
        }

        if( std::numeric_limits<ret_type>::is_signed )
            return std::numeric_limits<ret_type>::lowest() + 1;
        else
            return 0;
    }
#if __cplusplus >= 202302L // isnan is not constexpr until C++23
    else if constexpr( std::is_floating_point_v<fp_type> )
    {
        if( std::isnan( v ) )
        {
            if( !aQuiet )
            {
                kimathLogOverflow( double( v ), typeid( ret_type ).name() );
            }

            return 0;
        }
    }
#endif

    return ret_type( max_ret( ret ) );
}

#ifdef HAVE_WIMPLICIT_FLOAT_CONVERSION
    _Pragma( "GCC diagnostic pop" )
#endif


template <typename T>
T rescale( T aNumerator, T aValue, T aDenominator )
{
    return aNumerator * aValue / aDenominator;
}

template <typename T>
constexpr int sign( T val )
{
    return ( T( 0 ) < val) - ( val < T( 0 ) );
}

// explicit specializations for integer types, taking care of overflow.
template <>
int rescale( int aNumerator, int aValue, int aDenominator );

template <>
int64_t rescale( int64_t aNumerator, int64_t aValue, int64_t aDenominator );


template <class T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
equals( T aFirst, T aSecond, T aEpsilon = std::numeric_limits<T>::epsilon() )
{
    const T diff = std::abs( aFirst - aSecond );

    if( diff < aEpsilon )
    {
        return true;
    }

    aFirst = std::abs( aFirst );
    aSecond = std::abs( aSecond );
    T largest = aFirst > aSecond ? aFirst : aSecond;

    if( diff <= largest * aEpsilon )
    {
        return true;
    }

    return false;
}


#endif // UTIL_H
