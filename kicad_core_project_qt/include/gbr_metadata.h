
// Handle special data (items attributes) during plot.
// Used in Gerber plotter to generate auxiliary data during plot

#ifndef GBR_METADATA_H
#define GBR_METADATA_H

#include <gbr_netlist_metadata.h>

/**
 * Create a gerber TF.CreationDate attribute.
 *
 * The attribute value must conform to the full version of the ISO 8601 date and time format,
 * including time and time zone.
 *
 * Example of structured comment (compatible X1 gerber)
 *  G04 #@! TF.CreationDate,2018-11-21T08:49:16+01:00* (example of X1 attribute)
 *
 * Example NC drill files
 *  ; #@! TF.CreationDate,2018-11-21T08:49:16+01:00*    (example of NC drill comment)
 *
 * Example of X2 attribute:
 *  %TF.CreationDate,2018-11-06T08:25:24+01:00*%
 *
 * @note This is the date the Gerber file is effectively created, not the time the project
 *       PCB was started.
 *
 * @param aFormat string compatibility: X1, X2, GBRJOB or NC drill syntax.
 */
enum GBR_NC_STRING_FORMAT       // Options for string format in some attribute strings
{
    GBR_NC_STRING_FORMAT_X1,
    GBR_NC_STRING_FORMAT_X2,
    GBR_NC_STRING_FORMAT_GBRJOB,
    GBR_NC_STRING_FORMAT_NCDRILL
};

QString GbrMakeCreationDateAttributeString( GBR_NC_STRING_FORMAT aFormat );


// Build a project GUID using format RFC4122 Version 1 or 4 from the project name
QString GbrMakeProjectGUIDfromString( const QString& aText );


// this class handle info which can be added in a gerber file as attribute
// of an aperture, by the %TA.AperFunction command
// This attribute is added when creating a new aperture (command %ADDxx)
// Only one aperture attribute can be added to a given aperture
//
class GBR_APERTURE_METADATA
{
public:
    enum GBR_APERTURE_ATTRIB
    {
        GBR_APERTURE_ATTRIB_NONE,           ///< uninitialized attribute.
        GBR_APERTURE_ATTRIB_ETCHEDCMP,      ///< Aperture used for etched components.

        /// Aperture used for connected items like tracks (not vias).
        GBR_APERTURE_ATTRIB_CONDUCTOR,
        GBR_APERTURE_ATTRIB_EDGECUT,        ///< Aperture used for board cutout,

        /// Aperture used for not connected items (texts, outlines on copper).
        GBR_APERTURE_ATTRIB_NONCONDUCTOR,
        GBR_APERTURE_ATTRIB_VIAPAD,         ///< Aperture used for vias.

        /// Aperture used for through hole component on outer layer.
        GBR_APERTURE_ATTRIB_COMPONENTPAD,

        /// Aperture used for SMD pad. Excluded BGA pads which have their own type.
        GBR_APERTURE_ATTRIB_SMDPAD_SMDEF,

        /// Aperture used for SMD pad with a solder mask defined by the solder mask.
        GBR_APERTURE_ATTRIB_SMDPAD_CUDEF,

        /// Aperture used for BGA pads with a solder mask defined by the copper shape.
        GBR_APERTURE_ATTRIB_BGAPAD_SMDEF,

        /// Aperture used for BGA pad with a solder mask defined by the solder mask.
        GBR_APERTURE_ATTRIB_BGAPAD_CUDEF,

        /// Aperture used for edge connector pad (outer layers).
        GBR_APERTURE_ATTRIB_CONNECTORPAD,
        GBR_APERTURE_ATTRIB_WASHERPAD,      ///< Aperture used for mechanical pads (NPTH).
        GBR_APERTURE_ATTRIB_TESTPOINT,      ///< Aperture used for test point pad (outer layers).

        /// Aperture used for fiducial pad (outer layers), at board level.
        GBR_APERTURE_ATTRIB_FIDUCIAL_GLBL,

        /// Aperture used for fiducial pad (outer layers), at footprint level.
        GBR_APERTURE_ATTRIB_FIDUCIAL_LOCAL,

        /// Aperture used for heat sink pad (typically for SMDs).
        GBR_APERTURE_ATTRIB_HEATSINKPAD,

        /// Aperture used for castellated pads in copper layer files.
        GBR_APERTURE_ATTRIB_CASTELLATEDPAD,

        /// Aperture used for castellated pads in drill files.
        GBR_APERTURE_ATTRIB_CASTELLATEDDRILL,

        GBR_APERTURE_ATTRIB_VIADRILL,       ///< Aperture used for via holes in drill files.
        GBR_APERTURE_ATTRIB_CMP_DRILL,      ///< Aperture used for pad holes in drill files.

        /// Aperture used for pads oblong holes in drill files.
        GBR_APERTURE_ATTRIB_CMP_OBLONG_DRILL,

