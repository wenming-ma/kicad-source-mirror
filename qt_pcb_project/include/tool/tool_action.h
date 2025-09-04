
#ifndef __TOOL_ACTION_H
#define __TOOL_ACTION_H

#include <cassert>
#include <optional>
#include <string>
#include <string_view>

#include <ki_any.h>
#include <QString>
#include <QDebug>

class TOOL_EVENT;

enum class BITMAPS : unsigned int;

/// Scope of tool actions
enum TOOL_ACTION_SCOPE
{
    AS_CONTEXT = 1,  ///< Action belongs to a particular tool (i.e. a part of a pop-up menu)
    AS_ACTIVE,       ///< All active tools
    AS_GLOBAL        ///< Global action (toolbar/main menu event, global shortcut)
};

/// Flags for tool actions
enum TOOL_ACTION_FLAGS
{
    AF_NONE     = 0,
    AF_ACTIVATE = 1,    ///< Action activates a tool
    AF_NOTIFY   = 2     ///< Action is a notification (it is by default passed to all tools)
};

class TOOL_ACTION_GROUP
{
public:
    TOOL_ACTION_GROUP( std::string aName ) :
        m_name( aName )
    {
        // Assign a unique group ID to each group
        static int groupIDs = 0;
        m_groupID = ++groupIDs;
    };

    TOOL_ACTION_GROUP( const TOOL_ACTION_GROUP& aOther )
    {
        // Ensure a copy of a group is exactly the same as this one to get
        // proper comparisons
        m_name    = aOther.GetName();
        m_groupID = aOther.GetGroupID();
    }

    int                GetGroupID() const { return m_groupID; }
    const std::string& GetName()    const { return m_name; }

    bool operator==( const TOOL_ACTION_GROUP& aOther ) const
    {
        return m_groupID == aOther.m_groupID;
    }

private:
    int         m_groupID;
    std::string m_name;
};

class TOOL_ACTION_ARGS
{
public:
    TOOL_ACTION_ARGS() = default;

    TOOL_ACTION_ARGS& Name( const std::string_view& aName )
    {
        m_name = aName;
        return *this;
    }

    TOOL_ACTION_ARGS& FriendlyName( const std::string_view& aName )
    {
        m_friendlyName = aName;
        return *this;
    }

    TOOL_ACTION_ARGS& Scope( TOOL_ACTION_SCOPE aScope )
    {
        m_scope = aScope;
        return *this;
    }

    TOOL_ACTION_ARGS& DefaultHotkey( int aDefaultHotkey )
    {
        m_defaultHotKey = aDefaultHotkey;
        return *this;
    }

    TOOL_ACTION_ARGS& DefaultHotkeyAlt( int aDefaultHotkeyAlt )
    {
        m_defaultHotKeyAlt = aDefaultHotkeyAlt;
        return *this;
    }

    TOOL_ACTION_ARGS& LegacyHotkeyName( const std::string_view& aLegacyName )
    {
        m_legacyName = aLegacyName;
        return *this;
    }

    TOOL_ACTION_ARGS& MenuText( const std::string_view& aMenuText )
    {
        m_menuText = aMenuText;
        return *this;
    }

    TOOL_ACTION_ARGS& Tooltip( const std::string_view& aTooltip )
    {
        m_tooltip = aTooltip;
        return *this;
    }

    TOOL_ACTION_ARGS& Description( const std::string_view& aDescription )
    {
        m_description = aDescription;
        return *this;
    }

    TOOL_ACTION_ARGS& Icon( BITMAPS aIcon )
    {
        m_icon = aIcon;
        return *this;
    }

    TOOL_ACTION_ARGS& Flags( TOOL_ACTION_FLAGS aFlags )
    {
        m_flags = aFlags;
        return *this;
    }

    template<typename T>
    TOOL_ACTION_ARGS& Parameter( T aParam )
    {
        m_param = aParam;
        return *this;
    }

    TOOL_ACTION_ARGS& UIId( int aUIId )
    {
        m_uiid = aUIId;
        return *this;
    }

    TOOL_ACTION_ARGS& Group( const TOOL_ACTION_GROUP& aGroup )
    {
        m_group = aGroup;
        return *this;
    }

protected:
    // Let the TOOL_ACTION constructor have direct access to the members here
    friend class TOOL_ACTION;

    std::optional<std::string_view>     m_name;
    std::optional<std::string_view>     m_friendlyName;
    std::optional<TOOL_ACTION_SCOPE>    m_scope;
    std::optional<TOOL_ACTION_FLAGS>    m_flags;

    std::optional<int>                  m_uiid;

    std::optional<int>                  m_defaultHotKey;
    std::optional<int>                  m_defaultHotKeyAlt;
    std::optional<std::string_view>     m_legacyName;

