// QT_TRANSFORMATION_COMPLETED
#ifndef WX_MENUBAR_H_
#define WX_MENUBAR_H_

#include <QMenuBar>
#include <QKeySequence>
#include <QWidget>
#include <QAction>
#include <tool/action_menu.h>

// Wrapper around a QMenuBar object that prevents accelerator conflicts with text controls
// On Windows, accelerators can interfere with key events reaching text controls
class WX_MENUBAR : public QMenuBar
{
    Q_OBJECT

public:
    WX_MENUBAR(QWidget* parent = nullptr) : QMenuBar(parent) {}

    void SetAcceleratorTable( const QList<QKeySequence>& aTable )
    {
        // Don't use the passed in accelerator table to avoid conflicts with text controls
    }

    QString GetMenuLabelText( int aPos ) const
    {
        QList<QAction*> actionList = this->actions();
        if( aPos < 0 || aPos >= actionList.size() )
            return QString();

        QAction* action = actionList.at(aPos);
        if( !action )
            return QString();

        if( ACTION_MENU* actionMenu = dynamic_cast<ACTION_MENU*>( action->menu() ) )
        {
            QString title = actionMenu->GetTitle();

            // Clear accelerator key markings
            title.replace( " & ", " {amp} " );
            title.replace( "&", "" );
            title.replace( "{amp}", "&" );

            return title;
        }

        return action->text();
    }
};

#endif // WX_MENUBAR_H_
