

#include <gal/cairo/cairo_compositor.h>
#include <QtCore/QDebug>

using namespace KIGFX;

CAIRO_COMPOSITOR::CAIRO_COMPOSITOR( cairo_t** aMainContext ) :
        m_current( 0 ),
        m_currentContext( aMainContext ),
        m_mainContext( *aMainContext ),
        m_currentAntialiasingMode( CAIRO_ANTIALIAS_DEFAULT )
{
    // Do not have uninitialized members:
    cairo_matrix_init_identity( &m_matrix );
    m_stride = 0;
    m_bufferSize = 0;
}


CAIRO_COMPOSITOR::~CAIRO_COMPOSITOR()
{
    clean();
}


void CAIRO_COMPOSITOR::Initialize()
{
    // Nothing has to be done
}


void CAIRO_COMPOSITOR::SetAntialiasingMode( CAIRO_ANTIALIASING_MODE aMode )
{
    switch( aMode )
    {
    case CAIRO_ANTIALIASING_MODE::FAST: m_currentAntialiasingMode = CAIRO_ANTIALIAS_FAST; break;
    case CAIRO_ANTIALIASING_MODE::GOOD: m_currentAntialiasingMode = CAIRO_ANTIALIAS_GOOD; break;
    default: m_currentAntialiasingMode = CAIRO_ANTIALIAS_NONE;
    }

    clean();
}


void CAIRO_COMPOSITOR::Resize( unsigned int aWidth, unsigned int aHeight )
{
    clean();

    m_width = aWidth;
    m_height = aHeight;

    m_stride = cairo_format_stride_for_width( CAIRO_FORMAT_ARGB32, m_width );
    m_bufferSize = m_stride * m_height;
}


unsigned int CAIRO_COMPOSITOR::CreateBuffer()
{
    // Pixel storage
    BitmapPtr bitmap = new uint8_t[m_bufferSize]();

    // Create the Cairo surface
    cairo_surface_t* surface = cairo_image_surface_create_for_data(
            (unsigned char*) bitmap, CAIRO_FORMAT_ARGB32, m_width, m_height, m_stride );
    cairo_t* context = cairo_create( surface );

#ifdef DEBUG
    cairo_status_t status = cairo_status( context );
    Q_ASSERT_X( status == CAIRO_STATUS_SUCCESS, "CAIRO_COMPOSITOR", "Cairo context creation error" );
#endif /* DEBUG */

    // Set default settings for the buffer
    cairo_set_antialias( context, m_currentAntialiasingMode );

    // Use the same transformation matrix as the main context
    cairo_get_matrix( m_mainContext, &m_matrix );
    cairo_set_matrix( context, &m_matrix );

    // Store the new buffer
    CAIRO_BUFFER buffer = { context, surface, bitmap };
    m_buffers.push_back( buffer );

    return usedBuffers();
}


void CAIRO_COMPOSITOR::SetBuffer( unsigned int aBufferHandle )
{
    Q_ASSERT_X( aBufferHandle <= usedBuffers(), "CAIRO_COMPOSITOR", "Tried to use a not existing buffer" );

    // Get currently used transformation matrix, so it can be applied to the new buffer
    cairo_get_matrix( *m_currentContext, &m_matrix );

    m_current = aBufferHandle - 1;
    *m_currentContext = m_buffers[m_current].context;

    // Apply the current transformation matrix
    cairo_set_matrix( *m_currentContext, &m_matrix );
}


void CAIRO_COMPOSITOR::Begin()
{
}


void CAIRO_COMPOSITOR::ClearBuffer( const COLOR4D& aColor )
{
    // Clear the pixel storage
    memset( m_buffers[m_current].bitmap, 0x00, m_bufferSize );
}


void CAIRO_COMPOSITOR::DrawBuffer( unsigned int aSourceHandle, unsigned int aDestHandle,
                                   cairo_operator_t op )
{
    Q_ASSERT_X( aSourceHandle <= usedBuffers() && aDestHandle <= usedBuffers(),
                  "CAIRO_COMPOSITOR", "Tried to use a not existing buffer" );

    // Reset the transformation matrix, so it is possible to composite images using
    // screen coordinates instead of world coordinates
    cairo_get_matrix( m_mainContext, &m_matrix );
    cairo_identity_matrix( m_mainContext );

    // Draw the selected buffer contents
    cairo_t* ct = cairo_create( m_buffers[aDestHandle - 1].surface );
    cairo_set_operator( ct, op );
    cairo_set_source_surface( ct, m_buffers[aSourceHandle - 1].surface, 0.0, 0.0 );
    cairo_paint( ct );
    cairo_destroy( ct );

    // Restore the transformation matrix
    cairo_set_matrix( m_mainContext, &m_matrix );
}


void CAIRO_COMPOSITOR::DrawBuffer( unsigned int aBufferHandle )
{
    Q_ASSERT_X( aBufferHandle <= usedBuffers(), "CAIRO_COMPOSITOR", "Tried to use a not existing buffer" );

    // Reset the transformation matrix, so it is possible to composite images using
    // screen coordinates instead of world coordinates
    cairo_get_matrix( m_mainContext, &m_matrix );
    cairo_identity_matrix( m_mainContext );

    // Draw the selected buffer contents
    cairo_set_source_surface( m_mainContext, m_buffers[aBufferHandle - 1].surface, 0.0, 0.0 );
    cairo_paint( m_mainContext );

    // Restore the transformation matrix
    cairo_set_matrix( m_mainContext, &m_matrix );
}


void CAIRO_COMPOSITOR::Present()
{
}


void CAIRO_COMPOSITOR::clean()
{
    CAIRO_BUFFERS::const_iterator it;

    for( it = m_buffers.begin(); it != m_buffers.end(); ++it )
    {
        cairo_destroy( it->context );
        cairo_surface_destroy( it->surface );
        delete[] it->bitmap;
    }

    m_buffers.clear();
}
