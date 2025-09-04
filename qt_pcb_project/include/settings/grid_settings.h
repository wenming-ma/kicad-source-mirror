// QT_TRANSFORMATION_COMPLETED

#ifndef _GRID_SETTINGS_H
#define _GRID_SETTINGS_H

#include <eda_units.h>
#include <QString>
#include <QVector>
#include <nlohmann/json_fwd.hpp>

class UNITS_PROVIDER;

struct KICOMMON_API GRID
{
    bool operator==( const GRID& aOther ) const;

    QString MessageText( EDA_IU_SCALE aScale, EDA_UNITS aUnits, bool aDisplayUnits = true ) const;

    QString UserUnitsMessageText( UNITS_PROVIDER* aProvider, bool aDisplayUnits = true ) const;

    VECTOR2D ToDouble( EDA_IU_SCALE aScale ) const;

    QString name;
    QString x;
    QString y;
};

KICOMMON_API bool operator!=( const GRID& lhs, const GRID& rhs );
KICOMMON_API bool  operator<( const GRID& lhs, const GRID& rhs );

KICOMMON_API void to_json( nlohmann::json& j, const GRID& g );
KICOMMON_API void  from_json( const nlohmann::json& j, GRID& g );


struct GRID_SETTINGS
{
    bool         axes_enabled;
    QVector<GRID> grids;
    QString      user_grid_x;
    QString      user_grid_y;
    int               last_size_idx;
    int               fast_grid_1;
    int               fast_grid_2;
    double            line_width;
    double            min_spacing;
    bool              show;
    int               style;
    int               snap;
    bool              force_component_snap;
    bool              overrides_enabled;
    bool              override_connected;
    int               override_connected_idx;
    bool              override_wires;
    int               override_wires_idx;
    bool              override_vias;
    int               override_vias_idx;
    bool              override_text;
    int               override_text_idx;
    bool              override_graphics;
    int               override_graphics_idx;
};

#endif
