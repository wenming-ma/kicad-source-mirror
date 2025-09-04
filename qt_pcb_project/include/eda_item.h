
#ifndef EDA_ITEM_H
#define EDA_ITEM_H

#include <deque>

#include <api/serializable.h>
#include <core/typeinfo.h>
#include <eda_item_flags.h>
#include <eda_search_data.h>
#include <view/view_item.h>
#include <kiid.h>

enum class BITMAPS : unsigned int;


enum class INSPECT_RESULT
{
    QUIT,
    CONTINUE
};


class UNITS_PROVIDER;
class EDA_DRAW_FRAME;
class MSG_PANEL_ITEM;
class EMBEDDED_FILES;

namespace google { namespace protobuf { class Any; } }


typedef std::function< INSPECT_RESULT ( EDA_ITEM* aItem, void* aTestData ) > INSPECTOR_FUNC;

/// std::function passed to nested users by ref, avoids copying std::function.
typedef const INSPECTOR_FUNC& INSPECTOR;


class EDA_ITEM : public KIGFX::VIEW_ITEM, public SERIALIZABLE
{
public:
    virtual ~EDA_ITEM() { };

    inline KICAD_T Type() const { return m_structType; }

    EDA_ITEM* GetParent() const { return m_parent; }
    virtual void SetParent( EDA_ITEM* aParent )   { m_parent = aParent; }

    inline bool IsModified() const { return m_flags & IS_CHANGED; }
    inline bool IsNew() const { return m_flags & IS_NEW; }
    inline bool IsMoving() const { return m_flags & IS_MOVING; }

    inline bool IsSelected() const { return m_flags & SELECTED; }
    inline bool IsEntered() const { return m_flags & ENTERED; }
    inline bool IsBrightened() const { return m_flags & BRIGHTENED; }

    inline bool IsRollover() const { return m_isRollover; }
    inline void SetIsRollover( bool aIsRollover ) { m_isRollover = aIsRollover; }

    inline void SetSelected() { SetFlags( SELECTED ); }
    inline void SetBrightened() { SetFlags( BRIGHTENED ); }

    inline void ClearSelected() { ClearFlags( SELECTED ); }
    inline void ClearBrightened() { ClearFlags( BRIGHTENED ); }

    void SetModified();

    void           SetFlags( EDA_ITEM_FLAGS aMask ) { m_flags |= aMask; }
    void           XorFlags( EDA_ITEM_FLAGS aMask ) { m_flags ^= aMask; }
    void           ClearFlags( EDA_ITEM_FLAGS aMask = EDA_ITEM_ALL_FLAGS ) { m_flags &= ~aMask; }
    EDA_ITEM_FLAGS GetFlags() const { return m_flags; }
    bool           HasFlag( EDA_ITEM_FLAGS aFlag ) const { return ( m_flags & aFlag ) == aFlag; }

    EDA_ITEM_FLAGS GetEditFlags() const
    {
        constexpr int mask =
                ( IS_NEW | IS_PASTED | IS_MOVING | IS_BROKEN | IS_CHANGED | STRUCT_DELETED );

        return m_flags & mask;
    }

    virtual void ClearEditFlags()
    {
        ClearFlags( GetEditFlags() );
    }

    EDA_ITEM_FLAGS GetTempFlags() const
    {
        constexpr int mask = ( CANDIDATE | SELECTED_BY_DRAG | IS_LINKED | SKIP_STRUCT );

        return m_flags & mask;
    }

    virtual void ClearTempFlags()
    {
        ClearFlags( GetTempFlags() );
    }

    virtual bool RenderAsBitmap( double aWorldScale ) const { return false; }

    void SetIsShownAsBitmap( bool aBitmap )
    {
        if( aBitmap )
            SetFlags( IS_SHOWN_AS_BITMAP );
        else
            ClearFlags( IS_SHOWN_AS_BITMAP );
    }

    inline bool IsShownAsBitmap() const { return m_flags & IS_SHOWN_AS_BITMAP; }

    virtual bool IsType( const std::vector<KICAD_T>& aScanTypes ) const
    {
        for( KICAD_T scanType : aScanTypes )
        {
            if( scanType == SCH_LOCATE_ANY_T || scanType == m_structType )
                return true;
        }

        return false;
    }

    void SetForceVisible( bool aEnable ) { m_forceVisible = aEnable; }
    bool IsForceVisible() const { return m_forceVisible; }

