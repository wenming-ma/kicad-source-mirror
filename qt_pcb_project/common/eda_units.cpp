
#include <eda_units.h>
#include <fmt/core.h>
#include <math/util.h>      // for KiROUND
#include <macros.h>
#include <charconv>
#include <QString>
#include <QLocale>


static void removeTrailingZeros( QString& aText )
{
    int len = aText.length();
    int removeLast = 0;

    while( --len > 0 && aText[len] == '0' )
        removeLast++;

    if( len >= 0 && ( aText[len] == '.' || aText[len] == ',' ) )
        removeLast++;

    aText = aText.left( aText.length() - removeLast );
}


bool EDA_UNIT_UTILS::IsImperialUnit( EDA_UNITS aUnit )
{
    switch( aUnit )
    {
    case EDA_UNITS::INCH:
    case EDA_UNITS::MILS:
        return true;

    default:
        return false;
    }
}


bool EDA_UNIT_UTILS::IsMetricUnit( EDA_UNITS aUnit )
{
    switch( aUnit )
    {
    case EDA_UNITS::UM:
    case EDA_UNITS::MM:
    case EDA_UNITS::CM:
        return true;

    default:
        return false;
    }
}


int EDA_UNIT_UTILS::Mm2mils( double aVal )
{
    return KiROUND( aVal * 1000. / 25.4 );
}


int EDA_UNIT_UTILS::Mils2mm( double aVal )
{
    return KiROUND( aVal * 25.4 / 1000. );
}


bool EDA_UNIT_UTILS::FetchUnitsFromString( const QString& aTextValue, EDA_UNITS& aUnits )
{
    QString buf( aTextValue.trimmed() );
    unsigned brk_point = 0;

    while( brk_point < buf.length() )
    {
        QChar c = buf[brk_point];

        if( !( ( c >= '0' && c <= '9' ) || ( c == '.' ) || ( c == ',' ) || ( c == '-' )
               || ( c == '+' ) ) )
            break;

        ++brk_point;
    }

    // Check the unit designator (2 ch significant)
    QString unit( buf.mid( brk_point ).trimmed().left( 2 ).toLower() );

    //check for um, μm (µ is MICRO SIGN) and µm (µ is GREEK SMALL LETTER MU) for micrometre
    if( unit == "um" || unit == "\u00B5m" || unit == "\u03BCm" )
        aUnits = EDA_UNITS::UM;
    else if( unit == "mm" )
        aUnits = EDA_UNITS::MM;
    if( unit == "cm" )
        aUnits = EDA_UNITS::CM;
    else if( unit == "mi" || unit == "th" ) // "mils" or "thou"
        aUnits = EDA_UNITS::MILS;
    else if( unit == "in" || unit == "\"" )
        aUnits = EDA_UNITS::INCH;
    else if( unit == "de" || unit == "ra" ) // "deg" or "rad"
        aUnits = EDA_UNITS::DEGREES;
    else
        return false;

    return true;
}


QString EDA_UNIT_UTILS::GetText( EDA_UNITS aUnits, EDA_DATA_TYPE aType )
{
    QString label;

    switch( aUnits )
    {
    case EDA_UNITS::UM:       label = " \u00B5m";  break; //00B5 for µ
    case EDA_UNITS::MM:       label = " mm";       break;
    case EDA_UNITS::CM:       label = " cm";       break;
    case EDA_UNITS::DEGREES:  label = "°";         break;
    case EDA_UNITS::MILS:     label = " mils";     break;
    case EDA_UNITS::INCH:       label = " in";       break;
    case EDA_UNITS::PERCENT:  label = "%";         break;
    case EDA_UNITS::UNSCALED:                             break;
    default: UNIMPLEMENTED_FOR( "Unknown units" ); break;
    }

    switch( aType )
    {
    case EDA_DATA_TYPE::VOLUME:      label += "³";       break;
    case EDA_DATA_TYPE::AREA:        label += "²";       break;
    case EDA_DATA_TYPE::DISTANCE:                               break;
    default: UNIMPLEMENTED_FOR( "Unknown measurement" ); break;
    }

    return label;
}


QString EDA_UNIT_UTILS::GetLabel( EDA_UNITS aUnits, EDA_DATA_TYPE aType )
{
    return GetText( aUnits, aType ).trimmed();
}


std::string EDA_UNIT_UTILS::FormatAngle( const EDA_ANGLE& aAngle )
{
    std::string temp = fmt::format( "{:.10g}", aAngle.AsDegrees() );

    return temp;
}


