// QT_TRANSFORMATION_COMPLETED
#ifndef HTML_WINDOW_H
#define HTML_WINDOW_H

#include <kicommon.h>
#include <QTextBrowser>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QString>
#include <QEvent>

class KICOMMON_API HTML_WINDOW : public QTextBrowser
{
    Q_OBJECT

public:
    HTML_WINDOW( QWidget* aParent = nullptr, int aId = -1,
                 const QPoint& aPos = QPoint(), const QSize& aSize = QSize(),
                 long aStyle = 0, const QString& aName = "htmlWindow" );

    ~HTML_WINDOW();

    bool SetPage( const QString& aSource );
    bool AppendToPage( const QString& aSource );

    // Notify the HTML window the theme has changed.
    void ThemeChanged();

private slots:
    void onThemeChanged();
    void onRightClick( QMouseEvent* event );

private:
    void contextMenuEvent( QContextMenuEvent* event ) override;

    QString m_pageSource;
};

#endif /* HTML_WINDOW_H */
