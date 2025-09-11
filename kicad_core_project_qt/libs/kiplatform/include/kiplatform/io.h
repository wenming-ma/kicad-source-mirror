
#ifndef KIPLATFORM_IO_H_
#define KIPLATFORM_IO_H_

#include <stdio.h>
#include <QString>
#include <QDir>
#include <QFileInfo>

class QString;
class QFileInfo;

namespace KIPLATFORM
{
namespace IO
{
    FILE* SeqFOpen( const QString& aPath, const QString& mode );

    bool DuplicatePermissions( const QString& aSrc, const QString& aDest );

    bool IsFileHidden( const QString& aFileName );

    void LongPathAdjustment( QFileInfo& aFilename );
} // namespace IO
} // namespace KIPLATFORM

#endif // KIPLATFORM_IO_H_
