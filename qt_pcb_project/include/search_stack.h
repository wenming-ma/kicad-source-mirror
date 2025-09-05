// QT_TRANSFORMATION_COMPLETED

#ifndef SEARCH_STACK_H_
#define SEARCH_STACK_H_

#include <kicommon.h>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <project.h>


class KICOMMON_API SEARCH_STACK : public PROJECT::_ELEM
{
public:
    PROJECT::ELEM ProjectElementType() override { return PROJECT::ELEM::SEARCH_STACK; }

#if defined(DEBUG)
    void Show( const std::string& aPrefix ) const;
#endif

    std::string FilenameWithRelativePathInSearchList( const std::string& aFullFilename,
                                                       const std::string& aBaseDir );

    void AddPaths( const std::string& aPaths, int aIndex = -1 );

    void RemovePaths( const std::string& aPaths );

    static int Split( std::vector<std::string>* aResult, const std::string& aPathString );

#if 1   // this function is so poorly designed it deserves not to exist.
    const std::string LastVisitedPath( const std::string& aSubPathToSearch = std::string() );
#endif

    // Basic path list operations to replace wxPathList functionality
    void Add( const std::string& path );
    void AddEnvList( const std::string& envVariable );
    void Clear();
    std::string FindValidPath( const std::string& file ) const;
    std::string FindAbsoluteValidPath( const std::string& file ) const;
    size_t GetCount() const;
    std::string Item( size_t index ) const;
    
    // std::vector interface methods
    size_t size() const { return m_paths.size(); }
    bool empty() const { return m_paths.empty(); }
    std::string& operator[]( size_t index ) { return m_paths[index]; }
    const std::string& operator[]( size_t index ) const { return m_paths[index]; }
    void push_back( const std::string& path ) { m_paths.push_back( path ); }
    void insert( size_t index, const std::string& path ) {
        if( index >= m_paths.size() ) m_paths.push_back( path );
        else m_paths.insert( m_paths.begin() + index, path );
    }
    void removeAt( size_t index ) {
        if( index < m_paths.size() ) m_paths.erase( m_paths.begin() + index );
    }
    auto begin() { return m_paths.begin(); }
    auto end() { return m_paths.end(); }
    auto begin() const { return m_paths.begin(); }
    auto end() const { return m_paths.end(); }
    int indexOf( const std::string& path ) const {
        auto it = std::find( m_paths.begin(), m_paths.end(), path );
        return it == m_paths.end() ? -1 : std::distance( m_paths.begin(), it );
    }

private:
    std::vector<std::string> m_paths;

};

#endif  // SEARCH_STACK_H_
