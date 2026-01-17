#include <geometry/ellipse.h>


template <typename NumericType>
ELLIPSE<NumericType>::ELLIPSE( const VECTOR2<NumericType>& aCenter, NumericType aMajorRadius,
                               NumericType aMinorRadius, EDA_ANGLE aRotation, EDA_ANGLE aStartAngle,
                               EDA_ANGLE aEndAngle ) :
        Center( aCenter ),
        MajorRadius( aMajorRadius ),
        MinorRadius( aMinorRadius ),
        Rotation( aRotation ),
        StartAngle( aStartAngle ),
        EndAngle( aEndAngle )
{
}


template <typename NumericType>
ELLIPSE<NumericType>::ELLIPSE( const VECTOR2<NumericType>& aCenter,
                               const VECTOR2<NumericType>& aMajor, double aRatio,
                               EDA_ANGLE aStartAngle, EDA_ANGLE aEndAngle ) :
        Center( aCenter ),
        StartAngle( aStartAngle ),
        EndAngle( aEndAngle )
{
    MajorRadius = aMajor.EuclideanNorm();
    MinorRadius = NumericType( MajorRadius * aRatio );
    Rotation = EDA_ANGLE( std::atan2( aMajor.y, aMajor.x ), RADIANS_T );
}

template class ELLIPSE<double>;
template class ELLIPSE<int>;