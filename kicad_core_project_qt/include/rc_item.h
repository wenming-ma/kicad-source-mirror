// QT_TRANSFORMATION_COMPLETED

#ifndef RC_ITEM_H
#define RC_ITEM_H

#include <QAbstractItemModel>
#include <QTreeView>
#include <QModelIndex>
#include <QVariant>
#include <units_provider.h>
#include <kiid.h>
#include <reporter.h>
#include <math/vector2d.h>

class MARKER_BASE;
class EDA_BASE_FRAME;
class RC_ITEM;
class EDA_ITEM;
class EDA_DRAW_FRAME;

namespace RC_JSON
{
struct VIOLATION;
}

class RC_ITEMS_PROVIDER
{
public:
    virtual void SetSeverities( int aSeverities ) = 0;

    virtual int GetCount( int aSeverity = -1 ) const = 0;

    virtual std::shared_ptr<RC_ITEM> GetItem( int aIndex ) const = 0;

    virtual void DeleteItem( int aIndex, bool aDeep ) = 0;

    virtual ~RC_ITEMS_PROVIDER() { }
};


class RC_ITEM
{
public:
    typedef std::vector<KIID> KIIDS;

    RC_ITEM() :
        m_errorCode( 0 ),
        m_parent( nullptr )
    {
    }

    RC_ITEM( const std::shared_ptr<RC_ITEM>& aItem )
    {
        m_errorCode    = aItem->m_errorCode;
        m_errorMessage = aItem->m_errorMessage;
        m_errorTitle   = aItem->m_errorTitle;
        m_settingsKey  = aItem->m_settingsKey;
        m_parent       = aItem->m_parent;
        m_ids          = aItem->m_ids;
    }

    virtual ~RC_ITEM() { }

    void SetErrorMessage( const QString& aMessage ) { m_errorMessage = aMessage; }

    void SetItems( const KIIDS& aIds ) { m_ids = aIds; }

    void AddItem( EDA_ITEM* aItem );

    void SetItems( const EDA_ITEM* aItem, const EDA_ITEM* bItem = nullptr,
                   const EDA_ITEM* cItem = nullptr, const EDA_ITEM* dItem = nullptr );

    void SetItems( const KIID& aItem, const KIID& bItem = niluuid, const KIID& cItem = niluuid,
                   const KIID& dItem = niluuid )
    {
        m_ids.clear();

        m_ids.push_back( aItem );
        m_ids.push_back( bItem );
        m_ids.push_back( cItem );
        m_ids.push_back( dItem );
    }

    virtual KIID GetMainItemID() const { return m_ids.size() > 0 ? m_ids[0] : niluuid; }
    virtual KIID GetAuxItemID() const { return m_ids.size() > 1 ? m_ids[1] : niluuid; }
    virtual KIID GetAuxItem2ID() const { return m_ids.size() > 2 ? m_ids[2] : niluuid; }
    virtual KIID GetAuxItem3ID() const { return m_ids.size() > 3 ? m_ids[3] : niluuid; }

    std::vector<KIID> GetIDs() const { return m_ids; }

    void SetParent( MARKER_BASE* aMarker ) { m_parent = aMarker; }
    MARKER_BASE* GetParent() const { return m_parent; }


    virtual QString ShowReport( UNITS_PROVIDER* aUnitsProvider, SEVERITY aSeverity,
                                 const std::map<KIID, EDA_ITEM*>& aItemMap ) const;

    virtual void GetJsonViolation( RC_JSON::VIOLATION& aViolation, UNITS_PROVIDER* aUnitsProvider,
                                   SEVERITY aSeverity,
                                   const std::map<KIID, EDA_ITEM*>& aItemMap ) const;

    int GetErrorCode() const { return m_errorCode; }
    void SetErrorCode( int aCode ) { m_errorCode = aCode; }

    virtual QString GetErrorMessage() const;

    QString GetErrorText() const
    {
        return m_errorTitle;
    }

    QString GetSettingsKey() const
    {
        return m_settingsKey;
    }

