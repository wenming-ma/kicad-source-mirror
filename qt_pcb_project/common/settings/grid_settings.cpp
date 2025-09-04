
#include <settings/grid_settings.h>
#include <json_common.h>
#include <QString>
#include <core/json_serializers.h>

#include <units_provider.h>

QString GRID::MessageText( EDA_IU_SCALE aScale, EDA_UNITS aUnits, bool aDisplayUnits ) const
{
    EDA_DATA_TYPE type = EDA_DATA_TYPE::DISTANCE;

    QString xStr = EDA_UNIT_UTILS::UI::MessageTextFromValue(
                        aScale, aUnits,
                        EDA_UNIT_UTILS::UI::DoubleValueFromString( aScale, EDA_UNITS::MM, x, type ),
                        aDisplayUnits );
    QString yStr = EDA_UNIT_UTILS::UI::MessageTextFromValue(
                        aScale, aUnits,
                        EDA_UNIT_UTILS::UI::DoubleValueFromString( aScale, EDA_UNITS::MM, y, type ),
                        aDisplayUnits );

    if( xStr == yStr )
        return xStr;

    return QString( "%1 x %2" ).arg( xStr, yStr );
}

QString GRID::UserUnitsMessageText( UNITS_PROVIDER* aProvider, bool aDisplayUnits ) const
{
    return MessageText( aProvider->GetIuScale(), aProvider->GetUserUnits(), aDisplayUnits );
}


VECTOR2D GRID::ToDouble( EDA_IU_SCALE aScale ) const
{
    return VECTOR2D( EDA_UNIT_UTILS::UI::DoubleValueFromString( aScale, EDA_UNITS::MM, x ),
                     EDA_UNIT_UTILS::UI::DoubleValueFromString( aScale, EDA_UNITS::MM, y ) );
}


bool GRID::operator==( const GRID& aOther ) const
{
    return x == aOther.x && y == aOther.y && name == aOther.name;
}


bool operator!=( const GRID& lhs, const GRID& rhs )
{
    return !( lhs == rhs );
}


bool operator<( const GRID& lhs, const GRID& rhs )
{
    return lhs.name < rhs.name;
}


void to_json( nlohmann::json& j, const GRID& g )
{
    j = nlohmann::json{
        { "name", g.name },
        { "x", g.x },
        { "y", g.y },
    };
}


void from_json( const nlohmann::json& j, GRID& g )
{
    j.at( "name" ).get_to( g.name );
    j.at( "x" ).get_to( g.x );
    j.at( "y" ).get_to( g.y );
}
