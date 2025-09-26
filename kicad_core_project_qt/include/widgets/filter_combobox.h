
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#pragma once

#include <memory>
#include <optional>

#include <QWidget>
#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QValidator>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QSize>
#include <QStringList>


class QValidator;
class QLineEdit;
class QListWidget;


class FILTER_COMBOPOPUP : public QWidget
{
public:
    FILTER_COMBOPOPUP();

    bool Create( QWidget* aParent );

    QWidget* GetControl() { return this; }

    void SetStringList( const QStringList& aStringList );

    QString GetStringValue() const;
    void SetStringValue( const QString& aNetName );

    void SetSelectedString( const QString& aString );

    void OnPopup();

    void OnStartingKey( QKeyEvent& aEvent );

    QSize GetAdjustedSize( int aMinWidth, int aPrefHeight, int aMaxHeight );

    virtual void Accept();

protected:
    /**
     * Get the currently selected value in the list, or std::nullopt
     */
    std::optional<QString> getSelectedValue() const;

    /**
     * Get the current value of the filter control. Can be empty.
     */
    QString getFilterValue() const;

    /**
     * Fill the combobox list
     */
    virtual void getListContent( QStringList& aStringList );

    /**
     * Call this to rebuild the list from the getListContent() method.
     */
    void rebuildList();

private:
    QSize updateSize();

    void onIdle( QEvent& aEvent );

    // Hot-track the mouse (for focus and listbox selection)
    void onMouseMoved( const QPoint aScreenPos );
    void onMouseClick( QMouseEvent& aEvent );
    void onKeyDown( QKeyEvent& aEvent );
    void onEnter( QEvent& aEvent );
    void onFilterEdit( QEvent& aEvent );
    void doStartingKey( QKeyEvent& aEvent );
    void doSetFocus( QWidget* aWindow );

protected:
    QValidator*      m_filterValidator;
    QLineEdit*       m_filterCtrl;
    QListWidget*     m_listBox;
    int              m_minPopupWidth;
    int              m_maxPopupHeight;

    QObject*         m_focusHandler;

    QString          m_selectedString;
    QStringList      m_stringList;
};


// Event declaration - Qt signal/slot will be used instead

/**
 * A combobox that has a filterable popup.
 *
 * Useful when the list of items is long and you want the user to
 * be able to filter it by typing.
 */
class FILTER_COMBOBOX : public QComboBox
{
public:
    // C'tor matching wxFormBuilder's Custom Control
    FILTER_COMBOBOX( QWidget* parent, int id, const QPoint& pos = QPoint(),
                     const QSize& size = QSize(), long style = 0 );

    // C'tor matching wxFormBuilder's ComboxBox.
    FILTER_COMBOBOX( QWidget* parent, int id, const QString& value,
                     const QPoint& pos = QPoint(), const QSize& size = QSize(),
                     int count = 0, QString strings[] = nullptr, long style = 0 );

    ~FILTER_COMBOBOX();

    virtual void SetStringList( const QStringList& aStringList );

    virtual void SetSelectedString( const QString& aString );

protected:
    void setFilterPopup( FILTER_COMBOPOPUP* aPopup );

    void onKeyDown( QKeyEvent& aEvt );

    FILTER_COMBOPOPUP* m_filterPopup;
};
