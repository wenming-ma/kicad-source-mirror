#ifndef DESIGN_BLOCK_IO_H
#define DESIGN_BLOCK_IO_H

#include <kicommon.h>
#include <io/io_base.h>
#include <io/io_mgr.h>
#include <QStringList>

class DESIGN_BLOCK;
class DESIGN_BLOCK_IO;

class KICOMMON_API DESIGN_BLOCK_IO_MGR : public IO_MGR
{
public:
    enum DESIGN_BLOCK_FILE_T
    {
        DESIGN_BLOCK_FILE_UNKNOWN = 0,
        KICAD_SEXP,

        FILE_TYPE_NONE
    };

    static const QString      ShowType( DESIGN_BLOCK_FILE_T aFileType );
    static DESIGN_BLOCK_IO*    FindPlugin( DESIGN_BLOCK_FILE_T aFileType );
    static DESIGN_BLOCK_FILE_T EnumFromStr( const QString& aFileType );
    static DESIGN_BLOCK_FILE_T GuessPluginTypeFromLibPath( const QString& aLibPath, int aCtl = 0 );

    static bool ConvertLibrary( std::map<std::string, UTF8>* aOldFileProps,
                                const QString& aOldFilePath, const QString& aNewFilePath );
};


class KICOMMON_API DESIGN_BLOCK_IO : public IO_BASE
{
public:
    DESIGN_BLOCK_IO() : IO_BASE( "KiCad" ) {}

    const IO_BASE::IO_FILE_DESC GetLibraryDesc() const override;
    long long                   GetLibraryTimestamp( const QString& aLibraryPath ) const;

    void DesignBlockEnumerate( QStringList& aDesignBlockNames, const QString& aLibraryPath,
                               bool                               aBestEfforts,
                               const std::map<std::string, UTF8>* aProperties = nullptr );

    const DESIGN_BLOCK*
    GetEnumeratedDesignBlock( const QString& aLibraryPath, const QString& aDesignBlockName,
                              const std::map<std::string, UTF8>* aProperties = nullptr )
    {
        return DesignBlockLoad( aLibraryPath, aDesignBlockName, false, aProperties );
    }

    bool DesignBlockExists( const QString& aLibraryPath, const QString& aDesignBlockName,
                            const std::map<std::string, UTF8>* aProperties = nullptr )
    {
        return DesignBlockLoad( aLibraryPath, aDesignBlockName, true, aProperties ) != nullptr;
    }

    DESIGN_BLOCK* ImportDesignBlock( const QString&                    aDesignBlockPath,
                                     QString&                          aDesignBlockNameOut,
                                     const std::map<std::string, UTF8>* aProperties = nullptr )
    {
        return nullptr;
    }

    void CreateLibrary( const QString&                    aLibraryPath,
                        const std::map<std::string, UTF8>* aProperties = nullptr ) override;

    virtual bool DeleteLibrary( const QString&                    aLibraryPath,
                                const std::map<std::string, UTF8>* aProperties = nullptr ) override;


    bool IsLibraryWritable( const QString& aLibraryPath ) override;

    DESIGN_BLOCK* DesignBlockLoad( const QString& aLibraryPath, const QString& aDesignBlockName,
                                   bool                               aKeepUUID = false,
                                   const std::map<std::string, UTF8>* aProperties = nullptr );

    void DesignBlockSave( const QString& aLibraryPath, const DESIGN_BLOCK* aDesignBlock,
                          const std::map<std::string, UTF8>* aProperties = nullptr );

    void DesignBlockDelete( const QString& aLibraryPath, const QString& aDesignBlockName,
                            const std::map<std::string, UTF8>* aProperties = nullptr );
};

#endif
