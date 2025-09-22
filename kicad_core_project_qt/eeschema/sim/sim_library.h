
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef SIM_LIBRARY_H
#define SIM_LIBRARY_H

#include <sim/sim_model.h>
#include <reporter.h>
#include <QString>


class SIM_LIBRARY
{
public:
    static constexpr auto LIBRARY_FIELD = "Sim.Library";
    static constexpr auto NAME_FIELD = "Sim.Name";

    struct MODEL
    {
        std::string name;
        SIM_MODEL&  model;
    };

    virtual ~SIM_LIBRARY() = default;
    SIM_LIBRARY() = default;

    /**
     * Read library from a source file (e.g. in Spice format), and return a newly constructed
     * object of an appropriate subclass.
     *
     * @param aFilePath Path to the file.
     * @param aReporter The reporter the library reports to
     * @param aForceFullParse Caller requires fully parsed models.  If false fallback models can
     *                        be generarted for performance.
     * @return The library loaded in a newly constructed object.
     */
    static std::unique_ptr<SIM_LIBRARY>
    Create( const QString& aFilePath, bool aForceFullParse, REPORTER& aReporter,
            const std::function<QString( const QString&, const QString& )>& aResolver );

    /**
     * Read library from a source file. Must be in the format appropriate to the subclass, e.g.
     * Spice for SIM_LIBRARY_SPICE).
     *
     * @param aFilePath Path to the file.
     * @throw IO_ERROR on read or parsing error.
     */
    virtual void ReadFile( const QString& aFilePath, REPORTER& aReporter ) = 0;

    SIM_MODEL* FindModel( const std::string& aModelName ) const;

    std::vector<MODEL> GetModels() const;

    std::string GetFilePath() const { return m_filePath; }

protected:
    std::vector<std::string>                m_modelNames;
    std::vector<std::unique_ptr<SIM_MODEL>> m_models;

    std::function<QString( const QString&, const QString& )> m_pathResolver;

    std::string m_filePath;
};



#endif // SIM_LIBRARY_H
