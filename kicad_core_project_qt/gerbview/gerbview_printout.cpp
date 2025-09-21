
#include <base_units.h>
#include <base_screen.h>
#include <gal/graphics_abstraction_layer.h>
#include <gerbview_frame.h>
#include <gerber_file_image.h>
#include <gerber_file_image_list.h>
#include "gerbview_printout.h"

#include <lseq.h>
#include <lset.h>
#include <view/view.h>
#include <gerbview_painter.h>
#include <math/util.h>      // for KiROUND


GERBVIEW_PRINTOUT::GERBVIEW_PRINTOUT( GBR_LAYOUT* aLayout, const BOARD_PRINTOUT_SETTINGS& aParams,
                                      const KIGFX::VIEW* aView, const QString& aTitle ) :
    BOARD_PRINTOUT( aParams, aView, aTitle )
{
    m_layout = aLayout;
    m_gerbviewPrint = true;
}


bool GERBVIEW_PRINTOUT::OnPrintPage( int aPage )
{
    // Store the layerset, as it is going to be modified below and the original settings are needed
    LSET lset = m_settings.m_LayerSet;

    LSEQ seq = lset.UIOrder();
    Q_ASSERT( unsigned( aPage - 1 ) < seq.size() );
    if( unsigned( aPage - 1 ) >= seq.size() ) return false;
    auto layerId = seq[aPage - 1];

    // In gerbview, draw layers are always printed on separate pages because handling negative
    // objects when using only one page is tricky

    // Enable only one layer to create a printout
    m_settings.m_LayerSet = LSET( { layerId } );

    GERBER_FILE_IMAGE_LIST& gbrImgList = GERBER_FILE_IMAGE_LIST::GetImagesList();
    GERBER_FILE_IMAGE*      gbrImage = gbrImgList.GetGbrImage( layerId );
    QString                 gbr_filename;

    if( gbrImage )
        gbr_filename = gbrImage->m_FileName;

    DrawPage( gbr_filename, aPage, m_settings.m_pageCount );

    // Restore the original layer set, so the next page can be printed
    m_settings.m_LayerSet = lset;

    return true;
}


int GERBVIEW_PRINTOUT::milsToIU( double aMils ) const
{
    return KiROUND( gerbIUScale.IU_PER_MILS * aMils );
}


void GERBVIEW_PRINTOUT::setupViewLayers( KIGFX::VIEW& aView, const LSET& aLayerSet )
{
    BOARD_PRINTOUT::setupViewLayers( aView, aLayerSet );

    for( PCB_LAYER_ID layer : m_settings.m_LayerSet.Seq() )
        aView.SetLayerVisible( static_cast<int>( GERBVIEW_LAYER_ID_START ) + layer, true );
}


void GERBVIEW_PRINTOUT::setupGal( KIGFX::GAL* aGal )
{
    BOARD_PRINTOUT::setupGal( aGal );
    aGal->SetWorldUnitLength( 1.0/ gerbIUScale.IU_PER_MM /* 10 nm */ / 25.4 /* 1 inch in mm */ );
}


BOX2I GERBVIEW_PRINTOUT::getBoundingBox()
{
    return m_layout->ComputeBoundingBox();
}


std::unique_ptr<KIGFX::PAINTER> GERBVIEW_PRINTOUT::getPainter( KIGFX::GAL* aGal )
{
    return std::make_unique<KIGFX::GERBVIEW_PAINTER>( aGal );
}
