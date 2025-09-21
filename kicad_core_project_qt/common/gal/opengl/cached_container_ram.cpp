
#include <gal/opengl/cached_container_ram.h>
#include <gal/opengl/vertex_manager.h>
#include <gal/opengl/vertex_item.h>
#include <gal/opengl/shader.h>
#include <gal/opengl/utils.h>

#include <confirm.h>
#include <list>
#include <cassert>

#include <QLoggingCategory>
#include <QDebug>
#ifdef KICAD_GAL_PROFILE
#include <core/profile.h>
#endif /* KICAD_GAL_PROFILE */

using namespace KIGFX;


// Flag to enable debug output of the GAL OpenGL cached container.
// Use "KICAD_GAL_CACHED_CONTAINER" to enable GAL OpenGL cached container tracing.
Q_LOGGING_CATEGORY(traceGalCachedContainer, "KICAD_GAL_CACHED_CONTAINER")


CACHED_CONTAINER_RAM::CACHED_CONTAINER_RAM( unsigned int aSize ) :
        CACHED_CONTAINER( aSize ),
        m_verticesBuffer( 0 )
{
    glGenBuffers( 1, &m_verticesBuffer );
    checkGlError( "generating vertices buffer", __FILE__, __LINE__ );

    m_vertices = static_cast<VERTEX*>( malloc( aSize * VERTEX_SIZE ) );

    if( !m_vertices )
        throw std::bad_alloc();
}


CACHED_CONTAINER_RAM::~CACHED_CONTAINER_RAM()
{
    if( glDeleteBuffers )
        glDeleteBuffers( 1, &m_verticesBuffer );

    free( m_vertices );
}


void CACHED_CONTAINER_RAM::Unmap()
{
    if( !m_dirty )
        return;

    // Upload vertices coordinates and shader types to GPU memory
    glBindBuffer( GL_ARRAY_BUFFER, m_verticesBuffer );
    checkGlError( "binding vertices buffer", __FILE__, __LINE__ );
    glBufferData( GL_ARRAY_BUFFER, m_maxIndex * VERTEX_SIZE, m_vertices, GL_STREAM_DRAW );
    checkGlError( "transferring vertices", __FILE__, __LINE__ );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    checkGlError( "unbinding vertices buffer", __FILE__, __LINE__ );
}


bool CACHED_CONTAINER_RAM::defragmentResize( unsigned int aNewSize )
{
    qCDebug(traceGalCachedContainer) << "Resizing & defragmenting container (memcpy) from" << m_currentSize << "to" << aNewSize;

    // No shrinking if we cannot fit all the data
    if( usedSpace() > aNewSize )
        return false;

#ifdef KICAD_GAL_PROFILE
    PROF_TIMER totalTime;
#endif /* KICAD_GAL_PROFILE */

    VERTEX* newBufferMem = static_cast<VERTEX*>( malloc( aNewSize * VERTEX_SIZE ) );

    if( !newBufferMem )
        throw std::bad_alloc();

    defragment( newBufferMem );

    // Switch to the new vertex buffer
    free( m_vertices );
    m_vertices = newBufferMem;

#ifdef KICAD_GAL_PROFILE
    totalTime.Stop();

    qCDebug(traceGalCachedContainer) << "Defragmented container storing" << (m_currentSize - m_freeSpace) << "vertices /" << totalTime.msecs() << "ms";
#endif /* KICAD_GAL_PROFILE */

    m_freeSpace += ( aNewSize - m_currentSize );
    m_currentSize = aNewSize;

    // Now there is only one big chunk of free memory
    m_freeChunks.clear();
    m_freeChunks.insert( std::make_pair( m_freeSpace, m_currentSize - m_freeSpace ) );
    m_dirty = true;

    return true;
}
