
#include <advanced_config.h>
#include <pgm_base.h>
#include <thread_pool.h>

static thread_pool* tp = nullptr;

thread_pool& GetKiCadThreadPool()
{
    if( tp )
        return *tp;

    // If we have a PGM_BASE, use its thread pool
    if( PGM_BASE* pgm = PgmOrNull() )
    {
        tp = &pgm->GetThreadPool();
        return *tp;
    }

    // Otherwise, we are running in scripting or some other context where we don't have a PGM_BASE
    // so we need to create our own thread pool
    int num_threads = std::max( 0, ADVANCED_CFG::GetCfg().m_MaximumThreads );
    tp = new thread_pool( num_threads );

    return *tp;
}
