// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#pragma once

#ifndef _PCBNEW_PARSER_H_
#define _PCBNEW_PARSER_H_

#include <QString>
#include <QStringList>
#include <QHash>
#include <hashtables.h>
#include <layer_ids.h>     // PCB_LAYER_ID
#include <lset.h>
#include <pcb_lexer.h>
#include <kiid.h>
#include <math/box2.h>
#include <string_any_map.h>

#include <chrono>
#include <unordered_map>


class PCB_ARC;
class BOARD;
class BOARD_ITEM;
class BOARD_ITEM_CONTAINER;
class PAD;
class BOARD_DESIGN_SETTINGS;
class PCB_DIMENSION_BASE;
class PCB_SHAPE;
class PCB_REFERENCE_IMAGE;
class EDA_TEXT;
class PCB_TEXT;
class PCB_TRACK;
class PCB_TABLE;
class PCB_TABLECELL;
class FOOTPRINT;
class PCB_GROUP;
class PCB_TARGET;
class PCB_VIA;
class ZONE;
class FP_3DMODEL;
class SHAPE_LINE_CHAIN;
struct LAYER;
class PROGRESS_REPORTER;
class TEARDROP_PARAMETERS;


// Read a Pcbnew s-expression formatted LINE_READER object and returns the appropriate BOARD_ITEM object.
class PCB_IO_KICAD_SEXPR_PARSER : public PCB_LEXER
{
public:

    typedef std::unordered_map< std::string, PCB_LAYER_ID > LAYER_ID_MAP;
    typedef std::unordered_map< std::string, LSET >         LSET_MAP;
    typedef std::unordered_map< QString, KIID >             KIID_MAP;

    PCB_IO_KICAD_SEXPR_PARSER( LINE_READER* aReader, BOARD* aAppendToMe,
                std::function<bool( QString, int, QString, QString )> aQueryUserCallback,
                PROGRESS_REPORTER* aProgressReporter = nullptr, unsigned aLineCount = 0 ) :
        PCB_LEXER( aReader ),
        m_board( aAppendToMe ),
        m_appendToExisting( aAppendToMe != nullptr ),
        m_progressReporter( aProgressReporter ),
        m_lastProgressTime( std::chrono::steady_clock::now() ),
        m_lineCount( aLineCount ),
        m_queryUserCallback( std::move( aQueryUserCallback ) )
    {
        init();
    }

    // ~PCB_IO_KICAD_SEXPR_PARSER() {}

    BOARD_ITEM* Parse();

    // Parse a footprint. aInitialComments may be nullptr or a pointer to a heap allocated comment block.
    FOOTPRINT* parseFOOTPRINT( QStringList* aInitialComments = nullptr );

    // Return whether a version number, if any was parsed, was too recent
    bool IsTooRecent()
    {
        return m_tooRecent;
    }

    // Return a string representing the version of KiCad required to open this file.
    QString GetRequiredVersion();

    // Partially parse the input and check if it matches expected header
    bool IsValidBoardHeader();

private:

    // Group membership info refers to other Uuids in the file.
    // We don't want to rely on group declarations being last in the file, so
    // we store info about the group declarations here during parsing and then resolve
    // them into BOARD_ITEM* after we've parsed the rest of the file.
    struct GROUP_INFO
    {
        virtual ~GROUP_INFO() = default; // Make polymorphic

        BOARD_ITEM*       parent;
        QString           name;
        bool              locked;
        KIID              uuid;
        std::vector<KIID> memberUuids;
    };

    struct GENERATOR_INFO : GROUP_INFO
    {
        PCB_LAYER_ID   layer;
        QString        genType;
        STRING_ANY_MAP properties;
    };

    ///< Convert net code using the mapping table if available,
    ///< otherwise returns unchanged net code if < 0 or if it's out of range
    inline int getNetCode( int aNetCode )
    {
        if( ( aNetCode >= 0 ) && ( aNetCode < (int) m_netCodes.size() ) )
            return m_netCodes[aNetCode];

        return aNetCode;
    }

    // Add aValue value in netcode mapping (m_netCodes) at aIndex.
    void pushValueIntoMap( int aIndex, int aValue );

    // Clear and re-establish m_layerMap with the default layer names.
    void init();

    void checkpoint();

    // Create a mapping from the (short-lived) bug where layer names were translated.
    void createOldLayerMapping( std::unordered_map< std::string, std::string >& aMap );

    // Skip the current token level, i.e search for the RIGHT parenthesis which closes the current description.
    void skipCurrent();

    void parseHeader();
    void parseGeneralSection();
    void parsePAGE_INFO();
    void parseTITLE_BLOCK();

    void parseLayers();
    void parseLayer( LAYER* aLayer );

    void parseBoardStackup();

    void parseSetup();
    void parseDefaults( BOARD_DESIGN_SETTINGS& aSettings );
    void parseDefaultTextDims( BOARD_DESIGN_SETTINGS& aSettings, int aLayer );
    void parseNETINFO_ITEM();
    void parseNETCLASS();

    void parseTEARDROP_PARAMETERS( TEARDROP_PARAMETERS* tdParams );

    void parseTextBoxContent( PCB_TEXTBOX* aTextBox );

    PCB_SHAPE*           parsePCB_SHAPE( BOARD_ITEM* aParent );
    PCB_TEXT*            parsePCB_TEXT( BOARD_ITEM* aParent, PCB_TEXT* aBaseText = nullptr );
    void                 parsePCB_TEXT_effects( PCB_TEXT* aText, PCB_TEXT* aBaseText = nullptr );
    PCB_REFERENCE_IMAGE* parsePCB_REFERENCE_IMAGE( BOARD_ITEM* aParent );
    PCB_TEXTBOX*         parsePCB_TEXTBOX( BOARD_ITEM* aParent );
    PCB_TABLECELL*       parsePCB_TABLECELL( BOARD_ITEM* aParent );
    PCB_TABLE*           parsePCB_TABLE( BOARD_ITEM* aParent );
    PCB_DIMENSION_BASE*  parseDIMENSION( BOARD_ITEM* aParent );

