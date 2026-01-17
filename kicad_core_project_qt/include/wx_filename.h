
#ifndef WX_FILENAME_H
#define WX_FILENAME_H

#include <kicommon.h>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QDateTime>

enum FN_NORMALIZE_FLAGS {
    FN_NORM_DOTS = 0x0001,
    FN_NORM_TILDE = 0x0002,
    FN_NORM_ABSOLUTE = 0x0004,
    FN_NORM_LONG = 0x0008,
    FN_NORM_SHORTCUT = 0x0010,
    FN_NORMALIZE_DEFAULT = FN_NORM_DOTS | FN_NORM_TILDE | FN_NORM_ABSOLUTE | FN_NORM_LONG | FN_NORM_SHORTCUT
};

class KICOMMON_API WX_FILENAME
{
public:
    WX_FILENAME( const QString& aPath, const QString& aFilename );

    void SetFullName( const QString& aFileNameAndExtension );
    void SetPath( const QString& aPath );

    QString GetName() const;
    QString GetFullName() const;
    QString GetPath() const;
    QString GetFullPath() const;

    long long GetTimestamp();

    static void ResolvePossibleSymlinks( QFileInfo& aFilename );

private:
    void resolve();

    QFileInfo m_fn;
    QString   m_path;
    QString   m_fullName;
};

#endif // WX_FILENAME_H
