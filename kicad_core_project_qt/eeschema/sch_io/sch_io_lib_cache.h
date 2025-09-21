
#ifndef _SCH_IO_LIB_CACHE_H_
#define _SCH_IO_LIB_CACHE_H_

#include <mutex>
#include <optional>

#include <QString>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

#include <symbol_library_common.h>


class LIB_SYMBOL;
class OUTPUTFORMATTER;


/**
 * A base cache assistant implementation for the symbol library portion of the #SCH_IO API.
 */
class SCH_IO_LIB_CACHE
{
public:
    SCH_IO_LIB_CACHE( const QString& aLibraryPath );
    virtual ~SCH_IO_LIB_CACHE();

    void IncrementModifyHash()
    {
        std::lock_guard<std::mutex> mut( m_modHashMutex );
        m_modHash++;
    }

    int GetModifyHash()
    {
        std::lock_guard<std::mutex> mut( m_modHashMutex );
        return m_modHash;
    }

    // Most all functions in this class throw IO_ERROR exceptions.  There are no
    // error codes nor user interface calls from here, nor in any SCH_IO objects.
    // Catch these exceptions higher up please.

    /// Save the entire library to file m_libFileName;
    virtual void Save( const std::optional<bool>& aOpt = std::nullopt );

    virtual void Load() = 0;

    virtual void AddSymbol( const LIB_SYMBOL* aSymbol );

    virtual void DeleteSymbol( const QString& aName ) = 0;

    virtual LIB_SYMBOL* GetSymbol( const QString& aName );

    // If m_libFileName is a symlink follow it to the real source file
    QFileInfo GetRealFile() const;

    QDateTime GetLibModificationTime();

    bool IsFile( const QString& aFullPathAndFileName ) const;

    bool IsFileChanged() const;

    void SetModified( bool aModified = true ) { m_isModified = aModified; }

    QString GetLogicalName() const { return m_libFileName.baseName(); }

    void SetFileName( const QString& aFileName ) { m_libFileName = QFileInfo(aFileName); }

    QString GetFileName() const { return m_libFileName.absoluteFilePath(); }

    const LIB_SYMBOL_MAP& GetSymbolMap() const { return m_symbols; }

protected:
    LIB_SYMBOL* removeSymbol( LIB_SYMBOL* aAlias );

    int               m_modHash;      // Keep track of the modification status of the library.
    std::mutex        m_modHashMutex;

    QString           m_fileName;     // Absolute path and file name.
    QFileInfo         m_libFileName;  // Absolute path and file name is required here.
    QDateTime         m_fileModTime;
    LIB_SYMBOL_MAP    m_symbols;      // Map of names of #LIB_SYMBOL pointers.
    bool              m_isWritable;
    bool              m_isModified;
    SCH_LIB_TYPE      m_libType;      // Is this cache a symbol or symbol library.
};

#endif   // _SCH_IO_LIB_CACHE_H_
