// QT_TRANSFORMATION_COMPLETED

#ifndef __SHAPE_H
#define __SHAPE_H

#include <sstream>
#include <vector>
#include <geometry/seg.h>
#include <geometry/eda_angle.h>
#include <geometry/approximation.h>
#include <math/vector2d.h>
#include <math/box2.h>
#include <QString>

class SHAPE_LINE_CHAIN;
class SHAPE_POLY_SET;

/**
 * Lists all supported shapes.
 */

enum SHAPE_TYPE
{
    SH_RECT = 0,         ///< axis-aligned rectangle
    SH_SEGMENT,          ///< line segment
    SH_LINE_CHAIN,       ///< line chain (polyline)
    SH_CIRCLE,           ///< circle
    SH_SIMPLE,           ///< simple polygon
    SH_POLY_SET,         ///< set of polygons (with holes, etc.)
    SH_COMPOUND,         ///< compound shape, consisting of multiple simple shapes
    SH_ARC,              ///< circular arc
    SH_NULL,             ///< empty shape (no shape...),
    SH_POLY_SET_TRIANGLE ///< a single triangle belonging to a POLY_SET triangulation
};

static inline QString SHAPE_TYPE_asString( SHAPE_TYPE a )
{
    switch( a )
    {
    case SH_RECT:              return QStringLiteral( "SH_RECT" );
    case SH_SEGMENT:           return QStringLiteral( "SH_SEGMENT" );
    case SH_LINE_CHAIN:        return QStringLiteral( "SH_LINE_CHAIN" );
    case SH_CIRCLE:            return QStringLiteral( "SH_CIRCLE" );
    case SH_SIMPLE:            return QStringLiteral( "SH_SIMPLE" );
    case SH_POLY_SET:          return QStringLiteral( "SH_POLY_SET" );
    case SH_COMPOUND:          return QStringLiteral( "SH_COMPOUND" );
    case SH_ARC:               return QStringLiteral( "SH_ARC" );
    case SH_NULL:              return QStringLiteral( "SH_NULL" );
    case SH_POLY_SET_TRIANGLE: return QStringLiteral( "SH_POLY_SET_TRIANGLE" );
    }

    return QString();  // Just to quiet GCC.
}

class SHAPE;

class SHAPE_BASE
{
public:
    SHAPE_BASE( SHAPE_TYPE aType ) :
        m_type( aType )
    {}

    virtual ~SHAPE_BASE()
    {}

    SHAPE_TYPE Type() const
    {
        return m_type;
    }

    QString TypeName() const
    {
        return SHAPE_TYPE_asString( m_type );
    }

    virtual bool HasIndexableSubshapes() const
    {
        return false;
    }

    virtual size_t GetIndexableSubshapeCount() const { return 0; }

    virtual void GetIndexableSubshapes( std::vector<const SHAPE*>& aSubshapes ) const { }

protected:
    SHAPE_TYPE m_type;
};

class SHAPE : public SHAPE_BASE
{
public:
    static const int MIN_PRECISION_IU = 4;

    SHAPE( SHAPE_TYPE aType ) :
        SHAPE_BASE( aType )
    {}

    virtual ~SHAPE()
    {}

    virtual SHAPE* Clone() const
    {
        assert( false );
        return nullptr;
    };

    int GetClearance( const SHAPE* aOther ) const;

    bool IsNull() const
    {
        return m_type == SH_NULL;
    }

    virtual bool Collide( const VECTOR2I& aP, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const
    {
        return Collide( SEG( aP, aP ), aClearance, aActual, aLocation );
    }

    virtual bool Collide( const SHAPE* aShape, int aClearance, VECTOR2I* aMTV ) const;

    virtual bool Collide( const SHAPE* aShape, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const;

    virtual bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const = 0;

    virtual const BOX2I BBox( int aClearance = 0 ) const = 0;

    virtual VECTOR2I Centre() const
    {
        return BBox( 0 ).Centre();
    }

    virtual int Distance( const VECTOR2I& aP ) const;

    virtual SEG::ecoord SquaredDistance( const VECTOR2I& aP, bool aOutlineOnly = false ) const;

    virtual bool PointInside( const VECTOR2I& aPt, int aAccuracy = 0, bool aUseBBoxCache = false ) const;

    virtual void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError, ERROR_LOC aErrorLoc ) const = 0;

    virtual void Rotate( const EDA_ANGLE& aAngle, const VECTOR2I& aCenter = { 0, 0 } ) = 0;

    virtual void Move( const VECTOR2I& aVector ) = 0;

    virtual bool IsSolid() const = 0;

    virtual bool Parse( std::stringstream& aStream );

    virtual const std::string Format( bool aCplusPlus = true ) const;

protected:
    typedef VECTOR2I::extended_type ecoord;
};


class SHAPE_LINE_CHAIN_BASE : public SHAPE
{
public:
    SHAPE_LINE_CHAIN_BASE( SHAPE_TYPE aType ) :
        SHAPE( aType )
    {
    }

    virtual ~SHAPE_LINE_CHAIN_BASE()
    {
    }

    virtual bool Collide( const VECTOR2I& aP, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const override;


    virtual bool Collide( const SEG& aSeg, int aClearance = 0, int* aActual = nullptr,
                          VECTOR2I* aLocation = nullptr ) const override;

    SEG::ecoord SquaredDistance( const VECTOR2I& aP, bool aOutlineOnly = false ) const override;

    bool PointInside( const VECTOR2I& aPt, int aAccuracy = 0, bool aUseBBoxCache = false ) const override;

    bool PointOnEdge( const VECTOR2I& aP, int aAccuracy = 0 ) const;

    int EdgeContainingPoint( const VECTOR2I& aP, int aAccuracy = 0 ) const;

    virtual const VECTOR2I GetPoint( int aIndex ) const   = 0;
    virtual const SEG      GetSegment( int aIndex ) const = 0;
    virtual size_t         GetPointCount() const          = 0;
    virtual size_t         GetSegmentCount() const        = 0;
    virtual bool IsClosed() const = 0;

    virtual BOX2I* GetCachedBBox() const { return nullptr; }

    void TransformToPolygon( SHAPE_POLY_SET& aBuffer, int aError,
                             ERROR_LOC aErrorLoc ) const override
    {}
};

#endif // __SHAPE_H
