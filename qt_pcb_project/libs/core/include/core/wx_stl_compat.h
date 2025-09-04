// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_QT_STL_COMPAT_H
#define KICAD_QT_STL_COMPAT_H

#include <string_view>

#include <QPoint>
#include <QSize>
#include <QString>

namespace std
{
    template <> struct hash<QPoint>
    {
        size_t operator() ( const QPoint& k ) const
        {
            return std::hash<int>()(k.x()) ^ (std::hash<int>()(k.y()) << 1);
        }
    };
}

namespace std
{
    template<> struct less<QPoint>
    {
        bool operator()( const QPoint& aA, const QPoint& aB ) const;
    };
}

QString ToQString( const std::string_view& view );

std::ostream& operator<<( std::ostream& out, const QSize& size );

std::ostream& operator<<( std::ostream& out, const QPoint& pt );

#endif // KICAD_QT_STL_COMPAT_H
