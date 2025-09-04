
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

extern KICOMMON_API const QChar* const traceFindItem;

extern KICOMMON_API const QChar* const traceFindReplace;

extern KICOMMON_API const QChar* const kicadTraceCoords;

extern KICOMMON_API const QChar* const kicadTraceKeyEvent;

extern KICOMMON_API const QChar* const kicadTraceToolStack;

extern KICOMMON_API const QChar* const kicadTraceCoroutineStack;

extern KICOMMON_API const QChar* const traceAutoSave;

extern KICOMMON_API const QChar* const traceSchLibMem;

extern KICOMMON_API const QChar* const traceSchPlugin;

extern KICOMMON_API const QChar* const traceSchLegacyPlugin;

extern KICOMMON_API const QChar* const traceKicadPcbPlugin;

extern KICOMMON_API const QChar* const traceGedaPcbPlugin;

extern KICOMMON_API const QChar* const tracePrinting;

extern KICOMMON_API const QChar* const tracePathsAndFiles;

extern KICOMMON_API const QChar* const traceLocale;

extern KICOMMON_API const QChar* const traceFonts;

extern KICOMMON_API const QChar* const traceScreen;

extern KICOMMON_API const QChar* const traceDisplayLocation;

extern KICOMMON_API const QChar* const traceZoomScroll;

extern KICOMMON_API const QChar* const traceSymbolResolver;

extern KICOMMON_API const QChar* const traceSchSheetPaths;

extern KICOMMON_API const QChar* const traceEnvVars;

extern KICOMMON_API const QChar* const traceGalProfile;

extern KICOMMON_API const QChar* const traceKiCad2Step;

extern KICOMMON_API const QChar* const traceUiProfile;

extern KICOMMON_API const QChar* const traceGit;

extern KICOMMON_API const QChar* const traceEagleIo;

extern KICOMMON_API const QChar* const traceDesignBlocks;


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

    void DoTrace( const QString aWhat, const QChar* aFmt, ... )
    {
        va_list argptr;
        va_start( argptr, aFmt );
        traceV( aWhat, aFmt, argptr );
        va_end( argptr );
    }


    bool IsTraceEnabled( const QString& aWhat );

private:
    void traceV( const QString& aWhat, const QString& aFmt, va_list vargs );
    void init();

    std::map<QString, bool> m_enabledTraces;
    bool                     m_globalTraceEnabled;
    bool                     m_printAllTraces;
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