    virtual QString GetViolatingRuleDesc() const
    {
        return QString();
    }

protected:
    static QString getSeverityString( SEVERITY aSeverity );

    int           m_errorCode;
    QString       m_errorMessage;
    QString       m_errorTitle;
    QString       m_settingsKey;
    MARKER_BASE*  m_parent;

    KIIDS         m_ids;

};


class RC_TREE_NODE
{
public:
    enum NODE_TYPE
    {
        MARKER,
        MAIN_ITEM,
        AUX_ITEM,
        AUX_ITEM2,
        AUX_ITEM3,
        COMMENT
    };

    RC_TREE_NODE( RC_TREE_NODE* aParent, const std::shared_ptr<RC_ITEM>& aRcItem,
                  NODE_TYPE aType ) :
            m_Type( aType ),
            m_RcItem( aRcItem ),
            m_Parent( aParent )
    {}

    ~RC_TREE_NODE()
    {
        for( RC_TREE_NODE* child : m_Children )
            delete child;
    }

    RC_TREE_NODE( const RC_TREE_NODE& ) = delete;
    RC_TREE_NODE& operator=( const RC_TREE_NODE& ) = delete;

    NODE_TYPE                  m_Type;
    std::shared_ptr<RC_ITEM>   m_RcItem;

    RC_TREE_NODE*              m_Parent;
    std::vector<RC_TREE_NODE*> m_Children;
};


class RC_TREE_MODEL : public QAbstractItemModel
{
    Q_OBJECT
public:
    static QModelIndex ToIndex( RC_TREE_NODE const* aNode, const RC_TREE_MODEL* aModel )
    {
        return aModel->createIndex( 0, 0, const_cast<void*>( static_cast<void const*>( aNode ) ) );
    }

    static RC_TREE_NODE* ToNode( const QModelIndex& aIndex )
    {
        return static_cast<RC_TREE_NODE*>( aIndex.internalPointer() );
    }

    const QTreeView* GetView() const { return m_view; }

    static KIID ToUUID( const QModelIndex& aIndex );

    RC_TREE_MODEL( EDA_DRAW_FRAME* aParentFrame, QTreeView* aView );

    ~RC_TREE_MODEL();

    RC_TREE_MODEL( const RC_TREE_MODEL& ) = delete;
    RC_TREE_MODEL& operator=( const RC_TREE_MODEL& ) = delete;

    void Update( std::shared_ptr<RC_ITEMS_PROVIDER> aProvider, int aSeverities );

    void ExpandAll();

    void PrevMarker();
    void NextMarker();
    void SelectMarker( const MARKER_BASE* aMarker );
    void CenterMarker( const MARKER_BASE* aMarker );

    bool hasChildren( const QModelIndex& aParent ) const override;

    QModelIndex parent( const QModelIndex& aChild ) const override;

    int rowCount( const QModelIndex& aParent = QModelIndex() ) const override;
    int columnCount( const QModelIndex& aParent = QModelIndex() ) const override;
    
    QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const override;



    QVariant data( const QModelIndex& aIndex, int aRole = Qt::DisplayRole ) const override;

    bool setData( const QModelIndex& aIndex, const QVariant& aValue, int aRole = Qt::EditRole ) override
    {
        return false;
    }


    void ValueChanged( RC_TREE_NODE* aNode );

    void DeleteCurrentItem( bool aDeep );

    void DeleteItems( bool aCurrentOnly, bool aIncludeExclusions, bool aDeep );

protected:
    void     rebuildModel( std::shared_ptr<RC_ITEMS_PROVIDER> aProvider, int aSeverities );
    bool     GetAttr( const QModelIndex& aIndex, unsigned int aCol, QFont& aFont, QColor& aTextColor ) const;

    EDA_DRAW_FRAME*                    m_editFrame;
    QTreeView*                         m_view;
    int                                m_severities;
    std::shared_ptr<RC_ITEMS_PROVIDER> m_rcItemsProvider;

    std::vector<RC_TREE_NODE*>         m_tree;
};

#endif      // RC_ITEM_H
