// QT_TRANSFORMATION_COMPLETED

#ifndef REFDES_UTILS__H
#define REFDES_UTILS__H

#include <QString>

namespace UTIL
{

QString GetRefDesPrefix( const QString& aRefDes );

QString GetRefDesUnannotated( const QString& aRefDes );

int GetRefDesNumber( const QString& aRefDes );

} // namespace UTIL

#endif // REFDES_UTILS__H
