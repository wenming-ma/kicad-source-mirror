
#ifndef KICAD_DATABASE_LIB_SETTINGS_H
#define KICAD_DATABASE_LIB_SETTINGS_H

#include <settings/json_settings.h>
#include <QString>


enum class DATABASE_SOURCE_TYPE
{
    ODBC,
    INVALID
};


struct KICOMMON_API DATABASE_SOURCE
{
    DATABASE_SOURCE_TYPE type;
    std::string          dsn;
    std::string          username;
    std::string          password;
    int                  timeout;
    std::string          connection_string;
};


struct KICOMMON_API DATABASE_FIELD_MAPPING
{
    std::string column;             // Database column name
    std::string name;               // KiCad field name
    QString     name_wx;            // KiCad field name (converted)
    bool        visible_on_add;     // Whether to show the field when placing the symbol
    bool        visible_in_chooser; // Whether the column is shown by default in the chooser
    bool        show_name;   // Whether or not to show the field name as well as its value
    bool        inherit_properties; // Whether or not to inherit properties from symbol field

    explicit DATABASE_FIELD_MAPPING( std::string aColumn, std::string aName, bool aVisibleOnAdd,
                                     bool aVisibleInChooser, bool aShowName,
                                     bool aInheritProperties );
};


struct KICOMMON_API MAPPABLE_SYMBOL_PROPERTIES
{
    std::string description;
    std::string footprint_filters;
    std::string keywords;
    std::string exclude_from_sim;
    std::string exclude_from_bom;
    std::string exclude_from_board;
};


struct KICOMMON_API DATABASE_LIB_TABLE
{
    std::string name;              // KiCad library nickname (will form part of the LIB_ID)
    std::string table;             // Database table to pull content from
    std::string key_col;           // Unique key column name (will form part of the LIB_ID)
    std::string symbols_col;       // Column name containing KiCad symbol refs
    std::string footprints_col;    // Column name containing KiCad footprint refs

    MAPPABLE_SYMBOL_PROPERTIES properties;
    std::vector<DATABASE_FIELD_MAPPING> fields;
};


struct KICOMMON_API DATABASE_CACHE_SETTINGS
{
    int max_size;    // Maximum number of single-row results to cache
    int max_age;     // Max age of cached rows before they expire, in seconds
};


class KICOMMON_API DATABASE_LIB_SETTINGS : public JSON_SETTINGS
{
public:
    DATABASE_LIB_SETTINGS( const std::string& aFilename );

    virtual ~DATABASE_LIB_SETTINGS() {}

    DATABASE_SOURCE m_Source;

    std::vector<DATABASE_LIB_TABLE> m_Tables;

    DATABASE_CACHE_SETTINGS m_Cache;

protected:
    QString getFileExt() const override;
};

#endif //KICAD_DATABASE_LIB_SETTINGS_H
