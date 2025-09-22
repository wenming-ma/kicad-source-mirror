
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

// ComboBox widget for pin type

#include <QComboBox>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QString>

#include <pin_type.h>

class PinTypeComboBox : public QComboBox
{
public:

    PinTypeComboBox( QWidget* parent = nullptr,
                     const QString& value = QString(),
                     const QPoint& pos = QPoint(),
                     const QSize& size = QSize(),
                     int n = 0,
                     const QString choices[] = nullptr,
                     const QString& name = QString() );

    ELECTRICAL_PINTYPE  GetPinTypeSelection();
    void                SetSelection( ELECTRICAL_PINTYPE aType );

private:
    // fixes hidden overloaded virtual function warnings
    using QComboBox::setCurrentIndex;
};