    virtual void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList )
    {
    }

    virtual QString GetFriendlyName() const;

    virtual bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const
    {
        return false;
    }

    virtual bool HitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const
    {
        return false;
    }

    virtual const BOX2I GetBoundingBox() const;

    virtual VECTOR2I GetPosition() const { return VECTOR2I(); }
    virtual void     SetPosition( const VECTOR2I& aPos ){};

    virtual const VECTOR2I GetFocusPosition() const { return GetPosition(); }

    virtual VECTOR2I GetSortPosition() const { return GetPosition(); }

    virtual EDA_ITEM* Clone() const;

    virtual INSPECT_RESULT Visit( INSPECTOR inspector, void* testData,
                                  const std::vector<KICAD_T>& aScanTypes );

    template< class T >
    static INSPECT_RESULT IterateForward( std::deque<T>& aList, INSPECTOR inspector, void* testData,
                                          const std::vector<KICAD_T>& scanTypes )
    {
        for( const auto& it : aList )
        {
            EDA_ITEM* item = static_cast<EDA_ITEM*>( it );

            if( item && item->Visit( inspector, testData, scanTypes ) == INSPECT_RESULT::QUIT )
            {
                return INSPECT_RESULT::QUIT;
            }
        }

        return INSPECT_RESULT::CONTINUE;
    }

    template <class T>
    static INSPECT_RESULT IterateForward( std::vector<T>& aList, INSPECTOR inspector,
                                          void* testData, const std::vector<KICAD_T>& scanTypes )
    {
        for( const auto& it : aList )
        {
            EDA_ITEM* item = static_cast<EDA_ITEM*>( it );

            if( item && item->Visit( inspector, testData, scanTypes ) == INSPECT_RESULT::QUIT )
            {
                return INSPECT_RESULT::QUIT;
            }
        }

        return INSPECT_RESULT::CONTINUE;
    }

    QString GetTypeDesc() const;

    virtual QString GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const;

    virtual BITMAPS GetMenuImage() const;

    virtual bool Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const
    {
        return false;
    }

    static bool Replace( const EDA_SEARCH_DATA& aSearchData, QString& aText );

    virtual bool Replace( const EDA_SEARCH_DATA& aSearchData, void* aAuxData = nullptr )
    {
        return false;
    }

    virtual bool IsReplaceable() const { return false; }

    bool operator<( const EDA_ITEM& aItem ) const;

    static bool Sort( const EDA_ITEM* aLeft, const EDA_ITEM* aRight ) { return *aLeft < *aRight; }

    EDA_ITEM& operator=( const EDA_ITEM& aItem );

    virtual const BOX2I ViewBBox() const override;

    virtual std::vector<int> ViewGetLayers() const override;

    virtual EMBEDDED_FILES* GetEmbeddedFiles() { return nullptr; }

#if defined(DEBUG)

    virtual void Show( int nestLevel, std::ostream& os ) const { ShowDummy( os ); };

    void ShowDummy( std::ostream& os ) const;

    static std::ostream& NestedSpace( int nestLevel, std::ostream& os );

#endif

protected:
    EDA_ITEM( EDA_ITEM* parent, KICAD_T idType, bool isSCH_ITEM = false, bool isBOARD_ITEM = false );
    EDA_ITEM( KICAD_T idType, bool isSCH_ITEM = false, bool isBOARD_ITEM = false );
    EDA_ITEM( const EDA_ITEM& base );

    bool Matches( const QString& aText, const EDA_SEARCH_DATA& aSearchData ) const;

public:
    const KIID  m_Uuid;

private:
    KICAD_T        m_structType;

protected:
    EDA_ITEM_FLAGS m_flags;
    EDA_ITEM*      m_parent;
    bool           m_forceVisible;
    bool           m_isRollover;
};


inline EDA_ITEM* new_clone( const EDA_ITEM& aItem ) { return aItem.Clone(); }

struct CompareByUuid
{
    bool operator()(const EDA_ITEM* item1, const EDA_ITEM* item2) const
    {
        assert( item1 != nullptr && item2 != nullptr );

        if( item1->m_Uuid == item2->m_Uuid )
            return item1 < item2;

        return item1->m_Uuid < item2->m_Uuid;
    }
};


typedef std::vector< EDA_ITEM* > EDA_ITEMS;

typedef std::set< EDA_ITEM*, CompareByUuid > EDA_ITEM_SET;

#endif // EDA_ITEM_H
