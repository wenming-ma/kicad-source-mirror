
#ifndef __CONTEXT_MENU_H
#define __CONTEXT_MENU_H

#include <map>
#include <list>
#include <functional>

#include <QMenu>
#include <QAction>
#include <QString>
#include <QPoint>
#include <tool/tool_event.h>

class KIFACE_BASE;
class TOOL_INTERACTIVE;
class TOOL_MANAGER;

enum class BITMAPS : unsigned int;

/**
 * Define the structure of a menu based on ACTIONs.
 */
class ACTION_MENU : public QMenu
{
public:
    /// Default constructor
    ACTION_MENU( bool isContextMenu, TOOL_INTERACTIVE* aTool = nullptr );

    ~ACTION_MENU() override;

    ACTION_MENU( const ACTION_MENU& aMenu ) = delete;
    ACTION_MENU& operator=( const ACTION_MENU& aMenu ) = delete;

    /**
     * Set title for the menu. The title is shown as a text label shown on the top of
     * the menu.
     *
     * @param aTitle is the new title.
     */
    void SetTitle( const QString& aTitle );

    // Yes, it hides a non-virtual method in the parent class.
    QString GetTitle() const { return m_title; }

    /**
     * Decide whether a title for a pop up menu should be displayed.
     */
    void DisplayTitle( bool aDisplay = true );

    /**
     * Assign an icon for the entry.
     *
     * @param aIcon is the icon to be assigned. NULL is used to remove icon.
     */
    void SetIcon( BITMAPS aIcon );

    /**
     * Add a Qt-style entry to the menu.
     *
     * After highlighting/selecting the entry, a Qt event is generated.
     */
    QAction* Add( const QString& aLabel, int aId, BITMAPS aIcon );
    QAction* Add( const QString& aLabel, const QString& aToolTip, int aId,
                     BITMAPS aIcon,  bool aIsCheckmarkEntry = false );

    /**
     * Add an entry to the menu based on the #TOOL_ACTION object.
     *
     * After selecting the entry, a #TOOL_EVENT command containing name of the action is sent.
     *
     * @param aAction is the action to be added to menu entry.
     * @param aIsCheckmarkEntry is true to indicate a check menu entry, false for normal menu entry
     * @param aOverrideLabel is the label to show in the menu (overriding the action's menu text)
     *        when non-empty
     */
    QAction* Add( const TOOL_ACTION& aAction, bool aIsCheckmarkEntry = false,
                     const QString& aOverrideLabel = QString() );

    /**
     * Add an action menu as a submenu.
     *
     * The difference between this function and QMenu::addMenu() is the capability to
     * handle icons.
     *
     * @param aMenu is the submenu to be added. This should be a new instance (use Clone()) if
     *        required as the menu is destructed after use.
     */
    QAction* Add( ACTION_MENU* aMenu );

    /**
     * Add a standard close item to the menu with the accelerator key CTRL-W.
     *
     * Emits the close event.
     *
     * @param aAppname is the application name to append to the tooltip.
     */
    void AddClose( const QString& aAppname = "" );

    /**
     * Add either a standard Quit or Close item to the menu.
     *
     * If \a aKiface is NULL or in single-instance then quit action is used, otherwise
     * close action is used.
     *
     * @param aAppname is the application name to append to the tooltip.
     */
    void AddQuitOrClose( KIFACE_BASE* aKiface, QString aAppname = "" );

    /**
     * Add a standard Quit item to the menu.
     *
     * Emits the quit event.
     *
     * @param aAppname is the application name to append to the tooltip.
     */
    void AddQuit( const QString& aAppname = "" );

    /**
     * Remove all the entries from the menu (as well as its title).
     *
     * It leaves the menu in the initial state.
     */
    void Clear();

    /**
     * Return true if the menu has any enabled items.
     */
    bool HasEnabledItems() const;

    /**
     * Return the position of selected item.
     *
     * If the returned value is negative, that means that menu was dismissed.
     *
     * @return The position of selected item in the action menu.
     */
    inline int GetSelected() const
    {
        return m_selected;
    }

    /**
     * Run update handlers for the menu and its submenus.
     */
    void UpdateAll();

    /**
     * Used by some menus to just-in-time translate their titles.
     */
    virtual void UpdateTitle() {}

    /**
     * Clear the dirty flag on the menu and all descendants.
     */
    void ClearDirty();
    void SetDirty();

    /**
     * Set a tool that is the creator of the menu.
     *
     * @param aTool is the tool that created the menu.
     */
    void SetTool( TOOL_INTERACTIVE* aTool );

    /**
     * Create a deep, recursive copy of this ACTION_MENU.
     */
    ACTION_MENU* Clone() const;

    void OnMenuEvent( QAction* aAction );
    void OnIdle();
    void OnAboutToShow();
    void OnAboutToHide();

    virtual bool PassHelpTextToHandler() { return false; }

    static constexpr bool NORMAL = false;
    static constexpr bool CHECK  = true;

protected:
    /// Return an instance of this class. It has to be overridden in inheriting classes.
    virtual ACTION_MENU* create() const;

    /// Return an instance of TOOL_MANAGER class.
    TOOL_MANAGER* getToolManager() const;

    /**
     * Update menu state stub.
     *
     * It is called before a menu is shown, in order to update its state.  Here you can tick
     * current settings, enable/disable entries, etc.
     */
    virtual void update()
    {
    }

    /**
     * Event handler stub.
     *
     * It should be used if you want to generate a #TOOL_EVENT from a QAction.  It will be
     * called when a menu entry is clicked.
     */
    virtual OPT_TOOL_EVENT eventHandler( QAction* aAction )
    {
        return OPT_TOOL_EVENT();
    }

    /**
     * Copy another menus data to this instance.
     *
     * Old entries are preserved and ones form aMenu are copied.
     */
    void copyFrom( const ACTION_MENU& aMenu );

protected:
    /**
     * Append a copy of QAction.
     */
    QAction* appendCopy( const QAction* aSource );

    /// Initialize handlers for events.
    void setupEvents();

    /// Update hot key settings for TOOL_ACTIONs in this menu.
    void updateHotKeys();

    /// Traverse the submenus tree looking for a submenu capable of handling a particular menu
    /// event. In case it is handled, it is returned the aToolEvent parameter.
    void runEventHandlers( QAction* aAction, OPT_TOOL_EVENT& aToolEvent );

    /// Run a function on the menu and all its submenus.
    void runOnSubmenus( std::function<void(ACTION_MENU*)> aFunction );

    /// Check if any of submenus contains a TOOL_ACTION with a specific ID.
    OPT_TOOL_EVENT findToolAction( int aId );

    /// Helper function to find action by ID
    QAction* findActionById( int aId ) const;

    /// Helper function to find menu containing action
    ACTION_MENU* findMenuContaining( QAction* aAction ) const;

    bool    m_isForcedPosition;
    QPoint m_forcedPosition;

    bool m_dirty;               // Menu requires update before display

    bool m_titleDisplayed;
    bool m_isContextMenu;

    /// Menu title.
    QString m_title;

    /// Optional icon.
    BITMAPS m_icon;

    /// Store the id number of selected item.
    int m_selected;

    /// Creator of the menu.
    TOOL_INTERACTIVE* m_tool;

    /// Associates tool actions with menu item IDs. Non-owning.
    std::map<int, const TOOL_ACTION*> m_toolActions;

    /// List of submenus.
    std::list<ACTION_MENU*> m_submenus;

    friend class TOOL_INTERACTIVE;
};

#endif
