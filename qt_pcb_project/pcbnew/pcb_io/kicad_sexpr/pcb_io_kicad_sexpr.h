
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCB_IO_KICAD_SEXPR_H_
#define PCB_IO_KICAD_SEXPR_H_

#include <pcb_io/pcb_io.h>
#include <pcb_io/pcb_io_mgr.h>
#include <ctl_flags.h>

#include <richio.h>
#include <string>
#include <layer_ids.h>
#include <lset.h>
#include <boost/ptr_container/ptr_map.hpp>
#include <QString>
#include <QFileInfo>
#include "widgets/report_severity.h"

class BOARD;
class BOARD_ITEM;
class FP_CACHE;
class LSET;
class PCB_IO_KICAD_SEXPR_PARSER;
class NETINFO_MAPPING;
class BOARD_DESIGN_SETTINGS;
class PCB_DIMENSION_BASE;
class PCB_SHAPE;
class PCB_REFERENCE_IMAGE;
class PCB_TARGET;
class PAD;
class PADSTACK;
class PCB_GROUP;
class PCB_GENERATOR;
class PCB_TRACK;
class ZONE;
class PCB_TEXT;
class PCB_TEXTBOX;
class PCB_TABLE;
class EDA_TEXT;
class SHAPE_LINE_CHAIN;
class TEARDROP_PARAMETERS;
class PCB_IO_KICAD_SEXPR;   // forward decl

/// Current s-expression file format version.  2 was the last legacy format version.

