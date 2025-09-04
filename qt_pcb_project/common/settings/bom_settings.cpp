
#include <settings/bom_settings.h>
#include <json_common.h>
#include <core/json_serializers.h>
#include <QString>


// Implementations for BOM_FMT_PRESET
bool BOM_FIELD::operator==( const BOM_FIELD& rhs ) const
{
    return this->name == rhs.name && this->label == rhs.label && this->show == rhs.show
           && this->groupBy == rhs.groupBy;
}


bool operator!=( const BOM_FIELD& lhs, const BOM_FIELD& rhs )
{
    return !( lhs == rhs );
}


bool operator<( const BOM_FIELD& lhs, const BOM_FIELD& rhs )
{
    return lhs.name < rhs.name;
}


void to_json( nlohmann::json& j, const BOM_FIELD& f )
{
    j = nlohmann::json{
        { "name", f.name }, { "label", f.label }, { "show", f.show }, { "group_by", f.groupBy }
    };
}


void from_json( const nlohmann::json& j, BOM_FIELD& f )
{
    j.at( "name" ).get_to( f.name );
    j.at( "label" ).get_to( f.label );
    j.at( "show" ).get_to( f.show );
    j.at( "group_by" ).get_to( f.groupBy );
}


bool operator!=( const BOM_PRESET& lhs, const BOM_PRESET& rhs )
{
    return !( lhs == rhs );
}


bool operator<( const BOM_PRESET& lhs, const BOM_PRESET& rhs )
{
    return lhs.name < rhs.name;
}


void to_json( nlohmann::json& j, const BOM_PRESET& p )
{
    j = nlohmann::json{
        { "name", p.name },
        { "sort_field", p.sortField },
        { "sort_asc", p.sortAsc },
        { "filter_string", p.filterString },
        { "group_symbols", p.groupSymbols },
        { "exclude_dnp", p.excludeDNP },
        { "include_excluded_from_bom", p.includeExcludedFromBOM },
    };

    if( p.fieldsOrdered.size() > 0 )
        j["fields_ordered"] = p.fieldsOrdered;
}


void from_json( const nlohmann::json& j, BOM_PRESET& f )
{
    j.at( "name" ).get_to( f.name );
    j.at( "fields_ordered" ).get_to( f.fieldsOrdered );
    j.at( "sort_field" ).get_to( f.sortField );
    j.at( "sort_asc" ).get_to( f.sortAsc );
    j.at( "filter_string" ).get_to( f.filterString );
    j.at( "group_symbols" ).get_to( f.groupSymbols );
    j.at( "exclude_dnp" ).get_to( f.excludeDNP );

    // Was not present in initial BOM settings in 8.0, so default to false if not found
    f.includeExcludedFromBOM = j.value( "include_excluded_from_bom", false );
}


// Implementations for BOM_PRESET
bool BOM_PRESET::operator==( const BOM_PRESET& rhs ) const
{
    return this->name == rhs.name
        && this->readOnly == rhs.readOnly
        && this->fieldsOrdered == rhs.fieldsOrdered
        && this->sortField == rhs.sortField
        && this->sortAsc == rhs.sortAsc
        && this->filterString == rhs.filterString
        && this->groupSymbols == rhs.groupSymbols
        && this->excludeDNP == rhs.excludeDNP
        && this->includeExcludedFromBOM == rhs.includeExcludedFromBOM;
}


BOM_PRESET BOM_PRESET::DefaultEditing()
{
    BOM_PRESET p{
        "Default Editing", true, {}, "Reference", true, "", true, false, true
    };

    p.fieldsOrdered = std::vector<BOM_FIELD>{
        { "Reference", "Reference", true, false },
        { "${QUANTITY}", "Qty", true, false },
        { "Value", "Value", true, true },
        { "${DNP}", "DNP", true, true },
        { "${EXCLUDE_FROM_BOM}", "Exclude from BOM", true, true },
        { "${EXCLUDE_FROM_BOARD}", "Exclude from Board", true, true },
        { "Footprint", "Footprint", true, true },
        { "Datasheet", "Datasheet", true, false },
    };

    return p;
}


BOM_PRESET BOM_PRESET::GroupedByValue()
{
    BOM_PRESET p{
        "Grouped By Value", true, {}, "Reference", true, "", true, false, false
    };

    p.fieldsOrdered = std::vector<BOM_FIELD>{
        { "Reference", "Reference", true, false },
        { "Value", "Value", true, true },
        { "Datasheet", "Datasheet", true, false },
        { "Footprint", "Footprint", true, false },
        { "${QUANTITY}", "Qty", true, false },
        { "${DNP}", "DNP", true, true },
    };

    return p;
}


