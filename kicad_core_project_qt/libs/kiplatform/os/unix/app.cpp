
#include <kiplatform/app.h>

#include <glib.h>

#include <QString>
#include <QWidget>


/*
 * Function to attach to the glib logger to eat the output it gives so we don't
 * get the message spam on the terminal from Qt's usage of the GTK API.
 */
static GLogWriterOutput nullLogWriter( GLogLevelFlags log_level, const GLogField* fields,
                                       gsize n_fields, gpointer user_data )
{
    return G_LOG_WRITER_HANDLED;
}


bool KIPLATFORM::APP::Init()
{
#if !defined( KICAD_SHOW_GTK_MESSAGES )
    // Attach a logger that will consume the annoying GTK error messages
    g_log_set_writer_func( nullLogWriter, nullptr, nullptr );
#endif

    return true;
}


bool KIPLATFORM::APP::AttachConsole( bool aTryAlloc )
{
    // Not implemented on this platform
    return true;
}


bool KIPLATFORM::APP::IsOperatingSystemUnsupported()
{
    // Not implemented on this platform
    return false;
}


bool KIPLATFORM::APP::RegisterApplicationRestart( const QString& aCommandLine )
{
    // Not implemented on this platform
    return true;
}


bool KIPLATFORM::APP::UnregisterApplicationRestart()
{
    // Not implemented on this platform
    return true;
}


bool KIPLATFORM::APP::SupportsShutdownBlockReason()
{
    return false;
}


void KIPLATFORM::APP::RemoveShutdownBlockReason( QWidget* aWindow )
{
}


void KIPLATFORM::APP::SetShutdownBlockReason( QWidget* aWindow, const QString& aReason )
{
}


void KIPLATFORM::APP::ForceTimerMessagesToBeCreatedIfNecessary()
{
}


void KIPLATFORM::APP::AddDynamicLibrarySearchPath( const QString& aPath )
{
}