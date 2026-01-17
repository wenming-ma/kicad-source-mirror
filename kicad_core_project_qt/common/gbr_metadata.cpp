// Qt transformation completed - wxWidgets to Qt framework migration

/**
 * @file gbr_metadata.cpp
 * @brief helper functions to handle the gerber metadata in files,
 * related to the netlist info and aperture attribute.
 */
#include <QString>
#include <QDateTime>

#include <gbr_metadata.h>
#include <core/utf8.h>


QString GbrMakeCreationDateAttributeString( GBR_NC_STRING_FORMAT aFormat )
{
    // creates the CreationDate attribute:
    // The attribute value must conform to the full version of the ISO 8601
    // date and time format, including time and time zone. Note that this is
    // the date the Gerber file was effectively created,
    // not the time the project of PCB was started
    QDateTime date = QDateTime::currentDateTime();

    // Date format: see http://www.cplusplus.com/reference/ctime/strftime
    QString timezone_offset;   // ISO 8601 offset from UTC in timezone
    timezone_offset = date.toString( "zzz" );  // Extract the time zone offset

    // The time zone offset format is +mm or +hhmm (or -mm or -hhmm)
    // (mm = number of minutes, hh = number of hours. 1h00mn is returned as +0100)
    // we want +(or -) hh:mm
    if( timezone_offset.length() > 3 )     // format +hhmm or -hhmm found
        // Add separator between hours and minutes
        timezone_offset.insert( 3, ":" );

    QString msg;

    switch( aFormat )
    {
    case GBR_NC_STRING_FORMAT_X2:
        msg = QString::asprintf( "%%TF.CreationDate,%s%s*%%", date.toString( Qt::ISODate ).toStdString().c_str(), timezone_offset.toStdString().c_str() );
        break;

    case GBR_NC_STRING_FORMAT_X1:
        msg = QString::asprintf( "G04 #@! TF.CreationDate,%s%s*", date.toString( Qt::ISODate ).toStdString().c_str(),
                    timezone_offset.toStdString().c_str() );
        break;

    case GBR_NC_STRING_FORMAT_GBRJOB:
        msg = QString::asprintf( "%s%s", date.toString( Qt::ISODate ).toStdString().c_str(), timezone_offset.toStdString().c_str() );
        break;

    case GBR_NC_STRING_FORMAT_NCDRILL:
        msg = QString::asprintf( "; #@! TF.CreationDate,%s%s", date.toString( Qt::ISODate ).toStdString().c_str(),
                    timezone_offset.toStdString().c_str() );
        break;
    }

    return msg;
}


QString GbrMakeProjectGUIDfromString( const QString& aText )
{
    /* Gerber GUID format should be RFC4122 Version 1 or 4.
     * See en.wikipedia.org/wiki/Universally_unique_identifier
     * The format is:
     * xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
     * with
     *   x = hexDigit lower/upper case
     * and
     *  M = '1' or '4' (UUID version: 1 (basic) or 4 (random)) (we use 4: UUID random)
     * and
     *  N = '8' or '9' or 'A|a' or 'B|b' : UUID variant 1: 2 MSB bits have meaning) (we use N = 9)
     *  N = 1000 or 1001 or 1010 or 1011  : 10xx means Variant 1 (Variant2: 110x and 111x are
     *  reserved)
     */

    QString guid;

    // Build a 32 digits GUID from the board name:
    // guid has 32 digits, so add chars in name to be sure we can build a 32 digits guid
    // (i.e. from a 16 char string name)
    // In fact only 30 digits are used, and 2 UID id
    QString bname = aText;
    int cnt = 16 - bname.length();

    if( cnt > 0 )
        bname.append( QString( cnt, 'X' ) );

    int chr_idx = 0;

    // Output the 8 first hex digits:
    for( unsigned ii = 0; ii < 4; ii++ )
    {
        int cc = bname[chr_idx++].unicode() & 0xFF;
        guid += QString::asprintf( "%2.2x", cc );
    }

    // Output the 4 next hex digits:
    guid += '-';

    for( unsigned ii = 0; ii < 2; ii++ )
    {
        int cc = bname[chr_idx++].unicode() & 0xFF;
        guid += QString::asprintf( "%2.2x", cc );
    }

    // Output the 4 next hex digits (UUID version and 3 digits):
    guid += "-4";   // first digit: UUID version 4 (M = 4)
    {
        int cc = bname[chr_idx++].unicode() << 4 & 0xFF0;
        cc += bname[chr_idx].unicode() >> 4 & 0x0F;
        guid += QString::asprintf( "%3.3x", cc );
    }

    // Output the 4 next hex digits (UUID variant and 3 digits):
    guid += "-9";  // first digit: UUID variant 1 (N = 9)
    {
        int cc = (bname[chr_idx++].unicode() & 0x0F) << 8;
        cc += bname[chr_idx++].unicode() & 0xFF;
        guid += QString::asprintf( "%3.3x", cc );
    }

    // Output the 12 last hex digits:
    guid += '-';

    for( unsigned ii = 0; ii < 6; ii++ )
    {
        int cc = bname[chr_idx++].unicode() & 0xFF;
        guid += QString::asprintf( "%2.2x", cc );
    }

    return guid;
}


