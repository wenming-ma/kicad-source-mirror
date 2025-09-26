// wxWidgets to Qt transformation completed
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef SVG_IMPORT_PLUGIN_H
#define SVG_IMPORT_PLUGIN_H

#include "graphics_import_plugin.h"
#include "graphics_importer_buffer.h"
#include <wildcards_and_files_ext.h>

#include <vector>
#include <stroke_params.h>


class SVG_IMPORT_PLUGIN : public GRAPHICS_IMPORT_PLUGIN
{
public:
    SVG_IMPORT_PLUGIN():
        GRAPHICS_IMPORT_PLUGIN(),
        m_parsedImage( nullptr )
    {
    }

    const QString GetName() const override
    {
        return "Scalable Vector Graphics";
    }

    const std::vector<std::string> GetFileExtensions() const override
    {
        static std::vector<std::string> exts = { "svg" };
        return exts;
    }

    /**
     * @return the list of messages in one string. Each message ends by '\n'
     */
    const QString& GetMessages() const override
    {
        return m_messages;
    }

    void ReportMsg( const QString& aMessage ) override;

    bool Import() override;
    bool Load( const QString& aFileName ) override;
    bool LoadFromMemory( const QByteArray& aMemBuffer ) override;

    virtual double GetImageHeight() const override;
    virtual double GetImageWidth() const override;
    virtual BOX2D GetImageBBox() const override;

private:
    void DrawPath( const float* aPoints, int aNumPoints, bool aClosedPath,
                   const IMPORTED_STROKE& aStroke, bool aFilled, const COLOR4D& aFillColor );

    void DrawPolygon( const std::vector<VECTOR2D>& aPoints, const IMPORTED_STROKE& aStroke,
                      bool aFilled, const COLOR4D& aFillColor );

    void DrawLineSegments( const std::vector<VECTOR2D>& aPoints, const IMPORTED_STROKE& aStroke );

    /**
     * Draw a path made up of cubic Bezier curves, adding them as real bezier curves.
     */
    void DrawSplinePath( const float* aPoints, int aNumPoints, const IMPORTED_STROKE& aStroke );

    struct NSVGimage* m_parsedImage;

    QString m_messages; // messages generated during svg file parsing.
                                // Each message ends by '\n'

    GRAPHICS_IMPORTER_BUFFER m_internalImporter;
};

// wxWidgets to Qt transformation completed

#endif /* SVG_IMPORT_PLUGIN_H */
