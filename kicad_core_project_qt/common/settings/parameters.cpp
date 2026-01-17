
#include <map>
#include <vector>
#include <QString>

#include <json_common.h>

#include <project/project_file.h>
#include <settings/parameters.h>

void PARAM_PATH_LIST::Store( JSON_SETTINGS* aSettings ) const
{
    nlohmann::json js = nlohmann::json::array();

    for( const auto& el : *m_ptr )
        js.push_back( toFileFormat( el ) );

    aSettings->Set<nlohmann::json>( m_path, js );
}


bool PARAM_PATH_LIST::MatchesFile( const JSON_SETTINGS& aSettings ) const
{
    if( std::optional<nlohmann::json> js = aSettings.GetJson( m_path ) )
    {
        if( js->is_array() )
        {
            std::vector<QString> val;

            for( const auto& el : js->items() )
                val.emplace_back( fromFileFormat( el.value().get<QString>() ) );

            return val == *m_ptr;
        }
    }

    return false;
}


void PARAM_QSTRING_MAP::Load( const JSON_SETTINGS& aSettings, bool aResetIfMissing ) const
{
    if( m_readOnly )
        return;

    if( std::optional<nlohmann::json> js = aSettings.GetJson( m_path ) )
    {
        if( js->is_object() )
        {
            m_ptr->clear();

            for( const auto& el : js->items() )
                ( *m_ptr )[QString::fromUtf8( el.key().c_str() )] = el.value().get<QString>();
        }
    }
    else if( aResetIfMissing )
    {
        *m_ptr = m_default;
    }
}


void PARAM_QSTRING_MAP::Store( JSON_SETTINGS* aSettings ) const
{
    nlohmann::json js( {} );

    for( const auto& el : *m_ptr )
    {
        std::string key = el.first.toStdString();
        js[key] = el.second;
    }

    aSettings->Set<nlohmann::json>( m_path, js );
}


bool PARAM_QSTRING_MAP::MatchesFile( const JSON_SETTINGS& aSettings ) const
{
    if( std::optional<nlohmann::json> js = aSettings.GetJson( m_path ) )
    {
        if( js->is_object() )
        {
            if( m_ptr->size() != js->size() )
                return false;

            std::map<QString, QString> val;

            for( const auto& el : js->items() )
            {
                QString key = QString::fromUtf8( el.key().c_str() );
                val[key] = el.value().get<QString>();
            }

            return val == *m_ptr;
        }
    }

    return false;
}

#if !defined( __MINGW32__ )
// Instantiate all required templates here and export
template class KICOMMON_API PARAM_LAMBDA<bool>;
template class KICOMMON_API PARAM_LAMBDA<int>;
template class KICOMMON_API PARAM_LAMBDA<nlohmann::json>;
template class KICOMMON_API PARAM_LAMBDA<std::string>;

template class KICOMMON_API PARAM_LIST<bool>;
template class KICOMMON_API PARAM_LIST<int>;
template class KICOMMON_API PARAM_LIST<double>;
template class KICOMMON_API PARAM_LIST<QString>;
template class KICOMMON_API PARAM_LIST<KIGFX::COLOR4D>;
//template KICOMMON_API class PARAM_LIST<FILE_INFO_PAIR>;
template class KICOMMON_API PARAM_LIST<GRID>;

template class KICOMMON_API PARAM_SET<QString>;

template class KICOMMON_API PARAM_MAP<int>;
template class KICOMMON_API PARAM_MAP<double>;
template class KICOMMON_API PARAM_MAP<bool>;
#endif