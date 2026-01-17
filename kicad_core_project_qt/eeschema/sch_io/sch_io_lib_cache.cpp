
#include "sch_io_lib_cache.h"

#include <lib_symbol.h>
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include <QtGlobal>


SCH_IO_LIB_CACHE::SCH_IO_LIB_CACHE( const QString& aFullPathAndFileName ) :
    m_modHash( 1 ),
    m_fileName( aFullPathAndFileName ),
    m_libFileName( aFullPathAndFileName ),
    m_isWritable( true ),
    m_isModified( false )
{
    m_libType = SCH_LIB_TYPE::LT_EESCHEMA;
}


SCH_IO_LIB_CACHE::~SCH_IO_LIB_CACHE()
{
    // When the cache is destroyed, all of the alias objects on the heap should be deleted.
    for( auto& symbol : m_symbols )
        delete symbol.second;

    m_symbols.clear();
}


void SCH_IO_LIB_CACHE::Save( const std::optional<bool>& aOpt )
{
    Q_ASSERT( false );
}


QFileInfo SCH_IO_LIB_CACHE::GetRealFile() const
{
    QFileInfo fn( m_libFileName );

    // If m_libFileName is a symlink follow it to the real source file
    if( fn.isSymLink() )
        fn = QFileInfo( fn.symLinkTarget() );
    return fn;
}


QDateTime SCH_IO_LIB_CACHE::GetLibModificationTime()
{
    QFileInfo fn = GetRealFile();

    // update the writable flag while we have a QFileInfo, in a network this
    // is possibly quite dynamic anyway.
    m_isWritable = fn.isWritable();

    return fn.lastModified();
}


bool SCH_IO_LIB_CACHE::IsFile( const QString& aFullPathAndFileName ) const
{
    return m_fileName == aFullPathAndFileName;
}


bool SCH_IO_LIB_CACHE::IsFileChanged() const
{
    QFileInfo fn = GetRealFile();

    if( m_fileModTime.isValid() && fn.exists() && fn.isFile() )
        return fn.lastModified() != m_fileModTime;

    return false;
}


LIB_SYMBOL* SCH_IO_LIB_CACHE::removeSymbol( LIB_SYMBOL* aSymbol )
{
    if( aSymbol == nullptr )
        return nullptr;

    LIB_SYMBOL* firstChild = nullptr;
    LIB_SYMBOL_MAP::iterator it = m_symbols.find( aSymbol->GetName() );

    if( it == m_symbols.end() )
        return nullptr;

    // If the entry pointer doesn't match the name it is mapped to in the library, we
    // have done something terribly wrong.
    if( &*it->second != aSymbol )
        return nullptr;

    // If the symbol is a root symbol used by other symbols find the first alias that uses
    // the root symbol and make it the new root.
    if( aSymbol->IsRoot() )
    {
        for( const std::pair<const QString, LIB_SYMBOL*>& entry : m_symbols )
        {
            if( entry.second->IsAlias()
              && entry.second->GetParent().lock() == aSymbol->SharedPtr() )
            {
                firstChild = entry.second;
                break;
            }
        }

        if( firstChild )
        {
            for( SCH_ITEM& drawItem : aSymbol->GetDrawItems() )
            {
                if( drawItem.Type() == SCH_FIELD_T )
                {
                    SCH_FIELD& field = static_cast<SCH_FIELD&>( drawItem );

                    if( firstChild->FindField( field.GetCanonicalName() ) )
                        continue;
                }

                SCH_ITEM* newItem = (SCH_ITEM*) drawItem.Clone();
                drawItem.SetParent( firstChild );
                firstChild->AddDrawItem( newItem );
            }

            // Reparent the remaining aliases.
            for( const std::pair<const QString, LIB_SYMBOL*>& entry : m_symbols )
            {
                if( entry.second->IsAlias()
                      && entry.second->GetParent().lock() == aSymbol->SharedPtr() )
                {
                    entry.second->SetParent( firstChild );
                }
            }
        }
    }

    m_symbols.erase( it );
    delete aSymbol;
    m_isModified = true;
    IncrementModifyHash();
    return firstChild;
}


void SCH_IO_LIB_CACHE::AddSymbol( const LIB_SYMBOL* aSymbol )
{
    // aSymbol is cloned in SYMBOL_LIB::AddSymbol().  The cache takes ownership of aSymbol.
    QString name = aSymbol->GetName();
    LIB_SYMBOL_MAP::iterator it = m_symbols.find( name );

    if( it != m_symbols.end() )
    {
        removeSymbol( it->second );
    }

    m_symbols[ name ] = const_cast< LIB_SYMBOL* >( aSymbol );
    m_isModified = true;
    IncrementModifyHash();
}


LIB_SYMBOL* SCH_IO_LIB_CACHE::GetSymbol( const QString& aName )
{
    LIB_SYMBOL_MAP::iterator it = m_symbols.find( aName );

    if( it != m_symbols.end() )
    {
        return it->second;
    }

    return nullptr;
}
