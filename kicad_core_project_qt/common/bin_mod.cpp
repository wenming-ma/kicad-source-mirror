#include <bin_mod.h>
#include <pgm_base.h>
#include <settings/app_settings.h>
#include <settings/settings_manager.h>


BIN_MOD::BIN_MOD( const char* aName ) :
    m_name( aName ),
    m_config( nullptr )
{
}


void BIN_MOD::Init()
{
    // Prepare On Line Help. Use only lower case for help file names, in order to
    // avoid problems with upper/lower case file names under windows and unix.
    // Help files are now using html format.
    // Old help files used pdf format.
    // so when searching a help file, the .html file will be searched,
    // and if not found, the .pdf file  will be searched.
    m_help_file = QString::fromUtf8( m_name );     // no ext given. can be .html or .pdf
}


void BIN_MOD::End()
{
    if( m_config )
    {
        // The settings manager will outlive this module so we need to clean up the module level
        // settings here instead of leaving it up to the manager
        Pgm().GetSettingsManager().FlushAndRelease( m_config );
        m_config = nullptr;
    }
}


BIN_MOD::~BIN_MOD()
{
}