#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QMenu>
#include <QAction>
#include <QListWidget>
#include <QTreeView>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QScrollBar>
#include <QTableWidget>
#include <QApplication>
#include <QFontMetrics>
#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QStyleOption>
#include <widgets/kiui_common.h>

#include <algorithm>
#include <dialog_shim.h>
#include <pgm_base.h>
#include <QGuiApplication>
#include <settings/common_settings.h>
#include <bitmaps/bitmap_types.h>
#include <string_utils.h>

const QString KIUI::s_FocusStealableInputName = "KI_NOFOCUS";


int KIUI::GetStdMargin()
{
    // This is the value used in (most) Qt dialogs
    return 5;
}


SEVERITY SeverityFromString( const QString& aSeverity )
{
    if( aSeverity == "warning" )
        return RPT_SEVERITY_WARNING;
    else if( aSeverity == "ignore" )
        return RPT_SEVERITY_IGNORE;
    else
        return RPT_SEVERITY_ERROR;
}


QString SeverityToString( const SEVERITY& aSeverity )
{
    if( aSeverity == RPT_SEVERITY_IGNORE )
        return "ignore";
    else if( aSeverity == RPT_SEVERITY_WARNING )
        return "warning";
    else
        return "error";
}


QSize KIUI::GetTextSize( const QString& aSingleLine, QWidget* aWindow )
{
    QFontMetrics fm( aWindow->font() );
    return fm.size( Qt::TextSingleLine, aSingleLine );
}


QFont KIUI::GetMonospacedUIFont()
{
    static int guiFontSize = QApplication::font().pointSize();

    QFont font = QFontDatabase::systemFont( QFontDatabase::FixedFont );
    font.setPointSize( guiFontSize );

#ifdef __APPLE__
    if( font.family().isEmpty() )
        font.setFamily( "Menlo" );
#endif

    return font;
}


QFont getGUIFont( QWidget* aWindow, int aRelativeSize )
{
    QFont font = aWindow->font();

    font.setPointSize( font.pointSize() + aRelativeSize );

    if( Pgm().GetCommonSettings()->m_Appearance.apply_icon_scale_to_fonts )
        font.setPointSize( KiROUND( KiIconScale( aWindow ) * font.pointSize() / 4.0 ) );

#ifdef __APPLE__
    if( font.family().isEmpty() )
        font.setFamily( "San Francisco" );
#endif

    return font;
}


QFont KIUI::GetStatusFont( QWidget* aWindow )
{
#ifdef __APPLE__
    int scale = -2;
#else
    int scale = 0;
#endif

    return getGUIFont( aWindow, scale );
}


QFont KIUI::GetDockedPaneFont( QWidget* aWindow )
{
#ifdef __APPLE__
    int scale = -1;
#else
    int scale = 0;
#endif

    return getGUIFont( aWindow, scale );
}


QFont KIUI::GetInfoFont( QWidget* aWindow )
{
    return getGUIFont( aWindow, -1 );
}


QFont KIUI::GetControlFont( QWidget* aWindow )
{
    return getGUIFont( aWindow, 0 );
}


bool KIUI::EnsureTextCtrlWidth( QLineEdit* aCtrl, const QString* aString )
{
    QWidget* window = aCtrl->parentWidget();

    if( !window )
        window = aCtrl;

    QString ctrlText;

    if( !aString )
    {
        ctrlText = aCtrl->text();
        aString  = &ctrlText;
    }

    QSize textz = GetTextSize( *aString, window );
    QSize ctrlz = aCtrl->size();

    if( ctrlz.width() < textz.width() + 10 )
    {
        ctrlz.setWidth( textz.width() + 10 );
        aCtrl->setMinimumSize( ctrlz );
        return true;
    }

    return false;
}


QString KIUI::EllipsizeStatusText( QWidget* aWindow, const QString& aString )
{
    QString msg = UnescapeString( aString );

    msg.replace( "\n", " " );
    msg.replace( "\r", " " );
    msg.replace( "\t", " " );

    QFontMetrics fm( aWindow->font() );
    int statusWidth = aWindow->size().width();

    // 30% of the first 800 pixels plus 60% of the remaining width
    int textWidth = std::min( statusWidth, 800 ) * 0.3 + std::max( statusWidth - 800, 0 ) * 0.6;

    return fm.elidedText( msg, Qt::ElideRight, textWidth );
}


QString KIUI::EllipsizeMenuText( const QString& aString )
{
    QString msg = UnescapeString( aString );

    msg.replace( "\n", " " );
    msg.replace( "\r", " " );
    msg.replace( "\t", " " );

    if( msg.length() > 36 )
        msg = msg.left( 34 ) + "...";

    return msg;
}


void KIUI::SelectReferenceNumber( QLineEdit* aTextEntry )
{
    QString ref = aTextEntry->text();

    if( ref.indexOf( '?' ) != -1 )
    {
        aTextEntry->setSelection( ref.indexOf( '?' ), ref.lastIndexOf( '?' ) + 1 - ref.indexOf( '?' ) );
    }
    else if( ref.indexOf( '*' ) != -1 )
    {
        aTextEntry->setSelection( ref.indexOf( '*' ), ref.lastIndexOf( '*' ) + 1 - ref.indexOf( '*' ) );
    }
    else
    {
        QString num = ref;

        while( !num.isEmpty() && ( !num.back().isDigit() || !num.front().isDigit() ) )
        {
            // Trim non-digit from end
            if( !num.back().isDigit() )
                num.chop(1);

            // Trim non-digit from the start
            if( !num.isEmpty() && !num.front().isDigit() )
                num = num.mid(1);
        }

        int startPos = ref.indexOf( num );
        aTextEntry->setSelection( startPos, num.length() );

        if( num.isEmpty() )
            aTextEntry->setSelection( 0, 0 );
    }
}


