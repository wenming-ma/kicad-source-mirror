

#include <view/qt_view_controls.h>
#include <QtWidgets/QWidget>
#include <QtCore/QDebug>
#include <QtGui/QPaintEvent>
#include <drawing_sheet/ds_proxy_view_item.h>

#include <gal/graphics_abstraction_layer.h>

#include <sch_preview_panel.h>
#include <sch_view.h>
#include <sch_painter.h>
#include <sch_edit_frame.h>
#include <settings/settings_manager.h>
#include <preview_items/selection_area.h>
#include <zoom_defines.h>

#include <functional>

#include <sch_sheet.h>
#include <pgm_base.h>

using namespace std::placeholders;

SCH_PREVIEW_PANEL::SCH_PREVIEW_PANEL( QWidget* aParentWindow, int aWindowId,
                                      const QPoint& aPosition, const QSize& aSize,
                                      KIGFX::GAL_DISPLAY_OPTIONS& aOptions, GAL_TYPE aGalType ) :
    EDA_DRAW_PANEL_GAL( aParentWindow, aWindowId, aPosition, aSize, aOptions, aGalType )
{
    m_view = new KIGFX::SCH_VIEW( nullptr );
    m_view->SetGAL( m_gal );

    m_gal->SetWorldUnitLength( SCH_WORLD_UNIT );

    m_painter.reset( new KIGFX::SCH_PAINTER( m_gal ) );

    SCH_RENDER_SETTINGS* renderSettings = GetRenderSettings();
    renderSettings->LoadColors( Pgm().GetSettingsManager().GetColorSettings() );
    renderSettings->m_ShowPinsElectricalType = false;
    renderSettings->m_ShowPinNumbers = false;
    renderSettings->m_TextOffsetRatio = 0.35;

    m_view->SetPainter( m_painter.get() );
    // This fixes the zoom in and zoom out limits:
    m_view->SetScaleLimits( ZOOM_MAX_LIMIT_EESCHEMA_PREVIEW, ZOOM_MIN_LIMIT_EESCHEMA_PREVIEW );
    m_view->SetMirror( false, false );

    setDefaultLayerOrder();
    setDefaultLayerDeps();

    view()->UpdateAllLayersOrder();
    // View controls is the first in the event handler chain, so the Tool Framework operates
    // on updated viewport data.
    m_viewControls = new KIGFX::QT_VIEW_CONTROLS( m_view, this );

    m_gal->SetGridColor( m_painter->GetSettings()->GetLayerColor( LAYER_SCHEMATIC_GRID ) );
    m_gal->SetCursorEnabled( false );
    m_gal->SetGridSize( VECTOR2D( schIUScale.MilsToIU( 100.0 ), schIUScale.MilsToIU( 100.0 ) ) );

    setEnabled( true );
    setFocus();
    show();
    raise();
    StartDrawing();
}


SCH_PREVIEW_PANEL::~SCH_PREVIEW_PANEL()
{
}


SCH_RENDER_SETTINGS* SCH_PREVIEW_PANEL::GetRenderSettings() const
{
    return static_cast<SCH_RENDER_SETTINGS*>( m_painter->GetSettings() );
}


void SCH_PREVIEW_PANEL::OnShow()
{
    //m_view->RecacheAllItems();
}


void SCH_PREVIEW_PANEL::setDefaultLayerOrder()
{
    for( int i = 0; (unsigned) i < sizeof( SCH_LAYER_ORDER ) / sizeof( int ); ++i )
    {
        int layer = SCH_LAYER_ORDER[i];
        Q_ASSERT( layer < KIGFX::VIEW::VIEW_MAX_LAYERS );

        m_view->SetLayerOrder( layer, i );
    }
}


void SCH_PREVIEW_PANEL::setDefaultLayerDeps()
{
    // An alias's fields don't know how to substitute in their parent's values, so we
    // don't let them draw themselves.  This means no caching.
    auto target = KIGFX::TARGET_NONCACHED;

    for( int i = 0; i < KIGFX::VIEW::VIEW_MAX_LAYERS; i++ )
        m_view->SetLayerTarget( i, target );

    m_view->SetLayerTarget( LAYER_GP_OVERLAY , KIGFX::TARGET_OVERLAY );
    m_view->SetLayerDisplayOnly( LAYER_GP_OVERLAY ) ;

    m_view->SetLayerTarget( LAYER_SELECT_OVERLAY , KIGFX::TARGET_OVERLAY );
    m_view->SetLayerDisplayOnly( LAYER_SELECT_OVERLAY ) ;

    m_view->SetLayerTarget( LAYER_DRAWINGSHEET , KIGFX::TARGET_NONCACHED );
    m_view->SetLayerDisplayOnly( LAYER_DRAWINGSHEET ) ;
}


KIGFX::SCH_VIEW* SCH_PREVIEW_PANEL::view() const
{
    return static_cast<KIGFX::SCH_VIEW*>( m_view );
}


void SCH_PREVIEW_PANEL::Refresh( bool aEraseBackground, const QRect* aRect )
{
    EDA_DRAW_PANEL_GAL::Refresh( aEraseBackground, aRect );
}


void SCH_PREVIEW_PANEL::onPaint( QPaintEvent& aEvent )
{
    if( isVisible() )
        EDA_DRAW_PANEL_GAL::onPaint( aEvent );
}