std::string GBR_APERTURE_METADATA::FormatAttribute( GBR_APERTURE_ATTRIB aAttribute,
                                                    bool aUseX1StructuredComment )
{
    std::string attribute_string;   // the specific aperture attribute (TA.xxx)
    std::string comment_string;     // a optional G04 comment line to write before the TA. line

    // generate a string to print a Gerber Aperture attribute
    switch( aAttribute )
    {
    // Dummy value (aAttribute must be < GBR_APERTURE_ATTRIB_END).
    case GBR_APERTURE_ATTRIB_END:
    case GBR_APERTURE_ATTRIB_NONE:          // idle command: do nothing
        break;

    case GBR_APERTURE_ATTRIB_ETCHEDCMP:     // print info associated to an item
                                            // which connects 2 different nets
                                            // (Net tees, microwave component)
        attribute_string = "TA.AperFunction,EtchedComponent";
        break;

    case GBR_APERTURE_ATTRIB_CONDUCTOR:     // print info associated to a track
        attribute_string = "TA.AperFunction,Conductor";
        break;

    case GBR_APERTURE_ATTRIB_EDGECUT:        // print info associated to a board outline
        attribute_string = "TA.AperFunction,Profile";
        break;

    case GBR_APERTURE_ATTRIB_VIAPAD:        // print info associated to a flashed via
        attribute_string = "TA.AperFunction,ViaPad";
        break;

    case GBR_APERTURE_ATTRIB_NONCONDUCTOR:  // print info associated to a item on a copper layer
                                            // which is not a track (for instance a text)
        attribute_string = "TA.AperFunction,NonConductor";
        break;

    case GBR_APERTURE_ATTRIB_COMPONENTPAD:  // print info associated to a flashed
                                            // through hole component on outer layer
        attribute_string = "TA.AperFunction,ComponentPad";
        break;

    case GBR_APERTURE_ATTRIB_SMDPAD_SMDEF:  // print info associated to a flashed for SMD pad.
                                            // with  solder mask defined from the copper shape
                                            // Excluded BGA pads which have their own type
        attribute_string = "TA.AperFunction,SMDPad,SMDef";
        break;

    case GBR_APERTURE_ATTRIB_SMDPAD_CUDEF:  // print info associated to a flashed SMD pad with
                                            // a solder mask defined by the solder mask
        attribute_string = "TA.AperFunction,SMDPad,CuDef";
        break;

    case GBR_APERTURE_ATTRIB_BGAPAD_SMDEF:  // print info associated to flashed BGA pads with
                                            // a solder mask defined by the copper shape
        attribute_string = "TA.AperFunction,BGAPad,SMDef";
        break;

    case GBR_APERTURE_ATTRIB_BGAPAD_CUDEF:  // print info associated to a flashed BGA pad with
                                            // a solder mask defined by the solder mask
        attribute_string = "TA.AperFunction,BGAPad,CuDef";
        break;

    case GBR_APERTURE_ATTRIB_CONNECTORPAD:
        // print info associated to a flashed edge connector pad (outer layers)
        attribute_string = "TA.AperFunction,ConnectorPad";
        break;

    case GBR_APERTURE_ATTRIB_WASHERPAD:
        // print info associated to flashed mechanical pads (NPTH)
        attribute_string = "TA.AperFunction,WasherPad";
        break;

    case GBR_APERTURE_ATTRIB_HEATSINKPAD:   // print info associated to a flashed heat sink pad
                                            // (typically for SMDs)
        attribute_string = "TA.AperFunction,HeatsinkPad";
        break;

    case GBR_APERTURE_ATTRIB_TESTPOINT:   // print info associated to a flashed test point pad
                                            // (typically for SMDs)
        attribute_string = "TA.AperFunction,TestPad";
        break;

    case GBR_APERTURE_ATTRIB_FIDUCIAL_GLBL: // print info associated to a flashed fiducial pad
                                            // (typically for SMDs)
        attribute_string = "TA.AperFunction,FiducialPad,Global";
        break;

    case GBR_APERTURE_ATTRIB_FIDUCIAL_LOCAL:    // print info associated to a flashed fiducial pad
                                                // (typically for SMDs)
        attribute_string = "TA.AperFunction,FiducialPad,Local";
        break;

    case GBR_APERTURE_ATTRIB_CASTELLATEDPAD:
        // print info associated to a flashed castellated pad (typically for SMDs)
        attribute_string = "TA.AperFunction,CastellatedPad";
        break;

    case GBR_APERTURE_ATTRIB_CASTELLATEDDRILL:
        // print info associated to a flashed castellated pad in drill files
        attribute_string = "TA.AperFunction,CastellatedDrill";
        break;

    case GBR_APERTURE_ATTRIB_VIADRILL:      // print info associated to a via hole in drill files
        attribute_string = "TA.AperFunction,ViaDrill";
        break;

    case GBR_APERTURE_ATTRIB_CMP_DRILL:     // print info associated to a component
                                            // round pad hole in drill files
        attribute_string = "TA.AperFunction,ComponentDrill";
        break;

    // print info associated to a component oblong pad hole in drill files
    // Same as a round pad hole, but is a specific aperture in drill file and
    // a G04 comment is added to the aperture function
    case GBR_APERTURE_ATTRIB_CMP_OBLONG_DRILL:
        comment_string = "aperture for slot hole";
        attribute_string = "TA.AperFunction,ComponentDrill";
        break;

    case GBR_APERTURE_ATTRIB_CMP_POSITION:      // print info associated to a component
                                                // flashed shape at the component position
                                                // in placement files
        attribute_string = "TA.AperFunction,ComponentMain";
        break;

    case GBR_APERTURE_ATTRIB_PAD1_POS:     // print info associated to a component
                                                // flashed shape at pad 1 position
                                                // (pad 1 is also pad A1 or pad AA1)
                                                // in placement files
        attribute_string = "TA.AperFunction,ComponentPin";
        break;

    case GBR_APERTURE_ATTRIB_PADOTHER_POS: // print info associated to a component
                                                // flashed shape at pads position (all but pad 1)
                                                // in placement files
                                                // Currently: (could be changed later) same as
                                                // GBR_APERTURE_ATTRIB_PADOTHER_POS
        attribute_string = "TA.AperFunction,ComponentPin";
        break;

    case GBR_APERTURE_ATTRIB_CMP_BODY:          // print info associated to a component
                                                // print the component physical body
                                                // polygon in placement files
        attribute_string = "TA.AperFunction,ComponentOutline,Body";
        break;

    case GBR_APERTURE_ATTRIB_CMP_LEAD2LEAD:     // print info associated to a component
                                                // print the component physical lead to lead
                                                // polygon in placement files
        attribute_string = "TA.AperFunction,ComponentOutline,Lead2Lead";
        break;

    case GBR_APERTURE_ATTRIB_CMP_FOOTPRINT:     // print info associated to a component
                                                // print the component footprint bounding box
                                                // polygon in placement files
        attribute_string = "TA.AperFunction,ComponentOutline,Footprint";
        break;

    case GBR_APERTURE_ATTRIB_CMP_COURTYARD:     // print info associated to a component
                                                // print the component courtyard
                                                // polygon in placement files
        attribute_string = "TA.AperFunction,ComponentOutline,Courtyard";
        break;

    break;
    }

    std::string full_attribute_string;
    QString eol_string;

    if( !attribute_string.empty() )
    {
        if( !comment_string.empty() )
        {
            full_attribute_string = "G04 " + comment_string + "*\n";
        }

        if( aUseX1StructuredComment )
        {
            full_attribute_string += "G04 #@! ";
            eol_string = "*\n";
        }
        else
        {
            full_attribute_string += "%";
            eol_string = "*%\n";
        }
    }

    full_attribute_string += attribute_string + eol_string.toStdString();

    return full_attribute_string;
}


