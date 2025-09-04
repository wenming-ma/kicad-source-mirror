// QT_TRANSFORMATION_COMPLETED

#ifndef LIB_TREE_H
#define LIB_TREE_H

#include <QWidget>
#include <QTimer>
#include <QLineEdit>
#include <QTreeView>
#include <QBoxLayout>
#include <QPoint>
#include <QRect>
#include <QModelIndex>
#include <lib_tree_model_adapter.h>
#include <widgets/html_window.h>
#include <widgets/wx_dataviewctrl.h>

class QLineEdit;
class QTimer;
class QWidget;
class STD_BITMAP_BUTTON;
class ACTION_MENU;
class LIB_ID;
class LIB_TABLE;

class LIB_TREE : public QWidget
{
    Q_OBJECT
public:
    ///< Flags to select extra widgets and options
    enum FLAGS
    {
        FLAGS_NONE  = 0x00,
        SEARCH      = 0x01,
        FILTERS     = 0x02,
        DETAILS     = 0x04,
        ALL_WIDGETS = 0x0F,
        MULTISELECT = 0x10
    };

    LIB_TREE( QWidget* aParent, const QString& aRecentSearchesKey, LIB_TABLE* aLibTable,
              std::shared_ptr<LIB_TREE_MODEL_ADAPTER>& aAdapter, int aFlags = ALL_WIDGETS,
              HTML_WINDOW* aDetails = nullptr );

    ~LIB_TREE() override;

    LIB_ID GetSelectedLibId( int* aUnit = nullptr ) const;

    int GetSelectionCount() const
    {
        return m_tree_ctrl->GetSelectedItemsCount();
    }

    int GetSelectedLibIds( std::vector<LIB_ID>& aSelection,
                           std::vector<int>* aUnit = nullptr ) const;

    LIB_TREE_NODE* GetCurrentTreeNode() const;

    int GetSelectedTreeNodes( std::vector<LIB_TREE_NODE*>& aSelection ) const;

    void SelectLibId( const LIB_ID& aLibId );

    void CenterLibId( const LIB_ID& aLibId );

    void Unselect();

    void ExpandLibId( const LIB_ID& aLibId );

    void ExpandAll();
    void CollapseAll();

    void SetSearchString( const QString& aSearchString );
    QString GetSearchString() const;

    void SetSortMode( LIB_TREE_MODEL_ADAPTER::SORT_MODE aMode ) { m_adapter->SetSortMode( aMode ); }
    LIB_TREE_MODEL_ADAPTER::SORT_MODE GetSortMode() const { return m_adapter->GetSortMode(); }

    void Regenerate( bool aKeepState );

    void RefreshLibTree();

    QWidget* GetFocusTarget();

    QBoxLayout* GetFiltersSizer() { return m_filtersSizer; }

    void FocusSearchFieldIfExists();

    void ShowChangedLanguage();

    void BlockPreview( bool aBlock )
    {
        m_previewDisabled = aBlock;
    }

    void ShutdownPreviews();

protected:
    void toggleExpand( const QModelIndex& aTreeId );

    void selectIfValid( const QModelIndex& aTreeId );

    void centerIfValid( const QModelIndex& aTreeId );

    void expandIfValid( const QModelIndex& aTreeId );

    void postPreselectEvent();

    void postSelectEvent();

    struct STATE
    {
        std::vector<QModelIndex> expanded;

        LIB_ID selection;
    };

    STATE getState() const;

    void setState( const STATE& aState );

    void updateRecentSearchMenu();

    void showPreview( QModelIndex aItem );
    void hidePreview();

private slots:
    void onQueryText();
    void onQueryCharHook();
    void onQueryMouseMoved();

    void onTreeSelect();
    void onTreeActivate();
    void onTreeCharHook();

    void onIdle();
    void onHoverTimer();

    void onDetailsLink();
    void onPreselect();
    void onItemContextMenu();
    void onHeaderContextMenu();

    void onDebounceTimer();

protected:
    std::shared_ptr<LIB_TREE_MODEL_ADAPTER> m_adapter;

    QLineEdit*         m_query_ctrl;
    STD_BITMAP_BUTTON* m_sort_ctrl;
    WX_DATAVIEWCTRL*   m_tree_ctrl;
    HTML_WINDOW*       m_details_ctrl;
    QTimer*            m_debounceTimer;
    bool               m_inTimerEvent;

    QString            m_recentSearchesKey;

    QBoxLayout*        m_filtersSizer;

    bool               m_skipNextRightClick;

    QPoint             m_hoverPos;
    QModelIndex        m_hoverItem;
    QRect              m_hoverItemRect;
    QTimer             m_hoverTimer;
    QModelIndex        m_previewItem;
    QRect              m_previewItemRect;
    QWidget*           m_previewWindow;
    bool               m_previewDisabled;
signals:
    void libItemSelected();
    void libItemChosen();
};

#endif /* LIB_TREE_H */
