// QT_TRANSFORMATION_COMPLETED

#ifndef RAII_H
#define RAII_H

#include <QWidget>


/*
 * Exception-safe (and 'return' safe) scoped handlers following the "resource allocation is
 * initialization" pattern.
 */


// Exception-safe method for nulling a pointer
class NULLER
{
public:
    NULLER( void*& aPtr ) :
            m_what( aPtr )
    {}

    ~NULLER()
    {
        m_what = nullptr;
    }

private:
    void*&  m_what;
};


// Temporarily un-freeze a window, and then re-freeze on destruction
class WINDOW_THAWER
{
public:
    WINDOW_THAWER( QWidget* aWindow )
    {
        m_window = aWindow;
        m_freezeCount = 0;

        while( !m_window->isEnabled() )
        {
            m_window->setEnabled(true);
            m_freezeCount++;
        }
    }

    ~WINDOW_THAWER()
    {
        while( m_freezeCount > 0 )
        {
            m_window->setEnabled(false);
            m_freezeCount--;
        }
    }

protected:
    QWidget* m_window;
    int      m_freezeCount;
};


/// Temporarily disable a window, and then re-enable on destruction.
class WINDOW_DISABLER
{
public:
    WINDOW_DISABLER( QWidget* aWindow ) :
            m_win( aWindow )
    {
        if( m_win )
            m_win->setEnabled(false);
    }

    ~WINDOW_DISABLER()
    {
        if( m_win )
        {
            m_win->setEnabled(true);
            m_win->raise(); // let's focus back on the parent window
        }
    }

    void SuspendForTrueModal()
    {
        if( m_win )
            m_win->setEnabled(true);
    }

    void ResumeAfterTrueModal()
    {
        if( m_win )
            m_win->setEnabled(false);
    }

private:
    QWidget* m_win;
};


#endif  // RAII_H