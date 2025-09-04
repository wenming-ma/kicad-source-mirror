#pragma once

#include <gal/gal.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <memory>

#include <math/box2.h>
#include <gal/definitions.h>

#include <view/view_overlay.h>

namespace KIGFX
{
class PAINTER;
class GAL;
class VIEW_ITEM;
class VIEW_GROUP;
class VIEW_RTREE;

class GAL_API VIEW
{
public:
    friend class VIEW_ITEM;

    typedef std::pair<VIEW_ITEM*, int> LAYER_ITEM_PAIR;

    VIEW();
    virtual ~VIEW();

    // We own at least one list of raw pointers.  Don't let the compiler fill in copy c'tors that
    // will only land us in trouble.
    VIEW( const VIEW& ) = delete;
    VIEW& operator=( const VIEW& ) = delete;

    static void OnDestroy( VIEW_ITEM* aItem );

    virtual void Add( VIEW_ITEM* aItem, int aDrawPriority = -1 );

    virtual void Remove( VIEW_ITEM* aItem );

    int Query( const BOX2I& aRect, std::vector<LAYER_ITEM_PAIR>& aResult ) const;

    void Query( const BOX2I& aRect, const std::function<bool( VIEW_ITEM* )>& aFunc ) const;

    void SetVisible( VIEW_ITEM* aItem, bool aIsVisible = true );

    void Hide( VIEW_ITEM* aItem, bool aHide = true, bool aHideOverlay = false );

    bool IsVisible( const VIEW_ITEM* aItem ) const;

    bool IsHiddenOnOverlay( const VIEW_ITEM* aItem ) const;

    bool HasItem( const VIEW_ITEM* aItem ) const;

    virtual void Update( const VIEW_ITEM* aItem, int aUpdateFlags ) const;
    virtual void Update( const VIEW_ITEM* aItem ) const;

    void SetRequired( int aLayerId, int aRequiredId, bool aRequired = true );

    void CopySettings( const VIEW* aOtherView );

    void SetGAL( GAL* aGal );

    inline GAL* GetGAL() const
    {
        return m_gal;
    }

    inline void SetPainter( PAINTER* aPainter )
    {
        m_painter = aPainter;
    }

    inline PAINTER* GetPainter() const
    {
        return m_painter;
    }

    void SetViewport( const BOX2D& aViewport );

    BOX2D GetViewport() const;

    void SetMirror( bool aMirrorX, bool aMirrorY );

    bool IsMirroredX() const
    {
        return m_mirrorX;
    }

    bool IsMirroredY() const
    {
        return m_mirrorY;
    }

    virtual void SetScale( double aScale, VECTOR2D aAnchor = { 0, 0 } );

    inline double GetScale() const
    {
        return m_scale;
    }

    inline void SetBoundary( const BOX2D& aBoundary )
    {
        m_boundary = aBoundary;
    }

    inline void SetBoundary( const BOX2I& aBoundary )
    {
        m_boundary.SetOrigin( aBoundary.GetOrigin() );
        m_boundary.SetEnd( aBoundary.GetEnd() );
    }

    inline const BOX2D& GetBoundary() const
    {
        return m_boundary;
    }

    void SetScaleLimits( double aMaximum, double aMinimum )
    {
        Q_ASSERT_X( aMaximum > aMinimum, "SetScaleLimits", "Maximum must be greater than minimum" );

        m_minScale = aMinimum;
        m_maxScale = aMaximum;
    }

    void SetCenter( const VECTOR2D& aCenter );

    void SetCenter( const VECTOR2D& aCenter, const std::vector<BOX2D>& obscuringScreenRects );

    const VECTOR2D& GetCenter() const
    {
        return m_center;
    }

    VECTOR2D ToWorld( const VECTOR2D& aCoord, bool aAbsolute = true ) const;

    double ToWorld( double aSize ) const;

    VECTOR2D ToScreen( const VECTOR2D& aCoord, bool aAbsolute = true ) const;

    double ToScreen( double aSize ) const;

