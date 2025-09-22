
#include <confirm.h> // DisplayError

#include <gal/opengl/kiglew.h> // Must be included first

#include <stdexcept>
#include <QDebug> // Qt logging
#include <QString>


/**
 * Flag to enable debug output of the GAL OpenGL error checking.
 *
 * Use "KICAD_GAL_OPENGL_ERROR" to enable GAL OpenGL error tracing.
 *
 * @ingroup trace_env_vars
 */
static const QString traceGalOpenGlError = "KICAD_GAL_OPENGL_ERROR";


int checkGlError( const std::string& aInfo, const char* aFile, int aLine, bool aThrow )
{
    int      result = glGetError();
    QString errorMsg;

    switch( result )
    {
    case GL_NO_ERROR:
        // all good
        break;

    case GL_INVALID_ENUM:
        errorMsg = QString::asprintf( "Error: %s: invalid enum", aInfo.c_str() );
        break;

    case GL_INVALID_VALUE:
        errorMsg = QString::asprintf( "Error: %s: invalid value", aInfo.c_str() );
        break;

    case GL_INVALID_OPERATION:
        errorMsg = QString::asprintf( "Error: %s: invalid operation", aInfo.c_str() );
        break;

    case GL_INVALID_FRAMEBUFFER_OPERATION:
    {
        GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

        if( status != GL_FRAMEBUFFER_COMPLETE_EXT )
        {
            switch( status )
            {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                errorMsg = "The framebuffer attachment points are incomplete.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                errorMsg = "No images attached to the framebuffer.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                errorMsg = "The framebuffer does not have at least one image attached to it.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                errorMsg = "The framebuffer read buffer is incomplete.";
                break;

            case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                errorMsg = "The combination of internal formats of the attached images violates "
                           "an implementation dependent set of restrictions.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
                errorMsg = "GL_RENDERBUFFER_SAMPLES is not the same for all attached render "
                           "buffers.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT:
                errorMsg = "Framebuffer incomplete layer targets errors.";
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                errorMsg = "Framebuffer attachments have different dimensions";
                break;

            default:
                errorMsg = QString::asprintf( "Unknown incomplete framebuffer error id %X", status );
            }
        }
        else
        {
            errorMsg = QString::asprintf( "Error: %s: invalid framebuffer operation", aInfo.c_str() );
        }
    }
    break;

    case GL_OUT_OF_MEMORY:
        errorMsg = QString::asprintf( "Error: %s: out of memory", aInfo.c_str() );
        break;

    case GL_STACK_UNDERFLOW:
        errorMsg = QString::asprintf( "Error: %s: stack underflow", aInfo.c_str() );
        break;

    case GL_STACK_OVERFLOW:
        errorMsg = QString::asprintf( "Error: %s: stack overflow", aInfo.c_str() );
        break;

    default:
        errorMsg = QString::asprintf( "Error: %s: unknown error", aInfo.c_str() );
        break;
    }

    if( result != GL_NO_ERROR )
    {
        if( aThrow )
        {
            qDebug() << traceGalOpenGlError << QString::asprintf( "Throwing exception for glGetError() '%s' "
                                                  "in file '%s' on line %d.",
                        qPrintable(errorMsg),
                        aFile,
                        aLine );

            throw std::runtime_error( errorMsg.toStdString() );
        }
        else
        {
            QString msg = QString::asprintf( "glGetError() '%s' in file '%s' on line %d.",
                                             qPrintable(errorMsg),
                                             aFile,
                                             aLine );

            DisplayErrorMessage( nullptr, "OpenGL Error", errorMsg );
        }
    }

    return result;
}


// debugMsgCallback is a callback function for glDebugMessageCallback.
// It must have the right type ( GLAPIENTRY )
static void GLAPIENTRY debugMsgCallback( GLenum aSource, GLenum aType, GLuint aId, GLenum aSeverity,
                                         GLsizei aLength, const GLchar* aMessage,
                                         const void* aUserParam )
{
    switch( aSeverity )
    {
    case GL_DEBUG_SEVERITY_HIGH:
        qDebug() << traceGalOpenGlError << QString::asprintf( "OpenGL ERROR: %s", aMessage );
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        qDebug() << traceGalOpenGlError << QString::asprintf( "OpenGL WARNING: %s", aMessage );
        break;
    case GL_DEBUG_SEVERITY_LOW:
        qDebug() << traceGalOpenGlError << QString::asprintf( "OpenGL INFO: %s", aMessage );
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return;
    }
}


void enableGlDebug( bool aEnable )
{
    if( aEnable )
    {
        glEnable( GL_DEBUG_OUTPUT );
        glDebugMessageCallback( (GLDEBUGPROC) debugMsgCallback, nullptr );
    }
    else
    {
        glDisable( GL_DEBUG_OUTPUT );
    }
}