        /// Aperture used for flashed cmp position in placement files.
        GBR_APERTURE_ATTRIB_CMP_POSITION,

        /// Aperture used for flashed pin 1 (or A1 or AA1) position in placement files.
        GBR_APERTURE_ATTRIB_PAD1_POS,

        /// Aperture used for flashed pads position in placement files.
        GBR_APERTURE_ATTRIB_PADOTHER_POS,

        /// Aperture used to draw component physical body outline without pins in placement files.
        GBR_APERTURE_ATTRIB_CMP_BODY,

        /// Aperture used to draw component physical body outline with pins in placement files.
        GBR_APERTURE_ATTRIB_CMP_LEAD2LEAD,

        /// Aperture used to draw component footprint bounding box in placement files.
        GBR_APERTURE_ATTRIB_CMP_FOOTPRINT,

        /// Aperture used to draw component outline courtyard in placement files.
        GBR_APERTURE_ATTRIB_CMP_COURTYARD,
        GBR_APERTURE_ATTRIB_END             ///< sentinel: max value
    };

    GBR_APERTURE_METADATA()
        :m_ApertAttribute( GBR_APERTURE_ATTRIB_NONE )
    {}

    /**
     * @return the string corresponding to the aperture attribute.
     */
    static std::string GetAttributeName( GBR_APERTURE_ATTRIB aAttribute );
    std::string GetAttributeName()
    {
        return GetAttributeName( m_ApertAttribute );
    }

    /**
     * @param aUseX1StructuredComment false in X2 mode and true in X1 mode to add the net
     *                                attribute inside a compatible X1 structured comment
     *                                starting by "G04 #@! "
     * @return the full command string corresponding to the aperture attribute
     *         like "%TA.AperFunction,<function>*%"
     */
    static std::string FormatAttribute( GBR_APERTURE_ATTRIB aAttribute,
                                        bool aUseX1StructuredComment );

    std::string FormatAttribute( bool aUseX1StructuredComment )
    {
        return FormatAttribute( m_ApertAttribute, aUseX1StructuredComment );
    }

    // The id of the aperture attribute
    GBR_APERTURE_ATTRIB m_ApertAttribute;
};


/**
 * Metadata which can be added in a gerber file as attribute in X2 format.
 */
class GBR_METADATA
{
public:
    GBR_METADATA(): m_isCopper( false)  {}

    void SetApertureAttrib( GBR_APERTURE_METADATA::GBR_APERTURE_ATTRIB aApertAttribute )
    {
        m_ApertureMetadata.m_ApertAttribute = aApertAttribute;
    }

    GBR_APERTURE_METADATA::GBR_APERTURE_ATTRIB GetApertureAttrib()
    {
        return m_ApertureMetadata.m_ApertAttribute;
    }

    void SetNetAttribType( int aNetAttribType )
    {
        m_NetlistMetadata.m_NetAttribType = aNetAttribType;
    }

    int GetNetAttribType() const
    {
        return m_NetlistMetadata.m_NetAttribType;
    }

    void SetNetName( const QString& aNetname ) { m_NetlistMetadata.m_Netname = aNetname; }

    void SetPadName( const QString& aPadname, bool aUseUTF8 = false, bool aEscapeString = false )
    {
        m_NetlistMetadata.m_Padname.SetField( aPadname, aUseUTF8, aEscapeString );
    }

    void SetPadPinFunction( const QString& aPadPinFunction, bool aUseUTF8, bool aEscapeString )
    {
        m_NetlistMetadata.m_PadPinFunction.SetField( aPadPinFunction, aUseUTF8, aEscapeString );
    }

    void SetCmpReference( const QString& aComponentRef )
    {
        m_NetlistMetadata.m_Cmpref = aComponentRef;
    }

    // Allowed attributes are not the same on board copper layers and on other layers
    bool IsCopper() { return m_isCopper; }
    void SetCopper( bool aValue ) { m_isCopper = aValue; }

    // An item to handle aperture attribute
    GBR_APERTURE_METADATA m_ApertureMetadata;

    // An item to handle object attribute
    GBR_NETLIST_METADATA m_NetlistMetadata;

private:
    // If the metadata is relative to a copper layer or not
    bool m_isCopper;
};


// Normalize aString and convert it to a Gerber std::string
std::string FormatStringToGerber( const QString& aString );


// Normalize aString and convert it to a Gerber compatible QString
QString ConvertNotAllowedCharsInGerber( const QString& aString, bool aAllowUtf8Chars,
                                         bool aQuoteString );

// Convert a gerber string into a 16 bit Unicode string
QString FormatStringFromGerber( const QString& aString );

// Generate the string to set a net attribute for a graphic object to print to a gerber file
bool FormatNetAttribute( std::string& aPrintedText, std::string& aLastNetAttributes,
                         const GBR_NETLIST_METADATA* aData, bool& aClearPreviousAttributes,
                         bool aUseX1StructuredComment );

#endif      // GBR_METADATA_H
