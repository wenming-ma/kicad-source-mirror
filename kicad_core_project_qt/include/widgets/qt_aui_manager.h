#ifndef QT_AUI_MANAGER_H
#define QT_AUI_MANAGER_H

#include <QObject>
#include <QMainWindow>
#include <QDockWidget>
#include <QSize>
#include <QString>
#include <QWidget>

// Forward declarations
class EDA_PANE;

/**
 * Qt compatibility stub for wxAuiPaneInfo
 * This provides a basic interface compatible with the existing wx-to-Qt transformed code
 */
class QAuiPaneInfo
{
public:
    QAuiPaneInfo() : m_dockWidget(nullptr) {}
    QAuiPaneInfo(QDockWidget* dockWidget) : m_dockWidget(dockWidget) {}

    // Stub methods for compatibility with transformed wxWidgets code
    QAuiPaneInfo& MinSize(const QSize& size) { if(m_dockWidget) m_dockWidget->setMinimumSize(size); return *this; }
    QAuiPaneInfo& BestSize(const QSize& size) { if(m_dockWidget) m_dockWidget->resize(size); return *this; }
    QAuiPaneInfo& FloatingSize(const QSize& size) { if(m_dockWidget) m_dockWidget->resize(size); return *this; }
    QAuiPaneInfo& Show(bool show = true) { if(m_dockWidget) m_dockWidget->setVisible(show); return *this; }

    // Check if the pane is docked (not floating)
    bool IsDocked() const { return m_dockWidget ? !m_dockWidget->isFloating() : false; }

private:
    QDockWidget* m_dockWidget;
};

/**
 * Qt compatibility stub for wxAuiManager
 * This provides a basic interface compatible with the existing wx-to-Qt transformed code
 */
class QAuiManager : public QObject
{
    Q_OBJECT

public:
    explicit QAuiManager(QObject* parent = nullptr);
    virtual ~QAuiManager();

    // Compatibility methods for transformed wxWidgets code
    void SetManagedWindow(QMainWindow* window);
    void AddPane(QWidget* widget, const EDA_PANE& paneInfo);
    QAuiPaneInfo& GetPane(QWidget* widget);
    QAuiPaneInfo& GetPane(const QString& name);
    void Update();

private:
    QMainWindow* m_managedWindow;
    QAuiPaneInfo m_dummyPaneInfo; // For cases where pane is not found
};

#endif // QT_AUI_MANAGER_H