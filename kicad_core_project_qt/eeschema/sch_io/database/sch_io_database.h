
#ifndef SCH_IO_DATABASE_H_
#define SCH_IO_DATABASE_H_

#include <database/database_connection.h>
#include <sch_io/sch_io.h>
#include <sch_io/sch_io_mgr.h>
#include <wildcards_and_files_ext.h>
#include <optional>


class DATABASE_LIB_SETTINGS;
struct DATABASE_LIB_TABLE;


/**
 * A KiCad database library provides both symbol and footprint metadata, so there are "shim" plugins
 * on both the symbol and footprint side of things that expose the database contents to the
 * schematic and board editors.  The architecture of these is slightly different from the other
 * plugins because the backing file is just a configuration file rather than something that
 * contains symbol or footprint data.
 */
class SCH_IO_DATABASE : public SCH_IO
{
public:

    SCH_IO_DATABASE();
    virtual ~SCH_IO_DATABASE();

    const IO_BASE::IO_FILE_DESC GetLibraryDesc() const override
    {
        return IO_BASE::IO_FILE_DESC( _HKI( "KiCad database library files" ),
                                      { FILEEXT::DatabaseLibraryFileExtension } );
    }

    int GetModifyHash() const override { return 0; }

    void EnumerateSymbolLib( QStringList&    aSymbolNameList,
                             const QString&   aLibraryPath,
                             const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    void EnumerateSymbolLib( std::vector<LIB_SYMBOL*>& aSymbolList,
                             const QString&           aLibraryPath,
                             const std::map<std::string, UTF8>*         aProperties = nullptr ) override;

    LIB_SYMBOL* LoadSymbol( const QString& aLibraryPath, const QString& aAliasName,
                            const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    bool SupportsSubLibraries() const override { return true; }

    void GetSubLibraryNames( std::vector<QString>& aNames ) override;

    void GetAvailableSymbolFields( std::vector<QString>& aNames ) override;

    void GetDefaultSymbolFields( std::vector<QString>& aNames ) override;

    // Database libraries can never be written using the symbol editing API
    bool IsLibraryWritable( const QString& aLibraryPath ) override { return false; }

    void SetLibTable( SYMBOL_LIB_TABLE* aTable ) override
    {
        m_libTable = aTable;
    }

    DATABASE_LIB_SETTINGS* Settings() const { return m_settings.get(); }

    bool TestConnection( QString* aErrorMsg = nullptr );

private:
    void cacheLib();

    void ensureSettings( const QString& aSettingsPath );

    void ensureConnection();

    void connect();

    std::unique_ptr<LIB_SYMBOL> loadSymbolFromRow( const QString& aSymbolName,
                                                   const DATABASE_LIB_TABLE& aTable,
                                                   const DATABASE_CONNECTION::ROW& aRow );

    static std::optional<bool> boolFromAny( const std::any& aVal );

    SYMBOL_LIB_TABLE* m_libTable;

    std::unique_ptr<DATABASE_LIB_SETTINGS> m_settings;

    /// Generally will be null if no valid connection is established
    std::unique_ptr<DATABASE_CONNECTION> m_conn;

    std::set<QString> m_customFields;

    std::set<QString> m_defaultShownFields;

    std::map<QString, std::unique_ptr<LIB_SYMBOL>> m_nameToSymbolcache;

    long long m_cacheTimestamp;

    int m_cacheModifyHash;



    QString m_lastError;
};

#endif //SCH_IO_DATABASE_H_
