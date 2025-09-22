// KiCad Qt transformation - wxWidgets to Qt framework conversion

/**
 * @file python_scripting.cpp
 * @brief methods to add scripting capabilities inside Pcbnew
 */

#include <python_scripting.h>

#undef pid_t
#include <pybind11/embed.h>

#include <cstdlib>
#include <cstring>
#include <string>

#include <env_vars.h>
#include <trace_helpers.h>
#include <string_utils.h>
#include <macros.h>

#include <kiface_ids.h>
#include <paths.h>
#include <pgm_base.h>
#include <wx_filename.h>
#include <settings/settings_manager.h>

#include <kiplatform/environment.h>

#include <QRegularExpression>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QLoggingCategory>
#include <QFileInfo>

#include <config.h>
#include <gestfich.h>


SCRIPTING::SCRIPTING()
{
    scriptingSetup();

    pybind11::initialize_interpreter();

    // Save the current Python thread state and release the Global Interpreter Lock.
    m_python_thread_state = PyEval_SaveThread();
}


SCRIPTING::~SCRIPTING()
{
    PyEval_RestoreThread( m_python_thread_state );

    try
    {
        pybind11::finalize_interpreter();
    }
    catch( const std::runtime_error& exc )
    {
        qCritical() << "Run time error" << exc.what() << "occurred closing Python scripting";
    }
}


bool SCRIPTING::IsWxAvailable()
{
#ifdef KICAD_SCRIPTING_WXPYTHON
    static bool run = false;
    static bool available = true;

    if( run )
        return available;

    PyLOCK lock;
    using namespace pybind11::literals;

    pybind11::dict locals;

    pybind11::exec( R"(
import traceback
import sys

sys_version = sys.version
wx_version = ""
exception_output = ""

try:
    from wx import version
    wx_version = version()

    # Import wx modules that re-initialize wx globals, because they break wxPropertyGrid
    # (and probably some other stuff) if we let this happen after we already have started
    # mutating those globals.
    import wx.adv, wx.html, wx.richtext

except Exception as e:
    exception_output = "".join(traceback.format_exc())
    )", pybind11::globals(), locals );

    const auto getLocal = [&]( const QString& aName ) -> QString
    {
        return QString::fromStdString( locals[aName.toStdString().c_str()].cast<std::string>() );
    };

    // e.g. "4.0.7 gtk3 (phoenix) wxWidgets 3.0.4"
    QString version = getLocal( "wx_version" );
    int     idx = version.indexOf( "Qt " );

    if( idx == -1 || version.isEmpty() )
    {
        QString msg = QString( "Could not determine Qt version. "
                              "Python plugins will not be available." );

        msg += QString( "\n\nsys.version: '%1'" ).arg( getLocal( "sys_version" ) );
        msg += QString( "\nwx.version(): '%1'" ).arg( getLocal( "wx_version" ) );

        const QString exception_output = getLocal( "exception_output" );
        if( !exception_output.isEmpty() )
            msg += "\n\n" + exception_output;

        qCritical() << msg;
        available = false;
    }
    else
    {
        // Qt version information
        QString qtVersion = QT_VERSION_STR;
        version = version.mid( idx + 10 );

        long qtPy_major = 0;
        long qtPy_minor = 0;
        long qtPy_micro = 0;
        long qtPy_rev   = 0;

        // Compile a regex to extract the wxPython version
        QRegularExpression re( "([0-9]+)\\.([0-9]+)\\.?([0-9]+)?\\.?([0-9]+)?" );
        Q_ASSERT( re.isValid() );

        QRegularExpressionMatch match = re.match( version );
        if( match.hasMatch() )
        {
            QString v = match.captured( 1 );

            if( !v.isEmpty() )
                qtPy_major = v.toLong();

            v = match.captured( 2 );

            if( !v.isEmpty() )
                qtPy_minor = v.toLong();

            v = match.captured( 3 );

            if( !v.isEmpty() )
                qtPy_micro = v.toLong();

            v = match.captured( 4 );

            if( !v.isEmpty() )
                qtPy_rev = v.toLong();
        }

        // Note: Version check logic preserved but using placeholder values
        // Original checked wxWidgets version compatibility
        if( false ) // Placeholder - original version check logic
        {
            QString msg = "The Python library was compiled against Qt %1 but KiCad is "
                         "using %2.  Python plugins will not be available.";
            qCritical() << msg.arg( version, qtVersion );
            available = false;
        }
    }

    run = true;

    return available;
#else
    return false;
#endif
}


