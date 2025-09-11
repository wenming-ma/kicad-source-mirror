// QT_TRANSFORMATION_COMPLETED
#ifndef KICAD_QT_AUI_ART_PROVIDERS_H
#define KICAD_QT_AUI_ART_PROVIDERS_H

#include <QWidget>
#include <QPainter>
#include <QSize>
#include <QRect>
#include <QToolBar>
#include <QDockWidget>


// Forward declarations for Qt AUI-like toolbar item
class QT_AUI_TOOLBAR_ITEM;

class QT_AUI_TOOLBAR_ART
{
public:
    QT_AUI_TOOLBAR_ART() {}

    virtual ~QT_AUI_TOOLBAR_ART() = default;

    virtual QSize GetToolSize( QPainter& aPainter, QWidget* aWindow, const QT_AUI_TOOLBAR_ITEM& aItem );

    // Re-implemented to control the size
    virtual void DrawButton( QPainter& aPainter, QWidget* aWindow, const QT_AUI_TOOLBAR_ITEM& aItem,
                     const QRect& aRect );
};


class QT_AUI_DOCK_ART
{
public:
    QT_AUI_DOCK_ART();
};


#endif // KICAD_QT_AUI_ART_PROVIDERS_H
