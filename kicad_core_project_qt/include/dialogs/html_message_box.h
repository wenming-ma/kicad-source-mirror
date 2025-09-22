
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef HTML_MESSAGE_BOX_H
#define HTML_MESSAGE_BOX_H

#include <dialogs/dialog_display_html_text_base.h>


class HTML_MESSAGE_BOX : public DIALOG_DISPLAY_HTML_TEXT_BASE
{
public:
    HTML_MESSAGE_BOX( QWidget* aParent, const QString& aTitle = QString(),
                      const QPoint& aPosition = QPoint(),
                      const QSize& aSize = QSize() );

    ~HTML_MESSAGE_BOX() override;

    /**
     * Set the dialog size, using a "logical" value.
     *
     * The physical size in pixel will depend on the display definition so a value used here
     * should be OK with any display (HDPI for instance).
     *
     * @param aWidth is a "logical" value of the dialog width.
     * @param aHeight is a "logical" value of the dialog height.
     */
    void SetDialogSizeInDU( int aWidth, int aHeight )
    {
        setSizeInDU( aWidth, aHeight );
        Center();
    }

    /**
     * Add a list of items.
     *
     * @param aList is a string containing HTML items. Items are separated by '\n'
     */
    void ListSet( const QString& aList );

    /**
     * Add a list of items.
     *
     * @param aList is the list of HTML strings to display.
     */
    void ListSet( const QStringList& aList );

    void ListClear();

    /**
     * Add a message (in bold) to message list.
     */
    void MessageSet( const QString& message );

    /**
     * Add HTML text (without any change) to message list.
     */
    void AddHTML_Text( const QString& message );

    /**
     * Show a modeless version of the dialog (without an OK button).
     */
    void ShowModeless();

    void OnHTMLLinkClicked( QMouseEvent& event ) override;


protected:
    void reload();

    void onThemeChanged( QPaintEvent &aEvent );
    virtual void OnCharHook( QKeyEvent& aEvt ) override;

private:
    QString  m_source;
};

#endif // HTML_MESSAGE_BOX_H