//#define SEXPR_BOARD_FILE_VERSION    3         // first s-expression format, used legacy cu stack
//#define SEXPR_BOARD_FILE_VERSION    4         // reversed cu stack, changed Inner* to In* in reverse order
//                                              // went to 32 Cu layers from 16.
//----------------- Start of 5.0 development -----------------
//#define SEXPR_BOARD_FILE_VERSION    20160815  // differential pair settings per net class
//#define SEXPR_BOARD_FILE_VERSION    20170123  // EDA_TEXT refactor, moved 'hide'
//#define SEXPR_BOARD_FILE_VERSION    20170920  // long pad names and custom pad shape
//#define SEXPR_BOARD_FILE_VERSION    20170922  // Keepout zones can exist on multiple layers
//#define SEXPR_BOARD_FILE_VERSION    20171114  // Save 3D model offset in mm, instead of inches
//#define SEXPR_BOARD_FILE_VERSION    20171125  // Locked/unlocked FP_TEXT
//#define SEXPR_BOARD_FILE_VERSION    20171130  // 3D model offset written using "offset" parameter
//----------------- Start of 6.0 development -----------------
//#define SEXPR_BOARD_FILE_VERSION    20190331  // hatched zones and chamfered round rect pads
//#define SEXPR_BOARD_FILE_VERSION    20190421  // curves in custom pads
//#define SEXPR_BOARD_FILE_VERSION    20190516  // Remove segment count from zones
//#define SEXPR_BOARD_FILE_VERSION    20190605  // Add layer defaults
//#define SEXPR_BOARD_FILE_VERSION    20190905  // Add board physical stackup info in setup section
//#define SEXPR_BOARD_FILE_VERSION    20190907  // Keepout areas in footprints
//#define SEXPR_BOARD_FILE_VERSION    20191123  // pin function in pads
//#define SEXPR_BOARD_FILE_VERSION    20200104  // pad property for fabrication
//#define SEXPR_BOARD_FILE_VERSION    20200119  // arcs in tracks
//#define SEXPR_BOARD_FILE_VERSION    20200512  // page -> paper
//#define SEXPR_BOARD_FILE_VERSION    20200518  // save hole_to_hole_min
//#define SEXPR_BOARD_FILE_VERSION    20200614  // Add support for fp_rects and gr_rects
//#define SEXPR_BOARD_FILE_VERSION    20200625  // Multilayer zones, zone names, island controls
//#define SEXPR_BOARD_FILE_VERSION    20200628  // remove visibility settings
//#define SEXPR_BOARD_FILE_VERSION    20200724  // Add KIID to footprints
//#define SEXPR_BOARD_FILE_VERSION    20200807  // Add zone hatch advanced settings
//#define SEXPR_BOARD_FILE_VERSION    20200808  // Add properties to footprints
//#define SEXPR_BOARD_FILE_VERSION    20200809  // Add REMOVE_UNUSED_LAYERS option to vias and THT pads
//#define SEXPR_BOARD_FILE_VERSION    20200811  // Add groups
//#define SEXPR_BOARD_FILE_VERSION    20200818  // Remove Status flag bitmap and setup counts
//#define SEXPR_BOARD_FILE_VERSION    20200819  // Add board-level properties
//#define SEXPR_BOARD_FILE_VERSION    20200825  // Remove host information
//#define SEXPR_BOARD_FILE_VERSION    20200828  // Add new fabrication attributes
//#define SEXPR_BOARD_FILE_VERSION    20200829  // Remove library name from exported footprints
//#define SEXPR_BOARD_FILE_VERSION    20200909  // Change DIMENSION format
//#define SEXPR_BOARD_FILE_VERSION    20200913  // Add leader dimension
//#define SEXPR_BOARD_FILE_VERSION    20200916  // Add center dimension
//#define SEXPR_BOARD_FILE_VERSION    20200921  // Add orthogonal dimension
//#define SEXPR_BOARD_FILE_VERSION    20200922  // Add user name to layer definition.
//#define SEXPR_BOARD_FILE_VERSION    20201002  // Add groups in footprints (for footprint editor).
//#define SEXPR_BOARD_FILE_VERSION    20201114  // Add first-class support for filled shapes.
//#define SEXPR_BOARD_FILE_VERSION    20201115  // module -> footprint and change fill syntax.
//#define SEXPR_BOARD_FILE_VERSION    20201116  // Write version and generator string in footprint files.
//#define SEXPR_BOARD_FILE_VERSION    20201220  // Add free via token
//#define SEXPR_BOARD_FILE_VERSION    20210108  // Pad locking moved from footprint to pads
//#define SEXPR_BOARD_FILE_VERSION    20210126  // Store pintype alongside pinfunction (in pads).
//#define SEXPR_BOARD_FILE_VERSION    20210228  // Move global margins back to board file
//#define SEXPR_BOARD_FILE_VERSION    20210424  // Correct locked flag syntax (remove parens).
//#define SEXPR_BOARD_FILE_VERSION    20210606  // Change overbar syntax from `~...~` to `~{...}`.
//#define SEXPR_BOARD_FILE_VERSION    20210623  // Add support for reading/writing arcs in polygons
//#define SEXPR_BOARD_FILE_VERSION    20210722  // Reading/writing group locked flags
//#define SEXPR_BOARD_FILE_VERSION    20210824  // Opacity in 3D colors
//#define SEXPR_BOARD_FILE_VERSION    20210925  // Locked flag for fp_text
//#define SEXPR_BOARD_FILE_VERSION    20211014  // Arc formatting
//----------------- Start of 7.0 development -----------------
//#define SEXPR_BOARD_FILE_VERSION    20211226  // Add radial dimension
//#define SEXPR_BOARD_FILE_VERSION    20211227  // Add thermal relief spoke angle overrides
//#define SEXPR_BOARD_FILE_VERSION    20211228  // Add allow_soldermask_bridges footprint attribute
//#define SEXPR_BOARD_FILE_VERSION    20211229  // Stroke formatting
//#define SEXPR_BOARD_FILE_VERSION    20211230  // Dimensions in footprints
//#define SEXPR_BOARD_FILE_VERSION    20211231  // Private footprint layers
//#define SEXPR_BOARD_FILE_VERSION    20211232  // Fonts
//#define SEXPR_BOARD_FILE_VERSION    20220131  // Textboxes
//#define SEXPR_BOARD_FILE_VERSION    20220211  // End support for V5 zone fill strategy
//#define SEXPR_BOARD_FILE_VERSION    20220225  // Remove TEDIT
//#define SEXPR_BOARD_FILE_VERSION    20220308  // Knockout text and Locked graphic text property saved
//#define SEXPR_BOARD_FILE_VERSION    20220331  // Plot on all layers selection setting
//#define SEXPR_BOARD_FILE_VERSION    20220417  // Automatic dimension precisions
//#define SEXPR_BOARD_FILE_VERSION    20220427  // Exclude Edge.Cuts & Margin from fp private layers
//#define SEXPR_BOARD_FILE_VERSION    20220609  // Add teardrop keywords to identify teardrop zones
//#define SEXPR_BOARD_FILE_VERSION    20220621  // Add Image support
//#define SEXPR_BOARD_FILE_VERSION    20220815  // Add allow-soldermask-bridges-in-FPs flag
//#define SEXPR_BOARD_FILE_VERSION    20220818  // First-class storage for net-ties
//#define SEXPR_BOARD_FILE_VERSION    20220914  // Number boxes for custom-shape pads
//#define SEXPR_BOARD_FILE_VERSION    20221018  // Via & pad zone-layer-connections
//----------------- Start of 8.0 development -----------------
//#define SEXPR_BOARD_FILE_VERSION    20230410  // DNP attribute propagated from schematic to attr
//#define SEXPR_BOARD_FILE_VERSION    20230517  // Teardrop parameters for pads and vias
//#define SEXPR_BOARD_FILE_VERSION    20230620  // PCB Fields
//#define SEXPR_BOARD_FILE_VERSION    20230730  // Connectivity for graphic shapes
//#define SEXPR_BOARD_FILE_VERSION    20230825  // Textbox explicit border flag
//#define SEXPR_BOARD_FILE_VERSION    20230906  // Multiple image type support in files
//#define SEXPR_BOARD_FILE_VERSION    20230913  // Custom-shaped-pad spoke templates
//#define SEXPR_BOARD_FILE_VERSION    20231007  // Generative objects
//#define SEXPR_BOARD_FILE_VERSION    20231014  // V8 file format normalization
//#define SEXPR_BOARD_FILE_VERSION    20231212  // Reference image locking/UUIDs, footprint boolean format
//#define SEXPR_BOARD_FILE_VERSION    20231231  // Use 'uuid' rather than 'id' for generators and groups
//#define SEXPR_BOARD_FILE_VERSION    20240108  // Convert teardrop parameters to explicit bools
//----------------- Start of 9.0 development -----------------
//#define SEXPR_BOARD_FILE_VERSION    20240201  // Use nullable properties for overrides
//#define SEXPR_BOARD_FILE_VERSION    20240202  // Tables
//#define SEXPR_BOARD_FILE_VERSION    20240225  // Rationalization of solder_paste_margin
//#define SEXPR_BOARD_FILE_VERSION    20240609  // Add 'tenting' keyword
//#define SEXPR_BOARD_FILE_VERSION    20240617  // Table angles
//#define SEXPR_BOARD_FILE_VERSION    20240703  // User layer types
//#define SEXPR_BOARD_FILE_VERSION    20240706  // Embedded Files
//#define SEXPR_BOARD_FILE_VERSION    20240819  // Embedded Files - Update hash algorithm to Murmur3
//#define SEXPR_BOARD_FILE_VERSION    20240928  // Component classes
//#define SEXPR_BOARD_FILE_VERSION    20240929  // Complex padstacks
//#define SEXPR_BOARD_FILE_VERSION    20241006  // Via stacks
//#define SEXPR_BOARD_FILE_VERSION    20241007  // Tracks can have soldermask layer and margin
//#define SEXPR_BOARD_FILE_VERSION    20241009  // Evolve placement rule areas file format
//#define SEXPR_BOARD_FILE_VERSION    20241010  // Graphic shapes can have soldermask layer and margin
//#define SEXPR_BOARD_FILE_VERSION    20241030  // Dimension arrow directions, suppress_zeroes normalization
//#define SEXPR_BOARD_FILE_VERSION    20241129  // Normalise keep_text_aligned and fill properties
//#define SEXPR_BOARD_FILE_VERSION    20241228  // Convert teardrop curve points to bool
#define SEXPR_BOARD_FILE_VERSION      20241229  // Expand User layers to arbitrary count


