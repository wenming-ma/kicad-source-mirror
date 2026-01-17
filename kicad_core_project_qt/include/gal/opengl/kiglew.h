
// This file is used for including the proper GLEW header for the platform.

#ifndef KIGLEW_H_
#define KIGLEW_H_

// Pull in Qt platform configuration
#include <QtGlobal>
#include <QtOpenGLWidgets/QOpenGLWidget>

// Apple, in their infinite wisdom, has decided to mark OpenGL as deprecated.
// Luckily we can silence warnings about its deprecation. This is needed on the GLEW
// includes since they transitively include the OpenGL headers.
#define GL_SILENCE_DEPRECATION 1

#if defined( __unix__ ) and not defined( __APPLE__ )

    #ifdef KICAD_USE_EGL

        #ifdef QT_OPENGL_ES
            // Qt was compiled with OpenGL ES support, so use the EGL header for GLEW
            #include <GL/eglew.h>
        #else
            #error "KICAD_USE_EGL can only be used when Qt is compiled with OpenGL ES support"
        #endif

    #else   // KICAD_USE_EGL

        #ifdef QT_OPENGL_ES
            #error "KICAD_USE_EGL must be defined since Qt has been compiled with OpenGL ES support"
        #else
            // Qt wasn't compiled with OpenGL ES, so use the X11 GLEW
            #include <GL/glxew.h>
        #endif

    #endif  // KICAD_USE_EGL

#else   // defined( __unix__ ) and not defined( __APPLE__ )

    // Non-Unix platforms only need the normal GLEW include
    #include <GL/glew.h>

#endif  // defined( __unix__ ) and not defined( __APPLE__ )

#ifdef _WIN32

    #include <GL/wglew.h>

#endif  // _WIN32

#endif  // KIGLEW_H_