    const VECTOR2I& GetScreenPixelSize() const;

    void Clear();

    inline void SetLayerVisible( int aLayer, bool aVisible = true )
    {
        auto it = m_layers.find( aLayer );

        if( it == m_layers.end() )
            return;

        VIEW_LAYER& layer = it->second;

        if( layer.visible != aVisible )
        {
            // Target has to be redrawn after changing its visibility
            MarkTargetDirty( layer.target );
            layer.visible = aVisible;
        }
    }

    inline bool IsLayerVisible( int aLayer ) const
    {
        auto it = m_layers.find( aLayer );

        if( it == m_layers.end() )
            return false;

        return it->second.visible;
    }

    inline void SetLayerDiff( int aLayer, bool aDiff = true )
    {
        auto it = m_layers.find( aLayer );

        if( it == m_layers.end() )
            return;

        VIEW_LAYER& layer = it->second;

        if( layer.diffLayer != aDiff )
        {
            // Target has to be redrawn after changing its layers' diff status
            MarkTargetDirty( layer.target );
            layer.diffLayer = aDiff;
        }
    }

    inline void SetLayerHasNegatives( int aLayer, bool aNegatives = true )
    {
        auto it = m_layers.find( aLayer );

        if( it == m_layers.end() )
            return;

        VIEW_LAYER& layer = it->second;

        if( layer.hasNegatives != aNegatives )
        {
            // Target has to be redrawn after changing a layers' negatives
            MarkTargetDirty( layer.target );
            layer.hasNegatives = aNegatives;
        }
    }

    inline void SetLayerDisplayOnly( int aLayer, bool aDisplayOnly = true )
    {
        auto it = m_layers.find( aLayer );

        if( it == m_layers.end() )
            return;

        it->second.displayOnly = aDisplayOnly;
    }

    inline void SetLayerTarget( int aLayer, RENDER_TARGET aTarget )
    {
        auto it = m_layers.find( aLayer );

        if( it == m_layers.end() )
            return;

        it->second.target = aTarget;
    }

    void SetLayerOrder( int aLayer, int aRenderingOrder );

    int GetLayerOrder( int aLayer ) const;

    void SortLayers( std::vector<int>& aLayers ) const;

    void ReorderLayerData( std::unordered_map<int, int> aReorderMap );

    void UpdateLayerColor( int aLayer );

    void UpdateAllLayersColor();

    virtual void SetTopLayer( int aLayer, bool aEnabled = true );

    virtual void EnableTopLayer( bool aEnable );

    virtual int GetTopLayer() const;

    void ClearTopLayers();

    void UpdateAllLayersOrder();

    void ClearTargets();

    virtual void Redraw();

    void RecacheAllItems();

    bool IsDirty() const
    {
        for( int i = 0; i < TARGETS_NUMBER; ++i )
        {
            if( IsTargetDirty( i ) )
                return true;
        }

        return false;
    }

    bool IsTargetDirty( int aTarget ) const
    {
        Q_ASSERT( aTarget < TARGETS_NUMBER );
        if( aTarget >= TARGETS_NUMBER ) return false;
        return m_dirtyTargets[aTarget];
    }

    inline void MarkTargetDirty( int aTarget )
    {
        Q_ASSERT( aTarget < TARGETS_NUMBER );
        if( aTarget >= TARGETS_NUMBER ) return;
        m_dirtyTargets[aTarget] = true;
    }

    inline bool IsCached( int aLayer ) const
    {
        auto it = m_layers.find( aLayer );

        if( it == m_layers.end() )
            return false;

        return it->second.target == TARGET_CACHED;
    }

    void MarkDirty()
    {
        for( int i = 0; i < TARGETS_NUMBER; ++i )
            m_dirtyTargets[i] = true;
    }

    void MarkClean()
    {
        for( int i = 0; i < TARGETS_NUMBER; ++i )
            m_dirtyTargets[i] = false;
    }

    void UpdateItems();

    void UpdateAllItems( int aUpdateFlags );

