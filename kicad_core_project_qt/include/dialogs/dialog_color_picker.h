
#ifndef DIALOG_COLOR_PICKER_H
#define DIALOG_COLOR_PICKER_H


#include <gal/color4d.h>
#include "../../common/dialogs/dialog_color_picker_base.h"

#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtGui/QPixmap>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>

class COLOR_SWATCH;


/**
 * A class to handle a custom color (predefined color) for the color picker dialog.
 */
struct CUSTOM_COLOR_ITEM
{
    KIGFX::COLOR4D m_Color;
    QString m_ColorName;

    CUSTOM_COLOR_ITEM( double red, double green, double blue, const QString& aName )
    {
        m_Color.r = red;
        m_Color.g = green;
        m_Color.b = blue;
        m_ColorName = aName;
    }

    CUSTOM_COLOR_ITEM( double red, double green, double blue, double alpha, const QString& aName )
    {
        m_Color.r = red;
        m_Color.g = green;
        m_Color.b = blue;
        m_Color.a = alpha;
        m_ColorName = aName;
    }

    CUSTOM_COLOR_ITEM( const KIGFX::COLOR4D& aColor, const QString& aName )
        : m_Color( aColor ), m_ColorName( aName)
    {}
};


typedef std::vector<CUSTOM_COLOR_ITEM> CUSTOM_COLORS_LIST;


enum CHANGED_COLOR
{
    ALL_CHANGED,
    RED_CHANGED,
    BLUE_CHANGED,
    GREEN_CHANGED,
    ALPHA_CHANGED,
    HUE_CHANGED,
    SAT_CHANGED,
    VAL_CHANGED,
    HEX_CHANGED
};

class DIALOG_COLOR_PICKER : public DIALOG_COLOR_PICKER_BASE
{
public:
    /**
     * Dialog constructor
     * @param aParent is the caller
     * @param aCurrentColor is the current color, used to show it in dialog
     * @param aAllowOpacityControl true to allow opacity (alpha channel) setting
     * false to not show this setting (opacity = 1.0 always)
     * @param aUserColors if not null is a list of defined colors replacing the dialog
     *                    predefined colors
     */
	DIALOG_COLOR_PICKER( QWidget* aParent, const KIGFX::COLOR4D& aCurrentColor,
                         bool aAllowOpacityControl, CUSTOM_COLORS_LIST* aUserColors = nullptr,
                         const KIGFX::COLOR4D& aDefaultColor = KIGFX::COLOR4D::UNSPECIFIED );
	~DIALOG_COLOR_PICKER();

	KIGFX::COLOR4D GetColor() { return m_newColor4D; };

private:
    /* When the dialog is created, the mouse cursor can be on the RGB or HSV palette selector
     * Because this dialog is created by clicking double clicking on a widget, the left mouse
     * button is down, thus creating a not wanted mouse event inside this dialog
     * m_allowMouseEvents is first set to false, and then set to true on the first left mouse
     * clicking inside this dialog to prevent not wanted mouse drag event
     */
    bool m_allowMouseEvents;
    bool m_allowOpacityCtrl;            ///< true to show the widget,
                                        ///< false to keep alpha channel = 1.0
    KIGFX::COLOR4D m_previousColor4D;   ///< the initial color4d
    KIGFX::COLOR4D m_newColor4D;        ///< the current color4d
    KIGFX::COLOR4D m_defaultColor;      ///< The default color4d

    /// the list of color4d ordered by button ID, for predefined colors
    std::vector<KIGFX::COLOR4D> m_Color4DList;
    int m_cursorsSize;

    QPoint m_cursorBitmapRed;          ///< the red cursor on the RGB bitmap palette.
    QPoint m_cursorBitmapGreen;        ///< the green cursor on the RGB bitmap palette.
    QPoint m_cursorBitmapBlue;         ///< the blue cursor on the RGB bitmap palette.
    QPoint m_cursorBitmapHSV;          ///< the cursor on the HSV bitmap palette.
    QPoint* m_selectedCursor;          ///< the ref cursor to the selected cursor, if any, or null.

    double m_hue;                       ///< the current hue, in degrees (0 ... 360)
    double m_sat;                       ///< the current saturation (0 ... 1.0)
    double m_val;                       ///< the current value (0 ... 1.0)

    QPixmap* m_bitmapRGB;              ///< the basic RGB palette
    QPixmap* m_bitmapHSV;              ///< the basic HUV palette

    std::vector<QLabel*> m_colorSwatches;    ///< list of defined colors buttons

    void SetEditVals( CHANGED_COLOR aChanged, bool aCheckTransparency );
	void drawAll();

	void createHSVBitmap();             ///< generate the bitmap that shows the HSV color circle
	void drawHSVPalette();              ///< draws the HSV color circle
    void createRGBBitmap();             ///< generate the bitmap that shows the RVB color space
    void drawRGBPalette();              ///< draws the RVB color space

    ///< repaint a static bitmap with the aColor4D color
    void updatePreview( QLabel* aStaticBitmap, KIGFX::COLOR4D& aColor4D );

    ///< Event handler from QSlider: brightness (value) control
	void OnChangeBrightness( int value );

    ///< Event handler from QSlider: alpha (transparency) control
    void OnChangeAlpha( int value );

    ///< Event handlers from QSpinBox
    void OnChangeEditRed( int value );
    void OnChangeEditGreen( int value );
    void OnChangeEditBlue( int value );
    void OnChangeEditHue( int value );
    void OnChangeEditSat( int value );

    ///< mouse handlers, when clicking on a palette bitmap
	void onRGBMouseClick( QMouseEvent* event );
	void onRGBMouseDrag( QMouseEvent* event );
	void onHSVMouseClick( QMouseEvent* event );
	void onHSVMouseDrag( QMouseEvent* event );

    void onSize( QResizeEvent* event );

    void OnColorValueText();

    ///< Event handler for the reset button press
    void OnResetButton();

    /**
     * Manage the Hue and Saturation settings when the mouse cursor is at aMouseCursor.
     *
     * @param aMouseCursor is the mouse cursor position on the HSV bitmap
     * @return true if the Hue and Saturation can be set from aMouseCursor,
     * if Saturation value computed from aMouseCursor is <= 1.0,
     * and false if aMouseCursor is outside this area.
     */
	bool setHSvaluesFromCursor( const QPoint& aMouseCursor );

    ///< Event handler for defined color buttons
    void buttColorClick( QMouseEvent* event );

    ///< Event handler for double click on color buttons
    void colorDClick( QMouseEvent* event );

    ///< called when creating the dialog
    bool TransferDataToWindow() override;

    /**
     * Create the bitmap buttons for each defined colors.
     *
     * If aPredefinedColors is nullptr, a internal predefined list will be used.
     */
    void initDefinedColors( CUSTOM_COLORS_LIST* aPredefinedColors );

    void updateHandleSize();

    // convert double value 0 ... 1 to int 0 ... aValMax
    int normalizeToInt( double aValue, int aValMax = 255 )
    {
        return ( aValue * aValMax ) + 0.5;
    }

};

#endif  // #define DIALOG_COLOR_PICKER_H
