#include <QString>
#include <QCoreApplication>
#include <QSysInfo>
#include <QLocale>
#include <QByteArray>
#include <config.h>
#include <boost/version.hpp>
#include <kiplatform/app.h>
#include <font/version_info.h>
#include <build_version.h>

#include <tuple>
#include <mutex>

// kicad_curl.h can create conflicts for some defines, at least on Windows
// so we are using here 2 proxy functions to know Curl version to avoid
// including kicad_curl.h to know Curl version
extern std::string GetKicadCurlVersion();
extern std::string GetCurlLibVersion();

#include <Standard_Version.hxx>

#include <ngspice/sharedspice.h>

// The include file version.h is always created even if the repo version cannot be
// determined. In this case KICAD_VERSION_FULL will default to the KICAD_VERSION
// that is set in KiCadVersion.cmake.
#define INCLUDE_KICAD_VERSION
#include <kicad_build_version.h>
#undef INCLUDE_KICAD_VERSION

// Mutex for platform info
static std::recursive_mutex s_platformInfoMutex;

// Remember OpenGL info
static QString s_glVendor;
static QString s_glRenderer;
static QString s_glVersion;

void SetOpenGLInfo( const char* aVendor, const char* aRenderer, const char* aVersion )
{
    s_glVendor = QString::fromUtf8( aVendor );
    s_glRenderer = QString::fromUtf8( aRenderer );
    s_glVersion = QString::fromUtf8( aVersion );
}


QString GetPlatformGetBitnessName()
{
    std::unique_lock lock(s_platformInfoMutex);
    
    return QSysInfo::currentCpuArchitecture();
}


bool IsNightlyVersion()
{
    return !!KICAD_IS_NIGHTLY;
}


QString GetBuildVersion()
{
    QString msg = QString( KICAD_VERSION_FULL );
    return msg;
}


QString GetBaseVersion()
{
    QString msg = QString( KICAD_VERSION );
    return msg;
}


QString GetBuildDate()
{
    QString msg = QString( "%1 %2" ).arg( __DATE__ ).arg( __TIME__ );
    return msg;
}


QString GetSemanticVersion()
{
    QString msg = QString( KICAD_SEMANTIC_VERSION );
    return msg;
}


QString GetMajorMinorVersion()
{
    QString msg = QString( KICAD_MAJOR_MINOR_VERSION );
    return msg;
}


QString GetCommitHash()
{
    QString msg = QString( KICAD_COMMIT_HASH );
    return msg;
}


QString GetMajorMinorPatchVersion()
{
    QString msg = QString( KICAD_MAJOR_MINOR_PATCH_VERSION );
    return msg;
}


const std::tuple<int,int,int>& GetMajorMinorPatchTuple()
{
    static std::tuple<int, int, int> retval = KICAD_MAJOR_MINOR_PATCH_TUPLE;

    return retval;
}