bool KIUI::IsInputControlFocused( QWidget* aFocus )
{
    if( aFocus == nullptr )
        aFocus = QApplication::focusWidget();

    if( !aFocus )
        return false;

    // These widgets are never considered focused
    if( aFocus->objectName() == s_FocusStealableInputName )
        return false;

    QLineEdit*     textEntry = qobject_cast<QLineEdit*>( aFocus );
    QTextEdit*     styledText = qobject_cast<QTextEdit*>( aFocus );
    QListWidget*   listBox = qobject_cast<QListWidget*>( aFocus );
    QLineEdit*     searchCtrl = qobject_cast<QLineEdit*>( aFocus );
    QCheckBox*     checkboxCtrl = qobject_cast<QCheckBox*>( aFocus );
    QComboBox*     choiceCtrl = qobject_cast<QComboBox*>( aFocus );
    QRadioButton*  radioBtn = qobject_cast<QRadioButton*>( aFocus );
    QSpinBox*      spinCtrl = qobject_cast<QSpinBox*>( aFocus );
    QDoubleSpinBox* spinDblCtrl = qobject_cast<QDoubleSpinBox*>( aFocus );
    QSlider*       sliderCtl = qobject_cast<QSlider*>( aFocus );

    // Tree view control focus handling
    QTreeView* dataViewCtrl = nullptr;

    QWidget* parent = aFocus->parentWidget();

    if( parent )
        dataViewCtrl = qobject_cast<QTreeView*>( parent );

    return ( textEntry || styledText || listBox || searchCtrl || checkboxCtrl || choiceCtrl
                || radioBtn || spinCtrl || spinDblCtrl || sliderCtl || dataViewCtrl );
}


bool KIUI::IsInputControlEditable( QWidget* aFocus )
{
    QLineEdit*     textEntry = qobject_cast<QLineEdit*>( aFocus );
    QTextEdit*     styledText = qobject_cast<QTextEdit*>( aFocus );
    QLineEdit*     searchCtrl = qobject_cast<QLineEdit*>( aFocus );

    if( textEntry )
        return !textEntry->isReadOnly();
    else if( styledText )
        return !styledText->isReadOnly();
    else if( searchCtrl )
        return !searchCtrl->isReadOnly();

    // Must return true if we can't determine the state, intentionally true for non inputs as well.
    return true;
}


bool KIUI::IsModalDialogFocused()
{
    return !Pgm().m_ModalDialogs.empty();
}


void KIUI::Disable( QWidget* aWindow )
{
    QScrollBar*      scrollBar = qobject_cast<QScrollBar*>( aWindow );
    QWidget*         hyperlink = aWindow; // Qt doesn't have direct hyperlink equivalent
    QTableWidget*    grid = qobject_cast<QTableWidget*>( aWindow );
    QTextEdit*       scintilla = qobject_cast<QTextEdit*>( aWindow );

    if( scrollBar || (hyperlink && hyperlink->objectName().contains("hyperlink")) )
    {
        // Leave navigation controls active
    }
    else if( grid )
    {
        for( int row = 0; row < grid->rowCount(); ++row )
        {
            for( int col = 0; col < grid->columnCount(); ++col )
            {
                QTableWidgetItem* item = grid->item( row, col );
                if( item )
                    item->setFlags( item->flags() & ~Qt::ItemIsEditable );
            }
        }
    }
    else if( scintilla )
    {
        scintilla->setReadOnly( true );
    }
    else if( aWindow )
    {
        aWindow->setEnabled( false );
    }
    else
    {
        QList<QWidget*> children = aWindow->findChildren<QWidget*>();
        for( QWidget* child : children )
            Disable( child );
    }
}


void KIUI::AddBitmapToMenuItem( QAction* aAction, const QIcon& aImage )
{
    // Retrieve the global application show icon option:
    bool useImagesInMenus = Pgm().GetCommonSettings()->m_Appearance.use_icons_in_menus;

    if( useImagesInMenus && !aAction->isCheckable() )
    {
        aAction->setIcon( aImage );
    }
}


QAction* KIUI::AddMenuItem( QMenu* aMenu, int aId, const QString& aText,
                           const QIcon& aImage, bool aCheckable )
{
    QAction* action = new QAction( aText, aMenu );
    action->setData( aId );
    action->setCheckable( aCheckable );
    AddBitmapToMenuItem( action, aImage );

    aMenu->addAction( action );

    return action;
}


QAction* KIUI::AddMenuItem( QMenu* aMenu, int aId, const QString& aText,
                           const QString& aHelpText, const QIcon& aImage,
                           bool aCheckable )
{
    QAction* action = new QAction( aText, aMenu );
    action->setData( aId );
    action->setStatusTip( aHelpText );
    action->setCheckable( aCheckable );
    AddBitmapToMenuItem( action, aImage );

    aMenu->addAction( action );

    return action;
}


QAction* KIUI::AddMenuItem( QMenu* aMenu, QMenu* aSubMenu, int aId, const QString& aText,
                           const QIcon& aImage )
{
    QAction* action = aSubMenu->menuAction();
    action->setText( aText );
    action->setData( aId );
    AddBitmapToMenuItem( action, aImage );

    aMenu->addMenu( aSubMenu );

    return action;
}


QAction* KIUI::AddMenuItem( QMenu* aMenu, QMenu* aSubMenu, int aId, const QString& aText,
                           const QString& aHelpText, const QIcon& aImage )
{
    QAction* action = aSubMenu->menuAction();
    action->setText( aText );
    action->setData( aId );
    action->setStatusTip( aHelpText );
    AddBitmapToMenuItem( action, aImage );

    aMenu->addMenu( aSubMenu );

    return action;
}