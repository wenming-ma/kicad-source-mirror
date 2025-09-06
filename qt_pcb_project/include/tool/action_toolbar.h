
#ifndef ACTION_TOOLBAR_H
#define ACTION_TOOLBAR_H

#include <map>
#include <memory>
#include <vector>
#include <QBitmap>
#include <QPixmap>
#include <QToolBar>
#include <QDockWidget>
#include <QWidget>
#include <QBoxLayout>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QContextMenuEvent>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QPalette>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QAction>
#include <tool/action_manager.h>

class ACTION_MENU;
class BITMAP_BUTTON;
class EDA_BASE_FRAME;
class TOOL_ACTION;
class TOOL_MANAGER;

/**
 * A group of actions that will be displayed together on a toolbar palette.
 */
class ACTION_GROUP
{
public:
    // Make the toolbar a friend so it can easily access everything inside here
    friend class ACTION_TOOLBAR;

    ACTION_GROUP( const std::string& aName, const std::vector<const TOOL_ACTION*>& aActions );

    /**
     * Set the default action to use when first creating the toolbar palette icon.
     *
     * If no default action is provided, the default will be the first action in the
     * vector.
     *
     * @param aDefault is the default action.
     */
    void SetDefaultAction( const TOOL_ACTION& aDefault );

    /**
     * Get the default action to use when first creating this group's toolbar palette icon.
     */
    const TOOL_ACTION* GetDefaultAction() const { return m_defaultAction; }

    /**
     * Get the name of the group.
     */
    std::string GetName() const { return m_name; }

    /**
     * Get the ID used in the UI to reference this group
     */
    int GetUIId() const;

    const std::vector<const TOOL_ACTION*>& GetActions() const { return m_actions; }

protected:
    ///< The action ID for this action group
    int m_id;

    ///< The name of this action group
    std::string m_name;

    ///< The default action to display on the toolbar item
    const TOOL_ACTION* m_defaultAction;

    std::vector<const TOOL_ACTION*> m_actions;
};


class ACTION_TOOLBAR_PALETTE : public QWidget
{
    Q_OBJECT
public:
    ACTION_TOOLBAR_PALETTE( QWidget* aParent, bool aVertical );

    /**
     * Add an action to the palette.
     *
     * @param aAction is the action to add
     */
    void AddAction( const TOOL_ACTION& aAction );

    /**
     * Enable the button for an action on the palette.
     *
     * @param aAction is the action who's button should be enabled
     * @param aEnable is true to enable the button, false to disable
     */
    void EnableAction( const TOOL_ACTION& aAction, bool aEnable = true );

    /**
     * Check/Toggle the button for an action on the palette.
     *
     * @param aAction is the action who's button should be checked
     * @param aCheck is true to check the button, false to uncheck
     */
    void CheckAction( const TOOL_ACTION& aAction, bool aCheck = true );

    void SetButtonSize( QRect& aSize ) { m_buttonSize = aSize; }

    void Popup( QWidget* aFocus = nullptr );

    /**
     * Set the action group that this palette contains the actions for
     */
    void SetGroup( ACTION_GROUP* aGroup ) { m_group = aGroup; }
    ACTION_GROUP* GetGroup() { return m_group; }

signals:
    void actionTriggered( QAction* action );

protected:
    void keyPressEvent( QKeyEvent* aEvent ) override;

    // The group that the buttons in the palette are part of
     ACTION_GROUP* m_group;

    QRect         m_buttonSize;

    bool           m_isVertical;

    QWidget*       m_panel;
    QBoxLayout*    m_mainSizer;
    QBoxLayout*    m_buttonSizer;

    std::map<int, BITMAP_BUTTON*> m_buttons;
};


class ACTION_TOOLBAR : public QToolBar
{
    Q_OBJECT
public:
    ACTION_TOOLBAR( EDA_BASE_FRAME* parent, int id = -1,
                    const QPoint& pos = QPoint(), const QSize& size = QSize(),
                    Qt::ToolBarAreas style = Qt::AllToolBarAreas );

    virtual ~ACTION_TOOLBAR();

