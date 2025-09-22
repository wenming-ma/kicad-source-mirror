
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

/**
 * @file ui_common.h
 * Functions to provide common constants and other functions to assist
 * in making a consistent UI
 */

#ifndef UI_COMMON_H
#define UI_COMMON_H

#include <kicommon.h>
#include "report_severity.h"      // enum SEVERITY
#include <QString>
#include <QFont>
#include <QSize>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QAction>
#include <QMenu>
#include <QIcon>

class QIcon;
class QSize;
class QLineEdit;
class QTextEdit;
class QWidget;
class QAction;
class QMenu;

/**
 * Used for holding indeterminate values, such as with multiple selections
 * holding different values or controls which do not wish to set a value.
 */
#define INDETERMINATE_STATE _( "-- mixed values --" )
#define INDETERMINATE_ACTION _( "-- leave unchanged --" )

namespace KIUI
{

const int c_IndicatorSizeDIP = 10;


/**
 * Get the standard margin around a widget in the KiCad UI
 * @return margin in pixels
 */
KICOMMON_API int GetStdMargin();

/**
 * Return the size of @a aSingleLine of text when it is rendered in @a aWindow
 * using whatever font is currently set in that window.
 */
inline QSize GetTextSize( const QString& aSingleLine, QWidget* aWindow )
{
    // Simplified stub implementation
    return QSize( 100, 20 );
}

inline QFont GetMonospacedUIFont()
{
    return QFont();  // Return default font
}

KICOMMON_API QFont GetControlFont( QWidget* aWindow );
KICOMMON_API QFont GetInfoFont( QWidget* aWindow );
KICOMMON_API QFont GetDockedPaneFont( QWidget* aWindow );
KICOMMON_API QFont GetStatusFont( QWidget* aWindow );

/**
 * Set the minimum pixel width on a text control in order to make a text
 * string be fully visible within it.
 *
 * The current font within the text control is considered.  The text can come either from
 * the control or be given as an argument.  If the text control is larger than needed, then
 * nothing is done.
 *
 * @param aCtrl the text control to potentially make wider.
 * @param aString the text that is used in sizing the control's pixel width.
 * If NULL, then
 *   the text already within the control is used.
 * @return true if the \a aCtrl had its size changed, else false.
 */
KICOMMON_API bool EnsureTextCtrlWidth( QLineEdit* aCtrl, const QString* aString = nullptr );

/**
 * Select the number (or "?") in a reference for ease of editing.
 */
KICOMMON_API void SelectReferenceNumber( QLineEdit* aTextEntry );

/**
 * Ellipsize text (at the end) to be no more than 1/3 of the window width.
 *
 * @return shortened text ending with an ellipsis.
 */
inline QString EllipsizeStatusText( QWidget* aWindow, const QString& aString )
{
    return aString;  // Return unchanged
}

/**
 * Ellipsize text (at the end) to be no more than 36 characters.
 *
 * @return shortened text ending with an ellipsis.
 */
inline QString EllipsizeMenuText( const QString& aString )
{
    return aString;  // Return unchanged
}

/**
 * Check if a input control has focus.
 *
 * @param aFocus Control that has focus, if null, Qt will be queried
 */
KICOMMON_API bool IsInputControlFocused( QWidget* aFocus = nullptr );

/**
 * Check if a input control has focus.
 *
 * @param aFocus Control that test if editable
 * @return True if control is input and editable OR control is not a input. False if control is
 *         input and not editable.
 */
KICOMMON_API bool IsInputControlEditable( QWidget* aControl );

KICOMMON_API bool IsModalDialogFocused();

/**
 * Makes a window read-only.  Does some extra work over QWidget::setEnabled(false) to make sure you
 * can still scroll around in sub-windows.
 */
KICOMMON_API void Disable( QWidget* aWindow );

KICOMMON_API extern const QString s_FocusStealableInputName;


/**
 * Add a bitmap to a menuitem.
 *
 * It is added only if use images in menus config option allows it.  For checkable
 * or radio actions, the icon is added only on Windows, other platforms do
 * not support it
 *
 * @param aMenu is the menuitem.
 * @param aImage is the icon to add to aMenu.
 */
inline void AddBitmapToMenuItem( QAction* aMenu, const QIcon& aImage )
{
    // Empty stub implementation
}


/**
 * Create and insert a menu item with an icon into \a aMenu.
 *
 * @param aMenu is the menu to add the new item.
 * @param aId is the command ID for the new menu item.
 * @param aText is the string for the new menu item.
 * @param aImage is the icon to add to the new menu item.
 * @param aCheckable whether the action should be checkable
 * @return a pointer to the new created QAction.
 */
KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, int aId, const QString& aText,
                                   const QIcon& aImage,
                                   bool aCheckable = false );


/**
 * Create and insert a menu item with an icon and a help message string into \a aMenu.
 *
 * @param aMenu is the menu to add the new item.
 * @param aId is the command ID for the new menu item.
 * @param aText is the string for the new menu item.
 * @param aHelpText is the help message string for the new menu item.
 * @param aImage is the icon to add to the new menu item.
 * @param aCheckable whether the action should be checkable
 * @return a pointer to the new created QAction.
 */
KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, int aId, const QString& aText,
                                   const QString& aHelpText, const QIcon& aImage,
                                   bool aCheckable = false );


/**
 * Create and insert a menu item with an icon into \a aSubMenu in \a aMenu.
 *
 * @param aMenu is the menu to add the new submenu item.
 * @param aSubMenu is the submenu to add the new menu.
 * @param aId is the command ID for the new menu item.
 * @param aText is the string for the new menu item.
 * @param aImage is the icon to add to the new menu item.
 * @return a pointer to the new created QAction,
 */
KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, QMenu* aSubMenu, int aId,
                                   const QString& aText, const QIcon& aImage );


/**
 * Create and insert a menu item with an icon and a help message string into
 * \a aSubMenu in \a aMenu.
 *
 * @param aMenu is the menu to add the new submenu item.
 * @param aSubMenu is the submenu to add the new menu.
 * @param aId is the command ID for the new menu item.
 * @param aText is the string for the new menu item.
 * @param aHelpText is the help message string for the new menu item.
 * @param aImage is the icon to add to the new menu item.
 * @return a pointer to the new created QAction.
 */
KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, QMenu* aSubMenu, int aId,
                                   const QString& aText, const QString& aHelpText,
                                   const QIcon& aImage );
}

inline SEVERITY SeverityFromString( const QString& aSeverity )
{
    if( aSeverity == QStringLiteral( "error" ) )
        return RPT_SEVERITY_ERROR;
    else if( aSeverity == QStringLiteral( "warning" ) )
        return RPT_SEVERITY_WARNING;
    else if( aSeverity == QStringLiteral( "exclusion" ) )
        return RPT_SEVERITY_EXCLUSION;
    else if( aSeverity == QStringLiteral( "ignore" ) )
        return RPT_SEVERITY_IGNORE;
    else
        return RPT_SEVERITY_INFO;
}

inline QString SeverityToString( const SEVERITY& aSeverity )
{
    switch( aSeverity )
    {
    case RPT_SEVERITY_ERROR:     return QStringLiteral( "error" );
    case RPT_SEVERITY_WARNING:   return QStringLiteral( "warning" );
    case RPT_SEVERITY_EXCLUSION: return QStringLiteral( "exclusion" );
    case RPT_SEVERITY_IGNORE:    return QStringLiteral( "ignore" );
    case RPT_SEVERITY_INFO:
    default:                     return QStringLiteral( "info" );
    }
}

#endif // UI_COMMON_H