#define BOARD_FILE_HOST_VERSION       20200825  ///< Earlier files than this include the host tag
#define LEGACY_ARC_FORMATTING         20210925  ///< These were the last to use old arc formatting
#define LEGACY_NET_TIES               20220815  ///< These were the last to use the keywords field
                                                ///<   to indicate a net-tie.
#define FIRST_NORMALIZED_VERISON      20230924  ///< Earlier files did not have normalized bools


// common combinations of the above:

/// Format output for the clipboard instead of footprint library or BOARD
#define CTL_FOR_CLIPBOARD           (CTL_OMIT_INITIAL_COMMENTS) // (CTL_OMIT_NETS)

/// Format output for a footprint library instead of clipboard or BOARD
#define CTL_FOR_LIBRARY \
    ( CTL_OMIT_PAD_NETS | CTL_OMIT_UUIDS | CTL_OMIT_PATH | CTL_OMIT_AT | CTL_OMIT_LIBNAME )

/// The zero arg constructor when PCB_PLUGIN is used for PLUGIN::Load() and PLUGIN::Save()ing
/// a BOARD file underneath IO_MGR.
#define CTL_FOR_BOARD               (CTL_OMIT_INITIAL_COMMENTS|CTL_OMIT_FOOTPRINT_VERSION)

