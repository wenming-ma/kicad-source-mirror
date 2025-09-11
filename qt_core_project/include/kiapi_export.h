#ifndef KIAPI_EXPORT_H
#define KIAPI_EXPORT_H

// Define KIAPI_IMPORTEXPORT for protobuf generated files
#if defined(_WIN32)
    #define KIAPI_IMPORTEXPORT __declspec(dllimport)
#elif defined(__GNUC__) && __GNUC__ >= 4
    #define KIAPI_IMPORTEXPORT __attribute__ ((visibility("default")))
#else
    #define KIAPI_IMPORTEXPORT
#endif

#endif // KIAPI_EXPORT_H