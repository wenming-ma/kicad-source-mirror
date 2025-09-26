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
    // LINKER_FIX: Inline constructor implementation to resolve linker error
    HTML_WINDOW( QWidget* aParent = nullptr, int aId = -1,
                 const QPoint& aPos = QPoint(), const QSize& aSize = QSize(),
                 long aStyle = 0, const QString& aName = "htmlWindow" )
        : QTextBrowser( aParent )
    {
        if( !aPos.isNull() || !aSize.isEmpty() )
        {
            if( !aPos.isNull() )
                move( aPos );
            if( !aSize.isEmpty() )
                resize( aSize );
        }
        setObjectName( aName );
        setOpenExternalLinks( true );
        setReadOnly( true );
    }

    ~HTML_WINDOW() {}

    bool SetPage( const QString& aSource )
    {
        // LINKER_FIX: Inline implementation
        m_pageSource = aSource;
        setHtml( aSource );
        return true;
    }

    bool AppendToPage( const QString& aSource )
    {
        // LINKER_FIX: Inline implementation
        m_pageSource += aSource;
        setHtml( m_pageSource );
        return true;
    }

    // Notify the HTML window the theme has changed.
    void ThemeChanged()
    {
        // LINKER_FIX: Inline implementation
        onThemeChanged();
    }

private slots:
    // LINKER_FIX: Inline slot implementations to resolve linker errors
    void onThemeChanged()
    {
        // Re-apply the current page source to refresh with new theme
        if( !m_pageSource.isEmpty() )
        {
            QString current = m_pageSource;
            clear();
            SetPage( current );
        }
    }

    void onRightClick( QMouseEvent* event )
    {
        // Handle right-click via context menu event instead
        // This is a compatibility stub
    }

private:
    void contextMenuEvent( QContextMenuEvent* event ) override
    {
        // LINKER_FIX: Inline implementation
        // Basic context menu handling - can be extended as needed
        QTextBrowser::contextMenuEvent( event );
    }

    QString m_pageSource;
};

#endif /* HTML_WINDOW_H */