    void SetAuiManager( QDockWidget* aManager ) { m_auiManager = aManager; }

    void SetToolManager( TOOL_MANAGER* aManager ) { m_toolManager = aManager; }

    /**
     * Add a TOOL_ACTION-based button to the toolbar.
     *
     * After selecting the entry, a #TOOL_EVENT command containing name of the action is sent.
     *
     * @param aAction is the action to add.
     * @param aIsToggleEntry makes the toolbar item a toggle entry when true.
     * @param aIsCancellable when true, cancels the tool if clicked when tool is active.
     */
    void Add( const TOOL_ACTION& aAction, bool aIsToggleEntry = false,
              bool aIsCancellable = false );

    /**
     * Add a large button such as used in the KiCad Manager Frame's launch bar.
     *
     * @param aAction
     */
    void AddButton( const TOOL_ACTION& aAction );

    void AddScaledSeparator( QWidget* aWindow );

    /**
     * Add a context menu to a specific tool item on the toolbar.
     *
     * This toolbar gets ownership of the menu object, and will delete it when the
     * ClearToolbar() function is called.
     *
     * @param aAction is the action to get the menu
     * @param aMenu is the context menu
     */
    void AddToolContextMenu( const TOOL_ACTION& aAction, std::unique_ptr<ACTION_MENU> aMenu );

    /**
     * Add a set of actions to a toolbar as a group. One action from the group will be displayed
     * at a time.
     *
     * @param aGroup is the group to add. The first action in the group will be the first shown
     *               on the toolbar.
     * @param aIsToggleEntry makes the toolbar item a toggle entry when true
     */
    void AddGroup( ACTION_GROUP* aGroup, bool aIsToggleEntry = false );

    /**
     * Select an action inside a group
     *
     * @param aGroup is the group that contains the action
     * @param aAction is the action inside the group
     */
    void SelectAction( ACTION_GROUP* aGroup, const TOOL_ACTION& aAction );

    const TOOL_ACTION* GetSelectedAction( const std::string& aGroupName );

    /**
     * Update the toolbar item width of a control using its best size.
     *
     * @param aID is the ID of the toolbar item to update the width for
     */
    void UpdateControlWidth( int aID );

    /**
     * Clear the toolbar and remove all associated menus.
     */
    void ClearToolbar();

    void SetToolBitmap( const TOOL_ACTION& aAction, const QPixmap& aBitmap );

    /**
     * Apply the default toggle action.
     *
     * For checked items this is check/uncheck; for non-checked items it's enable/disable.
     */
    void Toggle( const TOOL_ACTION& aAction, bool aState );

    void Toggle( const TOOL_ACTION& aAction, bool aEnabled, bool aChecked );

    bool KiRealize();

    void RefreshBitmaps();

    static constexpr bool TOGGLE = true;
    static constexpr bool CANCEL = true;

protected:
    // Qt event overrides
    void contextMenuEvent( QContextMenuEvent* aEvent ) override;
    void mousePressEvent( QMouseEvent* aEvent ) override;
    void mouseReleaseEvent( QMouseEvent* aEvent ) override;
    void mouseMoveEvent( QMouseEvent* aEvent ) override;
    void paintEvent( QPaintEvent* aEvent ) override;

    // Internal methods
    void doSelectAction( ACTION_GROUP* aGroup, const TOOL_ACTION& aAction );
    void popupPalette( QAction* aItem );
    QAction* findActionById( int id );

protected slots:
    void onToolEvent( QAction* aAction );
    void onPaletteEvent( QAction* aAction );
    void onTimerDone();
    void onThemeChanged();

protected:
    QTimer* m_paletteTimer;

    QDockWidget*           m_auiManager;
    TOOL_MANAGER*           m_toolManager;
    ACTION_TOOLBAR_PALETTE* m_palette;

    std::map<int, bool>                m_toolKinds;
    std::map<int, bool>                m_toolCancellable;
    std::map<int, const TOOL_ACTION*>  m_toolActions;
    std::map<int, ACTION_GROUP*>       m_actionGroups;

    std::map<int, std::unique_ptr<ACTION_MENU>> m_toolMenus;
};

#endif
