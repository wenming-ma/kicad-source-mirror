// QT_TRANSFORMATION_COMPLETED

#ifndef VECTOR2QT_H_
#define VECTOR2QT_H_

#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include "vector2d.h"

inline VECTOR2I ToVECTOR2I(const QSize& aSize)
{
    return { aSize.width(), aSize.height() };
}

inline VECTOR2I ToVECTOR2I( const QPoint & aPoint )
{
    return { aPoint.x(), aPoint.y() };
}

inline VECTOR2D ToVECTOR2D( const QPoint& aPoint )
{
    return VECTOR2D( aPoint.x(), aPoint.y() );
}

inline VECTOR2D ToVECTOR2D( const QPointF& aPoint )
{
    return VECTOR2D( aPoint.x(), aPoint.y() );
}

inline VECTOR2D ToVECTOR2D( const QSize& aSize )
{
    return VECTOR2D( aSize.width(), aSize.height() );
}

inline VECTOR2D ToVECTOR2D( const QSizeF& aSize )
{
    return VECTOR2D( aSize.width(), aSize.height() );
}

inline QPoint ToQPoint( const VECTOR2I& aVector )
{
    return QPoint( aVector.x, aVector.y );
}

inline QPointF ToQPointF( const VECTOR2D& aVector )
{
    return QPointF( aVector.x, aVector.y );
}

inline QSize ToQSize( const VECTOR2I& aVector )
{
    return QSize( aVector.x, aVector.y );
}

inline QSizeF ToQSizeF( const VECTOR2D& aVector )
{
    return QSizeF( aVector.x, aVector.y );
}

#endif