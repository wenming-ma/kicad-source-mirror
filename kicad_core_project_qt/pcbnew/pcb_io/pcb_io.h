// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCB_IO_H_
#define PCB_IO_H_

#include <io/io_base.h>
#include <pcb_io/pcb_io_mgr.h>

#include <cstdint>
#include <config.h>
#include <vector>
#include <QStringList>
#include <i18n_utility.h>

class BOARD;
class FOOTPRINT;
class PROJECT;
class PROGRESS_REPORTER;

// Base class for BOARD loading and saving plugins
class PCB_IO : public IO_BASE
{
public:
    // Returns board file description for the PCB_IO
    virtual const IO_BASE::IO_FILE_DESC GetBoardFileDesc() const
    {
        return IO_BASE::IO_FILE_DESC( QString(), {} );
    }


    // Checks if this PCB_IO can read the specified board file
    virtual bool CanReadBoard( const QString& aFileName ) const;

    // Checks if this PCB_IO can read a footprint from specified file or directory
    virtual bool CanReadFootprint( const QString& aFileName ) const;

    // Registers a KIDIALOG callback for collecting info from the user
    virtual void SetQueryUserCallback( std::function<bool( QString aTitle, int aIcon,
                                                           QString aMessage,
                                                           QString aAction )> aCallback )
    { }

    // Load information from input file into new or existing BOARD
    virtual BOARD* LoadBoard( const QString& aFileName, BOARD* aAppendToMe,
                              const std::map<std::string, UTF8>* aProperties = nullptr,
                              PROJECT* aProject = nullptr );

    // Return cached library footprints from last Load call
    virtual std::vector<FOOTPRINT*> GetImportedCachedLibraryFootprints();

    // Write BOARD to storage file
    virtual void SaveBoard( const QString& aFileName, BOARD* aBoard,
                            const std::map<std::string, UTF8>* aProperties = nullptr );

    // Return list of footprint names in library
    virtual void FootprintEnumerate( QStringList& aFootprintNames, const QString& aLibraryPath,
                                     bool aBestEfforts,
                                     const std::map<std::string, UTF8>* aProperties = nullptr );

    // Generate timestamp for all library files
    virtual long long GetLibraryTimestamp( const QString& aLibraryPath ) const = 0;

    // Load single footprint from path and return its name
    virtual FOOTPRINT* ImportFootprint( const QString& aFootprintPath, QString& aFootprintNameOut,
                                        const std::map<std::string, UTF8>* aProperties = nullptr );

    // Load footprint by name from library
    virtual FOOTPRINT* FootprintLoad( const QString& aLibraryPath,
                                      const QString& aFootprintName,
                                      bool  aKeepUUID = false,
                                      const std::map<std::string, UTF8>* aProperties = nullptr );

    // Efficient version of FootprintLoad for use after FootprintEnumerate
    virtual const FOOTPRINT* GetEnumeratedFootprint( const QString& aLibraryPath,
                                                     const QString& aFootprintName,
                                                     const std::map<std::string, UTF8>* aProperties = nullptr );

    // Check for footprint existence
    virtual bool FootprintExists( const QString& aLibraryPath, const QString& aFootprintName,
                                  const std::map<std::string, UTF8>* aProperties = nullptr );

    // Write footprint to library
    virtual void FootprintSave( const QString& aLibraryPath, const FOOTPRINT* aFootprint,
                                const std::map<std::string, UTF8>* aProperties = nullptr );

    // Delete footprint from library
    virtual void FootprintDelete( const QString& aLibraryPath, const QString& aFootprintName,
                                  const std::map<std::string, UTF8>* aProperties = nullptr );

    // Append supported plugin options with descriptions
    virtual void GetLibraryOptions( std::map<std::string, UTF8>* aListToAppendTo ) const override;

    virtual ~PCB_IO()
    {};

protected:
    PCB_IO( const QString& aName ) : IO_BASE( aName ),
        m_board( nullptr ),
        m_props( nullptr )
    {}

    // Board being worked on, no ownership
    BOARD* m_board;

    // Properties passed via Save() or Load(), no ownership
    const std::map<std::string, UTF8>* m_props;
};

#endif // PCB_IO_H_