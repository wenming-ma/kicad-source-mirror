// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-04

#include <ostream>
#include <core/wx_stl_compat.h>

bool std::less<QPoint>::operator()( const QPoint& aA, const QPoint& aB ) const
{
    if( aA.x() == aB.x() )
        return aA.y() < aB.y();

    return aA.x() < aB.x();
}

QString ToQString( const std::string_view& view )
{
    return QString::fromUtf8( view.data(), static_cast<int>( view.length() ) );
}

std::ostream& operator<<( std::ostream& out, const QSize& size )
{
    out << " width=\"" << size.width() << "\" height=\"" << size.height() << "\"";
    return out;
}

std::ostream& operator<<( std::ostream& out, const QPoint& pt )
{
    out << " x=\"" << pt.x() << "\" y=\"" << pt.y() << "\"";
    return out;
}