// Helper function to convert a ascii hex char to its integer value
// If the char is not a hexa char, return -1
int char2Hex( unsigned aCode )
{
    if( aCode >= '0' && aCode <= '9' )
        return aCode - '0';

    if( aCode >= 'A' && aCode <= 'F' )
        return aCode - 'A' + 10;

    if( aCode >= 'a' && aCode <= 'f' )
        return aCode - 'a' + 10;

    return -1;
}


QString FormatStringFromGerber( const QString& aString )
{
    // make the inverse conversion of FormatStringToGerber()
    // It converts a "normalized" gerber string containing escape sequences
    // and convert it to a 16 bits Unicode char
    // and return a QString (Unicode 16) from the gerber string
    // Note the initial gerber string can already contain Unicode chars.
    QString txt;           // The string converted from Gerber string

    unsigned count = aString.length();

    for( unsigned ii = 0; ii < count; ++ii )
    {
        unsigned code = aString[ii].unicode();

        if( code == '\\' && ii < count-5 && aString[ii+1] == 'u' )
        {
            // Note the latest Gerber X2 spec (2019 06) uses \uXXXX to encode
            // the Unicode XXXX hexadecimal value
            // If 4 chars next to 'u' are hexadecimal chars,
            // Convert these 4 hexadecimal digits to a 16 bit Unicode
            // (Gerber allows only 4 hexadecimal digits)
            // If an error occurs, the escape sequence is not translated,
            // and used "as this"
            long value = 0;
            bool error = false;

            for( int jj = 0; jj < 4; jj++ )
            {
                value <<= 4;
                code = aString[ii+jj+2].unicode();

                int hexa = char2Hex( code );

                if( hexa >= 0 )
                    value += hexa;
                else
                {
                    error = true;
                    break;
                }
            }

            if( !error )
            {
                if( value >= ' ' )  // Is a valid QChar ?
                    txt.append( QChar( static_cast<ushort>( value ) ) );

                ii += 5;
            }
            else
            {
                txt.append( aString[ii] );
                continue;
            }
        }
        else
        {
            txt.append( aString[ii] );
        }
    }

    return txt;
}


