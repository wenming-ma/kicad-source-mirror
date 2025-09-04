#ifndef KIPLATFORM_UI_H_
#define KIPLATFORM_UI_H_

#include <QtCore/Qt>

class QComboBox;
class QWidget;

namespace KIPLATFORM
{
    namespace UI
    {
        /**
         * Determine if the desktop interface is currently using a dark theme or a light theme.
         *
         * @return true if a dark theme is being used.
         */
        bool IsDarkTheme();

        QColor GetDialogBGColour();

        /**
         * Pass the current focus to the window. On OSX this will forcefully give the focus to
         * the desired window, while on MSW and GTK it will simply call the Qt setFocus()
         * function.
         *
         * @param aWindow is the window to pass focus to
         */
        void ForceFocus( QWidget* aWindow );

        /**
         * Check to see if the given window is the currently active window (e.g. the window
         * in the foreground the user is interacting with).
         *
         * @param aWindow is the window to check
         */
        bool IsWindowActive( QWidget* aWindow );

        /**
         * Move a window's parent to be the top-level window and force the window to be on top.
         *
         * This only has an affect for OSX, it is a NOP for GTK and MSW.
         *
         * Apple in its infinite wisdom will raise a disabled window before even passing
         * us the event, so we have no way to stop it.  Instead, we must set an order on
         * the windows so that the quasi-modal will be pushed in front of the disabled
         * window when it is raised.
         *
         * @param aWindow is the window to reparent
         */
        void ReparentModal( QWidget* aWindow );

        /*
         * An ugly hack to fix an issue on OSX: cmd+c closes the dialog instead of copying the
         * text if a button with QDialogButtonBox::Cancel is used: the label is &Cancel, and 
         * this accelerator key has priority over the standard copy accelerator.
         * Note: problem also exists in other languages; for instance cmd+a closes dialogs in
         * German because the button is &Abbrechen.
         */
        void FixupCancelButtonCmdKeyCollision( QWidget* aWindow );

        /**
         * Checks if we designated a stock cursor for this OS as "OK" or else we may need to load a custom one
         *
         * @param aCursor is Qt::CursorShape we want to see if its acceptable
         */
        bool IsStockCursorOk( Qt::CursorShape aCursor );

        /**
         * Configure a QComboBox control to support a lot of entries by disabling functionality that makes
         * adding new items become very expensive.
         *
         * @param aChoice is the choice box to modify
         */
        void LargeChoiceBoxHack( QComboBox* aChoice );

        /**
         * Configure a QComboBox control to ellipsize the shown text in the button with the ellipses
         * placed at the end of the string.
         *
         * @param aChoice is the choice box to ellipsize
         */
        void EllipsizeChoiceBox( QComboBox* aChoice );

        /**
         * Tries to determine the pixel scaling factor currently in use for the window.  Under some
         * platforms, proper scale factor detection may fail.
         * @param aWindow pointer to the window to check
         * @return Pixel scale factor in use, defaulting to the Qt method
         */
        double GetPixelScaleFactor( const QWidget* aWindow );

        /**
         * Tries to determine the content scaling factor currently in use for the window.
         * The content scaling factor is typically settable by the user and may differ from the
         * pixel scaling factor.
         */
        double GetContentScaleFactor( const QWidget* aWindow );

        /**
         * Return the background and foreground colors for info bars in the current scheme
         */
        void GetInfoBarColours( QColor& aFGColour, QColor& aBGColour );

        /**
         * Tries to determine the size of the viewport of a scrollable widget (QTreeView, QTableWidget)
         * that won't be obscured by scrollbars.
         * @param aWindow pointer to the scrollable widget to check
         * @return Viewport size that won't be obscured by scrollbars
         */
        QSize GetUnobscuredSize( const QWidget* aWindow );

        /**
         * Used to set overlay/non-overlay scrolling mode in a window.
         * Implemented only on GTK.
         */
        void SetOverlayScrolling( const QWidget* aWindow, bool overlay );

        /**
         * If the user has disabled icons system-wide, we check that here
         */
        bool AllowIconsInMenus();

        /**
         * Returns the mouse position in screen coordinates.
         * If we've just warped the cursor, returns the new coordinates.
         */
        QPoint GetMousePosition();

        /**
         * Move the mouse cursor to a specific position relative to the window
         * @param aWindow Window in which to position to mouse cursor
         * @param aX destination x position
         * @param aY destination y position
         * @return true if the warp was successful
         */
        bool WarpPointer( QWidget* aWindow, int aX, int aY );

        /**
         * Configures the IME mode of a given control handle
         */
        void ImmControl( QWidget* aWindow, bool aEnable );

        /**
         * Asks the IME to cancel
         */
        void ImeNotifyCancelComposition( QWidget* aWindow );

        /**
         * On Wayland, restricts the pointer movement to a rectangle slightly bigger than the given QWidget.
         * This way, the cursor doesn't exit the (bigger) application window and we retain control on it.
         * Required to make the infinite mouse-drag work with fast movement.
         * See https://gitlab.com/kicad/code/kicad/-/issues/7207#note_1562089503
         * @param aWindow Window in which to position to mouse cursor
         * @return true if infinite panning is supported
         */
        bool InfiniteDragPrepareWindow( QWidget* aWindow );

        /**
         * On Wayland, allows the cursor to freely move again after a drag (see `InfiniteDragPrepareWindow`).
         */
        void InfiniteDragReleaseWindow();

        /**
         * Intended to set the floating window level in macOS on a window
         */
        void SetFloatLevel( QWidget* aWindow );
    }
}

#endif // KIPLATFORM_UI_H_
