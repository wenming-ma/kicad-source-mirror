
#include <core/version_compare.h>
#include <kiplatform/drivers.h>

#include <QString>
#include <QSettings>

#define MIN_WIN_VERSION "10.7.2"

bool KIPLATFORM::DRIVERS::Valid3DConnexionDriverVersion()
{
    const QString versionValName = "Version";
    QSettings registry("HKEY_LOCAL_MACHINE\\Software\\3Dconnexion\\3DxSoftware", QSettings::NativeFormat);

    if( !registry.contains( versionValName ) )
        return false;

    QString versionStr = registry.value( versionValName ).toString();
    if( versionStr.isEmpty() )
        return false;

    return !versionStr.isEmpty()
           && compareVersionStrings( MIN_WIN_VERSION, versionStr.toStdString() );
}