QString ConvertNotAllowedCharsInGerber( const QString& aString, bool aAllowUtf8Chars,
                                         bool aQuoteString )
{
    /* format string means convert any code > 0x7E and unauthorized codes to a hexadecimal
     * 16 bits sequence Unicode
     * However if aAllowUtf8Chars is true only unauthorized codes will be escaped, because some
     * Gerber files accept UTF8 chars.
     * unauthorized codes are ',' '*' '%' '\' '"' and are used as separators in Gerber files
     */
    QString txt;

    if( aQuoteString )
        txt += "\"";

    for( unsigned ii = 0; ii < aString.length(); ++ii )
    {
        QChar code = aString[ii];
        bool convert = false;

        switch( code.unicode() )
        {
        case '\\':
        case '%':
        case '*':
        case ',':
            convert = true;
            break;

        case '"':
            if( aQuoteString )
                convert = true;
            break;

        default:
            break;
        }

        if( !aAllowUtf8Chars && code.unicode() > 0x7F )
            convert = true;

        if( convert )
        {
            // Convert code to 4 hexadecimal digit
            // (Gerber allows only 4 hexadecimal digit) in escape seq:
            // "\uXXXX", XXXX is the Unicode 16 bits hexa value
            char hexa[32];
            std::snprintf( hexa, sizeof( hexa ), "\\u%4.4X", code.unicode() & 0xFFFF );
            txt += hexa;
        }
        else
        {
            txt += code;
        }
    }

    if( aQuoteString )
        txt += "\"";

    return txt;
}


std::string GBR_DATA_FIELD::GetGerberString() const
{
    QString converted;

    if( !m_field.isEmpty() )
        converted = ConvertNotAllowedCharsInGerber( m_field, m_useUTF8, m_escapeString );

    // Convert the char string to std::string. Be careful when converting a QString to
    // a std::string: using static_cast<const char*> is mandatory
    std::string txt = converted.toUtf8().constData();

    return txt;
}


std::string FormatStringToGerber( const QString& aString )
{
    QString converted;

    /* format string means convert any code > 0x7E and unauthorized codes to a hexadecimal
     * 16 bits sequence Unicode
     * unauthorized codes are ',' '*' '%' '\'
     * This conversion is not made for quoted strings, because if the string is
     * quoted, the conversion is expected to be already made, and the returned string must use
     * UTF8 encoding
     */
    if( !aString.isEmpty() && ( aString[0] != '\"' || aString[aString.length()-1] != '\"' ) )
        converted = ConvertNotAllowedCharsInGerber( aString, false, false );
    else
        converted = aString;

    // Convert the char string to std::string. Be careful when converting a QString to
    // a std::string: using static_cast<const char*> is mandatory
    std::string txt = converted.toUtf8().constData();

    return txt;
}


