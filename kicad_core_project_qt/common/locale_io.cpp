#include <locale_io.h>
#include <QLocale>
#include <clocale>

// When reading/writing files, we need to switch to setlocale( LC_NUMERIC, "C" ).
// Works fine to read/write files with floating point numbers.
// We can call setlocale( LC_NUMERIC, "C" ) or QLocale::setDefault(QLocale::C)
// Qt discourages a direct call to setlocale
// However, for us, calling QLocale::setDefault(QLocale::C) has a unwanted effect:
// The I18N translations are no longer active, because the English dictionary is selected.
// To read files, this is not a major issues, but the result can differ
// from using setlocale(xx, "C").
// Previously, we used only setlocale( LC_NUMERIC, "C" )
//
// Known issues are
// on MSW
//    using setlocale( LC_NUMERIC, "C" ) generates an alert message in debug mode,
//    and this message ("Decimal separator mismatch") must be disabled.
//    But calling QLocale::setDefault(QLocale::C) works fine
// On unix:
//    calling QLocale::setDefault(QLocale::C) breaks env vars containing non ASCII7 chars.
//    these env vars return a empty string from qgetenv() in many cases, and if such a
//    var must be read after calling QLocale::setDefault(QLocale::C), it looks like empty
//
// So use QLocale on Windows and setlocale on unix

// On Windows, when using setlocale, a Qt debug message is generated
// in some cases (reading a bitmap for instance)
// So we disable debug messages during the time a file is read or written
#if !USE_QTLOCALE
#if defined( _WIN32 ) && defined( DEBUG )

#include <QString>
#include <QDebug>

// Qt message handler function to filter Qt alert messages when reading/writing a file
// when switching the locale to LC_NUMERIC, "C"
// It is used in class LOCALE_IO to hide a useless (in KiCad) Qt alert message
void KiMessageFilter( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
    if( !msg.contains( "Decimal separator mismatch" ) )
    {
        // Forward to default Qt message handler
        qInstallMessageHandler( nullptr );
        qt_message_output( type, context, msg );
        qInstallMessageHandler( KiMessageFilter );
    }
}
#endif
#endif

// allow for nesting of LOCALE_IO instantiations
static std::atomic<unsigned int> locale_count( 0 );

LOCALE_IO::LOCALE_IO()
#if USE_QTLOCALE
    : m_qtLocale( nullptr )
#endif
{
    // use thread safe, atomic operation
    if( locale_count++ == 0 )
    {
#if USE_QTLOCALE
        m_qtLocale = new QLocale( QLocale::C );
#else
        // Store the user locale name, to restore this locale later, in dtor
        m_user_locale = setlocale( LC_NUMERIC, nullptr );
#if defined( _WIN32 ) && defined( DEBUG )
        // Disable Qt debug messages
        qInstallMessageHandler( KiMessageFilter );
#endif
        // Switch the locale to C locale, to read/write files with fp numbers
        setlocale( LC_NUMERIC, "C" );
#endif
    }
}


LOCALE_IO::~LOCALE_IO()
{
    // use thread safe, atomic operation
    if( --locale_count == 0 )
    {
        // revert to the user locale
#if USE_QTLOCALE
        delete m_qtLocale;      // Deleting m_qtLocale restored previous locale
        m_qtLocale = nullptr;
#else
        setlocale( LC_NUMERIC, m_user_locale.c_str() );
#if defined( _WIN32 ) && defined( DEBUG )
        // Restore default Qt message handler
        qInstallMessageHandler( nullptr );
#endif
#endif
    }
}
