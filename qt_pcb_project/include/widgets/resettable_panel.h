// QT_TRANSFORMATION_COMPLETED

#ifndef RESETTABLE_PANEL_H_
#define RESETTABLE_PANEL_H_

#include <QWidget>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QObject>
#include <id.h>

class PAGED_DIALOG;

#define wxRESETTABLE 0x00008000
#define ID_RESET_PANEL ID_PREFERENCES_RESET_PANEL

class RESETTABLE_PANEL : public QWidget
{
    Q_OBJECT

public:
    RESETTABLE_PANEL( QWidget* aParent = nullptr )
        : QWidget( aParent )
    {
        connect( this, &RESETTABLE_PANEL::buttonClicked,
                [this]( int aId )
                {
                    if( aId == ID_RESET_PANEL )
                        ResetPanel();
                } );
    }

    virtual void ResetPanel() = 0;

    virtual QString GetResetTooltip() const
    {
        return "Reset all settings on this page to their default";
    }

    QString GetHelpTextAtPoint( const QPoint& aPt ) const
    {
        if( aPt == QPoint( -INT_MAX, INT_MAX ) )
            return GetResetTooltip();
        else
            return QString();
    }

signals:
    void buttonClicked( int aId );
};

#endif