BOM_PRESET BOM_PRESET::GroupedByValueFootprint()
{
    BOM_PRESET p{
        "Grouped By Value and Footprint", true, {}, "Reference", true, "",
        true, false, false
    };

    p.fieldsOrdered = std::vector<BOM_FIELD>{
        { "Reference", "Reference", true, false },
        { "Value", "Value", true, true },
        { "Datasheet", "Datasheet", true, false },
        { "Footprint", "Footprint", true, true },
        { "${QUANTITY}", "Qty", true, false },
        { "${DNP}", "DNP", true, true },
    };

    return p;
}


BOM_PRESET BOM_PRESET::Attributes()
{
    BOM_PRESET p{
        "Attributes", true, {}, "Reference", true, "", true, false, true
    };

    p.fieldsOrdered = std::vector<BOM_FIELD>{
        { "Reference", "Reference", true, false },
        { "Value", "Value", true, true },
        { "Datasheet", "Datasheet", false, false },
        { "Footprint", "Footprint", false, true },
        { "${DNP}", "Do Not Place", true, false },
        { "${EXCLUDE_FROM_BOM}", "Exclude from BOM", true, false },
        { "${EXCLUDE_FROM_BOARD}", "Exclude from Board", true, false },
        { "${EXCLUDE_FROM_SIM}", "Exclude from Simulation", true, false },
    };

    return p;
}


std::vector<BOM_PRESET> BOM_PRESET::BuiltInPresets()
{
    return { BOM_PRESET::DefaultEditing(), BOM_PRESET::GroupedByValue(),
             BOM_PRESET::GroupedByValueFootprint(), BOM_PRESET::Attributes() };
}


//Implementations for BOM_FMT_PRESET
bool BOM_FMT_PRESET::operator==( const BOM_FMT_PRESET& rhs ) const
{
    return this->name == rhs.name && this->readOnly == rhs.readOnly
           && this->fieldDelimiter == rhs.fieldDelimiter
           && this->stringDelimiter == rhs.stringDelimiter && this->refDelimiter == rhs.refDelimiter
           && this->refRangeDelimiter == rhs.refRangeDelimiter && this->keepTabs == rhs.keepTabs
           && this->keepLineBreaks == rhs.keepLineBreaks;
}


bool operator!=( const BOM_FMT_PRESET& lhs, const BOM_FMT_PRESET& rhs )
{
    return !( lhs == rhs );
}


bool operator<( const BOM_FMT_PRESET& lhs, const BOM_FMT_PRESET& rhs )
{
    return lhs.name < rhs.name;
}


void to_json( nlohmann::json& j, const BOM_FMT_PRESET& p )
{
    j = nlohmann::json{ { "name", p.name },
                        { "field_delimiter", p.fieldDelimiter },
                        { "string_delimiter", p.stringDelimiter },
                        { "ref_delimiter", p.refDelimiter },
                        { "ref_range_delimiter", p.refRangeDelimiter },
                        { "keep_tabs", p.keepTabs },
                        { "keep_line_breaks", p.keepLineBreaks } };
}


void from_json( const nlohmann::json& j, BOM_FMT_PRESET& f )
{
    j.at( "name" ).get_to( f.name );
    j.at( "field_delimiter" ).get_to( f.fieldDelimiter );
    j.at( "string_delimiter" ).get_to( f.stringDelimiter );
    j.at( "ref_delimiter" ).get_to( f.refDelimiter );
    j.at( "ref_range_delimiter" ).get_to( f.refRangeDelimiter );
    j.at( "keep_tabs" ).get_to( f.keepTabs );
    j.at( "keep_line_breaks" ).get_to( f.keepLineBreaks );
}


BOM_FMT_PRESET BOM_FMT_PRESET::CSV()
{
    return { "CSV", true, ",", "\"", ",", "", false, false };
}


BOM_FMT_PRESET BOM_FMT_PRESET::TSV()
{
    return { "TSV", true, "\t", "", ",", "", false, false };
}


BOM_FMT_PRESET BOM_FMT_PRESET::Semicolons()
{
    return {
        "Semicolons", true, ";", "'", ",", "", false, false
    };
}


std::vector<BOM_FMT_PRESET> BOM_FMT_PRESET::BuiltInPresets()
{
    return { BOM_FMT_PRESET::CSV(), BOM_FMT_PRESET::TSV(), BOM_FMT_PRESET::Semicolons() };
}


#if !defined( __MINGW32__ )
template class KICOMMON_API PARAM_LIST<BOM_PRESET>;
template class KICOMMON_API PARAM_LIST<BOM_FMT_PRESET>;
#endif
