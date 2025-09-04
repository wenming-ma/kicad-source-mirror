// QT_TRANSFORMATION_COMPLETED

#ifndef _BOM_SETTINGS_H
#define _BOM_SETTINGS_H

#include <QString>
#include <vector>
#include <settings/json_settings.h>
#include <settings/parameters.h>
#include <i18n_utility.h>

// A single field within a BOM, e.g. Reference, Value, Footprint
struct KICOMMON_API BOM_FIELD
{
    QString name;
    QString label;
    bool    show = false;
    bool    groupBy = false;

    bool operator==( const BOM_FIELD& rhs ) const;
};

KICOMMON_API bool operator!=( const BOM_FIELD& lhs, const BOM_FIELD& rhs );
KICOMMON_API bool operator<( const BOM_FIELD& lhs, const BOM_FIELD& rhs );

KICOMMON_API void to_json( nlohmann::json& j, const BOM_FIELD& f );
KICOMMON_API void from_json( const nlohmann::json& j, BOM_FIELD& f );


// A complete preset defining a BOM "View" with a list of all the fields to show,
// group by, order, filtering settings, etc.
struct KICOMMON_API BOM_PRESET
{
    QString            name;
    bool               readOnly = false;
    std::vector<BOM_FIELD> fieldsOrdered;
    QString            sortField;
    bool               sortAsc = true;
    QString            filterString;
    bool               groupSymbols = false;
    bool               excludeDNP = false;
    bool               includeExcludedFromBOM = false;

    bool operator==( const BOM_PRESET& rhs ) const;

    static BOM_PRESET DefaultEditing();
    static BOM_PRESET GroupedByValue();
    static BOM_PRESET GroupedByValueFootprint();
    static BOM_PRESET Attributes();

    static std::vector<BOM_PRESET> BuiltInPresets();
};

KICOMMON_API bool operator!=( const BOM_PRESET& lhs, const BOM_PRESET& rhs );
KICOMMON_API bool operator<( const BOM_PRESET& lhs, const BOM_PRESET& rhs );

KICOMMON_API void to_json( nlohmann::json& j, const BOM_PRESET& f );
KICOMMON_API void from_json( const nlohmann::json& j, BOM_PRESET& f );


// A formatting preset, like CSV (Comma Separated Values)
struct KICOMMON_API BOM_FMT_PRESET
{
    QString name;
    bool    readOnly = false;
    QString fieldDelimiter;
    QString stringDelimiter;
    QString refDelimiter;
    QString refRangeDelimiter;
    bool    keepTabs = false;
    bool    keepLineBreaks = false;

    bool operator==( const BOM_FMT_PRESET& rhs ) const;

    static BOM_FMT_PRESET CSV();
    static BOM_FMT_PRESET TSV();
    static BOM_FMT_PRESET Semicolons();

    static std::vector<BOM_FMT_PRESET> BuiltInPresets();
};

KICOMMON_API bool operator!=( const BOM_FMT_PRESET& lhs, const BOM_FMT_PRESET& rhs );
KICOMMON_API bool operator<( const BOM_FMT_PRESET& lhs, const BOM_FMT_PRESET& rhs );

KICOMMON_API void to_json( nlohmann::json& j, const BOM_FMT_PRESET& f );
KICOMMON_API void from_json( const nlohmann::json& j, BOM_FMT_PRESET& f );

#if defined( __MINGW32__ )
template class KICOMMON_API PARAM_LIST<struct BOM_PRESET>;
template class KICOMMON_API PARAM_LIST<struct BOM_FMT_PRESET>;
#else
extern template class APIVISIBLE PARAM_LIST<BOM_PRESET>;
extern template class APIVISIBLE PARAM_LIST<BOM_FMT_PRESET>;
#endif

#endif
