
#ifndef GRAPHICS_IMPORT_PLUGIN_H
#define GRAPHICS_IMPORT_PLUGIN_H

#include <math/box2.h>
#include <wildcards_and_files_ext.h>
#include <QString>
#include <QByteArray>

class GRAPHICS_IMPORTER;

/**
 * Interface for vector graphics import plugins.
 */
class GRAPHICS_IMPORT_PLUGIN
{
public:
    virtual ~GRAPHICS_IMPORT_PLUGIN() { }

    /**
     * Set the receiver of the imported shapes.
     */
    virtual void SetImporter( GRAPHICS_IMPORTER* aImporter ) { m_importer = aImporter; }

    /**
     * Return the plugin name.
     *
     * This string will be used as the description in the file dialog.
     */
    virtual const QString GetName() const = 0;

    /**
     * Return a vector of the file extensions handled by this plugin.
     */
    virtual const std::vector<std::string> GetFileExtensions() const = 0;

    /**
     * Return a list of wildcards that contains the file extensions
     * handled by this plugin, separated with a semi-colon.
     */
    QString GetWildcards() const
    {
        QString ret;
        bool first = true;

        for( const auto& extension : GetFileExtensions() )
        {
            if( first )
                first = false;
            else
                ret += ";";

            ret += "*." + formatWildcardExt( extension );
        }

        return ret;
    }

    /**
     * Load file for import.
     *
     * It is necessary to have the GRAPHICS_IMPORTER object set before.
     */
    virtual bool Load( const QString& aFileName ) = 0;

    /**
     * Set memory buffer with content for import.
     *
     * It is necessary to have the GRAPHICS_IMPORTER object set before.
     */
    virtual bool LoadFromMemory( const QByteArray& aMemBuffer ) = 0;

    /**
     * Return image height from original imported file.
     *
     * @return Original Image height in mm.
     */
    virtual double GetImageHeight() const = 0;

    /**
     * Return image width from original imported file.
     *
     * @return Original Image width in mm.
     */
    virtual double GetImageWidth() const = 0;

    /**
     * Return image bounding box from original imported file.
     *
     * @return Image bounding box.
     */
    virtual BOX2D GetImageBBox() const = 0;

    /**
     * Actually imports the file.
     *
     * It is necessary to have loaded the file beforehand.
     */
    virtual bool Import() = 0;

    virtual void SetLineWidthMM( double aLineWidth ) {}

    /**
     * Collect warning and error messages after loading/importing.
     *
     * @return the list of messages in one string. Each message ends by '\n'
     */
    const virtual QString& GetMessages() const = 0;

    virtual void ReportMsg( const QString& aMessage ) = 0;

protected:
    /// Importer used to create objects representing the imported shapes.
    GRAPHICS_IMPORTER* m_importer;
};


#endif /* GRAPHICS_IMPORT_PLUGIN_H */
