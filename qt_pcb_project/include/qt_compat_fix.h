/*
 * Qt Compatibility Fix Header
 * 
 * This header ensures proper include order to prevent conflicts between
 * Qt headers and Windows SDK headers.
 * 
 * IMPORTANT: This header should be included first in any source file that
 * uses both Qt and system headers to prevent compilation errors.
 */

#ifndef QT_COMPAT_FIX_H
#define QT_COMPAT_FIX_H

// On Windows, ensure Windows headers and system headers are included
// before Qt headers to prevent macro conflicts
#ifdef _WIN32
    // Include critical system headers first
    #include <assert.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <math.h>
    
    // Include Windows headers if needed
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    
    // Undefine problematic Windows macros that conflict with Qt
    #ifdef min
        #undef min
    #endif
    #ifdef max
        #undef max
    #endif
    #ifdef GetObject
        #undef GetObject
    #endif
    #ifdef DrawText
        #undef DrawText
    #endif
#endif

#endif // QT_COMPAT_FIX_H