
/**
 * @file X2_gerber_attributes.cpp
 */

/*
 * Manage the gerber extensions (attributes) in the new X2 version
 * only few extensions are handled
 * See http://www.ucamco.com/files/downloads/file/81/the_gerber_file_format_specification.pdf
 *
 * gerber attributes in the new X2 version look like:
 * %TF.FileFunction,Copper,L1,Top*%
 *
 * Currently:
 * .FileFunction .FileFunction Identifies the file's function in the PCB.
 *  Other Standard Attributes, not yet used in Gerbview:
 * .Part Identifies the part the file represents, e.g. a single PCB
 * .MD5 Sets the MD5 file signature or checksum.
 */

#include <QDebug>
#include <QString>
#include <X2_gerber_attributes.h>
#include <string_utils.h>


X2_ATTRIBUTE::X2_ATTRIBUTE()
{
}


X2_ATTRIBUTE::~X2_ATTRIBUTE()
{
}


const QString& X2_ATTRIBUTE::GetAttribute()
{
    return m_Prms.at( 0 );
}


const QString& X2_ATTRIBUTE::GetPrm( int aIdx )
{
    static const QString dummy;

    if( GetPrmCount() > aIdx && aIdx >= 0 )
        return m_Prms.at( aIdx );

    return dummy;
}


void X2_ATTRIBUTE::DbgListPrms()
{
    qDebug() << "prms count" << GetPrmCount();

    for( int ii = 0; ii < GetPrmCount(); ii++ )
        qDebug() << m_Prms.at( ii );
}


bool X2_ATTRIBUTE::ParseAttribCmd( FILE* aFile, char *aBuffer, int aBuffSize, char* &aText,
                                   int& aLineNum )
{
    // parse a TF, TA, TO ... command and fill m_Prms by the parameters found.
    // the "%TF" (start of command) is already read by the caller

    bool ok = true;
    std::string data;

    for( ; ; )
    {
        while( *aText )
        {
            switch( *aText )
            {
            case '%':       // end of command
                return ok;  // success completion

            case ' ':
            case '\r':
            case '\n':
                aText++;
                break;

            case '*':       // End of block
                m_Prms.append( From_UTF8( data.c_str() ) );
                data.clear();
                aText++;
                break;

            case ',':       // End of parameter (separator)
                aText++;
                m_Prms.append( From_UTF8( data.c_str() ) );
                data.clear();
                break;

            default:
                data += *aText;
                aText++;
                break;
            }
        }

        // end of current line, read another one.
        if( aBuffer && aFile )
        {
            if( fgets( aBuffer, aBuffSize, aFile ) == nullptr )
            {
                // end of file
                ok = false;
                break;
            }

            aLineNum++;
            aText = aBuffer;
        }
        else
        {
            return ok;
        }
    }

    return ok;
}


X2_ATTRIBUTE_FILEFUNCTION::X2_ATTRIBUTE_FILEFUNCTION( X2_ATTRIBUTE& aAttributeBase )
    : X2_ATTRIBUTE()
{
    m_Prms = aAttributeBase.GetPrms();
    m_z_order = 0;

    // ensure at least 7 parameters exist.
    while( GetPrmCount() < 7 )
        m_Prms.append( QString() );

    set_Z_Order();
}


const QString& X2_ATTRIBUTE_FILEFUNCTION::GetFileType()
{
    // the type of layer (Copper, Soldermask ... )
    return m_Prms.at( 1 );
}


const QString& X2_ATTRIBUTE_FILEFUNCTION::GetBrdLayerId()
{
    // the brd layer identifier: Ln (for Copper type) or Top, Bot
    return m_Prms.at( 2 );
}


const QString X2_ATTRIBUTE_FILEFUNCTION::GetDrillLayerPair()
{
    // the layer pair identifiers, for drill files, i.e.
    // with m_Prms.at( 1 ) = "Plated" or "NonPlated"
    QString lpair = m_Prms.at( 2 ) + ',' + m_Prms.at( 3 );
    return lpair;
}


const QString& X2_ATTRIBUTE_FILEFUNCTION::GetBrdLayerSide()
{
    if( IsCopper() )
        // the brd layer identifier: Top, Bot, Inr
        return m_Prms.at( 3 );
    else
        // the brd layer identifier: Top, Bot ( same as GetBrdLayerId() )
        return m_Prms.at( 2 );
}


const QString& X2_ATTRIBUTE_FILEFUNCTION::GetLabel()
{
    if( IsCopper() )
       return m_Prms.at( 4 );
    else
        return m_Prms.at( 3 );
}


const QString& X2_ATTRIBUTE_FILEFUNCTION::GetLPType()
{
    // Only for drill files:  the Layer Pair type (PTH, NPTH, Blind or Buried)
    return m_Prms.at( 4 );
}


const QString& X2_ATTRIBUTE_FILEFUNCTION::GetRouteType()
{
    // Only for drill files:  the drill/routing type(Drill, Route, Mixed)
    return m_Prms.at( 5 );
}


bool X2_ATTRIBUTE_FILEFUNCTION::IsCopper()
{
    // the filefunction label, if any
    return GetFileType().compare( "Copper", Qt::CaseInsensitive ) == 0;
}


bool X2_ATTRIBUTE_FILEFUNCTION::IsDrillFile()
{
    // the filefunction label, if any
    return GetFileType().compare( "Plated", Qt::CaseInsensitive ) == 0
           || GetFileType().compare( "NonPlated", Qt::CaseInsensitive ) == 0;
}


void X2_ATTRIBUTE_FILEFUNCTION::set_Z_Order()
{
    m_z_order = 100;     // high level
    m_z_sub_order = 0;

    if( IsCopper() )
    {
        // Copper layer: the priority is the layer Id
        m_z_order = 0;
        QString num = GetBrdLayerId().mid( 1 );
        long lnum;

        if( (lnum = num.toLong()) || num == "0" )
            m_z_sub_order = -lnum;
    }

    if( GetFileType().compare( "Soldermask", Qt::CaseInsensitive ) == 0 )
    {
        // solder mask layer: the priority is top then bottom
        m_z_order = 1;       // for top

        if( GetBrdLayerId().compare( "Bot", Qt::CaseInsensitive ) == 0 )
            m_z_order = -m_z_order;
    }

    if( GetFileType().compare( "Legend", Qt::CaseInsensitive ) == 0 )
    {
        // Silk screen layer: the priority is top then bottom
        m_z_order = 2;       // for top

        if( GetBrdLayerId().compare( "Bot", Qt::CaseInsensitive ) == 0 )
            m_z_order = -m_z_order;
    }

    if( GetFileType().compare( "Paste", Qt::CaseInsensitive ) == 0 )
    {
        // solder paste layer: the priority is top then bottom
        m_z_order = 3;       // for top

        if( GetBrdLayerId().compare( "Bot", Qt::CaseInsensitive ) == 0 )
            m_z_order = -m_z_order;
    }

    if( GetFileType().compare( "Glue", Qt::CaseInsensitive ) == 0 )
    {
        // Glue spots used to fix components to the board prior to soldering:
        // the priority is top then bottom
        m_z_order = 4;       // for top

        if( GetBrdLayerId().compare( "Bot", Qt::CaseInsensitive ) == 0 )
            m_z_order = -m_z_order;
    }
}