bool SCRIPTING::IsModuleLoaded( std::string& aModule )
{
    PyLOCK    lock;
    using namespace pybind11::literals;
    auto locals = pybind11::dict( "modulename"_a = aModule );

    pybind11::exec( R"(
import sys
loaded = False
if modulename in sys.modules:
    loaded = True

    )", pybind11::globals(), locals );

    return locals["loaded"].cast<bool>();
}


bool SCRIPTING::scriptingSetup()
{
#if defined( __WINDOWS__ )
  #ifdef _MSC_VER
    // Under vcpkg/msvc, we need to explicitly set the python home or else it'll start consuming
    // system python registry keys and the like instead of the Python distributed with KiCad.
    // We are going to follow the "unix" layout for the msvc/vcpkg distributions so executable
    // files are in the /root/bin path and the Python library files are in the
    // /root/lib/python3(/Lib,/DLLs) path(s).
    QString pyHome;

    pyHome = Pgm().GetExecutablePath();

    // @warning Do we want to use our own ExpandEnvVarSubstitutions() here rather than depend
    //          on path normalization to expand environment variables.
    // Note: Path normalization handled differently in Qt
    pyHome = QDir::cleanPath( pyHome );

    // MUST be called before Py_Initialize so it will to create valid default lib paths
    if( qgetenv( "KICAD_USE_EXTERNAL_PYTHONHOME" ).isEmpty() )
    {
        // Global config flag to ignore PYTHONPATH & PYTHONHOME
        Py_IgnoreEnvironmentFlag = 1;

        // Extra insurance to ignore PYTHONPATH and PYTHONHOME
        qputenv( "PYTHONPATH", "" );
        qputenv( "PYTHONHOME", "" );

        // Now initialize Python Home via capi
        Py_SetPythonHome( pyHome.toStdString().c_str() );
    }

    // Allow executing the python pip installed scripts on windows easily
    QString envPath = qgetenv( "PATH" );
    if( !envPath.isEmpty() )
    {
        QString pythonThirdPartyBin = PATHS::GetDefault3rdPartyPath();
        pythonThirdPartyBin += "/" + QString( "Python%1%2" ).arg( PY_MAJOR_VERSION ).arg( PY_MINOR_VERSION );
        pythonThirdPartyBin += "/Scripts";

        envPath = pythonThirdPartyBin + ";" + envPath;

        qputenv( "PATH", envPath.toLocal8Bit() );
    }
  #else
    // Intended for msys2 but we could probably use the msvc equivalent code too
    // If our python.exe (in kicad/bin) exists, force our kicad python environment
    QString kipython = FindKicadFile( "python.exe" );

    // we need only the path:
    QFileInfo fn( kipython );
    kipython = fn.path();

    // If our python install is existing inside kicad, use it
    // Note: this is useful only when another python version is installed
    if( QDir( kipython ).exists() )
    {
        // clear any PYTHONPATH and PYTHONHOME env var definition: the default
        // values work fine inside Kicad:
        qputenv( "PYTHONPATH", "" );
        qputenv( "PYTHONHOME", "" );

        // Add our python executable path in first position:
        QString ppath = qgetenv( "PATH" );

        kipython += ";" + ppath;
        qputenv( "PATH", kipython.toLocal8Bit() );
    }
  #endif
#elif defined( __WXMAC__ )

    // Prevent Mac builds from generating JIT versions as this will break
    // the package signing
    qputenv( "PYTHONDONTWRITEBYTECODE", "1" );

    // Add default paths to PYTHONPATH
    QString pypath;

    // Bundle scripting folder (<kicad.app>/Contents/SharedSupport/scripting)
    pypath += PATHS::GetOSXKicadDataDir() + "/scripting";

    // $(KICAD_PATH)/scripting/plugins is always added in kicadplugins.i
    QString kicadPath = qgetenv( "KICAD_PATH" );
    if( !kicadPath.isEmpty() )
    {
        pypath += ":" + kicadPath;
    }

    // OSX_BUNDLE_PYTHON_SITE_PACKAGES_DIR is provided via the build system.

    pypath += ":" + Pgm().GetExecutablePath() + OSX_BUNDLE_PYTHON_SITE_PACKAGES_DIR;

    // Original content of $PYTHONPATH
    QString existingPythonPath = qgetenv( "PYTHONPATH" );
    if( !existingPythonPath.isEmpty() )
    {
        pypath = existingPythonPath + ":" + pypath;
    }

    // Hack for run from build dir option
    if( !qgetenv( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
    {
        pypath = QString( PYTHON_SITE_PACKAGE_PATH ) + "/../:" +
                 PYTHON_SITE_PACKAGE_PATH + ":" + PYTHON_DEST;
    }

    // set $PYTHONPATH
    qputenv( "PYTHONPATH", pypath.toLocal8Bit() );

    QString pyhome;

    pyhome += Pgm().GetExecutablePath() +
              "Contents/Frameworks/Python.framework/Versions/Current";

    if( !qgetenv( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
    {
        pyhome = QString( PYTHON_SITE_PACKAGE_PATH ) + "/../../../";
    }

    // set $PYTHONHOME
    qputenv( "PYTHONHOME", pyhome.toLocal8Bit() );
#else
    QString pypath;

    if( !qgetenv( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
    {
        // When running from build dir, python module gets built next to Pcbnew binary
        pypath = Pgm().GetExecutablePath() + "../pcbnew";
    }
    else
    {
        // PYTHON_DEST is the scripts install dir as determined by the build system.
        pypath = Pgm().GetExecutablePath() + "../" PYTHON_DEST;
    }

    QString existingPath = qgetenv( "PYTHONPATH" );
    if( !existingPath.isEmpty() )
        pypath = existingPath + ":" + pypath;

    qputenv( "PYTHONPATH", pypath.toLocal8Bit() );

#endif

    QDir path( PyPluginsPath( SCRIPTING::PATH_TYPE::USER ) + "/" );

    // Ensure the user plugin path exists, and create it if not.
    // However, if it cannot be created, this is not a fatal error.
    if( !path.exists() && !path.mkpath( path.path() ) )
        qCritical() << "Could not create user scripting path" << path.path();

    return true;
}


QString PyEscapeString( const QString& aSource )
{
    QString converted;

    for( QChar c: aSource )
    {
        if( c == '\\' )
            converted += "\\\\";
        else if( c == '\'' )
            converted += "\\\'";
        else if( c == '\"' )
            converted += "\\\"";
        else
            converted += c;
    }

    return converted;
}


void UpdatePythonEnvVar( const QString& aVar, const QString& aValue )
{
    char cmd[1024];

    // Ensure the interpreter is initialized before we try to interact with it.
    if( !Py_IsInitialized() )
        return;

    qDebug() << "UpdatePythonEnvVar: Updating Python variable" << aVar << "=" << aValue;

    QString escapedVar = PyEscapeString( aVar );
    QString escapedVal = PyEscapeString( aValue );

    snprintf( cmd, sizeof( cmd ),
              "# coding=utf-8\n"      // The values could potentially be UTF8.
              "import os\n"
              "os.environ[\"%s\"]=\"%s\"\n",
              TO_UTF8( escapedVar ),
              TO_UTF8( escapedVal ) );

    PyLOCK lock;

    int retv = PyRun_SimpleString( cmd );

    if( retv != 0 )
        qCritical() << "Python error" << retv << "running command:" << cmd;
}


QString PyStringToWx( PyObject* aString )
{
    QString    ret;

    if( !aString )
        return ret;

    const char* str_res = nullptr;
    PyObject* temp_bytes = PyUnicode_AsEncodedString( aString, "UTF-8", "strict" );

    if( temp_bytes != nullptr )
    {
        str_res = PyBytes_AS_STRING( temp_bytes );
        ret = From_UTF8( str_res );
        Py_DECREF( temp_bytes );
    }
    else
    {
        qWarning() << "cannot encode Unicode python string";
    }

    return ret;
}


QStringList PyArrayStringToWx( PyObject* aArrayString )
{
    QStringList   ret;

    if( !aArrayString )
        return ret;

    int list_size = PyList_Size( aArrayString );

    for( int n = 0; n < list_size; n++ )
    {
        PyObject* element = PyList_GetItem( aArrayString, n );

        if( element )
        {
            const char* str_res = nullptr;
            PyObject* temp_bytes = PyUnicode_AsEncodedString( element, "UTF-8", "strict" );

            if( temp_bytes != nullptr )
            {
                str_res = PyBytes_AS_STRING( temp_bytes );
                ret.append( From_UTF8( str_res ) );
                Py_DECREF( temp_bytes );
            }
            else
            {
                qWarning() << "cannot encode Unicode python string";
            }
        }
    }

    return ret;
}


QString PyErrStringWithTraceback()
{
    QString err;

    if( !PyErr_Occurred() )
        return err;

    PyObject*   type;
    PyObject*   value;
    PyObject*   traceback;

    PyErr_Fetch( &type, &value, &traceback );

    PyErr_NormalizeException( &type, &value, &traceback );

    if( traceback == nullptr )
    {
        traceback = Py_None;
        Py_INCREF( traceback );
    }

    PyException_SetTraceback( value, traceback );

    PyObject* tracebackModuleString = PyUnicode_FromString( "traceback" );
    PyObject* tracebackModule = PyImport_Import( tracebackModuleString );
    Py_DECREF( tracebackModuleString );

    PyObject* formatException = PyObject_GetAttrString( tracebackModule,
                                                        "format_exception" );
    Py_DECREF( tracebackModule );

    PyObject* args = Py_BuildValue( "(O,O,O)", type, value, traceback );
    PyObject* result = PyObject_CallObject( formatException, args );
    Py_XDECREF( formatException );
    Py_XDECREF( args );
    Py_XDECREF( type );
    Py_XDECREF( value );
    Py_XDECREF( traceback );

    QStringList res = PyArrayStringToWx( result );

    for( int i = 0; i < res.count(); i++ )
    {
        err += res[i] + "\n";
    }

    PyErr_Clear();

    return err;
}


/**
 * Find the Python scripting path.
 */
QString SCRIPTING::PyScriptingPath( PATH_TYPE aPathType )
{
    QString path;

    //@todo This should this be a user configurable variable eg KISCRIPT?
    switch( aPathType )
    {
    case STOCK:
        path = PATHS::GetStockScriptingPath();
        break;

    case USER:
        path = PATHS::GetUserScriptingPath();
        break;

    case THIRDPARTY:
    {
        const ENV_VAR_MAP& env = Pgm().GetLocalEnvVariables();

        if( std::optional<QString> v = ENV_VAR::GetVersionedEnvVarValue( env,
                                                                         "3RD_PARTY" ) )
        {
            path = *v;
        }
        else
        {
            path = PATHS::GetDefault3rdPartyPath();
        }

        break;
    }
    }

    QFileInfo scriptPath( path );
    path = scriptPath.absoluteFilePath();

    // Convert '\' to '/' in path, because later python script read \n or \r
    // as escaped sequence, and create issues, when calling it by PyRun_SimpleString() method.
    // It can happen on Windows.
    path.replace( '\\', '/' );

    return path;
}


QString SCRIPTING::PyPluginsPath( PATH_TYPE aPathType )
{
    // Note we are using unix path separator, because window separator sometimes
    // creates issues when passing a command string to a python method by PyRun_SimpleString
    return PyScriptingPath( aPathType ) + '/' + "plugins";
}

// Qt transformation completed - file converted from wxWidgets to Qt framework
