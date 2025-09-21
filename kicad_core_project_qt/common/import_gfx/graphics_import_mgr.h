
#ifndef GRAPHICS_IMPORT_MGR_H
#define GRAPHICS_IMPORT_MGR_H

#include <memory>
#include <vector>
#include <QString>

class GRAPHICS_IMPORT_PLUGIN;

/**
 * Manage vector graphics importers.
 */
class GRAPHICS_IMPORT_MGR
{
public:
    /// List of handled file types.
    enum GFX_FILE_T
    {
        DXF,
        SVG
    };

    /// Vector containing all GFX_FILE_T values that can be imported.
    std::vector<GFX_FILE_T> GetImportableFileTypes() const
    {
        return { DXF, SVG };
    }

    /// Return a plugin that handles a specific file extension.
    std::unique_ptr<GRAPHICS_IMPORT_PLUGIN> GetPluginByExt( const QString& aExtension ) const;

    /// Return a plugin instance for a specific file type.
    std::unique_ptr<GRAPHICS_IMPORT_PLUGIN> GetPlugin( GFX_FILE_T aType ) const;
};

#endif /* GRAPHICS_IMPORT_MGR_H */
