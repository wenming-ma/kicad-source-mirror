
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef SYMBOL_PREVIEW_WIDGET_H
#define SYMBOL_PREVIEW_WIDGET_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QResizeEvent>
#include <kiway.h>
#include <gal_display_options_common.h>
#include <class_draw_panel_gal.h>


class LIB_ID;
class LIB_SYMBOL;
class QLabel;
class QLayout;


class SYMBOL_PREVIEW_WIDGET: public QWidget
{
public:

    /**
     * Construct a symbol preview widget.
     *
     * @param aParent - parent window
     * @param aKiway - an active Kiway instance
     * @param aCanvasType = the type of canvas (GAL_TYPE_OPENGL or GAL_TYPE_CAIRO only)
     */
    SYMBOL_PREVIEW_WIDGET( QWidget* aParent, KIWAY* aKiway, bool aIncludeStatus,
                           EDA_DRAW_PANEL_GAL::GAL_TYPE aCanvasType );

    ~SYMBOL_PREVIEW_WIDGET() override;

    /**
     * Set the contents of the status label and display it.
     */
    void SetStatusText( const QString& aText );

    /**
     * Set the currently displayed symbol.
     */
    void DisplaySymbol( const LIB_ID& aSymbolID, int aUnit, int aBodyStyle = 0 );

    void DisplayPart( LIB_SYMBOL* aSymbol, int aUnit, int aBodyStyle = 0 );

    EDA_DRAW_PANEL_GAL* GetCanvas() const { return m_preview; }

protected:
    void resizeEvent( QResizeEvent* aEvent ) override;

    void fitOnDrawArea();    // set the view scale to fit the item on screen and center

    KIWAY*                     m_kiway;

    GAL_DISPLAY_OPTIONS_IMPL   m_galDisplayOptions;
    EDA_DRAW_PANEL_GAL*        m_preview;

    QLabel*                    m_status;
    QWidget*                   m_statusPanel;
    QLayout*                   m_statusSizer;
    QLayout*                   m_outerSizer;

    /**
     * A local copy of the #LIB_SYMBOL to display on the canvas.
     */
    LIB_SYMBOL*                m_previewItem;

    /// The bounding box of the current item
    BOX2I                      m_itemBBox;
};


#endif // SYMBOL_PREVIEW_WIDGET_H
