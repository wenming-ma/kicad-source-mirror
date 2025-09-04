
#ifndef KIWAY_HOLDER_H_
#define KIWAY_HOLDER_H_

#include <kicommon.h>

class KIWAY;
class PROJECT;
class QWidget;

class KICOMMON_API KIWAY_HOLDER
{
public:
    enum HOLDER_TYPE { DIALOG, FRAME, PANEL };

    KIWAY_HOLDER( KIWAY* aKiway, HOLDER_TYPE aType ) :
        m_kiway( aKiway ),
        m_type( aType )
    {}

    HOLDER_TYPE GetType() const { return m_type; }

    KIWAY& Kiway() const
    {
        Q_ASSERT( m_kiway );    // smoke out bugs in Debug build, then Release runs fine.
        return *m_kiway;
    }

    bool HasKiway() const
    {
        return m_kiway != nullptr;
    }

    PROJECT& Prj() const;

    void SetKiway( QWidget* aDest, KIWAY* aKiway );

private:
    // private, all setting is done through SetKiway().
    KIWAY*          m_kiway;            // no ownership.
    HOLDER_TYPE     m_type;
};


#endif // KIWAY_HOLDER_H_
