
#include "graphics_importer.h"

#include <eda_item.h>
#include <eda_shape.h>

#include "graphics_import_plugin.h"

#include <QDebug>

GRAPHICS_IMPORTER::GRAPHICS_IMPORTER()
{
    m_millimeterToIu = 1.0;
    m_lineWidth = DEFAULT_LINE_WIDTH_DFX;
    m_scale = VECTOR2D( 1.0, 1.0 );
    m_originalWidth = 0.0;
    m_originalHeight = 0.0;
}


bool GRAPHICS_IMPORTER::Load( const QString& aFileName )
{
    m_items.clear();

    if( !m_plugin )
    {
        Q_ASSERT_X( false, "GRAPHICS_IMPORTER::Load", "Plugin must be set before load." );
        return false;
    }

    m_plugin->SetImporter( this );

    bool ret = m_plugin->Load( aFileName );

    if( ret )
    {
        m_originalWidth = m_plugin->GetImageWidth();
        m_originalHeight = m_plugin->GetImageHeight();
    }

    return ret;
}


bool GRAPHICS_IMPORTER::Import( const VECTOR2D& aScale )
{
    if( !m_plugin )
    {
        Q_ASSERT_X( false, "GRAPHICS_IMPORTER::Import", "Plugin must be set before import." );
        return false;
    }

    SetScale( aScale );

    m_plugin->SetImporter( this );

    bool success = false;

    try
    {
        success = m_plugin->Import();
    }
    catch( const std::bad_alloc& )
    {
        // Memory exhaustion
        // TODO report back an error message
        return false;
    }

    return success;
}


void GRAPHICS_IMPORTER::NewShape( POLY_FILL_RULE aFillRule )
{
    m_shapeFillRules.push_back( aFillRule );
}


void GRAPHICS_IMPORTER::addItem( std::unique_ptr<EDA_ITEM> aItem )
{
    m_items.emplace_back( std::move( aItem ) );
}


bool GRAPHICS_IMPORTER::setupSplineOrLine( EDA_SHAPE& aSpline, int aAccuracy )
{
    aSpline.SetShape( SHAPE_T::BEZIER );

    bool degenerate = false;

    SEG s_e{ aSpline.GetStart(), aSpline.GetEnd() };
    SEG s_c1{ aSpline.GetStart(), aSpline.GetBezierC1() };
    SEG e_c2{ aSpline.GetEnd(), aSpline.GetBezierC2() };

    if( s_e.ApproxCollinear( s_c1 ) && s_e.ApproxCollinear( e_c2 ) )
        degenerate = true;

    if( !degenerate )
    {
        aSpline.RebuildBezierToSegmentsPointsList( aAccuracy );
        if( aSpline.GetBezierPoints().size() <= 2 )
        {
            degenerate = true;
        }
    }

    // If the spline is degenerated (i.e. a segment) add it as segment or discard it if
    // null (i.e. very small) length
    if( degenerate )
    {
        aSpline.SetShape( SHAPE_T::SEGMENT );

        // segment smaller than MIN_SEG_LEN_ACCEPTABLE_NM nanometers are skipped.
        constexpr int MIN_SEG_LEN_ACCEPTABLE_NM = 20;

        if( s_e.Length() < MIN_SEG_LEN_ACCEPTABLE_NM )
            return false;
    }

    return true;
}
