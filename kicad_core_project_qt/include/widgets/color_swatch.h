
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef COLOR_SWATCH__H_
#define COLOR_SWATCH__H_

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QPaintDevice>
#include <QPainter>

#include <functional>

#include <gal/color4d.h>
#include <dialogs/dialog_color_picker.h>

enum SWATCH_SIZE
{
    SWATCH_SMALL,
    SWATCH_MEDIUM,
    SWATCH_LARGE,
    SWATCH_EXPAND
};


const static QSize SWATCH_SIZE_SMALL_DU( 8, 6 );
const static QSize SWATCH_SIZE_MEDIUM_DU( 24, 10 );
const static QSize SWATCH_SIZE_LARGE_DU( 24, 16 );
const static QSize CHECKERBOARD_SIZE_DU( 3, 3 );


/**
 * A simple color swatch of the kind used to set layer colors.
 */
class COLOR_SWATCH: public QWidget
{
public:

    COLOR_SWATCH( QWidget* aParent, const KIGFX::COLOR4D& aColor, int aID,
                  const KIGFX::COLOR4D& aBackground, const KIGFX::COLOR4D& aDefault,
                  SWATCH_SIZE aSwatchType, bool aTriggerWithSingleClick = false );

    COLOR_SWATCH( QWidget *aParent, int aId, const QPoint &aPos = QPoint(),
                  const QSize &aSize = QSize(), long aStyle = 0 );

    /**
     * Set the current swatch color directly.
     */
    void SetSwatchColor( const KIGFX::COLOR4D& aColor, bool aSendEvent );

    /**
     * Sets the color that will be chosen with the "Reset to Default" button in the chooser
     */
    void SetDefaultColor( const KIGFX::COLOR4D& aColor );

    /**
     * Set the swatch background color.
     */
    void SetSwatchBackground( const KIGFX::COLOR4D& aBackground );

    /**
     * Fetch a reference to the user colors list.
     */
    void SetUserColors( CUSTOM_COLORS_LIST* aUserColors ) { m_userColors = aUserColors; }

    /**
     * @return the current swatch color.
     */
    KIGFX::COLOR4D GetSwatchColor() const;

    void SetWindowID( int aId )
    {
        // Qt widgets don't use explicit IDs like wxWidgets
        // Store ID if needed for event handling
        m_id = aId;
    }

    /**
     * Prompt for a new colour, using the colour picker dialog.
     *
     * A colour change event will be sent if it's set.
     */
    void GetNewSwatchColor();

    void SetReadOnly( bool aReadOnly = true ) { m_readOnly = aReadOnly; }
    bool IsReadOnly() const { return m_readOnly; }

    void SetSupportsOpacity( bool aSupportsOpacity ) { m_supportsOpacity = aSupportsOpacity; }

    /// Register a handler for when the user tries to interact with a read-only swatch.
    void SetReadOnlyCallback( std::function<void()> aCallback ) { m_readOnlyCallback = aCallback; }

    /// Respond to a change in the OS's DarkMode setting.
    void OnDarkModeToggle();

    static QPixmap MakeBitmap( const KIGFX::COLOR4D& aColor, const KIGFX::COLOR4D& aBackground,
                               const QSize& aSize, const QSize& aCheckerboardSize,
                               const KIGFX::COLOR4D& aCheckerboardBackground );

    static void RenderToDC( QPainter* aPainter, const KIGFX::COLOR4D& aColor,
                            const KIGFX::COLOR4D& aBackground, const QRect& aRect,
                            const QSize&         aCheckerboardSize,
                            const KIGFX::COLOR4D& aCheckerboardBackground );

private:
    void setupEvents( bool aTriggerWithSingleClick );

    QPixmap makeBitmap();

    void rePostEvent( QEvent& aEvent );

    KIGFX::COLOR4D        m_color;
    KIGFX::COLOR4D        m_background;
    KIGFX::COLOR4D        m_default;
    CUSTOM_COLORS_LIST*   m_userColors;

    QLabel*               m_swatch;

    QSize                 m_size;
    QSize                 m_checkerboardSize;
    KIGFX::COLOR4D        m_checkerboardBg;

    bool                  m_readOnly;
    std::function<void()> m_readOnlyCallback;
    bool                  m_supportsOpacity;
    int                   m_id;
};


// Qt signal equivalent will be defined in the implementation

#endif // COLOR_SWATCH__H_
