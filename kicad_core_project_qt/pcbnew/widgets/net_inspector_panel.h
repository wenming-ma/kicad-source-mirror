
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef NET_INSPECTOR_PANEL_BASE_H
#define NET_INSPECTOR_PANEL_BASE_H

#include <bitmaps.h>
#include <widgets/bitmap_button.h>

#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTreeView>
#include <QtCore/QString>
#include <QtGui/QFocusEvent>
#include <QtGui/QResizeEvent>
#include <QtCore/QEvent>

class EDA_BASE_FRAME;

/**
 * A base class used to implement docking net inspector panels.
 *
 * Provides a filter control, a settings button, and a data-driven QTreeView
 */
class NET_INSPECTOR_PANEL : public QWidget
{
public:
    NET_INSPECTOR_PANEL( QWidget* parent, EDA_BASE_FRAME* aFrame, int id = -1,
                         const QPoint& pos = QPoint(),
                         const QSize& size = QSize( -1, -1 ), long style = 0,
                         const QString& name = QString() );

    ~NET_INSPECTOR_PANEL();

    /**
     * Rebuild inspector data if project settings updated
     *
     * Called by the parent EDA_EDIT_FRAME on change of settings (e.g. stackup, netclass
     * definitions)
     */
    virtual void OnParentSetupChanged(){};

    /**
     * Save the net inspector settings - called from EDA_EDIT_FRAME when hiding the panel
     */
    virtual void SaveSettings(){};

    /**
     * Prepare the panel when (re-)shown in the editor
     */
    virtual void OnShowPanel() {}

    /**
     * Notification from file loader when board changed and connectivity rebuilt
     */
    virtual void OnBoardChanged() {}

protected:
    // User-driven UI events (override in derrived classes as required)
    virtual void OnSetFocus( QFocusEvent& event ) { event.ignore(); }
    virtual void OnSize( QResizeEvent& event ) { event.ignore(); }
    virtual void OnSearchTextChanged( QEvent& event ) { event.ignore(); }
    virtual void OnConfigButton( QEvent& event ) { event.ignore(); }
    virtual void OnLanguageChanged( QEvent& event );

    /**
     * Implementation-specific implementation of language update handling
     */
    virtual void OnLanguageChangedImpl(){};

protected:
    EDA_BASE_FRAME* m_frame;

    QGridLayout*    m_sizerOuter;
    QLineEdit*      m_searchCtrl;
    BITMAP_BUTTON*  m_configureBtn;
    QTreeView*      m_netsList;
};

#endif
