#ifndef KICAD_ASSET_ARCHIVE_H
#define KICAD_ASSET_ARCHIVE_H

#include <kicommon.h>
#include <QHash>
#include <vector>

#include <QString>

class KICOMMON_API ASSET_ARCHIVE
{
public:
    ASSET_ARCHIVE( const QString& aFilePath, bool aLoadNow = true );

    ~ASSET_ARCHIVE() = default;

    bool Load();

    long GetFileContents( const QString& aFilePath, const unsigned char* aDest, size_t aMaxLen );

    long GetFilePointer( const QString& aFilePath, const unsigned char** aDest );

private:
    struct FILE_INFO
    {
        size_t offset;
        size_t length;
    };

    QHash<QString, FILE_INFO> m_fileInfoCache;

    std::vector<unsigned char> m_cache;

    QString m_filePath;
};

#endif // KICAD_ASSET_ARCHIVE_H