std::string EDA_UNIT_UTILS::FormatInternalUnits( const EDA_IU_SCALE& aIuScale, int aValue )
{
    std::string buf;
    double engUnits = aValue;

    engUnits /= aIuScale.IU_PER_MM;

    if( engUnits != 0.0 && fabs( engUnits ) <= 0.0001 )
    {
        buf = fmt::format( "{:.10f}", engUnits );

        // remove trailing zeros
        while( !buf.empty() && buf[buf.size() - 1] == '0' )
        {
            buf.pop_back();
        }

        // if the value was really small
        // we may have just stripped all the zeros after the decimal
        if( buf[buf.size() - 1] == '.' )
        {
            buf.pop_back();
        }
    }
    else
    {
        buf = fmt::format( "{:.10g}", engUnits );
    }

    return buf;
}


std::string EDA_UNIT_UTILS::FormatInternalUnits( const EDA_IU_SCALE& aIuScale,
                                                 const VECTOR2I&     aPoint )
{
    return FormatInternalUnits( aIuScale, aPoint.x ) + " "
           + FormatInternalUnits( aIuScale, aPoint.y );
}


#if 0   // No support for std::from_chars on MacOS yet

bool EDA_UNIT_UTILS::ParseInternalUnits( const std::string& aInput, const EDA_IU_SCALE& aIuScale,
                                         int& aOut )
{
    double value;

    if( std::from_chars( aInput.data(), aInput.data() + aInput.size(), value ).ec != std::errc() )
        return false;

    aOut = value * aIuScale.IU_PER_MM;
    return true;
}


bool EDA_UNIT_UTILS::ParseInternalUnits( const std::string& aInput, const EDA_IU_SCALE& aIuScale,
                                         VECTOR2I& aOut )
{
    size_t pos = aInput.find( ' ' );

    if( pos == std::string::npos )
        return false;

    std::string first = aInput.substr( 0, pos );
    std::string second = aInput.substr( pos + 1 );

    VECTOR2I vec;

    if( !ParseInternalUnits( first, aIuScale, vec.x ) )
        return false;

    if( !ParseInternalUnits( second, aIuScale, vec.y ) )
        return false;

    aOut = vec;

    return true;
}

#endif


#define IU_TO_MM( x, scale ) ( x / scale.IU_PER_MM )
#define IU_TO_IN( x, scale ) ( x / scale.IU_PER_MILS / 1000 )
#define IU_TO_MILS( x, scale ) ( x / scale.IU_PER_MILS )
#define MM_TO_IU( x, scale ) ( x * scale.IU_PER_MM )
#define IN_TO_IU( x, scale ) ( x * scale.IU_PER_MILS * 1000 )
#define MILS_TO_IU( x, scale ) ( x * scale.IU_PER_MILS )


double EDA_UNIT_UTILS::UI::ToUserUnit( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnit,
                                       double aValue )
{
    switch( aUnit )
    {
    case EDA_UNITS::UM:      return IU_TO_MM( aValue, aIuScale ) * 1000;
    case EDA_UNITS::MM:      return IU_TO_MM( aValue, aIuScale );
    case EDA_UNITS::CM:      return IU_TO_MM( aValue, aIuScale ) / 10;
    case EDA_UNITS::MILS:    return IU_TO_MILS( aValue, aIuScale );
    case EDA_UNITS::INCH:    return IU_TO_IN( aValue, aIuScale );
    case EDA_UNITS::DEGREES: return aValue;
    default:                 return aValue;
    }
}


QString EDA_UNIT_UTILS::UI::StringFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                              double aValue, bool aAddUnitsText,
                                              EDA_DATA_TYPE aType )
{
    double value_to_print = aValue;
    bool   is_eeschema = ( aIuScale.IU_PER_MM == SCH_IU_PER_MM );

    switch( aType )
    {
    case EDA_DATA_TYPE::VOLUME:
        value_to_print = ToUserUnit( aIuScale, aUnits, value_to_print );
        KI_FALLTHROUGH;

    case EDA_DATA_TYPE::AREA:
        value_to_print = ToUserUnit( aIuScale, aUnits, value_to_print );
        KI_FALLTHROUGH;

    case EDA_DATA_TYPE::DISTANCE:
        value_to_print = ToUserUnit( aIuScale, aUnits, value_to_print );
        break;

    case EDA_DATA_TYPE::UNITLESS:
        break;
    }

    const char* format = nullptr;

    switch( aUnits )
    {

    case EDA_UNITS::MILS:    format = is_eeschema ? "%.3f" : "%.5f"; break;
    case EDA_UNITS::INCH:    format = is_eeschema ? "%.6f" : "%.8f"; break;
    case EDA_UNITS::DEGREES: format = "%.4f";                               break;
    default:                 format = "%.10f";                              break;
    }

    QString text;
    text = QString::asprintf( format, value_to_print );
    removeTrailingZeros( text );

    if( value_to_print != 0.0 && ( text == "0" || text == "-0" ) )
    {
        text = QString::asprintf( "%.10f", value_to_print );
        removeTrailingZeros( text );
    }

    if( aAddUnitsText )
        text += EDA_UNIT_UTILS::GetText( aUnits, aType );

    return text;
}



