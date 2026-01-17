
#ifndef __WX_VIEW_CONTROLS_H
#define __WX_VIEW_CONTROLS_H

#include <view/view_controls.h>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QScrollArea>
#include <QWheelEvent>
#include <QGestureEvent>
#include <QPoint>
#include <QObject>
#include <memory>

class EDA_DRAW_PANEL_GAL;
class PROF_COUNTER;

namespace KIGFX
{

class ZOOM_CONTROLLER;

class QT_VIEW_CONTROLS : public VIEW_CONTROLS, public QObject
{
public:
    QT_VIEW_CONTROLS( VIEW* aView, EDA_DRAW_PANEL_GAL* aParentPanel );
    virtual ~QT_VIEW_CONTROLS();

    // Handler functions
    void onWheel( QWheelEvent* aEvent );
    void onMotion( QMouseEvent* aEvent );
    void onMagnify( QMouseEvent* aEvent );
    void onButton( QMouseEvent* aEvent );
    void onEnter( QMouseEvent* aEvent );
    void onLeave( QMouseEvent* aEvent );
    void onTimer();
    void onZoomGesture( QGestureEvent* aEvent );
    void onPanGesture( QGestureEvent* aEvent );
    void onScroll( QWheelEvent* aEvent );
    void onCaptureLost( QMouseEvent* aEvent );


    // Force the cursor to stay within the drawing panel area.
    void CaptureCursor( bool aEnabled ) override;


    void PinCursorInsideNonAutoscrollArea( bool aWarpMouseCursor ) override;

    VECTOR2D GetMousePosition( bool aWorldCoordinates = true ) const override;

    using VIEW_CONTROLS::GetCursorPosition;

    VECTOR2D GetCursorPosition( bool aSnappingEnabled ) const override;

    VECTOR2D GetRawCursorPosition( bool aSnappingEnabled = true ) const override;

    void SetCursorPosition( const VECTOR2D& aPosition, bool warpView,
                            bool aTriggeredByArrows, long aArrowCommand ) override;

    void SetCrossHairCursorPosition( const VECTOR2D& aPosition, bool aWarpView ) override;

    void WarpMouseCursor( const VECTOR2D& aPosition, bool aWorldCoordinates = false,
                          bool aWarpView = false ) override;

    void CenterOnCursor() override;

    // Adjusts the scrollbars position to match the current viewport.
    void UpdateScrollbars();

    // End any mouse drag action still in progress.
    void CancelDrag();

    void ForceCursorPosition( bool aEnabled,
                              const VECTOR2D& aPosition = VECTOR2D( 0, 0 ) ) override;

    // Applies VIEW_CONTROLS settings from the program COMMON_SETTINGS.
    void LoadSettings() override;

    // Event that forces mouse move event in the dispatcher (eg. used in autopanning, when
    // mouse cursor does not move in screen coordinates, but does in world coordinates)
    static const int EVT_REFRESH_MOUSE;

    std::unique_ptr<PROF_COUNTER> m_MotionEventCounter;

private:
    // Possible states for QT_VIEW_CONTROLS.
    enum STATE
    {
        IDLE = 1,           // Nothing is happening.
        DRAG_PANNING,       // Panning with mouse button pressed.
        AUTO_PANNING,       // Panning on approaching borders of the frame.
        DRAG_ZOOMING,       // Zooming with mouse button pressed.
    };

    // Set the interaction state, simply a internal setter to make it easier to debug changes.
    void setState( STATE aNewState );

    // Compute new viewport settings while in autopanning mode.
    bool handleAutoPanning( const QMouseEvent* aEvent );

    // Limit the cursor position to within the canvas by warping it
    void handleCursorCapture( int x, int y );

    // Send an event to refresh mouse position.
    // It is mostly used for notifying the tools that the cursor position in the world
    // coordinates has changed, whereas the screen coordinates remained the same (e.g.
    // frame edge autopanning).
    void refreshMouse( bool aSetModifiers);

    // Get the cursor position in the screen coordinates.
    QPoint getMouseScreenPosition() const;

    // Current state of VIEW_CONTROLS.
    STATE       m_state;

    // Panel that is affected by VIEW_CONTROLS.
    EDA_DRAW_PANEL_GAL* m_parentPanel;

    // Store information about point where dragging has started.
    VECTOR2D    m_dragStartPoint;

    // Current direction of panning (only autopanning mode).
    VECTOR2D    m_panDirection;

    // Timer responsible for handling autopanning.
    QTimer*     m_panTimer;

    // Ratio used for scaling world coordinates to scrollbar position.
    VECTOR2D    m_scrollScale;

    // Current scrollbar position.
    VECTOR2I    m_scrollPos;

    // The mouse position when a drag zoom started.
    VECTOR2D      m_zoomStartPoint;

    // Current cursor position (world coordinates).
    VECTOR2D    m_cursorPos;

    // Flag deciding whether the cursor position should be calculated using the mouse position.
    bool        m_updateCursor;

    // Flag to indicate if infinite panning works on this platform.
    bool m_infinitePanWorks;

    // A ZOOM_CONTROLLER that determines zoom steps. This is platform-specific.
    std::unique_ptr<ZOOM_CONTROLLER> m_zoomController;

    // Used to track gesture events.
    double   m_gestureLastZoomFactor;
    VECTOR2D m_gestureLastPos;
};
} // namespace KIGFX

#endif
