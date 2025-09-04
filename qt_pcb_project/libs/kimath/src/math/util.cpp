// QT_TRANSFORMATION_COMPLETED

#include <cmath>
#include <cstdlib>
#include <limits>
#include <cstdarg>
#include <math/util.h>
#include <QString>
#include <QDebug>
#include <QLoggingCategory>

#ifdef _MSC_VER
#include <windows.h>
#include <intrin.h>
#endif

Q_LOGGING_CATEGORY(logKimath, "kimath")

void kimathLogDebug( const char* aFormatString, ... )
{
    if( logKimath().isDebugEnabled() )
    {
        va_list argList;
        va_start( argList, aFormatString );

        QString message;
        message.vasprintf( aFormatString, argList );
        qCDebug( logKimath ) << message;

        va_end( argList );
    }
}


void kimathLogOverflow( double v, const char* aTypeName )
{
    QString typeName( aTypeName );
    QString message = QString( "\n\nOverflow converting value %1 to %2." ).arg( v ).arg( typeName );
    Q_ASSERT_X( false, "kimathLogOverflow", message.toLocal8Bit().constData() );
}


template<>
int rescale( int aNumerator, int aValue, int aDenominator )
{
    int64_t numerator = (int64_t) aNumerator * (int64_t) aValue;

    // round to nearest
    if( ( numerator < 0 ) ^ ( aDenominator < 0 ) )
        return ( numerator - aDenominator / 2 ) / aDenominator;
    else
        return ( numerator + aDenominator / 2 ) / aDenominator;

}


template<>
int64_t rescale( int64_t aNumerator, int64_t aValue, int64_t aDenominator )
{
#if defined( _M_X64 ) && ( _MSC_VER >= 1920 )
    int64_t  productHi;
    uint64_t productLo = static_cast<uint64_t>( _mul128( aNumerator, aValue, &productHi ) );

    int64_t r = ( ( productHi < 0 ) ^ ( aDenominator < 0 ) ) ? -aDenominator / 2 : aDenominator / 2;

    uint64_t rLo = static_cast<uint64_t>( r );
    int64_t  rHi = r < 0 ? -1ll : 0ll;

    productLo += rLo;
    productHi += rHi + ( productLo < rLo );

    __try
    {
        int64_t remainder;
        int64_t result = _div128( productHi, productLo, aDenominator, &remainder );

        return result;
    }
    __except( ( GetExceptionCode() == EXCEPTION_INT_OVERFLOW ) ? EXCEPTION_EXECUTE_HANDLER
                                                               : EXCEPTION_CONTINUE_SEARCH )
    {
        kimathLogDebug( "Overflow in rescale (%lld * %lld + %lld) / %lld", aNumerator, aValue, r,
                        aDenominator );
    }

    return 0;

#elif defined( __SIZEOF_INT128__ )
    __int128_t numerator = (__int128_t) aNumerator * (__int128_t) aValue;

    if( ( numerator < 0 ) ^ ( aDenominator < 0 ) )
        return ( numerator - aDenominator / 2 ) / aDenominator;
    else
        return ( numerator + aDenominator / 2 ) / aDenominator;

#else
    int64_t r = 0;
    int64_t sign = ( ( aNumerator < 0 ) ? -1 : 1 ) * ( aDenominator < 0 ? -1 : 1 ) *
                                                     ( aValue < 0 ? -1 : 1 );

    int64_t a = std::abs( aNumerator );
    int64_t b = std::abs( aValue );
    int64_t c = std::abs( aDenominator );

    r = c / 2;

    if( b <= std::numeric_limits<int>::max() && c <= std::numeric_limits<int>::max() )
    {
        if( a <= std::numeric_limits<int>::max() )
            return sign * ( ( a * b + r ) / c );
        else
            return sign * ( a / c * b + ( a % c * b + r ) / c);
    }
    else
    {
        uint64_t a0 = a & 0xFFFFFFFF;
        uint64_t a1 = a >> 32;
        uint64_t b0 = b & 0xFFFFFFFF;
        uint64_t b1 = b >> 32;
        uint64_t t1 = a0 * b1 + a1 * b0;
        uint64_t t1a = t1 << 32;
        int i;

        a0 = a0 * b0 + t1a;
        a1 = a1 * b1 + ( t1 >> 32 ) + ( a0 < t1a );
        a0 += r;
        a1 += a0 < (uint64_t)r;

        for( i = 63; i >= 0; i-- )
        {
            a1  += a1 + ( ( a0 >> i ) & 1 );
            t1  += t1;

            if( (uint64_t) c <= a1 )
            {
                a1 -= c;
                t1++;
            }
        }

        return t1 * sign;
    }
#endif
}
