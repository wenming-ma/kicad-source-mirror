
#ifndef HTML_MESSAGE_BOX_H
#define HTML_MESSAGE_BOX_H

#include <dialogs/dialog_display_html_text_base.h>
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>


class HTML_MESSAGE_BOX : public DIALOG_DISPLAY_HTML_TEXT_BASE
{
public:
    HTML_MESSAGE_BOX( QWidget* aParent, const QString& aTitle = QString(),
                      const QPoint& aPosition = QPoint(),
                      const QSize& aSize = QSize() );

    ~HTML_MESSAGE_BOX() override;

    void SetDialogSizeInDU( int aWidth, int aHeight )
    {
        setSizeInDU( aWidth, aHeight );
        // Center the dialog on parent or screen
        if( parentWidget() )
        {
            QRect parentGeometry = parentWidget()->frameGeometry();
            QRect dialogGeometry = frameGeometry();
            int x = parentGeometry.x() + (parentGeometry.width() - dialogGeometry.width()) / 2;
            int y = parentGeometry.y() + (parentGeometry.height() - dialogGeometry.height()) / 2;
            move(x, y);
        }
        else
        {
            // Center on screen if no parent
            QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
            QRect dialogGeometry = frameGeometry();
            int x = (screenGeometry.width() - dialogGeometry.width()) / 2;
            int y = (screenGeometry.height() - dialogGeometry.height()) / 2;
            move(x, y);
        }
    }

    void ListSet( const QString& aList );

    void ListSet( const QStringList& aList );

    void ListClear();

    void MessageSet( const QString& message );

    void AddHTML_Text( const QString& message );
    void ShowModeless();

    void OnHTMLLinkClicked( QUrl url );


protected:
    void reload();

    void onThemeChanged( QPalette::ColorRole aEvent );
    virtual void keyPressEvent( QKeyEvent* aEvt ) override;

private:
    QString  m_source;
};

#endif // HTML_MESSAGE_BOX_H
