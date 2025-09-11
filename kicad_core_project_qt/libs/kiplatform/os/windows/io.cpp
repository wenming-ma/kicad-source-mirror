
#include <kiplatform/io.h>

#include <QString>
#include <QFileInfo>
#include <QDir>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <io.h>
#include <fcntl.h>

// Define USE_MSYS2_FALlBACK if the code for _MSC_VER does not compile on msys2
//#define  USE_MSYS2_FALLBACK

FILE* KIPLATFORM::IO::SeqFOpen( const QString& aPath, const QString& aMode )
{
#if defined( _MSC_VER ) || !defined( USE_MSYS2_FALLBACK )
    // We need to use the win32 api to setup a file handle with sequential scan flagged
    // and pass it up the chain to create a normal FILE stream
    HANDLE hFile = INVALID_HANDLE_VALUE;
    hFile = CreateFileW( reinterpret_cast<const wchar_t*>( aPath.utf16() ),
                         GENERIC_READ,
                         FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         FILE_FLAG_SEQUENTIAL_SCAN,
                         NULL );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }

    int fd = _open_osfhandle( reinterpret_cast<intptr_t>( hFile ), 0 );

    if( fd == -1 )
    {
        // close the handle manually as the ownership didnt transfer
        CloseHandle( hFile );
        return NULL;
    }

    FILE* fp = _fdopen( fd, aMode.toLocal8Bit().constData() );

    if( !fp )
    {
        // close the file descriptor manually as the ownership didnt transfer
        _close( fd );
    }

    return fp;
#else
    // Fallback for MSYS2
    return fopen( aPath.toLocal8Bit().constData(), aMode.toLocal8Bit().constData() );
#endif
}

bool KIPLATFORM::IO::DuplicatePermissions( const QString &aSrc, const QString &aDest )
{
    bool retval = false;
    DWORD dwSize = 0;

    // Retrieve the security descriptor from the source file
    if( GetFileSecurity( reinterpret_cast<const wchar_t*>( aSrc.utf16() ),
            OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
            NULL, 0, &dwSize ) )
    {
        #ifdef __MINGW32__
        // pSD is used as PSECURITY_DESCRIPTOR, aka void* pointer
        // it create an annoying warning on gcc with "delete[] pSD;" :
        // "warning: deleting 'PSECURITY_DESCRIPTOR' {aka 'void*'} is undefined"
        // so use a BYTE* pointer (do not cast it to a void pointer)
        BYTE* pSD = new BYTE[dwSize];
        #else
        PSECURITY_DESCRIPTOR pSD = static_cast<PSECURITY_DESCRIPTOR>( new BYTE[dwSize] );
        #endif

        if( !pSD )
            return false;

        if( !GetFileSecurity( reinterpret_cast<const wchar_t*>( aSrc.utf16() ),
                OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION
                        | DACL_SECURITY_INFORMATION, pSD, dwSize, &dwSize ) )
        {
            delete[] pSD;
            return false;
        }

        // Assign the retrieved security descriptor to the destination file
        if( !SetFileSecurity( reinterpret_cast<const wchar_t*>( aDest.utf16() ),
                OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION
                        | DACL_SECURITY_INFORMATION, pSD ) )
        {
            retval = false;
        }

        delete[] pSD;
    }

    return retval;
}

bool KIPLATFORM::IO::IsFileHidden( const QString& aFileName )
{
    bool result = false;

    if( ( GetFileAttributesW( reinterpret_cast<const wchar_t*>( aFileName.utf16() ) ) & FILE_ATTRIBUTE_HIDDEN ) )
        result = true;

    return result;
}


void KIPLATFORM::IO::LongPathAdjustment( QFileInfo& aFilename )
{
    // dont shortcut this for shorter lengths as there are uses like directory
    // paths that exceed the path length when you start traversing their subdirectories
    // so we want to start with the long path prefix all the time

    QString fullPath = aFilename.absoluteFilePath();
    
    if( fullPath.length() >= 2 && fullPath[1] == ':' && fullPath[0].isLetter() )
    {
        // assume single letter == drive volume
        QString newPath = "\\\\?\\" + fullPath;
        aFilename = QFileInfo(newPath);
    }
    else if( fullPath.startsWith("\\\\") && !fullPath.startsWith("\\\\?") )
    {
        // unc path aka network share
        // so skip the first slash and combine with the prefix
        // which in the case of UNCs is actually \\?\UNC\<server>\<share>
        // where UNC is literally the text UNC
        QString newPath = "\\\\?\\UNC" + fullPath.mid(1);
        aFilename = QFileInfo(newPath);
    }
    else if( fullPath.startsWith("\\\\?") && fullPath.contains("UNC") )
    {
        // Qt should handle \\?\UNC paths correctly
        // UNC gets stored into a directory
        // volume gets reduced to just \\?
        // so we need to repair it
        // Qt parsing should handle \\\\?\\UNC paths correctly
        // but if there are issues, the path is already in long format
        // so we keep it as-is
    }
}