// A lower-precision (for readability) version of StringFromValue()
QString EDA_UNIT_UTILS::UI::MessageTextFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                   int aValue,
                                                   bool aAddUnitLabel,
                                                   EDA_DATA_TYPE aType )
{
    return MessageTextFromValue( aIuScale, aUnits, double( aValue ), aAddUnitLabel, aType );
}


// A lower-precision (for readability) version of StringFromValue()
QString EDA_UNIT_UTILS::UI::MessageTextFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                   long long int aValue,
                                                   bool aAddUnitLabel,
                                                   EDA_DATA_TYPE aType )
{
    return MessageTextFromValue( aIuScale, aUnits, double( aValue ), aAddUnitLabel, aType );
}


QString EDA_UNIT_UTILS::UI::MessageTextFromValue( EDA_ANGLE aValue, bool aAddUnitLabel )
{
    if( aAddUnitLabel )
        return QString::asprintf( "%.1f°", aValue.AsDegrees() );
    else
        return QString::asprintf( "%.1f", aValue.AsDegrees() );
}


// A lower-precision (for readability) version of StringFromValue()
QString EDA_UNIT_UTILS::UI::MessageTextFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                   double aValue, bool aAddUnitsText,
                                                   EDA_DATA_TYPE aType )
{
    QString      text;
    const char* format;
    double        value = aValue;
    bool          is_eeschema = ( aIuScale.IU_PER_MM == SCH_IU_PER_MM );

    switch( aType )
    {
    case EDA_DATA_TYPE::VOLUME:
        value = ToUserUnit( aIuScale, aUnits, value );
        // Fall through to continue computation
        KI_FALLTHROUGH;

    case EDA_DATA_TYPE::AREA:
        value = ToUserUnit( aIuScale, aUnits, value );
        // Fall through to continue computation
        KI_FALLTHROUGH;

    case EDA_DATA_TYPE::DISTANCE:
        value = ToUserUnit( aIuScale, aUnits, value );
        break;

    case EDA_DATA_TYPE::UNITLESS:
        break;
    }

    switch( aUnits )
    {
    default:
    case EDA_UNITS::UM:       format = is_eeschema ? "%.0f" : "%.1f"; break;
    case EDA_UNITS::MM:       format = is_eeschema ? "%.3f" : "%.4f"; break;
    case EDA_UNITS::CM:       format = is_eeschema ? "%.3f" : "%.5f"; break;
    case EDA_UNITS::MILS:     format = is_eeschema ? "%.0f" : "%.2f"; break;
    case EDA_UNITS::INCH:     format = is_eeschema ? "%.3f" : "%.4f"; break;
    case EDA_UNITS::DEGREES:  format = "%.3f";                               break;
    case EDA_UNITS::UNSCALED: format = "%.0f";                               break;
    }

    text = QString::asprintf( format, value );

    // Trim to 2-1/2 digits after the decimal place for short-form mm
    if( is_eeschema && aUnits == EDA_UNITS::MM )
    {
        QLocale locale;
        int     length = text.length();

        if( length > 4 && text[length - 4] == locale.decimalPoint() && text[length - 1] == '0' )
            text = text.left( length - 1 );
    }

    if( aAddUnitsText )
        text += EDA_UNIT_UTILS::GetText( aUnits, aType );

    return text;
}


QString EDA_UNIT_UTILS::UI::MessageTextFromMinOptMax( const EDA_IU_SCALE& aIuScale,
                                                       EDA_UNITS aUnits,
                                                       const MINOPTMAX<int>& aValue )
{
    QString msg;

    if( aValue.HasMin() && aValue.Min() > 0 )
    {
        msg += _( "min" ) + " " + MessageTextFromValue( aIuScale, aUnits, aValue.Min() );
    }

    if( aValue.HasOpt() )
    {
        if( !msg.isEmpty() )
            msg += "; ";

        msg += _( "opt" ) + " " + MessageTextFromValue( aIuScale, aUnits, aValue.Opt() );
    }

    if( aValue.HasMax() )
    {
        if( !msg.isEmpty() )
            msg += "; ";

        msg += _( "max" ) + " " + MessageTextFromValue( aIuScale, aUnits, aValue.Max() );
    }

    return msg;
};


double EDA_UNIT_UTILS::UI::FromUserUnit( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                         double aValue )
{
    switch( aUnits )
    {
    case EDA_UNITS::UM:       return MM_TO_IU( aValue / 1000.0, aIuScale );
    case EDA_UNITS::MM:       return MM_TO_IU( aValue, aIuScale );
    case EDA_UNITS::CM:       return MM_TO_IU( aValue * 10, aIuScale );
    case EDA_UNITS::MILS:     return MILS_TO_IU( aValue, aIuScale );
    case EDA_UNITS::INCH:     return IN_TO_IU( aValue, aIuScale );
    default:
    case EDA_UNITS::DEGREES:
    case EDA_UNITS::UNSCALED:
    case EDA_UNITS::PERCENT:  return aValue;
    }
}


