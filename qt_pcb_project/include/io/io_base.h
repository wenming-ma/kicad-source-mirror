

#ifndef IO_BASE_H_
#define IO_BASE_H_

#include <map>
#include <vector>
#include <string>

#include <kicommon.h>
#include <core/utf8.h>
#include <QString>
#include <widgets/report_severity.h>

class REPORTER;
class PROGRESS_REPORTER;

class KICOMMON_API IO_BASE
{
public:
    /**
    * Container that describes file type info
    */
    struct KICOMMON_API IO_FILE_DESC
    {
        QString                  m_Description;    // Description shown in the file picker dialog

        /// Filter used for file pickers if m_IsFile is true.
        std::vector<std::string> m_FileExtensions;

        ///< In case of folders: extensions of files inside.
        std::vector<std::string> m_ExtensionsInDir;
        bool                     m_IsFile;          ///< Whether the library is a folder or a file
        bool                     m_CanRead;         ///< Whether the IO can read this file type
        bool                     m_CanWrite;        ///< Whether the IO can write this file type

        IO_FILE_DESC( const QString& aDescription, const std::vector<std::string>& aFileExtensions,
                      const std::vector<std::string>& aExtsInFolder = {}, bool aIsFile = true,
                      bool aCanRead = true, bool aCanWrite = true ) :
                m_Description( aDescription ),
                m_FileExtensions( aFileExtensions ), m_ExtensionsInDir( aExtsInFolder ),
                m_IsFile( aIsFile ), m_CanRead( aCanRead ), m_CanWrite( aCanWrite )
        {
        }

        IO_FILE_DESC() : IO_FILE_DESC( QString(), {} ) {}

        QString FileFilter() const;

        operator bool() const { return !m_Description.empty(); }
    };

    virtual ~IO_BASE() = default;

    const QString& GetName() const { return m_name; }

    virtual void SetReporter( REPORTER* aReporter ) { m_reporter = aReporter; }

    virtual void SetProgressReporter( PROGRESS_REPORTER* aReporter )
    {
        m_progressReporter = aReporter;
    }


    virtual const IO_FILE_DESC GetLibraryDesc() const = 0;

    virtual const IO_FILE_DESC GetLibraryFileDesc() const { return GetLibraryDesc(); }

    virtual bool CanReadLibrary( const QString& aFileName ) const;

    virtual void CreateLibrary( const QString& aLibraryPath,
                                const std::map<std::string, UTF8>* aProperties = nullptr );

    virtual bool DeleteLibrary( const QString& aLibraryPath,
                                const std::map<std::string, UTF8>* aProperties = nullptr );

    virtual bool IsLibraryWritable( const QString& aLibraryPath );

    virtual void GetLibraryOptions( std::map<std::string, UTF8>* aListToAppendTo ) const;

    virtual void Report( const QString& aText, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED );

    virtual void AdvanceProgressPhase();

protected:
    // Delete the zero-argument base constructor to force proper construction
    IO_BASE() = delete;

    IO_BASE( const QString& aName ) :
        m_name( aName ),
        m_reporter( nullptr ),
        m_progressReporter( nullptr )
    {
    }


    QString m_name;
    REPORTER* m_reporter;
    PROGRESS_REPORTER* m_progressReporter;
};

#endif // IO_BASE_H_
