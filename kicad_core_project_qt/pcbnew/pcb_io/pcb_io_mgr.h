
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCB_IO_MGR_H_
#define PCB_IO_MGR_H_

#include <cstdint>
#include <config.h>
#include <vector>
#include <QString>
#include <i18n_utility.h>
#include <io/io_base.h>
#include <io/io_mgr.h>
#include <pcb_io/pcb_io.h>

class BOARD;
class PCB_IO;
class FOOTPRINT;
class PROJECT;
class PROGRESS_REPORTER;
class REPORTER;

/**
 * A factory which returns an instance of a #PLUGIN.
 */
class PCB_IO_MGR : public IO_MGR
{
public:

    /**
     * The set of file types that the PCB_IO_MGR knows about, and for which there has been a
     * plugin written, in alphabetical order.
     */
    enum PCB_FILE_T
    {
        PCB_FILE_UNKNOWN = 0,   ///< 0 is not a legal menu id on Mac
        KICAD_SEXP,             ///< S-expression Pcbnew file format.
        LEGACY,                 ///< Legacy Pcbnew file formats prior to s-expression.
        ALTIUM_CIRCUIT_MAKER,
        ALTIUM_CIRCUIT_STUDIO,
        ALTIUM_DESIGNER,
        CADSTAR_PCB_ARCHIVE,
        EAGLE,
        EASYEDA,
        EASYEDAPRO,
        FABMASTER,
        GEDA_PCB, ///< Geda PCB file formats.
        PCAD,
        SOLIDWORKS_PCB,
        IPC2581,
        ODBPP,
        // add your type here.

        // etc.

        FILE_TYPE_NONE
    };

    /**
     * Hold a list of available plugins, created using a singleton REGISTER_PLUGIN object.
     * This way, plugins can be added link-time.
     */
    class PLUGIN_REGISTRY
    {
        public:
            struct ENTRY
            {
                PCB_FILE_T m_type;
                std::function<PCB_IO*(void)> m_createFunc;
                QString m_name;
            };

            static PLUGIN_REGISTRY *Instance()
            {
                static PLUGIN_REGISTRY *self = nullptr;

                if( !self )
                {
                    self = new PLUGIN_REGISTRY;
                }
                return self;
            }

            void Register( PCB_FILE_T aType, const QString& aName,
                           std::function<PCB_IO*(void)> aCreateFunc )
            {
                ENTRY ent;
                ent.m_type = aType;
                ent.m_createFunc = aCreateFunc;
                ent.m_name = aName;
                m_plugins.push_back( ent );
            }

            PCB_IO* Create( PCB_FILE_T aFileType ) const
            {
                for( auto& ent : m_plugins )
                {
                    if ( ent.m_type == aFileType )
                    {
                        return ent.m_createFunc();
                    }
                }

                return nullptr;
            }

            const std::vector<ENTRY>& AllPlugins() const
            {
                return m_plugins;
            }

        private:
            std::vector<ENTRY> m_plugins;
    };

    /**
     * Register a plugin.
     *
     * Declare as a static variable in an anonymous namespace.
     *
     * @param aType type of the plugin
     * @param aName name of the file format
     * @param aCreateFunc function that creates a new object for the plugin.
     */
    struct REGISTER_PLUGIN
    {
         REGISTER_PLUGIN( PCB_FILE_T aType, const QString& aName,
                          std::function<PCB_IO*(void)> aCreateFunc )
         {
             PLUGIN_REGISTRY::Instance()->Register( aType, aName, aCreateFunc );
         }
    };


    /**
     * Return a #PLUGIN which the caller can use to import, export, save, or load
     * design documents.
     *
     * @note The caller owns the returned object.
     *
     * @param aFileType is from #PCB_FILE_T and tells which plugin to find.
     * @return the plug in corresponding to \a aFileType or NULL if not found.
     */
    static PCB_IO* PluginFind( PCB_FILE_T aFileType );

    /**
     * Return a brief name for a plugin given \a aFileType enum.
     */
    static const QString ShowType( PCB_FILE_T aFileType );

    /**
     * Return the #PCB_FILE_T from the corresponding plugin type name: "kicad", "legacy", etc.
     */
    static PCB_FILE_T EnumFromStr( const QString& aFileType );

    /**
     * Return a plugin type given a path for a board file. FILE_TYPE_NONE if the file is not known.
     */
    static PCB_FILE_T FindPluginTypeFromBoardPath( const QString& aFileName, int aCtl = 0 );

    /**
     * Return a plugin type given a footprint library's libPath.
     */
    static PCB_FILE_T GuessPluginTypeFromLibPath( const QString& aLibPath, int aCtl = 0 );

    /**
     * Find the requested #PLUGIN and if found, calls the #PLUGIN::LoadBoard() function
     * on it using the arguments passed to this function.  After the #PLUGIN::LoadBoard()
     * function returns, the #PLUGIN is Released() as part of this call.
     *
     * @param aFileType is the #PCB_FILE_T of file to load.
     * @param aFileName is the name of the file to load.
     * @param aAppendToMe is an existing BOARD to append to, use NULL if fresh
     *                    board load is wanted.
     * @param aProperties is an associative array that allows the caller to
     *                    pass additional tuning parameters to the PLUGIN.
     * @param aProject is the optional #PROJECT object primarily used by third party
     *                 importers.
     * @return the loaded #BOARD object.  The  caller owns it an it will never NULL because
     *         exception thrown if error.
     *
     * @throw IO_ERROR if the #PLUGIN cannot be found, file cannot be found, or file cannot
     *                 be loaded.
     */
    static BOARD* Load( PCB_FILE_T aFileType, const QString& aFileName,
                        BOARD* aAppendToMe = nullptr, const std::map<std::string, UTF8>* aProperties = nullptr,
                        PROJECT* aProject = nullptr,
                        PROGRESS_REPORTER* aProgressReporter = nullptr );

    /**
     * Write either a full \a aBoard to a storage file in a format that this implementation
     * knows about, or it can be used to write a portion of\a aBoard to a special kind of
     * export file.
     *
     * @param aFileType is the #PCB_FILE_T of file to save.
     * @param aFileName is the name of a file to save to on disk.
     * @param aBoard is the #BOARD document (data tree) to save or export to disk.
     * @param aBoard is the in memory document tree from which to extract information when
     *               writing to \a aFileName.  The caller continues to own the #BOARD, and
     *               the plugin should refrain from modifying the #BOARD if possible.
     * @param aProperties is an associative array that can be used to tell the saver how to
     *                    save the file, because it can take any number of additional named
     *                    tuning arguments that the plugin is known to support.  The caller
     *                    continues to own this object (plugin may not delete it), and plugins
     *                     should expect it to be optionally NULL.
     *
     * @throw IO_ERROR if there is a problem saving or exporting.
     */
    static void Save( PCB_FILE_T aFileType, const QString& aFileName, BOARD* aBoard,
                      const std::map<std::string, UTF8>* aProperties = nullptr );

    /**
     * Convert a schematic symbol library to the latest KiCad format
     */
    static bool ConvertLibrary( std::map<std::string, UTF8>* aOldFileProps, const QString& aOldFilePath,
                                const QString& aNewFilePath, REPORTER* aReporter );
};

#endif // PCB_IO_MGR_H_
