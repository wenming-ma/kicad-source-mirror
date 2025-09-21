
#ifndef GRAPHICS_IMPORTER_EESCHEMA_H
#define GRAPHICS_IMPORTER_EESCHEMA_H

#include <import_gfx/graphics_importer.h>


class LIB_SYMBOL;

class GRAPHICS_IMPORTER_LIB_SYMBOL : public GRAPHICS_IMPORTER
{
public:
    GRAPHICS_IMPORTER_LIB_SYMBOL( LIB_SYMBOL* aSymbol, int aUnit );

    void AddLine( const VECTOR2D& aStart, const VECTOR2D& aEnd,
                  const IMPORTED_STROKE& aStroke ) override;

    void AddCircle( const VECTOR2D& aCenter, double aRadius, const IMPORTED_STROKE& aStroke,
                    bool aFilled, const COLOR4D& aFillColor = COLOR4D::UNSPECIFIED ) override;

    void AddArc( const VECTOR2D& aCenter, const VECTOR2D& aStart, const EDA_ANGLE& aAngle,
                 const IMPORTED_STROKE& aStroke ) override;

    void AddPolygon( const std::vector<VECTOR2D>& aVertices, const IMPORTED_STROKE& aStroke,
                     bool aFilled, const COLOR4D& aFillColor = COLOR4D::UNSPECIFIED ) override;

    void AddText( const VECTOR2D& aOrigin, const QString& aText, double aHeight, double aWidth,
                  double aThickness, double aOrientation, GR_TEXT_H_ALIGN_T aHJustify,
                  GR_TEXT_V_ALIGN_T aVJustify,
                  const COLOR4D&    aColor = COLOR4D::UNSPECIFIED ) override;

    void AddSpline( const VECTOR2D& aStart, const VECTOR2D& aBezierControl1,
                    const VECTOR2D& aBezierControl2, const VECTOR2D& aEnd,
                    const IMPORTED_STROKE& aStroke ) override;

    /**
     * Convert an imported coordinate to a board coordinate, according to the internal units,
     * user scale and offset
     *
     * @param aCoordinate is the imported coordinate in mm.
     */
    VECTOR2I MapCoordinate( const VECTOR2D& aCoordinate );

    /**
     * If aLineWidth < 0, the default line thickness value is returned.
     *
     * @param aLineWidth is the line thickness in mm to convert.
     * @return a line thickness in a board Iu value, according to the internal units.
     */
    int MapLineWidth( double aLineWidth );

    STROKE_PARAMS MapStrokeParams( const IMPORTED_STROKE& aStroke );

    LIB_SYMBOL* m_symbol;
    int         m_unit;
};

#endif /* GRAPHICS_IMPORTER_EESCHEMA */
