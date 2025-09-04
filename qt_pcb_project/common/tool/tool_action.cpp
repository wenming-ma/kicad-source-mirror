#include <optional>
#include <tool/tool_action.h>
#include <tool/tool_event.h>
#include <tool/action_manager.h>

#include <algorithm>
#include <bitmaps.h>
#include <hotkeys_basic.h>

#include <QString>
#include <QObject>

TOOL_ACTION::TOOL_ACTION( const std::string& aName, TOOL_ACTION_SCOPE aScope,
                          int aDefaultHotKey, const std::string& aLegacyHotKeyName,
                          const QString& aLabel, const QString& aTooltip,
                          BITMAPS aIcon, TOOL_ACTION_FLAGS aFlags ) :
        m_name( aName ),
        m_scope( aScope ),
        m_group( std::nullopt ),
        m_defaultHotKey( aDefaultHotKey ),
        m_defaultHotKeyAlt( 0 ),
        m_legacyName( aLegacyHotKeyName ),
        m_menuLabel( aLabel ),
        m_tooltip( aTooltip ),
        m_icon( aIcon ),
        m_id( -1 ),
        m_flags( aFlags )
{
    SetHotKey( aDefaultHotKey );
    ACTION_MANAGER::GetActionList().push_back( this );
}


TOOL_ACTION::TOOL_ACTION() :
        m_scope( AS_GLOBAL ),
        m_group( std::nullopt ),
        m_defaultHotKey( 0 ),
        m_defaultHotKeyAlt( 0 ),
        m_icon( BITMAPS::INVALID_BITMAP ),
        m_id( -1 ),
        m_flags( AF_NONE )
{
    SetHotKey( 0 );
}


TOOL_ACTION::TOOL_ACTION( const TOOL_ACTION_ARGS& aArgs ) :
        m_name( aArgs.m_name.value_or( "" ) ),
        m_scope( aArgs.m_scope.value_or( AS_CONTEXT ) ),
        m_defaultHotKey( aArgs.m_defaultHotKey.value_or( 0 ) ),
        m_defaultHotKeyAlt( aArgs.m_defaultHotKeyAlt.value_or( 0 ) ),
        m_hotKey( aArgs.m_defaultHotKey.value_or( 0 ) ),
        m_hotKeyAlt( 0 ),
        m_legacyName( aArgs.m_legacyName.value_or( "" ) ),
        m_friendlyName( QString::fromStdString( aArgs.m_friendlyName.value_or( "" ) ) ),
        m_tooltip( QString::fromStdString( aArgs.m_tooltip.value_or( "" ) ) ),
        m_icon( aArgs.m_icon.value_or( BITMAPS::INVALID_BITMAP) ),
        m_id( -1 ),
        m_uiid( std::nullopt ),
        m_flags( aArgs.m_flags.value_or( AF_NONE ) )
{
    // Action name is the only mandatory part
    assert( !m_name.empty() );

    if( aArgs.m_menuText.has_value() )
        m_menuLabel = QString::fromStdString( aArgs.m_menuText.value() );

    if( aArgs.m_uiid.has_value() )
        m_uiid = aArgs.m_uiid.value();

    if( aArgs.m_param.has_value() )
        m_param = aArgs.m_param;

    if( aArgs.m_description.has_value() )
        m_description = QString::fromStdString( aArgs.m_description.value() );

    if( aArgs.m_group.has_value() )
        m_group = aArgs.m_group;

    ACTION_MANAGER::GetActionList().push_back( this );
}


TOOL_ACTION::~TOOL_ACTION()
{
    ACTION_MANAGER::GetActionList().remove( this );
}


TOOL_EVENT TOOL_ACTION::MakeEvent() const
{
    TOOL_EVENT evt;

    if( IsActivation() )
        evt = TOOL_EVENT( TC_COMMAND, TA_ACTIVATE, m_name, m_scope );
    else if( IsNotification() )
        evt = TOOL_EVENT( TC_MESSAGE, TA_NONE, m_name, m_scope );
    else
        evt = TOOL_EVENT( TC_COMMAND, TA_ACTION, m_name, m_scope );

    if( m_group.has_value() )
    {
        evt.SetActionGroup( m_group.value() );
    }

    if( m_param.has_value() )
        evt.SetParameter( m_param );

    return evt;
}


QString TOOL_ACTION::GetFriendlyName() const
{
    if( m_friendlyName.isEmpty() )
        return QString();

    return QObject::tr( m_friendlyName.toLocal8Bit() );
}


QString TOOL_ACTION::GetMenuLabel() const
{
    if( m_menuLabel.has_value() )
        return QObject::tr( m_menuLabel.value().toLocal8Bit() );

    return GetFriendlyName();
}


QString TOOL_ACTION::GetMenuItem() const
{
    QString label = GetMenuLabel();
    label.replace( QString( "&" ), QString( "&&" ) );
    return AddHotkeyName( label, m_hotKey, IS_HOTKEY );
}


QString TOOL_ACTION::GetDescription() const
{
    // If no description provided, use the tooltip without a hotkey
    if( !m_description.has_value() )
        return GetTooltip( false );

    return QObject::tr( m_description.value().toLocal8Bit() );
}


QString TOOL_ACTION::GetTooltip( bool aIncludeHotkey ) const
{
    QString tooltip = QObject::tr( m_tooltip.toLocal8Bit() );

    if( aIncludeHotkey && GetHotKey() )
        tooltip += QString( "  (%1)" ).arg( KeyNameFromKeyCode( GetHotKey() ) );

    return tooltip;
}


QString TOOL_ACTION::GetButtonTooltip() const
{
    // We don't show button text so use the action name as the first line of the tooltip
    QString tooltip = GetFriendlyName();

    if( GetHotKey() )
        tooltip += QString( "  (%1)" ).arg( KeyNameFromKeyCode( GetHotKey() ) );

    if( !GetTooltip( false ).isEmpty() )
        tooltip += '\n' + GetTooltip( false );

    return tooltip;
}


void TOOL_ACTION::SetHotKey( int aKeycode, int aKeycodeAlt )
{
    m_hotKey = aKeycode;
    m_hotKeyAlt = aKeycodeAlt;
}


std::string TOOL_ACTION::GetToolName() const
{
    int dotCount = std::count( m_name.begin(), m_name.end(), '.' );

    switch( dotCount )
    {
    case 0:
        assert( false );    // Invalid action name format
        return "";

    case 1:
        return m_name;

    case 2:
        return m_name.substr( 0, m_name.rfind( '.' ) );

    default:
        assert( false );    // TODO not implemented
        return "";
    }
}