#include <sstream>

#include <geometry/shape_circle.h>
#include <convert_basic_shapes_to_polygon.h>

const std::string SHAPE_CIRCLE::Format( bool aCplusPlus ) const
{
    std::stringstream ss;

    if( aCplusPlus )
    {
        ss << "SHAPE_CIRCLE( VECTOR2I( ";
        ss << m_circle.Center.x;
        ss << ", ";
        ss << m_circle.Center.y;
        ss << "), ";
        ss << m_circle.Radius;
        ss << "); ";
    }   else
    {
        ss << SHAPE::Format( aCplusPlus ) << " ";
        ss << m_circle.Center.x;
        ss << " ";
        ss << m_circle.Center.y;
        ss << " ";
        ss << m_circle.Radius;
    }
    return ss.str();
}


void SHAPE_CIRCLE::TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                                       ERROR_LOC aErrorLoc ) const
{
    TransformCircleToPolygon( aBuffer, m_circle.Center, m_circle.Radius, aError, aErrorLoc );
}