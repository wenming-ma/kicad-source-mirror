// QT_TRANSFORMATION_COMPLETED

#ifndef COLOR_SWATCH__H_
#define COLOR_SWATCH__H_

#include <QWidget>
#include <QLabel>
#include <QSize>
#include <QPixmap>
#include <QPaintEvent>

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


class COLOR_SWATCH: public QWidget
{
    Q_OBJECT

public:

    COLOR_SWATCH( QWidget* aParent, const KIGFX::COLOR4D& aColor, int aID,
                  const KIGFX::COLOR4D& aBackground, const KIGFX::COLOR4D& aDefault,
                  SWATCH_SIZE aSwatchType, bool aTriggerWithSingleClick = false );

    COLOR_SWATCH( QWidget *aParent, int aId, const QPoint &aPos = QPoint(),
                  const QSize &aSize = QSize(), long aStyle = 0 );

    void SetSwatchColor( const KIGFX::COLOR4D& aColor, bool aSendEvent );

    void SetDefaultColor( const KIGFX::COLOR4D& aColor );

    void SetSwatchBackground( const KIGFX::COLOR4D& aBackground );

    void SetUserColors( CUSTOM_COLORS_LIST* aUserColors ) { m_userColors = aUserColors; }

    KIGFX::COLOR4D GetSwatchColor() const;

    void SetWindowID( int aId )
    {
        setProperty("id", aId);
        m_swatch->setProperty("id", aId);
    }

    void GetNewSwatchColor();

    void SetReadOnly( bool aReadOnly = true ) { m_readOnly = aReadOnly; }
    bool IsReadOnly() const { return m_readOnly; }

    void SetSupportsOpacity( bool aSupportsOpacity ) { m_supportsOpacity = aSupportsOpacity; }

    void SetReadOnlyCallback( std::function<void()> aCallback ) { m_readOnlyCallback = aCallback; }

    void OnDarkModeToggle();

    static QPixmap MakeBitmap( const KIGFX::COLOR4D& aColor, const KIGFX::COLOR4D& aBackground,
                               const QSize& aSize, const QSize& aCheckerboardSize,
                               const KIGFX::COLOR4D& aCheckerboardBackground );

    static void RenderToPainter( QPainter* aPainter, const KIGFX::COLOR4D& aColor,
                                 const KIGFX::COLOR4D& aBackground, const QRect& aRect,
                                 const QSize&         aCheckerboardSize,
                                 const KIGFX::COLOR4D& aCheckerboardBackground );

signals:
    void colorChanged();

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
};



#endif // COLOR_SWATCH__H_