// Netname and Pan num fields cannot be empty in Gerber files
// Normalized names must be used, if any
#define NO_NET_NAME "N/C"    // net name of not connected pads (one pad net) (normalized)
#define NO_PAD_NAME ""       // pad name of pads without pad name/number (not normalized)


bool FormatNetAttribute( std::string& aPrintedText, std::string& aLastNetAttributes,
                         const GBR_NETLIST_METADATA* aData, bool& aClearPreviousAttributes,
                         bool aUseX1StructuredComment )
{
    aClearPreviousAttributes = false;
    QString prepend_string;
    QString eol_string;

    if( aUseX1StructuredComment )
    {
        prepend_string = "G04 #@! ";
        eol_string = "*\n";
    }
    else
    {
        prepend_string = "%";
        eol_string = "*%\n";
    }

    // print a Gerber net attribute record.
    // it is added to the object attributes dictionary
    // On file, only modified or new attributes are printed.
    if( aData == nullptr )
        return false;

    std::string pad_attribute_string;
    std::string net_attribute_string;
    std::string cmp_attribute_string;

    if( aData->m_NetAttribType == GBR_NETLIST_METADATA::GBR_NETINFO_UNSPECIFIED )
        return false;     // idle command: do nothing

    if( ( aData->m_NetAttribType & GBR_NETLIST_METADATA::GBR_NETINFO_PAD ) )
    {
        // print info associated to a flashed pad (cmpref, pad name, and optionally pin function)
        // example1: %TO.P,R5,3*%
        // example2: %TO.P,R5,3,reset*%
        pad_attribute_string = (prepend_string + "TO.P,").toStdString();
        pad_attribute_string += FormatStringToGerber( aData->m_Cmpref ) + ",";

        if( aData->m_Padname.IsEmpty() )
        {
            // Happens for "mechanical" or never connected pads
            pad_attribute_string += FormatStringToGerber( NO_PAD_NAME );
        }
        else
        {
            pad_attribute_string += aData->m_Padname.GetGerberString();

            // In Pcbnew, the pin function comes from the schematic.
            // so it exists only for named pads
            if( !aData->m_PadPinFunction.IsEmpty() )
            {
                pad_attribute_string += ',';
                pad_attribute_string += aData->m_PadPinFunction.GetGerberString();
            }
        }

        pad_attribute_string += eol_string.toStdString();
    }

    if( ( aData->m_NetAttribType & GBR_NETLIST_METADATA::GBR_NETINFO_NET ) )
    {
        // print info associated to a net
        // example: %TO.N,Clk3*%
        net_attribute_string = (prepend_string + "TO.N,").toStdString();

        if( aData->m_Netname.isEmpty() )
        {
            if( aData->m_NotInNet )
            {
                // Happens for not connectable pads: mechanical pads
                // and pads with no padname/num
                // In this case the net name must be left empty
            }
            else
            {
                // Happens for not connected pads: use a normalized
                // dummy name
                net_attribute_string += FormatStringToGerber( NO_NET_NAME );
            }
        }
        else
        {
            net_attribute_string += FormatStringToGerber( aData->m_Netname );
        }

        net_attribute_string += eol_string.toStdString();
    }

    if( ( aData->m_NetAttribType & GBR_NETLIST_METADATA::GBR_NETINFO_CMP ) &&
        !( aData->m_NetAttribType & GBR_NETLIST_METADATA::GBR_NETINFO_PAD ) )
    {
        // print info associated to a footprint
        // example: %TO.C,R2*%
        // Because GBR_NETINFO_PAD option already contains this info, it is not
        // created here for a GBR_NETINFO_PAD attribute
        cmp_attribute_string = (prepend_string + "TO.C,").toStdString();
        cmp_attribute_string += FormatStringToGerber( aData->m_Cmpref ) + eol_string.toStdString();
    }

    // the full list of requested attributes:
    std::string full_attribute_string = pad_attribute_string + net_attribute_string
                                   + cmp_attribute_string;
    // the short list of requested attributes
    // (only modified or new attributes are stored here):
    std::string short_attribute_string;

    // Attributes have changed: update attribute string, and see if the previous attribute
    // list (dictionary in Gerber language) must be cleared
    if( aLastNetAttributes != full_attribute_string )
    {
        // first, remove no longer existing attributes.
        // Because in KiCad the full attribute list is evaluated for each object,
        // the entire dictionary is cleared
        // If m_TryKeepPreviousAttributes is true, only the no longer existing attribute
        // is cleared.
        // Note: to avoid interaction between clear attributes and set attributes
        // the clear attribute is inserted first.
        bool clearDict = false;

        if( aLastNetAttributes.find( "TO.P," ) != std::string::npos )
        {
            if( pad_attribute_string.empty() )  // No more this attribute
            {
                if( aData->m_TryKeepPreviousAttributes )    // Clear only this attribute
                    short_attribute_string.insert( 0, (prepend_string + "TO.P" + eol_string).toStdString() );
                else
                    clearDict = true;
            }
            else if( aLastNetAttributes.find( pad_attribute_string ) == std::string::npos )
            {
                // This attribute has changed
                short_attribute_string += pad_attribute_string;
            }
        }
        else    // New attribute
        {
            short_attribute_string += pad_attribute_string;
        }

        if( aLastNetAttributes.find( "TO.N," ) != std::string::npos )
        {
            if( net_attribute_string.empty() )  // No more this attribute
            {
                if( aData->m_TryKeepPreviousAttributes )    // Clear only this attribute
                    short_attribute_string.insert( 0, (prepend_string + "TO.N" + eol_string).toStdString() );
                else
                    clearDict = true;
            }
            else if( aLastNetAttributes.find( net_attribute_string ) == std::string::npos )
            {
                // This attribute has changed.
                short_attribute_string += net_attribute_string;
            }
        }
        else    // New attribute
        {
            short_attribute_string += net_attribute_string;
        }

        if( aLastNetAttributes.find( "TO.C," ) != std::string::npos )
        {
            if( cmp_attribute_string.empty() )  // No more this attribute
            {
                if( aData->m_TryKeepPreviousAttributes )    // Clear only this attribute
                {
                    // Refinement:
                    // the attribute will be cleared only if there is no pad attribute.
                    // If a pad attribute exists, the component name exists so the old
                    // TO.C value will be updated, therefore no need to clear it before updating
                    if( pad_attribute_string.empty() )
                        short_attribute_string.insert( 0, (prepend_string + "TO.C" + eol_string).toStdString() );
                }
                else
                {
                    clearDict = true;
                }
            }
            else if( aLastNetAttributes.find( cmp_attribute_string ) == std::string::npos )
            {
                // This attribute has changed.
                short_attribute_string += cmp_attribute_string;
            }
        }
        else    // New attribute
        {
            short_attribute_string += cmp_attribute_string;
        }

        aClearPreviousAttributes = clearDict;

        aLastNetAttributes = full_attribute_string;

        if( clearDict )
            aPrintedText = full_attribute_string;
        else
            aPrintedText = short_attribute_string;
    }

    return true;
}


