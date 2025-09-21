
#pragma once

#include <map>
#include <vector>

#include <sim/sim_library.h>
#include <sim/sim_model.h>

#include <QString>

class EMBEDDED_FILES;
class PROJECT;
class SCH_SYMBOL;


class SIM_LIB_MGR
{
public:
    SIM_LIB_MGR( const PROJECT* aPrj );
    virtual ~SIM_LIB_MGR() = default;

    void SetForceFullParse() { m_forceFullParse = true; }

    void Clear();

    void SetFilesStack( std::vector<EMBEDDED_FILES*> aFilesStack ) { m_embeddedFilesStack = aFilesStack; }


    void SetLibrary( const QString& aLibraryPath, REPORTER& aReporter );

    SIM_MODEL& CreateModel( SIM_MODEL::TYPE aType, const std::vector<SCH_PIN*>& aPins,
                            REPORTER& aReporter );

    SIM_MODEL& CreateModel( const SIM_MODEL* aBaseModel, const std::vector<SCH_PIN*>& aPins,
                            REPORTER& aReporter );

    SIM_MODEL& CreateModel( const SIM_MODEL* aBaseModel, const std::vector<SCH_PIN*>& aPins,
                            const std::vector<SCH_FIELD>& aFields, REPORTER& aReporter );

    // TODO: The argument can be made const.
    SIM_LIBRARY::MODEL CreateModel( const SCH_SHEET_PATH* aSheetPath, SCH_SYMBOL& aSymbol,
                                    bool aResolve, int aDepth, REPORTER& aReporter );

    SIM_LIBRARY::MODEL CreateModel( const std::vector<SCH_FIELD>& aFields, bool aResolve, int aDepth,
                                    const std::vector<SCH_PIN*>& aPins, REPORTER& aReporter );

    SIM_LIBRARY::MODEL CreateModel( const QString& aLibraryPath,
                                    const std::string& aBaseModelName,
                                    const std::vector<SCH_FIELD>& aFields,
                                    const std::vector<SCH_PIN*>& aPins, REPORTER& aReporter );

    void SetModel( int aIndex, std::unique_ptr<SIM_MODEL> aModel );

    std::map<QString, std::reference_wrapper<const SIM_LIBRARY>> GetLibraries() const;
    std::vector<std::reference_wrapper<SIM_MODEL>> GetModels() const;

    QString ResolveLibraryPath( const QString& aLibraryPath, REPORTER& aReporter );
    QString ResolveEmbeddedLibraryPath( const QString& aLibPath, const QString& aRelativeLib,
                                         REPORTER& aReporter );

private:
    std::vector<EMBEDDED_FILES*>                     m_embeddedFilesStack;  // no ownership
    const PROJECT*                                   m_project;             // no ownership
    bool                                             m_forceFullParse;
    std::map<QString, std::unique_ptr<SIM_LIBRARY>> m_libraries;
    std::vector<std::unique_ptr<SIM_MODEL>>          m_models;
};

