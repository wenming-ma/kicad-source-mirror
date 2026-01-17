
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef DIALOG_PIN_PROPERTIES_H
#define DIALOG_PIN_PROPERTIES_H

#include <QComboBox>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QEvent>
#include <utility>

#include <pin_shape_combobox.h>
#include <pin_type_combobox.h>

#include <dialog_pin_properties_base.h>
#include <widgets/unit_binder.h>
#include <sch_pin.h>
#include <symbol_edit_frame.h>


enum COL_ORDER
{
    COL_NAME,
    COL_TYPE,
    COL_SHAPE,

    COL_COUNT       // keep as last
};


class ALT_PIN_DATA_MODEL;


class DIALOG_PIN_PROPERTIES : public DIALOG_PIN_PROPERTIES_BASE
{
public:
    DIALOG_PIN_PROPERTIES( SYMBOL_EDIT_FRAME* parent, SCH_PIN* aPin, bool aFocusPinNumber );
    ~DIALOG_PIN_PROPERTIES() override;

    bool TransferDataToWindow();
    bool TransferDataFromWindow();

    /**
     * Draw (on m_panelShowPin) the pin according to current settings in dialog.
     */
    void OnPaintShowPanel( QPaintEvent* event ) override;
    void OnPropertiesChange() override;
    void OnAddAlternate() override;
    void OnDeleteAlternate() override;
    void OnSize( QResizeEvent* event ) override;
    void OnUpdateUI( QEvent* event ) override;
    void OnCollapsiblePaneChange() override;

protected:
    void adjustGridColumns();
    QString getSyncPinsMessage();

private:
    SYMBOL_EDIT_FRAME*  m_frame;
    SCH_PIN*            m_pin;

    LIB_SYMBOL*         m_dummyParent;
    SCH_PIN*            m_dummyPin;                   // a working copy used to show changes

    UNIT_BINDER         m_posX;
    UNIT_BINDER         m_posY;
    UNIT_BINDER         m_pinLength;
    UNIT_BINDER         m_nameSize;
    UNIT_BINDER         m_numberSize;

    VECTOR2I            m_origPos;

    ALT_PIN_DATA_MODEL* m_alternatesDataModel;

    int                 m_delayedFocusRow;
    int                 m_delayedFocusColumn;

    std::map<int, int>  m_originalColWidths;          // map col-number : orig-col-width
    QSize               m_size;
    bool                m_initialized;
    inline static bool  s_alternatesTurndownOpen = false;
};

#endif // DIALOG_PIN_PROPERTIES_H
