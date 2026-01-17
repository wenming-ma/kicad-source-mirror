// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#ifndef WX_COMBOBOX_H
#define WX_COMBOBOX_H

#include <map>
#include <QComboBox>
#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QMouseEvent>
#include <QString>

// Fix some issues with ComboBox - custom implementation for Qt
class WX_COMBOBOX : public QComboBox
{
public:
    WX_COMBOBOX( QWidget* aParent, int aId = -1, const QString& aValue = QString(),
                 const QPoint& aPos = QPoint(), const QSize& aSize = QSize(),
                 int n = 0, const QString choices[] = nullptr, long style = 0 );

    virtual ~WX_COMBOBOX();

    void Append( const QString& aText, const QString& aMenuText = QString() );

    int GetCharHeight() const override;

protected:
    virtual void DoSetPopupControl( QWidget* aPopup );
    virtual void OnDrawItem( QPainter& aPainter, const QRect& aRect, int aItem, int aFlags ) const;
    virtual int OnMeasureItem( size_t aItem ) const;
    virtual int OnMeasureItemWidth( size_t aItem ) const;

    /// Veto a mouseover event if in the separator
    void TryVetoMouse( QMouseEvent& aEvent );

    // Veto a select event for the separator
    // @param aIndex - the selected index
    // @param aInner - true if event was called for the inner list (ie the popup)
    void TryVetoSelect( int aIndex, bool aInner );

    // Safely get a string for an item, returning empty string if the item doesn't exist.
    QString GetMenuText( int aItem ) const;

    // Get selection from either the outer (combo box) or inner (popup) list.
    int GetSelectionEither( bool aInner ) const;

    // Safely set selection for either the outer (combo box) or inner (popup) list, doing nothing
    // for invalid selections.
    void SetSelectionEither( bool aInner, int aSel );

private:
    std::map<int, QString> m_menuText;
    int                    m_lastSelection;
};

#endif // WX_COMBOBOX_H
