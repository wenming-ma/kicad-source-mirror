// QT_TRANSFORMATION_COMPLETED

#ifndef EDA_UNITS_H
#define EDA_UNITS_H

#include <kicommon.h>
#include <QString>
#include <geometry/eda_angle.h>
#include <math/vector2d.h>
#include <base_units.h>
#include <core/minoptmax.h>

enum class EDA_DATA_TYPE
{
    DISTANCE = 0,
    AREA     = 1,
    VOLUME   = 2,
    UNITLESS = 3
};

enum class EDA_UNITS
{
    INCH     = 0,       // Do not use IN: it conflicts with a Windows header
    MM       = 1,
    UNSCALED = 2,
    DEGREES  = 3,
    PERCENT  = 4,
    MILS     = 5,
    UM       = 6,
    CM       = 7
};

namespace EDA_UNIT_UTILS
{
    KICOMMON_API bool IsImperialUnit( EDA_UNITS aUnit );

    KICOMMON_API bool IsMetricUnit( EDA_UNITS aUnit );

    KICOMMON_API int Mm2mils( double aVal );

    KICOMMON_API int Mils2mm( double aVal );

    KICOMMON_API bool FetchUnitsFromString( const QString& aTextValue, EDA_UNITS& aUnits );

    KICOMMON_API QString GetText( EDA_UNITS aUnits,
                                   EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

    KICOMMON_API QString GetLabel( EDA_UNITS     aUnits,
                                    EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

    KICOMMON_API std::string FormatAngle( const EDA_ANGLE& aAngle );

    KICOMMON_API std::string FormatInternalUnits( const EDA_IU_SCALE& aIuScale, int aValue );
    KICOMMON_API std::string FormatInternalUnits( const EDA_IU_SCALE& aIuScale,
                                                  const VECTOR2I&     aPoint );

#if 0   // No support for std::from_chars on MacOS yet
    KICOMMON_API bool ParseInternalUnits( const std::string& aInput, const EDA_IU_SCALE& aIuScale,
                                          int& aOut );

    KICOMMON_API bool ParseInternalUnits( const std::string& aInput, const EDA_IU_SCALE& aIuScale,
                                          VECTOR2I& aOut );
#endif

    constexpr inline int Mils2IU( const EDA_IU_SCALE& aIuScale, int mils )
    {
        double x = mils * aIuScale.IU_PER_MILS;
        return int( x < 0 ? x - 0.5 : x + 0.5 );
    }

    namespace UI
    {
        KICOMMON_API double ToUserUnit( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnit,
                                        double aValue );

        KICOMMON_API QString StringFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                               double aValue,
                                               bool aAddUnitsText = false,
                                               EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

        KICOMMON_API QString MessageTextFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                    double aValue, bool aAddUnitsText = true,
                                                    EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

        KICOMMON_API QString MessageTextFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                    int aValue, bool aAddUnitLabel = true,
                                                    EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

        KICOMMON_API QString MessageTextFromValue( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                    long long int aValue, bool aAddUnitLabel = true,
                                                    EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

        KICOMMON_API QString MessageTextFromValue( EDA_ANGLE aValue, bool aAddUnitLabel = true );


        KICOMMON_API QString MessageTextFromMinOptMax( const EDA_IU_SCALE& aIuScale,
                                                        EDA_UNITS aUnits,
                                                        const MINOPTMAX<int>& aValue );

        KICOMMON_API double FromUserUnit( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnit,
                                          double aValue );


        KICOMMON_API double DoubleValueFromString( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                   const QString& aTextValue,
                                                   EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

        KICOMMON_API double DoubleValueFromString( const QString& aTextValue );

        KICOMMON_API long long int ValueFromString( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                                    const QString& aTextValue,
                                                    EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE );

        KICOMMON_API long long int ValueFromString( const QString& aTextValue );
    }
}

#endif
