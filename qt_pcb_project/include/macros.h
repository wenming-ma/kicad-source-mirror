// QT_TRANSFORMATION_COMPLETED

#ifndef MACROS_H
#define MACROS_H

#include <QString>
#include <QDebug>

#if defined( __has_attribute )
    #define KI_HAS_ATTRIBUTE( x ) __has_attribute( x )
#else
    #define KI_HAS_ATTRIBUTE( x ) 0
#endif

// Based on the declaration inside the LLVM source code
#if defined( __cplusplus ) && defined( __has_cpp_attribute )
    #define KI_HAS_CPP_ATTRIBUTE( x ) __has_cpp_attribute( x )
#else
    #define KI_HAS_CPP_ATTRIBUTE( x ) 0
#endif

#if __cplusplus >= 201703L
    // C++ 17 includes this macro on all compilers
    #define KI_FALLTHROUGH [[fallthrough]]

#elif KI_HAS_CPP_ATTRIBUTE( clang::fallthrough )
    // Clang provides this attribute to silence the "-Wimplicit-fallthrough" warning
    #define KI_FALLTHROUGH [[clang::fallthrough]]

#elif KI_HAS_CPP_ATTRIBUTE( gnu::fallthrough )
    // GNU-specific C++ attribute to silencing the warning
    #define KI_FALLTHROUGH [[gnu::fallthrough]]

#elif defined( __GNUC__ ) && __GNUC__ >= 7
    // GCC 7+ includes the "-Wimplicit-fallthrough" warning, and this attribute to silence it
    #define KI_FALLTHROUGH __attribute__ ((fallthrough))

#else
    // In every other case, don't do anything
    #define KI_FALLTHROUGH ( ( void ) 0 )

#endif

#define TO_STR2(x) #x
#define TO_STR(x) TO_STR2(x)

#define UNIMPLEMENTED_FOR( type ) \
        Q_ASSERT_X( false, __FUNCTION__, QString( "%1: unimplemented for %2" ).arg( __FUNCTION__ ).arg( type ).toLocal8Bit().constData() )

#endif // MACROS_H
