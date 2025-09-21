
#include "graphics_import_mgr.h"

#include <eda_item.h>
#include "dxf_import_plugin.h"
#include "svg_import_plugin.h"

#include <QRegularExpression>

std::unique_ptr<GRAPHICS_IMPORT_PLUGIN> GRAPHICS_IMPORT_MGR::GetPlugin( GFX_FILE_T aType ) const
{
    std::unique_ptr<GRAPHICS_IMPORT_PLUGIN> ret;

    switch( aType )
    {
    case DXF: ret = std::make_unique<DXF_IMPORT_PLUGIN>();             break;
    case SVG: ret = std::make_unique<SVG_IMPORT_PLUGIN>();             break;
    default:  throw std::runtime_error( "Unhandled graphics format" ); break;
    }

    return ret;
}


std::unique_ptr<GRAPHICS_IMPORT_PLUGIN> GRAPHICS_IMPORT_MGR::GetPluginByExt(
        const QString& aExtension ) const
{
    for( GRAPHICS_IMPORT_MGR::GFX_FILE_T fileType : GetImportableFileTypes() )
    {
        std::unique_ptr<GRAPHICS_IMPORT_PLUGIN> plugin = GetPlugin( fileType );
        const std::vector<std::string>&         fileExtensions = plugin->GetFileExtensions();

        if( compareFileExtensions( aExtension.toStdString(), fileExtensions ) )
            return plugin;
    }

    return {};
}
