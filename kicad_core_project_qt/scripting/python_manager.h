
#ifndef KICAD_PYTHON_MANAGER_H
#define KICAD_PYTHON_MANAGER_H

#include <functional>
#include <optional>

#include <QString>
#include <QProcessEnvironment>

#include <kicommon.h>


class KICOMMON_API PYTHON_MANAGER
{
public:
    PYTHON_MANAGER( const QString& aInterpreterPath );

    /**
     * Launches the Python interpreter with the given arguments
     * @param aArgs
     * @param aCallback
     * @param aEnv
     * @param aSaveOutput
     * @return the process ID of the created process, or 0 if one was not created
     */
    long Execute( const std::vector<QString>& aArgs,
                  const std::function<void(int, const QString&, const QString&)>& aCallback,
                  const QProcessEnvironment* aEnv = nullptr,
                  bool aSaveOutput = false );

    QString GetInterpreterPath() const { return m_interpreterPath; }
    void SetInterpreterPath( const QString& aPath ) { m_interpreterPath = aPath; }

    /**
     * Searches for a Python intepreter on the user's system
     * @return the absolute path to a Python interpreter, or an empty string if one was not found
     */
    static QString FindPythonInterpreter();

    static std::optional<QString> GetPythonEnvironment( const QString& aNamespace );

    /// Returns a full path to the python binary in a venv, if it exists
    static std::optional<QString> GetVirtualPython( const QString& aNamespace );

private:
    QString m_interpreterPath;
};


#endif //KICAD_PYTHON_MANAGER_H
