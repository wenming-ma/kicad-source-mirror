
#include <QDebug>

#include <settings/json_settings_internals.h>
#include <settings/nested_settings.h>
#include <locale_io.h>


NESTED_SETTINGS::NESTED_SETTINGS( const std::string& aName, int aVersion, JSON_SETTINGS* aParent,
                                  const std::string& aPath, bool aLoadFromFile ) :
        JSON_SETTINGS( aName, SETTINGS_LOC::NESTED, aVersion ),
        m_parent( aParent ), m_path( aPath )
{
    SetParent( aParent, aLoadFromFile );
}


NESTED_SETTINGS::~NESTED_SETTINGS()
{
    if( m_parent )
        m_parent->ReleaseNestedSettings( this );
}


bool NESTED_SETTINGS::LoadFromFile( const QString& aDirectory )
{
    m_internals->clear();
    bool success = false;

    if( m_parent )
    {
        nlohmann::json::json_pointer ptr = m_internals->PointerFromString( m_path );

        if( m_parent->m_internals->contains( ptr ) )
        {
            try
            {
                m_internals->update( m_parent->m_internals->at( ptr ) );

                qDebug() << "Loaded NESTED_SETTINGS" << GetFilename();

                success = true;
            }
            catch( ... )
            {
                qDebug() << "NESTED_SETTINGS" << m_filename << ": Could not load from"
                         << m_parent->GetFilename() << "at" << m_path;
            }
        }
    }

    if( success )
    {
        int filever = -1;

        try
        {
            filever = m_internals->Get<int>( "meta.version" );
        }
        catch( ... )
        {
            qDebug() << m_filename << ": nested settings version could not be read!";
            success = false;
        }

        if( filever >= 0 && filever < m_schemaVersion )
        {
            qDebug() << m_filename << ": attempting migration from version" << filever << "to" << m_schemaVersion;

            bool migrated = false;

            try
            {
                migrated = Migrate();
            }
            catch( ... )
            {
                success = false;
            }

            if( !migrated )
            {
                qDebug() << GetFullFilename() << ": migration failed!";
                success = false;
            }
        }
        else if( filever > m_schemaVersion )
        {
            qDebug() << m_filename << ": warning: nested settings version" << filever
                     << "is newer than latest (" << m_schemaVersion << ")";
        }
        else if( filever >= 0 )
        {
            qDebug() << m_filename << ": schema version" << filever << "is current";
        }
    }

    Load();

    return success;
}


bool NESTED_SETTINGS::SaveToFile( const QString& aDirectory, bool aForce )
{
    if( !m_parent )
        return false;

    LOCALE_IO dummy;

    try
    {
        bool modified = Store();

        auto jsonObjectInParent = m_parent->GetJson( m_path );

        if( !jsonObjectInParent )
            modified = true;
        else if( !nlohmann::json::diff( *m_internals, jsonObjectInParent.value() ).empty() )
            modified = true;

        if( modified || aForce )
        {
            ( *m_parent->m_internals )[m_path].update( *m_internals );

            qDebug() << "Stored NESTED_SETTINGS" << GetFilename() << "with schema" << m_schemaVersion;
        }

        return modified;
    }
    catch( ... )
    {
        qDebug() << "NESTED_SETTINGS" << m_filename << ": Could not store to"
                 << m_parent->GetFilename() << "at" << m_path;

        return false;
    }
}


void NESTED_SETTINGS::SetParent( JSON_SETTINGS* aParent, bool aLoadFromFile )
{
    m_parent = aParent;

    if( m_parent )
    {
        m_parent->AddNestedSettings( this );

        // In case we were created after the parent's ctor
        if( aLoadFromFile )
            LoadFromFile();
    }
}