    void UpdateAllItemsConditionally( int aUpdateFlags,
                                      std::function<bool( VIEW_ITEM* )> aCondition );

    void UpdateAllItemsConditionally( std::function<int( VIEW_ITEM* )> aItemFlagsProvider );

    bool IsUsingDrawPriority() const
    {
        return m_useDrawPriority;
    }

    void UseDrawPriority( bool aFlag )
    {
        m_useDrawPriority = aFlag;
    }

    void ReverseDrawOrder( bool aFlag )
    {
        m_reverseDrawOrder = aFlag;
    }

    std::shared_ptr<VIEW_OVERLAY> MakeOverlay();

    void InitPreview();

    void ClearPreview();
    void AddToPreview( VIEW_ITEM* aItem, bool aTakeOwnership = true );

    void ShowPreview( bool aShow = true );

    std::unique_ptr<VIEW> DataReference() const;

    static constexpr int VIEW_MAX_LAYERS = MAX_LAYERS_FOR_VIEW;

    static constexpr int TOP_LAYER_MODIFIER = -MAX_LAYERS_FOR_VIEW;

protected:
    struct VIEW_LAYER
    {
        bool                    visible;
        bool                    displayOnly;
        bool                    diffLayer;
        bool                    hasNegatives;
        std::shared_ptr<VIEW_RTREE> items;
        int                     renderingOrder;
        int                     id;
        RENDER_TARGET           target;
        std::set<int>           requiredLayers;

        bool operator< ( const VIEW_LAYER& aOther ) const
        {
            return id < aOther.id;
        }
    };

    void redrawRect( const BOX2I& aRect );

    inline void markTargetClean( int aTarget )
    {
        Q_ASSERT( aTarget < TARGETS_NUMBER );
        if( aTarget >= TARGETS_NUMBER ) return;
        m_dirtyTargets[aTarget] = false;
    }

    void draw( VIEW_ITEM* aItem, int aLayer, bool aImmediate = false );

    void draw( VIEW_ITEM* aItem, bool aImmediate = false );

    void draw( VIEW_GROUP* aGroup, bool aImmediate = false );

    void sortOrderedLayers();

    void clearGroupCache();

    void invalidateItem( VIEW_ITEM* aItem, int aUpdateFlags );

    void updateItemColor( VIEW_ITEM* aItem, int aLayer );

    void updateItemGeometry( VIEW_ITEM* aItem, int aLayer );

    void updateBbox( VIEW_ITEM* aItem );

    void updateLayers( VIEW_ITEM* aItem );

    static bool compareRenderingOrder( VIEW_LAYER* aI, VIEW_LAYER* aJ )
    {
        return aI->renderingOrder > aJ->renderingOrder;
    }

    bool areRequiredLayersEnabled( int aLayerId ) const;

    // Function objects that need to access VIEW/VIEW_ITEM private/protected members
    struct CLEAR_LAYER_CACHE_VISITOR;
    struct RECACHE_ITEM_VISITOR;
    struct DRAW_ITEM_VISITOR;
    struct UPDATE_COLOR_VISITOR;
    struct UPDATE_DEPTH_VISITOR;

    std::unique_ptr<KIGFX::VIEW_GROUP> m_preview;
    std::vector<VIEW_ITEM*>            m_ownedItems;

    bool                               m_enableOrderModifier;

    std::map<int, VIEW_LAYER>          m_layers;

    std::vector<VIEW_LAYER*>           m_orderedLayers;

    std::shared_ptr<std::vector<VIEW_ITEM*>> m_allItems;

    std::set<unsigned int>             m_topLayers;

    VECTOR2D                           m_center;

    double                             m_scale;
    BOX2D                              m_boundary;
    double                             m_minScale;
    double                             m_maxScale;

    bool                               m_mirrorX;
    bool                               m_mirrorY;

    PAINTER* m_painter;

    GAL* m_gal;

    bool m_dirtyTargets[TARGETS_NUMBER];

    bool m_useDrawPriority;

    int m_nextDrawPriority;

    bool m_reverseDrawOrder;
};
} // namespace KIGFX