double EDA_UNIT_UTILS::UI::DoubleValueFromString( const QString& aTextValue )
{
    double dtmp = 0;

    // Acquire the 'right' decimal point separator
    QLocale locale;

    QChar   decimal_point = locale.decimalPoint();
    QString buf( aTextValue.trimmed() );

    // Convert any entered decimal point separators to the 'right' one
    buf.replace( ".", QString( decimal_point ) );
    buf.replace( ",", QString( decimal_point ) );

    // Find the end of the numeric part
    unsigned brk_point = 0;

    while( brk_point < buf.length() )
    {
        QChar ch = buf[brk_point];

        if( !( ( ch >= '0' && ch <= '9' ) || ( ch == decimal_point ) || ( ch == '-' )
               || ( ch == '+' ) ) )
        {
            break;
        }

        ++brk_point;
    }

    // Extract the numeric part
    dtmp = buf.left( brk_point ).toDouble();

    return dtmp;
}


double EDA_UNIT_UTILS::UI::DoubleValueFromString( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                  const QString& aTextValue, EDA_DATA_TYPE aType )
{
    double dtmp = 0;

    // Acquire the 'right' decimal point separator
    QLocale locale;

    QChar      decimal_point = locale.decimalPoint();
    QString    buf( aTextValue.trimmed() );

    // Convert any entered decimal point separators to the 'right' one
    buf.replace( ".", QString( decimal_point ) );
    buf.replace( ",", QString( decimal_point ) );

    // Find the end of the numeric part
    unsigned brk_point = 0;

    while( brk_point < buf.length() )
    {
        QChar ch = buf[brk_point];

        if( !( (ch >= '0' && ch <= '9') || (ch == decimal_point) || (ch == '-') || (ch == '+') ) )
            break;

        ++brk_point;
    }

    // Extract the numeric part
    dtmp = buf.left( brk_point ).toDouble();

    // Check the optional unit designator (2 ch significant)
    QString unit( buf.mid( brk_point ).trimmed().left( 2 ).toLower() );

    if( aUnits == EDA_UNITS::UM
            || aUnits == EDA_UNITS::MM
            || aUnits == EDA_UNITS::CM
            || aUnits == EDA_UNITS::MILS
            || aUnits == EDA_UNITS::INCH )
    {
        //check for um, μm (µ is MICRO SIGN) and µm (µ is GREEK SMALL LETTER MU) for micrometre
        if( unit == "um" || unit == "\u00B5m" || unit == "\u03BCm" )
        {
            aUnits = EDA_UNITS::UM;
        }
        else if( unit == "mm" )
        {
            aUnits = EDA_UNITS::MM;
        }
        else if( unit == "cm" )
        {
            aUnits = EDA_UNITS::CM;
        }
        else if( unit == "mi" || unit == "th" )
        {
            aUnits = EDA_UNITS::MILS;
        }
        else if( unit == "in" || unit == "\"" )
        {
            aUnits = EDA_UNITS::INCH;
        }
        else if( unit == "oz" ) // 1 oz = 1.37 mils
        {
            aUnits = EDA_UNITS::MILS;
            dtmp *= 1.37;
        }
    }
    else if( aUnits == EDA_UNITS::DEGREES )
    {
        if( unit == "ra" ) // Radians
            dtmp *= 180.0f / M_PI;
    }

    switch( aType )
    {
    case EDA_DATA_TYPE::VOLUME:
        dtmp = FromUserUnit( aIuScale, aUnits, dtmp );
        KI_FALLTHROUGH;

    case EDA_DATA_TYPE::AREA:
        dtmp = FromUserUnit( aIuScale, aUnits, dtmp );
        KI_FALLTHROUGH;

    case EDA_DATA_TYPE::DISTANCE:
        dtmp = FromUserUnit( aIuScale, aUnits, dtmp );
        break;

    case EDA_DATA_TYPE::UNITLESS:
        break;
    }

    return dtmp;
}


long long int EDA_UNIT_UTILS::UI::ValueFromString( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                   const QString& aTextValue, EDA_DATA_TYPE aType )
{
    double value = DoubleValueFromString( aIuScale, aUnits, aTextValue, aType );

    return KiROUND<double, long long int>( value );
}


long long int EDA_UNIT_UTILS::UI::ValueFromString( const QString& aTextValue )
{
    double value = DoubleValueFromString( aTextValue );

    return KiROUND<double, long long int>( value );
}