class FP_CACHE_ENTRY
{
    QFileInfo                  m_filename;
    std::unique_ptr<FOOTPRINT> m_footprint;

public:
    FP_CACHE_ENTRY( FOOTPRINT* aFootprint, const QFileInfo& aFileName );

    const QFileInfo& GetFileName() const { return m_filename; }
    void SetFilePath( const QString& aFilePath ) { m_filename.setFile( aFilePath ); }
    std::unique_ptr<FOOTPRINT>& GetFootprint() { return m_footprint; }
};

class FP_CACHE
{
    PCB_IO_KICAD_SEXPR*   m_owner;          // Plugin object that owns the cache.
    QFileInfo             m_lib_path;       // The path of the library.
    QString               m_lib_raw_path;   // For quick comparisons.

    boost::ptr_map<QString, FP_CACHE_ENTRY> m_footprints;  // Map of footprint filename to
                                                           //   cache entry.

    bool      m_cache_dirty;       // Stored separately because it's expensive to check
                                   // m_cache_timestamp against all the files.
    long long m_cache_timestamp;   // A hash of the timestamps for all the footprint
                                   // files.

public:
    FP_CACHE( PCB_IO_KICAD_SEXPR* aOwner, const QString& aLibraryPath );

    QString GetPath() const { return m_lib_raw_path; }

    bool IsWritable() const { return m_lib_path.exists() && m_lib_path.isWritable(); }

    bool Exists() const { return m_lib_path.exists() && m_lib_path.isDir(); }

    boost::ptr_map<QString, FP_CACHE_ENTRY>& GetFootprints() { return m_footprints; }

    // Most all functions in this class throw IO_ERROR exceptions.  There are no
    // error codes nor user interface calls from here, nor in any PLUGIN.
    // Catch these exceptions higher up please.

    void Save( FOOTPRINT* aFootprintFilter = nullptr );

    void Load();

    void Remove( const QString& aFootprintName );

    static long long GetTimestamp( const QString& aLibPath );

    bool IsModified();

    bool IsPath( const QString& aPath ) const;

    void SetPath( const QString& aPath );
};


class PCB_IO_KICAD_SEXPR : public PCB_IO
{
public:
    const IO_BASE::IO_FILE_DESC GetBoardFileDesc() const override
    {
        // Would have used wildcards_and_files_ext.cpp's KiCadPcbFileExtension,
        // but to be pure, a plugin should not assume that it will always be linked
        // with the core of the Pcbnew code. (Might someday be a DLL/DSO.)  Besides,
        // file extension policy should be controlled by the plugin.
        return IO_BASE::IO_FILE_DESC( _HKI( "KiCad printed circuit board files" ), { "kicad_pcb" } );
    }

    const IO_BASE::IO_FILE_DESC GetLibraryFileDesc() const override
    {
        return IO_BASE::IO_FILE_DESC( _HKI( "KiCad footprint file" ), { "kicad_mod" } );
    }

    const IO_BASE::IO_FILE_DESC GetLibraryDesc() const override
    {
        return IO_BASE::IO_FILE_DESC( _HKI( "KiCad footprint files" ), {}, { "kicad_mod" }, false );
    }

    void SetQueryUserCallback( std::function<bool( QString aTitle, int aIcon, QString aMessage,
                                                   QString aOKButtonTitle )> aCallback ) override
    {
        m_queryUserCallback = std::move( aCallback );
    }

    bool CanReadBoard( const QString& aFileName ) const override;

