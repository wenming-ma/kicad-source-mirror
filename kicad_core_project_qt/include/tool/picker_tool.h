#ifndef PICKER_TOOL_H
#define PICKER_TOOL_H

#include <optional>
#include <gal/cursors.h>
#include <math/vector2d.h>
#include <tool/tool_interactive.h>

class EDA_DRAW_FRAME;


class PICKER_TOOL_BASE
{
public:
    typedef std::function<bool(const VECTOR2D&)> CLICK_HANDLER;
    typedef std::function<void(const VECTOR2D&)> MOTION_HANDLER;
    typedef std::function<void(void)> CANCEL_HANDLER;
    typedef std::function<void(const int&)> FINALIZE_HANDLER;

    enum pickerEndState
    {
        WAIT_CANCEL,
        CLICK_CANCEL,
        END_ACTIVATE,
        EVT_CANCEL,
        EXCEPTION_CANCEL
    };

    PICKER_TOOL_BASE() :
            m_frame( nullptr ),
            m_snap( false ),
            m_modifiers( 0 )
    {
        reset();
    }

    virtual ~PICKER_TOOL_BASE() = default;

    inline void SetCursor( KICURSOR aCursor ) { m_cursor = aCursor; }

    inline void SetSnapping( bool aSnap ) { m_snap = aSnap; }

    void ClearHandlers()
    {
        m_clickHandler.reset();
        m_motionHandler.reset();
        m_cancelHandler.reset();
        m_finalizeHandler.reset();
    }

    inline void SetClickHandler( CLICK_HANDLER aHandler )
    {
        Q_ASSERT( !m_clickHandler );
        m_clickHandler = aHandler;
    }

    inline void SetMotionHandler( MOTION_HANDLER aHandler )
    {
        Q_ASSERT( !m_motionHandler );
        m_motionHandler = aHandler;
    }

    inline void SetCancelHandler( CANCEL_HANDLER aHandler )
    {
        Q_ASSERT( !m_cancelHandler );
        m_cancelHandler = aHandler;
    }

    inline void SetFinalizeHandler( FINALIZE_HANDLER aHandler )
    {
        Q_ASSERT( !m_finalizeHandler );
        m_finalizeHandler = aHandler;
    }

    int CurrentModifiers() const { return m_modifiers; }

protected:
    virtual void reset();

    EDA_DRAW_FRAME* m_frame;
    KICURSOR        m_cursor;
    bool            m_snap;
    int             m_modifiers;

    std::optional<CLICK_HANDLER>    m_clickHandler;
    std::optional<MOTION_HANDLER>   m_motionHandler;
    std::optional<CANCEL_HANDLER>   m_cancelHandler;
    std::optional<FINALIZE_HANDLER> m_finalizeHandler;

    std::optional<VECTOR2D>         m_picked;
};


class PICKER_TOOL : public TOOL_INTERACTIVE, public PICKER_TOOL_BASE
{
public:
    PICKER_TOOL();

    PICKER_TOOL( const std::string& aName );

    virtual ~PICKER_TOOL() = default;

    bool Init() override;

    void Reset( RESET_REASON aReason ) override { }

    int Main( const TOOL_EVENT& aEvent );

protected:
    void setControls();

    void setTransitions() override;
};

#endif /* PICKER_TOOL_H */