void GBR_CMP_PNP_METADATA::ClearData()
{
    // Clear all strings
    m_Orientation = 0.0;
    m_Manufacturer.clear();
    m_MPN.clear();
    m_Package.clear();
    m_Value.clear();
    m_MountType = MOUNT_TYPE_UNSPECIFIED;
}


QString GBR_CMP_PNP_METADATA::FormatCmpPnPMetadata()
{
    QString text;
    QString start_of_line( "%TO." );
    QString end_of_line( "*%\n" );

    QString mountTypeStrings[] =
    {
        "Other", "SMD", "TH"
    };

    if( !m_Manufacturer.isEmpty() )
        text += start_of_line + "CMfr," + m_Manufacturer + end_of_line;

    if( !m_MPN.isEmpty() )
        text += start_of_line + "CMPN," + m_MPN + end_of_line;

    if( !m_Package.isEmpty() )
        text += start_of_line + "Cpkg," + m_Package + end_of_line;

    if( !m_Footprint.isEmpty() )
        text += start_of_line + "CFtp," + m_Footprint + end_of_line;

    if( !m_Value.isEmpty() )
        text += start_of_line + "CVal," + m_Value + end_of_line;

    if( !m_LibraryName.isEmpty() )
        text += start_of_line + "CLbN," + m_LibraryName + end_of_line;

    if( !m_LibraryDescr.isEmpty() )
        text += start_of_line + "CLbD," + m_LibraryDescr + end_of_line;

    text += start_of_line + "CMnt," + mountTypeStrings[m_MountType] + end_of_line;
    text += start_of_line + "CRot," + QString::number(m_Orientation) + end_of_line;

    return text;
}
