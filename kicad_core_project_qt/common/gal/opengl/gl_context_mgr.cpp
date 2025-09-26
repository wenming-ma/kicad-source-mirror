#include <gal/opengl/gl_context_mgr.h>
#include <QDebug>
#include <QOpenGLContext>
#include <QWindow>
#include <QtOpenGLWidgets/QOpenGLWidget>


QOpenGLContext* GL_CONTEXT_MANAGER::CreateCtx( QOpenGLWidget* aCanvas, const QOpenGLContext* aOther )
{
    QOpenGLContext* context = new QOpenGLContext();
    if( !context )
        return nullptr;

    if( aOther )
        context->setShareContext( const_cast<QOpenGLContext*>( aOther ) );
    
    context->setFormat( aCanvas->format() );
    
    if( !context->create() )
    {
        delete context;
        return nullptr;
    }

    m_glContexts.insert( std::make_pair( context, aCanvas ) );

    return context;
}


void GL_CONTEXT_MANAGER::DestroyCtx( QOpenGLContext* aContext )
{
    if( m_glContexts.count( aContext ) )
    {
        m_glContexts.erase( aContext );
        delete aContext;
    }
    else
    {
        // Do not delete unknown GL contexts
        Q_ASSERT( false );
    }

    if( m_glCtx == aContext )
        m_glCtx = nullptr;
}


void GL_CONTEXT_MANAGER::DeleteAll()
{
    m_glCtxMutex.lock();

    for( auto& ctx : m_glContexts )
        delete ctx.first;

    m_glContexts.clear();
    m_glCtx = nullptr;
    m_glCtxMutex.unlock();
}


void GL_CONTEXT_MANAGER::LockCtx( QOpenGLContext* aContext, QOpenGLWidget* aCanvas )
{
    if( !aContext || m_glContexts.count( aContext ) == 0 )
        return;

    m_glCtxMutex.lock();
    QOpenGLWidget* canvas = aCanvas ? aCanvas : m_glContexts.at( aContext );

    // Make context current on the surface
    if( canvas )
    {
        // In Qt, QOpenGLWidget provides its surface through the window handle
        // QOpenGLContext::makeCurrent() expects a QSurface*, not a QOpenGLWidget*
        if( canvas->windowHandle() )
        {
            aContext->makeCurrent( canvas->windowHandle() );
        }
    }

    m_glCtx = aContext;
}


void GL_CONTEXT_MANAGER::UnlockCtx( QOpenGLContext* aContext )
{
    if( !aContext || m_glContexts.count( aContext ) == 0 )
        return;

    if( m_glCtx == aContext )
    {
        m_glCtxMutex.unlock();
        m_glCtx = nullptr;
    }
    else
    {
        QString errorMsg = QString( "Trying to unlock GL context mutex from "
                          "a wrong context: aContext %1 m_glCtx %2" )
                          .arg( reinterpret_cast<quintptr>( aContext ) )
                          .arg( reinterpret_cast<quintptr>( m_glCtx ) );
        Q_ASSERT_X( false, "GL_CONTEXT_MANAGER::UnlockCtx", errorMsg.toLocal8Bit().data() );
    }
}