    std::optional<std::string_view>     m_menuText;
    std::optional<std::string_view>     m_tooltip;
    std::optional<std::string_view>     m_description;

    std::optional<BITMAPS>              m_icon;

    std::optional<TOOL_ACTION_GROUP>    m_group;

    ki::any                             m_param;
};

class TOOL_ACTION
{
public:
    TOOL_ACTION( const TOOL_ACTION_ARGS& aArgs );
    TOOL_ACTION( const std::string& aName, TOOL_ACTION_SCOPE aScope = AS_CONTEXT,
                 int aDefaultHotKey = 0, const std::string& aLegacyHotKeyName = "",
                 const QString& aMenuText = QString(),
                 const QString& aTooltip = QString(),
                 BITMAPS aIcon = static_cast<BITMAPS>( 0 ), TOOL_ACTION_FLAGS aFlags = AF_NONE );

    ~TOOL_ACTION();

    // TOOL_ACTIONS are singletons; don't be copying them around....
    TOOL_ACTION( const TOOL_ACTION& ) = delete;
    TOOL_ACTION& operator= ( const TOOL_ACTION& ) = delete;

    bool operator==( const TOOL_ACTION& aRhs ) const
    {
        return m_id == aRhs.m_id;
    }

    bool operator!=( const TOOL_ACTION& aRhs ) const
    {
        return m_id != aRhs.m_id;
    }

    const std::string& GetName() const { return m_name; }

    int GetDefaultHotKey() const { return m_defaultHotKey; }
    int GetDefaultHotKeyAlt() const { return m_defaultHotKeyAlt; }

    int GetHotKey() const { return m_hotKey; }
    int GetHotKeyAlt() const { return m_hotKeyAlt; }
    void SetHotKey( int aKeycode, int aKeycodeAlt = 0 );

    int GetId() const { return m_id; }

    bool HasCustomUIId() const { return m_uiid.has_value(); }

    int GetUIId() const { return m_uiid.value_or( m_id + ACTION_BASE_UI_ID ); }

    static int GetBaseUIId() { return ACTION_BASE_UI_ID; }

    TOOL_EVENT MakeEvent() const;

    QString GetMenuLabel() const;
    QString GetMenuItem() const;
    QString GetTooltip( bool aIncludeHotkey = true ) const;
    QString GetButtonTooltip() const;
    QString GetDescription() const;

    QString GetFriendlyName() const;

    TOOL_ACTION_SCOPE GetScope() const { return m_scope; }

    template<typename T>
    T GetParam() const
    {
        Q_ASSERT( m_param.has_value() );

        T param;

        try
        {
            param = ki::any_cast<T>( m_param );
        }
        catch( const ki::bad_any_cast& e )
        {
            Q_ASSERT( false );
        }

        return param;
    }

    const std::optional<TOOL_ACTION_GROUP> GetActionGroup() const { return m_group; }

    std::string GetToolName() const;

    bool IsActivation() const
    {
        return m_flags & AF_ACTIVATE;
    }

    bool IsNotification() const
    {
        return m_flags & AF_NOTIFY;
    }

    BITMAPS GetIcon() const
    {
        return m_icon;
    }

protected:
    TOOL_ACTION();

    friend class ACTION_MANAGER;

    /// Base ID to use inside the user interface system to offset the action IDs.
    static constexpr int ACTION_BASE_UI_ID = 20000;

    /// Name of the action (convention is "app.tool.actionName")
    std::string          m_name;
    TOOL_ACTION_SCOPE    m_scope;

    std::optional<TOOL_ACTION_GROUP> m_group; ///< Optional group for the action to belong to.

    const int         m_defaultHotKey;    ///< Default hot key.
    const int         m_defaultHotKeyAlt; ///< Default hot key alternate.
    int               m_hotKey;           ///< The current hotkey (post-user-settings-application).

    /// The alternate hotkey (post-user-settings-application).
    int               m_hotKeyAlt;
    const std::string m_legacyName;       ///< Name for reading legacy hotkey settings.

    QString                m_friendlyName; ///< User-friendly name.
    std::optional<QString> m_menuLabel;    ///< Menu label.
    QString                m_tooltip;      ///< User facing tooltip help text.
    std::optional<QString> m_description;  ///< Description of the action.

    BITMAPS m_icon; ///< Icon for the menu entry

    int                m_id;   ///< Unique ID for maps. Assigned by #ACTION_MANAGER.
    std::optional<int> m_uiid; ///< ID to use when interacting with the UI (if empty, generate one).

    TOOL_ACTION_FLAGS m_flags;
    ki::any           m_param; ///< Generic parameter.
};

#endif
