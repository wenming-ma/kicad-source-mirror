#include <QtDebug>

#include <core/ignore.h>
#include <kiway.h>
#include <kiway_holder.h>
#include <project.h>

#if defined(DEBUG)
 #include <typeinfo>
#endif


PROJECT& KIWAY_HOLDER::Prj() const
{
    return Kiway().Prj();
}


// this is not speed critical, hide it out of line.
void KIWAY_HOLDER::SetKiway( QWidget* aDest, KIWAY* aKiway )
{
#if defined(DEBUG)
    // offer a trap point for debugging most any window
    Q_ASSERT( aDest );
    if( !strcmp( typeid(aDest).name(), "DIALOG_EDIT_LIBENTRY_FIELDS_IN_LIB" ) )
    {
        int breakhere=1;
        ignore_unused( breakhere );
    }
#endif

    ignore_unused( aDest );

    m_kiway = aKiway;
}