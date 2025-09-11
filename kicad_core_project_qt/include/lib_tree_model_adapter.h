// QT_TRANSFORMATION_COMPLETED

#ifndef LIB_TREE_MODEL_ADAPTER_H
#define LIB_TREE_MODEL_ADAPTER_H

#include <eda_base_frame.h>
#include <lib_id.h>
#include <lib_tree_model.h>
#include <settings/app_settings.h>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QTreeView>
#include <QHeaderView>
#include <QWidget>
#include <vector>
#include <string>
#include <functional>
#include <set>
#include <map>


#include <project.h>

class TOOL_INTERACTIVE;
class EDA_BASE_FRAME;


class LIB_TREE_MODEL_ADAPTER: public QAbstractItemModel
{
    Q_OBJECT
public:
    static const QString GetPinningSymbol()
    {
        return QString::fromUtf8( "☆ " );
    }

public:
    ~LIB_TREE_MODEL_ADAPTER();

    enum TREE_COLS
    {
        NAME_COL = 0,
        DESC_COL,
        NUM_COLS
    };

    enum SORT_MODE
    {
        BEST_MATCH = 0,
        ALPHABETIC
    };

    void SaveSettings();

    void SetFilter( std::function<bool( LIB_TREE_NODE& aNode )>* aFilter ) { m_filter = aFilter; }

    std::function<bool( LIB_TREE_NODE& aNode )>* GetFilter() const { return m_filter; }

    void SetSortMode( SORT_MODE aMode ) { m_sort_mode = aMode; }
    SORT_MODE GetSortMode() const { return m_sort_mode; }

    void ShowUnits( bool aShow );

    void SetPreselectNode( const LIB_ID& aLibId, int aUnit );

    LIB_TREE_NODE_LIBRARY& DoAddLibrary( const QString& aNodeName, const QString& aDesc,
                                         const std::vector<LIB_TREE_ITEM*>& aItemList,
                                         bool pinned, bool presorted );

    void RemoveGroup( bool aRecentlyUsedGroup, bool aAlreadyPlacedGroup );

    std::vector<std::string> GetAvailableColumns() const { return m_availableColumns; }

    std::vector<std::string> GetShownColumns() const { return m_shownColumns; }

    std::vector<std::string> GetOpenLibs() const;
    void        OpenLibs( const std::vector<std::string>& aLibs );

    void SetShownColumns( const std::vector<std::string>& aColumnNames );

    void AssignIntrinsicRanks() { m_tree.AssignIntrinsicRanks(); }

    void UpdateSearchString( const QString& aSearch, bool aState );

    void AttachTo( QTreeView* aTreeView );

    void FinishTreeInitialization();

    LIB_ID GetAliasFor( const QModelIndex& aSelection ) const;

    int GetUnitFor( const QModelIndex& aSelection ) const;

    LIB_TREE_NODE::TYPE GetTypeFor( const QModelIndex& aSelection ) const;

    LIB_TREE_NODE* GetTreeNodeFor( const QModelIndex& aSelection ) const;

    virtual QString GenerateInfo( const LIB_ID& aLibId, int aUnit ) { return QString(); }

    virtual bool HasPreview( const QModelIndex& aItem ) { return false; }
    virtual void ShowPreview( QWidget* aParent, const QModelIndex& aItem ) {}
    virtual void ShutdownPreview( QWidget* aParent ) {}

    TOOL_DISPATCHER* GetToolDispatcher() const { return m_parent->GetToolDispatcher(); }

    int GetItemCount() const;

    virtual int GetLibrariesCount() const
    {
        return m_tree.m_Children.size();
    }

    QModelIndex FindItem( const LIB_ID& aLibId );

    virtual QModelIndex GetCurrentDataViewItem();

    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const override;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
    QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const override;
    QModelIndex parent( const QModelIndex& index ) const override;

    void Freeze() { m_freeze++; }
    void Thaw() { m_freeze--; }
    bool IsFrozen() const { return m_freeze; }

    void RefreshTree();

    virtual TOOL_INTERACTIVE* GetContextMenuTool() { return nullptr; }

    void PinLibrary( LIB_TREE_NODE* aTreeNode );
    void UnpinLibrary( LIB_TREE_NODE* aTreeNode );

    void ShowChangedLanguage();

protected:
    static QModelIndex ToItem( const LIB_TREE_NODE* aNode );

    static LIB_TREE_NODE* ToNode( const QModelIndex& aItem );

    LIB_TREE_MODEL_ADAPTER( EDA_BASE_FRAME* aParent, const QString& aPinnedKey,
                            APP_SETTINGS_BASE::LIB_TREE& aSettingsStruct );

    LIB_TREE_NODE_LIBRARY& DoAddLibraryNode( const QString& aNodeName, const QString& aDesc,
                                             bool pinned );

    bool hasChildren( const QModelIndex& parent ) const override;

    virtual PROJECT::LIB_TYPE_T getLibType() = 0;

    void resortTree();

private:
    const LIB_TREE_NODE* ShowResults();

    void doAddColumn( const QString& aHeader, bool aTranslate = true );

protected:
    void addColumnIfNecessary( const QString& aHeader );

    void recreateColumns();

    LIB_TREE_NODE_ROOT           m_tree;
    std::map<unsigned, std::string> m_colIdxMap;
    std::vector<std::string>     m_availableColumns;

    QTreeView*                   m_widget;

private:
    EDA_BASE_FRAME*              m_parent;
    APP_SETTINGS_BASE::LIB_TREE& m_cfg;

    SORT_MODE                    m_sort_mode;
    bool                         m_show_units;
    LIB_ID                       m_preselect_lib_id;
    int                          m_preselect_unit;
    int                          m_freeze;

    std::function<bool( LIB_TREE_NODE& aNode )>* m_filter;

    std::vector<std::string>                     m_shownColumns;
};

#endif // LIB_TREE_MODEL_ADAPTER_H

