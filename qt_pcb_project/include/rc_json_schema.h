// QT_TRANSFORMATION_COMPLETED

#ifndef RC_JSON_SCHEMA_H
#define RC_JSON_SCHEMA_H

#include <json_common.h>
#include <QString>
#include <QVector>
#include <json_conversions.h>
namespace RC_JSON
{
struct COORDINATE
{
    double x;
    double y;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( COORDINATE, x, y )

struct AFFECTED_ITEM
{
    QString uuid;
    QString description;
    COORDINATE pos;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( AFFECTED_ITEM, uuid, description, pos )

struct VIOLATION
{
    QString                   type;
    QString                   description;
    QString                   severity;
    QVector<AFFECTED_ITEM> items;
    bool                       excluded;
};

inline void to_json( nlohmann::json& aJson, const VIOLATION& aViolation )
{
    aJson["type"] = aViolation.type;
    aJson["description"] = aViolation.description;
    aJson["severity"] = aViolation.severity;
    aJson["items"] = aViolation.items;

    if( aViolation.excluded )
        aJson["excluded"] = aViolation.excluded;
}
inline void from_json( const nlohmann::json& aJson, VIOLATION& aViolation )
{
    aJson.at( "type" ).get_to( aViolation.type );
    aJson.at( "description" ).get_to( aViolation.description );
    aJson.at( "severity" ).get_to( aViolation.severity );
    aJson.at( "items" ).get_to( aViolation.items );
    aJson.at( "excluded" ).get_to( aViolation.excluded );
}

struct REPORT_BASE
{
    QString $schema;
    QString source;
    QString date;
    QString kicad_version;
    QString type;
    QString coordinate_units;
};

struct DRC_REPORT : REPORT_BASE
{
    DRC_REPORT() { type = QStringLiteral( "drc" ); }

    QVector<VIOLATION>                 violations;
    QVector<VIOLATION>                 unconnected_items;
    QVector<VIOLATION>                 schematic_parity;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( DRC_REPORT, $schema, source, date, kicad_version, violations,
                                    unconnected_items, schematic_parity, coordinate_units )

struct ERC_SHEET
{
    QString               uuid_path;
    QString               path;
    QVector<VIOLATION> violations;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ERC_SHEET, uuid_path, path, violations )

struct ERC_REPORT : REPORT_BASE
{
    ERC_REPORT() { type = QStringLiteral( "erc" ); }

    QVector<ERC_SHEET> sheets;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ERC_REPORT, $schema, source, date, kicad_version, sheets,
                                    coordinate_units )

} // namespace RC_JSON

#endif
