

#include <QString>
#include <QStringList>
#include <QColor>
#include <QCoreApplication>
#include <core/arraydim.h>
#include <board_design_settings.h>
#include <i18n_utility.h>       // _HKI definition
#include "stackup_predefined_prms.h"

// A list of copper finish standard type names.
// They are standard names in .gbdjob files, so avoid changing them or ensure they are
// compatible with .gbrjob file spec.
static QString copperFinishType[] =
{
    NotSpecifiedPrm(),            // Not specified, not in .gbrjob file
    _HKI( "ENIG" ),               // used in .gbrjob file
    _HKI( "ENEPIG" ),             // used in .gbrjob file
    _HKI( "HAL SnPb" ),           // used in .gbrjob file
    _HKI( "HAL lead-free" ),      // used in .gbrjob file
    _HKI( "Hard gold" ),          // used in .gbrjob file
    _HKI( "Immersion tin" ),      // used in .gbrjob file
    _HKI( "Immersion nickel" ),   // used in .gbrjob file
    _HKI( "Immersion silver" ),   // used in .gbrjob file
    _HKI( "Immersion gold" ),     // used in .gbrjob file
    _HKI( "HT_OSP" ),             // used in .gbrjob file
    _HKI( "OSP" ),                // used in .gbrjob file
    _HKI( "None" ),               // used in .gbrjob file
    _HKI( "User defined" )        // keep this option at end
};


// A list of available colors for solder mask and silkscreen.
// These names are used in .gbrjob file, so they are not fully free.  Use only what is allowed in
// .gbrjob files.
// For other colors (user defined), the defined value is the html color syntax in .kicad_pcb files
// and R<integer>G<integer>B<integer> in .gbrjob file.
static std::vector<FAB_LAYER_COLOR> gbrjobColors  =
{
    { NotSpecifiedPrm(),      QColor(  80,  80,  80 ) },  // Not specified, not in .gbrjob file
    { _HKI( "Green" ),        QColor(  60, 150,  80 ) },  // used in .gbrjob file
    { _HKI( "Red" ),          QColor( 128,   0,   0 ) },  // used in .gbrjob file
    { _HKI( "Blue" ),         QColor(   0,   0, 128 ) },  // used in .gbrjob file
    { _HKI( "Purple" ),       QColor(  80,   0,  80 ) },  // used in .gbrjob file
    { _HKI( "Black" ),        QColor(  20,  20,  20 ) },  // used in .gbrjob file
    { _HKI( "White" ),        QColor( 200, 200, 200 ) },  // used in .gbrjob file
    { _HKI( "Yellow" ),       QColor( 128, 128,   0 ) },  // used in .gbrjob file
    { _HKI( "User defined" ), QColor( 128, 128, 128 ) }   // Free; the name is a dummy name here
};


// These are used primarily as a source for the 3D renderer.  They are written
// as R<integer>G<integer>B<integer>  to the .gbrjob file.
static std::vector<FAB_LAYER_COLOR> dielectricColors =
{
    { NotSpecifiedPrm(),          QColor(  80,  80,  80, 255 ) },
    { _HKI( "FR4 natural" ),      QColor( 109, 116,  75, 212 ) },
    { _HKI( "PTFE natural" ),     QColor( 252, 252, 250, 230 ) },
    { _HKI( "Polyimide" ),        QColor( 205, 130,   0, 170 ) },
    { _HKI( "Phenolic natural" ), QColor(  92,  17,   6, 230 ) },
    { _HKI( "Aluminum" ),         QColor( 213, 213, 213, 255 ) },
    { _HKI( "User defined" ),     QColor( 128, 128, 128, 212 ) }
};


QStringList GetStandardCopperFinishes( bool aTranslate )
{
    QStringList list;

    for( unsigned ii = 0; ii < arrayDim( copperFinishType ); ii++ )
        list.append( aTranslate ? QCoreApplication::translate("", copperFinishType[ii].toStdString().c_str()) : copperFinishType[ii] );

    return list;
}

std::vector<FAB_LAYER_COLOR> dummy;
const std::vector<FAB_LAYER_COLOR>& GetStandardColors( BOARD_STACKUP_ITEM_TYPE aType )
{
    switch( aType )
    {
    case BS_ITEM_TYPE_SILKSCREEN: return gbrjobColors;
    case BS_ITEM_TYPE_SOLDERMASK: return gbrjobColors;
    case BS_ITEM_TYPE_DIELECTRIC: return dielectricColors;
    default:                      return dummy;
    }
}


int GetColorUserDefinedListIdx( BOARD_STACKUP_ITEM_TYPE aType )
{
    // this is the last item in list
    return GetStandardColors( aType ).size() - 1;
}


bool IsColorNameNormalized( const QString& aName )
{
    static std::vector<QString> list =
    {
        "Green", "Red", "Blue",
        "Black", "White", "Yellow"
    };

   for( QString& candidate : list )
   {
       if( candidate.compare( aName, Qt::CaseInsensitive ) == 0 )
           return true;
   }

   return false;
}


const QString FAB_LAYER_COLOR::GetColorAsString() const
{
    if( IsColorNameNormalized( m_colorName ) )
        return m_colorName;

    return QString::asprintf( "R%dG%dB%d",
                             int( m_color.red() ), int( m_color.green() ), int( m_color.blue() ) );
}