QString GetVersionInfoData( const QString& aTitle, bool aHtml, bool aBrief )
{
    QString aMsg;
    
    QString eol = aHtml ? "<br>" : "\n";
    
    QString indent4 = aHtml ? "&nbsp;&nbsp;&nbsp;&nbsp;" : "\t";


    QString version;
    version += ( KIPLATFORM::APP::IsOperatingSystemUnsupported() ? QString( "(UNSUPPORTED)" )
                                                                 : GetBuildVersion() );
#ifdef DEBUG
    version += ", debug";
#else
    version += ", release";
#endif
    version += " build";

    aMsg += "Application: " + aTitle;
    aMsg += " " + QSysInfo::currentCpuArchitecture() + " on " + QSysInfo::currentCpuArchitecture();

    aMsg += eol + eol;

    aMsg += "Version: " + version + eol + eol;
    aMsg += "Libraries:" + eol;

    aMsg += indent4 + QString("Qt %1").arg(QT_VERSION_STR) + eol;

    aMsg += indent4 + "FreeType " + KIFONT::VERSION_INFO::FreeType() + eol;
    aMsg += indent4 + "HarfBuzz " + KIFONT::VERSION_INFO::HarfBuzz() + eol;
    aMsg += indent4 + "FontConfig " + KIFONT::VERSION_INFO::FontConfig() + eol;

    if( !aBrief )
        aMsg += indent4 + QString::fromStdString(GetKicadCurlVersion()) + eol;

    aMsg += eol;

    QString osDescription;

#if __LINUX__
    osDescription = QSysInfo::prettyProductName();
#endif

    if( osDescription.isEmpty() )
        osDescription = QSysInfo::prettyProductName();

    {
        std::unique_lock lock( s_platformInfoMutex );

        aMsg += "Platform: " +
               osDescription + ", " +
               GetPlatformGetBitnessName() + ", " +
               QString(QSysInfo::ByteOrder == QSysInfo::BigEndian ? "big-endian" : "little-endian") + ", " +
               "Qt";
    }

#ifdef __linux__
    if( QCoreApplication::instance() )
    {
        aMsg += ", ";
        
        const char* sessionType = qgetenv("XDG_SESSION_TYPE").constData();
        if( sessionType && strlen(sessionType) > 0 )
        {
            if( strcmp(sessionType, "wayland") == 0 )
                aMsg += "Wayland";
            else if( strcmp(sessionType, "x11") == 0 )
                aMsg += "X11";
            else
                aMsg += sessionType;
        }
        else
        {
            aMsg += "Unknown";
        }
    }

    aMsg += ", " + QString::fromLocal8Bit(qgetenv("XDG_SESSION_DESKTOP")) +
           ", " + QString::fromLocal8Bit(qgetenv("XDG_SESSION_TYPE"));
#endif

    if( !s_glVendor.isEmpty() || !s_glRenderer.isEmpty() || !s_glVersion.isEmpty() )
    {
        aMsg += eol;
        aMsg += "OpenGL: " + s_glVendor + ", " + s_glRenderer + ", " + s_glVersion;
    }

    aMsg += eol + eol;

    if( !aBrief )
    {
        aMsg += "Build Info:" + eol;
        aMsg += indent4 + "Date: " + GetBuildDate() + eol;
    }

    aMsg += indent4 + QString("Qt: %1").arg(QT_VERSION_STR) + eol;

    aMsg += indent4 + QString("Boost: %1.%2.%3").arg(BOOST_VERSION / 100000)
                                                      .arg(BOOST_VERSION / 100 % 1000)
                                                      .arg(BOOST_VERSION % 100) + eol;

    aMsg += indent4 + "OCC: " + QString(OCC_VERSION_COMPLETE) + eol;
    aMsg += indent4 + "Curl: " + QString::fromStdString(GetCurlLibVersion()) + eol;

#if defined( NGSPICE_BUILD_VERSION )
    aMsg += indent4 + "ngspice: " + QString(NGSPICE_BUILD_VERSION) + eol;
#elif defined( NGSPICE_HAVE_CONFIG_H )
    #undef HAVE_STRNCASECMP     /* is redefined in ngspice/config.h */
    #include <ngspice/config.h>
    aMsg += indent4 + "ngspice: " + QString(PACKAGE_VERSION) + eol;
#elif defined( NGSPICE_PACKAGE_VERSION )
    aMsg += indent4 + "ngspice: " + QString(NGSPICE_PACKAGE_VERSION) + eol;
#else
    aMsg += indent4 + "ngspice: " + "unknown" + eol;
#endif

    aMsg += indent4 + "Compiler: ";
#if defined(__clang__)
    aMsg += QString("Clang %1.%2.%3").arg(__clang_major__).arg(__clang_minor__).arg(__clang_patchlevel__);
#elif defined(__GNUG__)
    aMsg += QString("GCC %1.%2.%3").arg(__GNUC__).arg(__GNUC_MINOR__).arg(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    aMsg += QString("Visual C++ %1").arg(_MSC_VER);
#elif defined(__INTEL_COMPILER)
    aMsg += QString("Intel C++ %1").arg(__INTEL_COMPILER);
#else
    aMsg += "Other Compiler ";
#endif

#if defined(__GXX_ABI_VERSION)
    aMsg += QString(" with C++ ABI %1").arg(__GXX_ABI_VERSION) + eol;
#else
    aMsg += " without C++ ABI" + eol;
#endif

    // Add build settings config (build options):
#if defined( KICAD_USE_EGL ) || ! defined( NDEBUG )
    aMsg += eol;
    aMsg += "Build settings:" + eol;
#endif

#ifdef KICAD_USE_EGL
    aMsg += indent4 + "KICAD_USE_EGL=" + "ON" + eol;
#endif

#ifdef KICAD_IPC_API
    aMsg += indent4 + "KICAD_IPC_API=" + "ON" + eol;
#else
    aMsg += indent4 + "KICAD_IPC_API=" + "OFF" + eol;
#endif

#ifndef NDEBUG
    aMsg += indent4 + "KICAD_STDLIB_DEBUG=";
#ifdef KICAD_STDLIB_DEBUG
    aMsg += "ON" + eol;
#else
    aMsg += "OFF" + eol;
    aMsg += indent4 + "KICAD_STDLIB_LIGHT_DEBUG=";
#ifdef KICAD_STDLIB_LIGHT_DEBUG
    aMsg += "ON" + eol;
#else
    aMsg += "OFF" + eol;
#endif
#endif

    aMsg += indent4 + "KICAD_SANITIZE_ADDRESS=";
#ifdef KICAD_SANITIZE_ADDRESS
    aMsg += "ON" + eol;
#else
    aMsg += "OFF" + eol;
#endif

    aMsg += indent4 + "KICAD_SANITIZE_THREADS=";
#ifdef KICAD_SANITIZE_THREADS
    aMsg += "ON" + eol;
#else
    aMsg += "OFF" + eol;
#endif
#endif

    QLocale locale;
    
    {
        aMsg += eol;
        aMsg += "Locale: " + eol;
        aMsg += indent4 + "Lang: " + locale.name() + eol;
        aMsg += indent4 + "Enc: UTF-8" + eol;
        aMsg += indent4 + QString("Num: 1%1234.5").arg(locale.groupSeparator()) + eol;

        QString testStr( "кΩ丈" );
        QString expectedUtf8Hex( "D0BACEA9E4B888" );
        QString utf8Hex;
        
        QByteArray utf8Bytes = testStr.toUtf8();
        for( int i = 0; i < utf8Bytes.length(); i++ )
        {
            utf8Hex += QString("%1").arg((unsigned char)utf8Bytes[i], 2, 16, QChar('0')).toUpper();
        }

        aMsg += indent4 + "Encoded " + testStr + ": " + utf8Hex + " (utf8)" + eol;

        Q_ASSERT_X( utf8Hex == expectedUtf8Hex, "GetVersionInfoData", 
                   QString("utf8 string %1 encoding bad result: %2, expected %3")
                   .arg(testStr).arg(utf8Hex).arg(expectedUtf8Hex).toLocal8Bit().constData() );
    }

    return aMsg;
}
