
#pragma once

#include <map>
#include <set>

#include <QString>
#include <QFileInfo>
#include <QHash>

#include <mmh3_hash.h>
#include <picosha2.h>
#include <wildcards_and_files_ext.h>
#include <functional>

class OUTPUTFORMATTER;

namespace KIFONT
{
    class OUTLINE_FONT;
}

class EMBEDDED_FILES
{
public:
    struct EMBEDDED_FILE
    {
        enum class FILE_TYPE
        {
            FONT,
            MODEL,
            WORKSHEET,
            DATASHEET,
            OTHER
        };

        EMBEDDED_FILE() :
                type( FILE_TYPE::OTHER ),
                is_valid( false )
        {}

        bool Validate()
        {
            MMH3_HASH hash( EMBEDDED_FILES::Seed() );
            hash.add( decompressedData );

            is_valid = ( hash.digest().ToString() == data_hash );
            return is_valid;
        }

        // This is the old way of validating the file.  It is deprecated and retained only
        // to validate files that were previously embedded.
        bool Validate_SHA256()
        {
            std::string new_sha;
            picosha2::hash256_hex_string( decompressedData, new_sha );

            is_valid = ( new_sha == data_hash );
            return is_valid;
        }

        QString GetLink() const
        {
            return QString("%1://%2").arg(FILEEXT::KiCadUriPrefix, name);
        }

        QString           name;
        FILE_TYPE         type;
        bool              is_valid;
        std::string       compressedEncodedData;
        std::vector<char> decompressedData;
        std::string       data_hash;
    };

    enum class RETURN_CODE : int
    {
        OK,                  ///< Success.
        FILE_NOT_FOUND,      ///< File not found on disk.
        PERMISSIONS_ERROR,   ///< Could not read/write file.
        FILE_ALREADY_EXISTS, ///< File already exists in the collection.
        OUT_OF_MEMORY,       ///< Could not allocate memory.
        CHECKSUM_ERROR,      ///< Checksum in file does not match data.
    };

    EMBEDDED_FILES() = default;

    EMBEDDED_FILES( EMBEDDED_FILES&& other ) noexcept;
    EMBEDDED_FILES( const EMBEDDED_FILES& other );

    ~EMBEDDED_FILES()
    {
        for( auto& file : m_files )
            delete file.second;
    }

    using FILE_ADDED_CALLBACK = std::function<void( EMBEDDED_FILE* )>;

    void SetFileAddedCallback( FILE_ADDED_CALLBACK callback )
    {
        m_fileAddedCallback = callback;
    }

    FILE_ADDED_CALLBACK GetFileAddedCallback() const
    {
        return m_fileAddedCallback;
    }

    EMBEDDED_FILE* AddFile( const QFileInfo& aName, bool aOverwrite );

    void AddFile( EMBEDDED_FILE* aFile );

    void RemoveFile( const QString& name, bool aErase = true );

    void WriteEmbeddedFiles( OUTPUTFORMATTER& aOut, bool aWriteData ) const;

    QString GetEmbeddedFileLink( const EMBEDDED_FILE& aFile ) const
    {
        return aFile.GetLink();
    }

    bool HasFile( const QString& name ) const
    {
        QFileInfo fileName( name );

        return m_files.find( fileName.fileName() ) != m_files.end();
    }

    bool IsEmpty() const
    {
        return m_files.empty();
    }

    virtual void RunOnNestedEmbeddedFiles( const std::function<void( EMBEDDED_FILES* )>& aFunction )
    {
    }

    const std::vector<QString>* UpdateFontFiles();

    const std::vector<QString>* GetFontFiles() const;

    void ClearEmbeddedFonts();

    static RETURN_CODE  CompressAndEncode( EMBEDDED_FILE& aFile );

    static RETURN_CODE  DecompressAndDecode( EMBEDDED_FILE& aFile );

    EMBEDDED_FILE* GetEmbeddedFile( const QString& aName ) const
    {
        auto it = m_files.find( aName );

        return it == m_files.end() ? nullptr : it->second;
    }

    const std::map<QString, EMBEDDED_FILE*>& EmbeddedFileMap() const
    {
        return m_files;
    }

    QFileInfo GetTemporaryFileName( const QString& aName ) const;

    QFileInfo GetTemporaryFileName( EMBEDDED_FILE* aFile ) const;

    void ClearEmbeddedFiles( bool aDeleteFiles = true )
    {
        for( auto& file : m_files )
        {
            if( aDeleteFiles )
                delete file.second;
        }

        m_files.clear();
    }

    virtual void EmbedFonts() {};

    virtual std::set<KIFONT::OUTLINE_FONT*> GetFonts() const
    {
        return std::set<KIFONT::OUTLINE_FONT*>();
    };

    void SetAreFontsEmbedded( bool aEmbedFonts )
    {
        m_embedFonts = aEmbedFonts;
    }

    bool GetAreFontsEmbedded() const
    {
        return m_embedFonts;
    }

    static uint32_t Seed()
    {
        return 0xABBA2345;
    }

    EMBEDDED_FILES& operator=( EMBEDDED_FILES&& other ) noexcept;
    EMBEDDED_FILES& operator=( const EMBEDDED_FILES& other );

private:
    std::map<QString, EMBEDDED_FILE*> m_files;
    std::vector<QString>              m_fontFiles;
    FILE_ADDED_CALLBACK                m_fileAddedCallback;

protected:
    bool m_embedFonts = false; ///< If set, fonts will be embedded in the element on save.
                               ///< Otherwise, font files embedded in the element will be
                               ///< removed on save.
};
