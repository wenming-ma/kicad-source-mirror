
#ifndef GLYPH_H
#define GLYPH_H

#include <vector>
#include <gal/gal.h>
#include <memory>
#include <math/box2.h>
#include <geometry/shape_poly_set.h>
#include <geometry/eda_angle.h>

#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4275 )
#endif

namespace KIFONT
{


class GAL_API GLYPH
{
public:
    virtual ~GLYPH()
    {}

    virtual bool IsOutline() const { return false; }
    virtual bool IsStroke() const  { return false; }

    virtual BOX2D BoundingBox() = 0;
};


class GAL_API OUTLINE_GLYPH : public GLYPH, public SHAPE_POLY_SET
{
public:
    OUTLINE_GLYPH() :
            SHAPE_POLY_SET()
    {}

    OUTLINE_GLYPH( const OUTLINE_GLYPH& aGlyph ) :
            SHAPE_POLY_SET( aGlyph )
    {}

    OUTLINE_GLYPH( const SHAPE_POLY_SET& aPoly ) :
            SHAPE_POLY_SET( aPoly )
    {}

    bool IsOutline() const override { return true; }

    BOX2D BoundingBox() override;

    void Triangulate( std::function<void( const VECTOR2I& aPt1,
                                          const VECTOR2I& aPt2,
                                          const VECTOR2I& aPt3 )> aCallback ) const;

    void CacheTriangulation( bool aPartition = true, bool aSimplify = false ) override;

    std::vector<std::unique_ptr<SHAPE_POLY_SET::TRIANGULATED_POLYGON>> GetTriangulationData() const;

    void CacheTriangulation(
            std::vector<std::unique_ptr<SHAPE_POLY_SET::TRIANGULATED_POLYGON>>& aHintData );
};


class GAL_API STROKE_GLYPH : public GLYPH, public std::vector<std::vector<VECTOR2D>>
{
public:
    STROKE_GLYPH()
    {}

    STROKE_GLYPH( const STROKE_GLYPH& aGlyph );

    bool IsStroke() const override { return true; }

    void AddPoint( const VECTOR2D& aPoint );
    void RaisePen();
    void Finalize();

    BOX2D BoundingBox() override { return m_boundingBox; }
    void SetBoundingBox( const BOX2D& bbox ) { m_boundingBox = bbox; }

    std::unique_ptr<GLYPH> Transform( const VECTOR2D& aGlyphSize,  const VECTOR2I& aOffset,
                                      double aTilt, const EDA_ANGLE& aAngle, bool aMirror,
                                      const VECTOR2I& aOrigin  );

    void Move( const VECTOR2I& aOffset );

private:
    bool  m_penIsDown = false;
    BOX2D m_boundingBox;
};



} // namespace KIFONT

#if defined( _MSC_VER )
#pragma warning( pop )
#endif

#endif  // GLYPH_H
