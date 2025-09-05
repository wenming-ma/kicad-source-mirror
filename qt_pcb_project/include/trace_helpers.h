
#ifndef _TRACE_HELPERS_H_
#define _TRACE_HELPERS_H_

#include <kicommon.h>
#include <QString>
#include <QStringList>
#include <QKeyEvent>
#include <QLoggingCategory>

#include <stdarg.h>
#include <map>

// Trace environment variables for debug output control

extern KICOMMON_API const char* const traceFindItem;

extern KICOMMON_API const char* const traceFindReplace;

extern KICOMMON_API const char* const kicadTraceCoords;

extern KICOMMON_API const char* const kicadTraceKeyEvent;

extern KICOMMON_API const char* const kicadTraceToolStack;

extern KICOMMON_API const char* const kicadTraceCoroutineStack;

extern KICOMMON_API const char* const traceAutoSave;

extern KICOMMON_API const char* const traceSchLibMem;

extern KICOMMON_API const char* const traceSchPlugin;

extern KICOMMON_API const char* const traceSchLegacyPlugin;

extern KICOMMON_API const char* const traceKicadPcbPlugin;

extern KICOMMON_API const char* const traceGedaPcbPlugin;

extern KICOMMON_API const char* const tracePrinting;

extern KICOMMON_API const char* const tracePathsAndFiles;

extern KICOMMON_API const char* const traceLocale;

extern KICOMMON_API const char* const traceFonts;

extern KICOMMON_API const char* const traceScreen;

extern KICOMMON_API const char* const traceDisplayLocation;

extern KICOMMON_API const char* const traceZoomScroll;

extern KICOMMON_API const char* const traceSymbolResolver;

extern KICOMMON_API const char* const traceSchSheetPaths;

extern KICOMMON_API const char* const traceEnvVars;

extern KICOMMON_API const char* const traceGalProfile;

extern KICOMMON_API const char* const traceKiCad2Step;

extern KICOMMON_API const char* const traceUiProfile;

extern KICOMMON_API const char* const traceGit;

extern KICOMMON_API const char* const traceEagleIo;

extern KICOMMON_API const char* const traceDesignBlocks;


extern KICOMMON_API QString dump( const QKeyEvent& aEvent );

extern KICOMMON_API QString dump( const QStringList& aArray );

class KICOMMON_API TRACE_MANAGER
{
public:
    TRACE_MANAGER() :
        m_globalTraceEnabled( false ),
        m_printAllTraces (false )
    {};
    ~TRACE_MANAGER(){};

    static TRACE_MANAGER& Instance();

    void Trace( const QString& aWhat, const QString& aFmt, ... );

    void DoTrace( const QString& aWhat, const char* aFmt, ... )
    {
        va_list argptr;
        va_start( argptr, aFmt );
        traceV( aWhat, QString(aFmt), argptr );
        va_end( argptr );
    }


    bool IsTraceEnabled( const QString& aWhat );

private:
    void traceV( const QString& aWhat, const QString& aFmt, va_list vargs );
    void init();

    std::map<std::string, bool> m_enabledTraces;
    bool                         m_globalTraceEnabled;
    bool                         m_printAllTraces;
};

#define KI_TRACE( aWhat, ... )                                                                     \
    if( TRACE_MANAGER::Instance().IsTraceEnabled( aWhat ) )                                        \
    {                                                                                              \
        TRACE_MANAGER::Instance().Trace( aWhat, __VA_ARGS__ );                                     \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
    }

#endif    // _TRACE_HELPERS_H_
