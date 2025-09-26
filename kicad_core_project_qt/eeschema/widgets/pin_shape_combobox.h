
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

// ComboBox widget for pin shape

#include <QComboBox>
#include <QWidget>
#include <pin_type.h>

class PinShapeComboBox : public QComboBox
{
public:
    PinShapeComboBox( QWidget* parent = nullptr );

    GRAPHIC_PINSHAPE GetPinShapeSelection();
    void             SetSelection( GRAPHIC_PINSHAPE aShape );

private:
    // fixes hidden overloaded virtual function warnings
    using QComboBox::setCurrentIndex;
};