    // Parse a footprint, but do not replace PARSE_ERROR with FUTURE_FORMAT_ERROR automatically.
    FOOTPRINT*  parseFOOTPRINT_unchecked( QStringList* aInitialComments = nullptr );

    PAD*        parsePAD( FOOTPRINT* aParent = nullptr );

    // Parse only the (option ...) inside a pad description
    bool        parsePAD_option( PAD* aPad );
    void        parsePadstack( PAD* aPad );

    PCB_ARC*    parseARC();
    PCB_TRACK*  parsePCB_TRACK();
    PCB_VIA*    parsePCB_VIA();
    void        parseViastack( PCB_VIA* aVia );
    ZONE*       parseZONE( BOARD_ITEM_CONTAINER* aParent );
    PCB_TARGET* parsePCB_TARGET();
    BOARD*      parseBOARD();
    void        parseGROUP_members( GROUP_INFO& aGroupInfo );
    void        parseGROUP( BOARD_ITEM* aParent );
    void        parseGENERATOR( BOARD_ITEM* aParent );

    // Parse a board, but do not replace PARSE_ERROR with FUTURE_FORMAT_ERROR automatically.
    BOARD*      parseBOARD_unchecked();

    // Parse the current token for the layer definition of a BOARD_ITEM object.
    PCB_LAYER_ID lookUpLayer( const LAYER_ID_MAP& aMap );
    LSET lookUpLayerSet( const LSET_MAP& aMap );

    // Parse the layer definition of a BOARD_ITEM object.
    PCB_LAYER_ID parseBoardItemLayer();

    // Parse the layers definition of a BOARD_ITEM object.
    LSET parseBoardItemLayersAsMask();

     // Parse the layers definition of a BOARD_ITEM object that has a single copper layer and optional soldermask layer.
    LSET parseLayersForCuItemWithSoldermask();

    // Parse a coordinate pair (xy X Y) in board units (mm).
    VECTOR2I parseXY();

    void parseXY( int* aX, int* aY );

    void parseMargins( int& aLeft, int& aTop, int& aRight, int& aBottom );

    std::pair<QString, QString> parseBoardProperty();

    // Parse possible outline points and store them into polygon.
    void parseOutlinePoints( SHAPE_LINE_CHAIN& aPoly );

    // Parse the common settings for any object derived from EDA_TEXT.
    void parseEDA_TEXT( EDA_TEXT* aText );

    // Parse the render cache for any object derived from EDA_TEXT.
    void parseRenderCache( EDA_TEXT* text );

    void parseTenting( PADSTACK& aPadstack );

    FP_3DMODEL* parse3DModel();

    // Parse the current token as an ASCII numeric string into a board unit value.
    int parseBoardUnits();

    int parseBoardUnits( const char* aExpected );

    inline int parseBoardUnits( PCB_KEYS_T::T aToken )
    {
        return parseBoardUnits( GetTokenText( aToken ) );
    }

    inline int parseInt()
    {
        return (int)strtol( CurText(), nullptr, 10 );
    }

    inline int parseInt( const char* aExpected )
    {
        NeedNUMBER( aExpected );
        return parseInt();
    }

    inline long parseHex()
    {
        NextTok();
        return strtol( CurText(), nullptr, 16 );
    }

    bool parseBool();

    // Parse a boolean flag inside a list that existed before boolean normalization.
    bool parseMaybeAbsentBool( bool aDefaultValue );

    /*
     * @return if m_appendToExisting, returns new KIID(), otherwise returns CurStr() as KIID.
     */
    KIID CurStrToKIID();

    // Called after parsing a footprint definition or board to build the group membership lists.
    void resolveGroups( BOARD_ITEM* aParent );

    ///< The type of progress bar timeout
    using TIMEOUT = std::chrono::milliseconds;

    ///< The clock used for the timestamp (guaranteed to be monotonic).
    using CLOCK = std::chrono::steady_clock;

    ///< The type of the time stamps.
    using TIME_PT = std::chrono::time_point<CLOCK>;

    BOARD*              m_board;
    LAYER_ID_MAP        m_layerIndices;     // map layer name to it's index
    LSET_MAP            m_layerMasks;       // map layer names to their masks
    std::set<QString>   m_undefinedLayers;  // set of layers not defined in layers section
    std::vector<int>    m_netCodes;         // net codes mapping for boards being loaded
    bool                m_tooRecent;        // true if version parses as later than supported
    int                 m_requiredVersion;  // set to the KiCad format version this board requires
    QString             m_generatorVersion; // Set to the generator version this board requires
    bool                m_appendToExisting; // reading into an existing board; reset UUIDs

    // if resetting UUIDs, record new ones to update groups with.
    KIID_MAP            m_resetKIIDMap;

    bool                m_showLegacySegmentZoneWarning;
    bool                m_showLegacy5ZoneWarning;

    PROGRESS_REPORTER*  m_progressReporter;  // optional; may be nullptr
    TIME_PT             m_lastProgressTime;  // for progress reporting
    unsigned            m_lineCount;         // for progress reporting

    std::vector<GROUP_INFO>     m_groupInfos;
    std::vector<GENERATOR_INFO> m_generatorInfos;

    std::function<bool( QString aTitle, int aIcon, QString aMsg, QString aAction )> m_queryUserCallback;
};


#endif    // _PCBNEW_PARSER_H_