    void SaveBoard( const QString& aFileName, BOARD* aBoard,
                    const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    BOARD* LoadBoard( const QString& aFileName, BOARD* aAppendToMe,
                      const std::map<std::string, UTF8>* aProperties = nullptr,
                      PROJECT* aProject = nullptr ) override;

    BOARD* DoLoad( LINE_READER& aReader, BOARD* aAppendToMe, const std::map<std::string,
                   UTF8>* aProperties, PROGRESS_REPORTER* aProgressReporter, unsigned aLineCount );

    void FootprintEnumerate( QStringList& aFootprintNames, const QString& aLibraryPath,
                             bool aBestEfforts, const std::map<std::string,
                             UTF8>* aProperties = nullptr ) override;

    const FOOTPRINT* GetEnumeratedFootprint( const QString& aLibraryPath,
                                             const QString& aFootprintName,
                                             const std::map<std::string,
                                             UTF8>* aProperties = nullptr ) override;

    bool FootprintExists( const QString& aLibraryPath, const QString& aFootprintName,
                          const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    FOOTPRINT* ImportFootprint( const QString& aFootprintPath, QString& aFootprintNameOut,
                                const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    FOOTPRINT* FootprintLoad( const QString& aLibraryPath, const QString& aFootprintName,
                              bool  aKeepUUID = false,
                              const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    void FootprintSave( const QString& aLibraryPath, const FOOTPRINT* aFootprint,
                        const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    void FootprintDelete( const QString& aLibraryPath, const QString& aFootprintName,
                          const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    long long GetLibraryTimestamp( const QString& aLibraryPath ) const override;

    void CreateLibrary( const QString& aLibraryPath,
                        const std::map<std::string, UTF8>* aProperties = nullptr) override;

    bool DeleteLibrary( const QString& aLibraryPath,
                        const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    bool IsLibraryWritable( const QString& aLibraryPath ) override;

    PCB_IO_KICAD_SEXPR( int aControlFlags = CTL_FOR_BOARD );

    virtual ~PCB_IO_KICAD_SEXPR();

    void Format( const BOARD_ITEM* aItem ) const;

    std::string GetStringOutput( bool doClear )
    {
        std::string ret = m_sf.GetString();

        if( doClear )
            m_sf.Clear();

        return ret;
    }

    void SetOutputFormatter( OUTPUTFORMATTER* aFormatter ) { m_out = aFormatter; }

    BOARD_ITEM* Parse( const QString& aClipboardSourceInput );

protected:
    void validateCache( const QString& aLibraryPath, bool checkModified = true );

    const FOOTPRINT* getFootprint( const QString& aLibraryPath, const QString& aFootprintName,
                                   const std::map<std::string, UTF8>* aProperties,
                                   bool checkModified );

    void init( const std::map<std::string, UTF8>* aProperties );

    void formatSetup( const BOARD* aBoard ) const;

    void formatGeneral( const BOARD* aBoard ) const;

    void formatBoardLayers( const BOARD* aBoard ) const;

    void formatNetInformation( const BOARD* aBoard ) const;

    void formatProperties( const BOARD* aBoard ) const;

    void formatHeader( const BOARD* aBoard ) const;

    void formatTeardropParameters( const TEARDROP_PARAMETERS& tdParams ) const;

private:
    void format( const BOARD* aBoard ) const;

    void format( const PCB_DIMENSION_BASE* aDimension ) const;

    void format( const PCB_REFERENCE_IMAGE* aBitmap ) const;

    void format( const PCB_GROUP* aGroup ) const;

    void format( const PCB_SHAPE* aSegment ) const;

    void format( const PCB_TARGET* aTarget ) const;

    void format( const FOOTPRINT* aFootprint ) const;

    void format( const PAD* aPad ) const;

    void format( const PCB_TEXT* aText ) const;
    void format( const PCB_TEXTBOX* aTextBox ) const;

    void format( const PCB_TABLE* aTable ) const;

    void format( const PCB_GENERATOR* aGenerator ) const;

    void format( const PCB_TRACK* aTrack ) const;

    void format( const ZONE* aZone ) const;

    void formatPolyPts( const SHAPE_LINE_CHAIN& outline,
                        const FOOTPRINT* aParentFP = nullptr ) const;

    void formatRenderCache( const EDA_TEXT* aText ) const;

    void formatLayer( PCB_LAYER_ID aLayer, bool aIsKnockout = false ) const;

    void formatLayers( LSET aLayerMask, bool aEnumerateLayers ) const;

    void formatTenting( const PADSTACK& aPadstack ) const;

    friend class FP_CACHE;

protected:
    QString               m_error;      ///< for throwing exceptions

    FP_CACHE*              m_cache;      ///< Footprint library cache

    LINE_READER*           m_reader;     ///< no ownership
    QString               m_filename;   ///< for saves only, name is in m_reader for loads

    STRING_FORMATTER       m_sf;
    OUTPUTFORMATTER*       m_out;        ///< output any Format()s to this, no ownership
    int                    m_ctl;
    NETINFO_MAPPING*       m_mapping;    ///< mapping for net codes, so only not empty net codes
                                         ///< are stored with consecutive integers as net codes

    std::function<bool( QString aTitle, int aIcon, QString aMsg, QString aAction )> m_queryUserCallback;
};

#endif  // PCB_IO_KICAD_SEXPR_H_
