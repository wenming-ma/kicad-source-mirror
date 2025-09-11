
#include <search_stack.h>
#include <string_utils.h>
#include <trace_helpers.h>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;


#if defined(_WIN32)
 #define PATH_SEPS          ";\r\n"
#else
 #define PATH_SEPS          ":;\r\n"       // unix == linux | mac
#endif


int SEARCH_STACK::Split( std::vector<std::string>* aResult, const std::string& aPathString )
{
    std::istringstream iss( aPathString );
    std::string path;
    
    // Split on any character in PATH_SEPS
    for( char c : aPathString )
    {
        bool isSep = false;
        for( const char* sep = PATH_SEPS; *sep; ++sep )
        {
            if( c == *sep )
            {
                isSep = true;
                break;
            }
        }
        
        if( isSep )
        {
            if( !path.empty() )
            {
                aResult->push_back( path );
                path.clear();
            }
        }
        else
        {
            path += c;
        }
    }
    
    if( !path.empty() )
        aResult->push_back( path );
        
    return aResult->size();
}


// Convert aRelativePath to an absolute path based on aBaseDir
static std::string base_dir( const std::string& aRelativePath, const std::string& aBaseDir )
{
    fs::path relativePath( aRelativePath );
    
    if( !relativePath.is_absolute() && !aBaseDir.empty() )
    {
        fs::path basePath( aBaseDir );
        if( !basePath.is_absolute() )
        {
            // Assert equivalent - in debug mode, this would cause an error
            #ifdef DEBUG
            std::cerr << "Error: Must pass absolute path in aBaseDir" << std::endl;
            #endif
            return aRelativePath;
        }
        return (basePath / relativePath).string();
    }
    
    return fs::absolute( relativePath ).string();
}


std::string SEARCH_STACK::FilenameWithRelativePathInSearchList(
        const std::string& aFullFilename, const std::string& aBaseDir )
{
    fs::path fullPath( aFullFilename );
    std::string filename = aFullFilename;
    
    size_t pathlen = fullPath.parent_path().string().length();
    
    for( size_t kk = 0; kk < GetCount(); kk++ )
    {
        std::string baseDirPath = base_dir( (*this)[kk], aBaseDir );
        fs::path baseDir( baseDirPath );
        
        try
        {
            fs::path relativePath = fs::relative( fullPath, baseDir );
            std::string relativePathStr = relativePath.string();
            
            if( !relativePathStr.empty() && relativePathStr != aFullFilename )
            {
                // Check if path goes outside (starts with "..")
                if( relativePathStr.substr(0, 2) == ".." )
                    continue;
                    
                if( pathlen > relativePath.parent_path().string().length() )
                {
                    filename = relativePathStr;
                    pathlen = relativePath.parent_path().string().length();
                }
            }
        }
        catch( const std::exception& )
        {
            // If relative path calculation fails, skip this entry
            continue;
        }
    }
    
    return filename;
}


void SEARCH_STACK::RemovePaths( const std::string& aPaths )
{
    std::vector<std::string> paths;
    
    Split( &paths, aPaths );
    
    for( const std::string& path : paths )
    {
        int index = indexOf( path );
        if( index != -1 )
        {
            removeAt( index );
        }
    }
}


void SEARCH_STACK::AddPaths( const std::string& aPaths, int aIndex )
{
    std::vector<std::string> paths;
    
    Split( &paths, aPaths );
    
    // appending all of them, on large or negative aIndex
    if( static_cast<size_t>( aIndex ) >= GetCount() || aIndex < 0 )
    {
        for( const std::string& path : paths )
        {
            std::error_code ec;
            if( fs::exists( path, ec ) && !ec && indexOf( path ) == -1 )
            {
                push_back( path );
            }
        }
    }
    // inserting all of them:
    else
    {
        for( const std::string& path : paths )
        {
            std::error_code ec;
            if( fs::exists( path, ec ) && !ec && indexOf( path ) == -1 )
            {
                insert( aIndex, path );
                aIndex++;
            }
        }
    }
}


#if 1       // this function is too convoluted for words.

const std::string SEARCH_STACK::LastVisitedPath( const std::string& aSubPathToSearch )
{
    std::string path;
    
    // Initialize default path to the main default lib path
    // this is the second path in list (the first is the project path).
    size_t pcount = GetCount();
    
    if( pcount )
    {
        size_t ipath = 0;
        
        std::string currentPath = fs::current_path().string();
        if( (*this)[0] == currentPath )
            ipath = 1;
            
        // First choice of path:
        if( ipath < pcount )
            path = (*this)[ipath];
            
        // Search a sub path matching this SEARCH_PATH
        if( !empty() )
        {
            for( ; ipath < pcount; ipath++ )
            {
                if( (*this)[ipath].find( aSubPathToSearch ) != std::string::npos )
                {
                    path = (*this)[ipath];
                    break;
                }
            }
        }
    }
    
    if( path.empty() )
        path = fs::current_path().string();
        
    return path;
}
#endif


#if defined( DEBUG )
void SEARCH_STACK::Show( const std::string& aPrefix ) const
{
    std::cout << aPrefix << " SEARCH_STACK:" << std::endl;
    
    for( size_t i = 0; i < GetCount(); ++i )
    {
        std::cout << "  [" << i << "]:" << (*this)[i] << std::endl;
    }
}
#endif


// Implementation of missing methods
void SEARCH_STACK::Add( const std::string& path )
{
    if( indexOf( path ) == -1 )
    {
        m_paths.push_back( path );
    }
}

void SEARCH_STACK::AddEnvList( const std::string& envVariable )
{
    const char* envValue = std::getenv( envVariable.c_str() );
    if( envValue )
    {
        AddPaths( std::string( envValue ) );
    }
}

void SEARCH_STACK::Clear()
{
    m_paths.clear();
}

std::string SEARCH_STACK::FindValidPath( const std::string& file ) const
{
    for( const std::string& path : m_paths )
    {
        fs::path fullPath = fs::path( path ) / file;
        std::error_code ec;
        if( fs::exists( fullPath, ec ) && !ec )
        {
            return fullPath.string();
        }
    }
    return std::string();
}

std::string SEARCH_STACK::FindAbsoluteValidPath( const std::string& file ) const
{
    std::string foundPath = FindValidPath( file );
    if( !foundPath.empty() )
    {
        return fs::absolute( foundPath ).string();
    }
    return std::string();
}

size_t SEARCH_STACK::GetCount() const
{
    return m_paths.size();
}

std::string SEARCH_STACK::Item( size_t index ) const
{
    if( index < m_paths.size() )
        return m_paths[index];
    return std::string();
}
