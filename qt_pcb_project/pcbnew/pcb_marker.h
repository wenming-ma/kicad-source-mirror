
#ifndef PCB_MARKER_H
#define PCB_MARKER_H


#include <board_item.h>
#include <pcb_shape.h>
#include <rc_item.h>
#include <marker_base.h>
#include <QString>

class DRC_ITEM;

// Coordinates count for the basic shape marker
#define MARKER_SHAPE_POINT_COUNT 9

class MSG_PANEL_ITEM;


class PCB_MARKER : public BOARD_ITEM, public MARKER_BASE
{
public:
    PCB_MARKER( std::shared_ptr<RC_ITEM> aItem, const VECTOR2I& aPos, int aLayer = F_Cu );

    ~PCB_MARKER();

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_MARKER_T == aItem->Type();
    }

    const KIID GetUUID() const override { return m_Uuid; }

    QString SerializeToString() const;

    static PCB_MARKER* DeserializeFromString( const QString& data );

    void Move( const VECTOR2I& aMoveVector ) override
    {
        m_Pos += aMoveVector;
    }

    void Rotate( const VECTOR2I& aRotCentre, const EDA_ANGLE& aAngle ) override;

    void Flip( const VECTOR2I& aCentre, FLIP_DIRECTION aFlipDirection ) override;

    VECTOR2I GetPosition() const override { return m_Pos; }
    void     SetPosition( const VECTOR2I& aPos ) override { m_Pos = aPos; }

    VECTOR2I GetCenter() const override
    {
        return GetPosition();
    }

    bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const override
    {
        if( GetMarkerType() == MARKER_RATSNEST )
            return false;
        else
            return HitTestMarker( aPosition, aAccuracy );
    }

    bool HitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const override
    {
        if( GetMarkerType() == MARKER_RATSNEST )
            return false;

        return HitTestMarker( aRect, aContained, aAccuracy );
    }

    EDA_ITEM* Clone() const override
    {
        return new PCB_MARKER( *this );
    }

    GAL_LAYER_ID GetColorLayer() const;

    std::shared_ptr<SHAPE> GetEffectiveShape( PCB_LAYER_ID aLayer,
            FLASHING aFlash = FLASHING::DEFAULT ) const override;

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    bool Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const override
    {
        return BOARD_ITEM::Matches( m_rcItem->GetErrorMessage(), aSearchData );
    }

    QString GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const override;

    BITMAPS GetMenuImage() const override;

    void SetZoom( double aZoomFactor );

    const BOX2I ViewBBox() const override;

    const BOX2I GetBoundingBox() const override;

    std::vector<int> ViewGetLayers() const override;

    SEVERITY GetSeverity() const override;

    double Similarity( const BOARD_ITEM& aBoardItem ) const override
    {
        return 0.0;
    }

    bool operator==( const BOARD_ITEM& aBoardItem ) const override
    {
        return false;
    }

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif

    /** Get class name
     * @return  string "PCB_MARKER"
     */
    virtual QString GetClass() const override
    {
        return "PCB_MARKER";
    }

    std::vector<PCB_SHAPE> GetShapes1() const { return m_shapes1; };
    std::vector<PCB_SHAPE> GetShapes2() const { return m_shapes2; };

    void SetShapes1( const std::vector<PCB_SHAPE>& aShapes ) { m_shapes1 = aShapes; };
    void SetShapes2( const std::vector<PCB_SHAPE>& aShapes ) { m_shapes2 = aShapes; };


protected:
    KIGFX::COLOR4D getColor() const override;
    std::vector<PCB_SHAPE> m_shapes1; // Shown on LAYER_DRC_SHAPE1
    std::vector<PCB_SHAPE> m_shapes2; // Shown on LAYER_DRC_SHAPE2
};

#endif      //  PCB_MARKER_H
