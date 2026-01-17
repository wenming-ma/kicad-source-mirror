
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
