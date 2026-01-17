
#ifndef __PYTHON_SCRIPTING_H
#define __PYTHON_SCRIPTING_H

// undefs explained here: https://bugzilla.redhat.com/show_bug.cgi?id=427617

#ifdef _POSIX_C_SOURCE
    #undef _POSIX_C_SOURCE
#endif
#ifdef _XOPEN_SOURCE
    #undef _XOPEN_SOURCE
#endif

#if defined(WIN32)
    #undef pid_t    // wxWidgets defines this, python typedefs, result is a conflict
#endif

#undef HAVE_CLOCK_GETTIME  // macro is defined in Python.h and causes redefine warning
#include <Python.h>
#undef HAVE_CLOCK_GETTIME

#include <QWidget>
#include <QString>
#include <QStringList>

#include <kicommon.h>

class KICOMMON_API SCRIPTING
{
public:
    SCRIPTING();
    ~SCRIPTING();

    /// We do not allow secondary creation of the scripting system
    SCRIPTING( SCRIPTING const& )       = delete;
    void operator= ( SCRIPTING const& ) = delete;

    static bool IsWxAvailable();

    static bool IsModuleLoaded( std::string& aModule );

    enum PATH_TYPE
    {
        STOCK,
        USER,
        THIRDPARTY
    };

    static QString PyScriptingPath( PATH_TYPE aPathType = STOCK );
    static QString PyPluginsPath( PATH_TYPE aPathType = STOCK );

private:

    bool scriptingSetup();

    PyThreadState* m_python_thread_state;
};

/**
 * Set an environment variable in the current Python interpreter.
 *
 * @param aVar is the variable to set
 * @param aValue is the value to give it
 */
KICOMMON_API void UpdatePythonEnvVar( const QString& aVar, const QString& aValue );

KICOMMON_API void RedirectStdio();
KICOMMON_API QWidget* CreatePythonShellWindow( QWidget* parent, const QString& aFramenameId );
KICOMMON_API bool       InitPythonScripting( const char* aStockScriptingPath,
                                            const char* aUserScriptingPath );
KICOMMON_API bool       IsWxPythonLoaded();

class KICOMMON_API PyLOCK
{
    PyGILState_STATE gil_state;
public:
    PyLOCK()      { gil_state = PyGILState_Ensure(); }
    ~PyLOCK()     { PyGILState_Release( gil_state ); }
};

KICOMMON_API QString PyStringToWx( PyObject* str );
KICOMMON_API QStringList PyArrayStringToWx( PyObject* arr );
KICOMMON_API QString       PyErrStringWithTraceback();

#endif    // __PYTHON_SCRIPTING_H
