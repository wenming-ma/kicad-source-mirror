
#include <kiface_base.h>
#include <kiway.h>
#include <search_stack.h>
#include <systemdirsappend.h>


bool KIFACE_BASE::start_common( int aCtlBits )
{
    m_start_flags = aCtlBits;
    m_bm.Init();
    GlobalPathsAppend( &m_bm.m_search, m_id );

    return true;
}


void KIFACE_BASE::end_common()
{
    m_bm.End();
}

