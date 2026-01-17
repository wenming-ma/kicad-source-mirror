
// QT_TRANSFORMATION_COMPLETED

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
#include <QMenu>
#include <QAction>
#include <QIcon>

#define INDETERMINATE_STATE QStringLiteral( "-- mixed values --" )
#define INDETERMINATE_ACTION QStringLiteral( "-- leave unchanged --" )

namespace KIUI
{

const int c_IndicatorSizeDIP = 10;

KICOMMON_API int GetStdMargin();

KICOMMON_API QSize GetTextSize( const QString& aSingleLine, QWidget* aWindow );

KICOMMON_API QFont GetMonospacedUIFont();

KICOMMON_API QFont GetControlFont( QWidget* aWindow );
KICOMMON_API QFont GetInfoFont( QWidget* aWindow );
KICOMMON_API QFont GetDockedPaneFont( QWidget* aWindow );
KICOMMON_API QFont GetStatusFont( QWidget* aWindow );

KICOMMON_API bool EnsureTextCtrlWidth( QLineEdit* aCtrl, const QString* aString = nullptr );

KICOMMON_API void SelectReferenceNumber( QLineEdit* aTextEntry );

KICOMMON_API QString EllipsizeStatusText( QWidget* aWindow, const QString& aString );

KICOMMON_API QString EllipsizeMenuText( const QString& aString );

KICOMMON_API bool IsInputControlFocused( QWidget* aFocus = nullptr );

KICOMMON_API bool IsInputControlEditable( QWidget* aControl );

KICOMMON_API bool IsModalDialogFocused();

KICOMMON_API void Disable( QWidget* aWindow );

KICOMMON_API extern const QString s_FocusStealableInputName;


// LINKER_FIX: Inline implementation to resolve linker error
// This is a wxWidgets to Qt transformation - original function used wxMenuItem and wxBitmapBundle
inline void AddBitmapToMenuItem( QAction* aMenu, const QIcon& aImage )
{
    // Qt transformation: QAction::setIcon() is the Qt equivalent of wxMenuItem::SetBitmap()
    // In Qt, icons are always displayed if set, so we directly set the icon
    if( !aImage.isNull() )
        aMenu->setIcon( aImage );
}


KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, int aId, const QString& aText,
                                   const QIcon& aImage,
                                   bool aCheckable = false );


KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, int aId, const QString& aText,
                                   const QString& aHelpText, const QIcon& aImage,
                                   bool aCheckable = false );


KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, QMenu* aSubMenu, int aId,
                                   const QString& aText, const QIcon& aImage );


KICOMMON_API QAction* AddMenuItem( QMenu* aMenu, QMenu* aSubMenu, int aId,
                                   const QString& aText, const QString& aHelpText,
                                   const QIcon& aImage );
}

KICOMMON_API SEVERITY SeverityFromString( const QString& aSeverity );

KICOMMON_API QString SeverityToString( const SEVERITY& aSeverity );

#endif // UI_COMMON_H
