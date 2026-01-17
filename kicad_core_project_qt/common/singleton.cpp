
#include <singleton.h>
#include <bs_thread_pool.hpp>
#include <gal/opengl/gl_context_mgr.h>


KICAD_SINGLETON::~KICAD_SINGLETON()
{
    // This will wait for all threads to finish and then join them to the main thread
    delete m_ThreadPool;

    m_ThreadPool = nullptr;

    // UI component - commented out for minimal build (OpenGL context)
    // m_GLContextManager->DeleteAll();
    // delete m_GLContextManager;
    m_GLContextManager = nullptr;
}


void KICAD_SINGLETON::Init()
{
    int num_threads = std::max( 0, ADVANCED_CFG::GetCfg().m_MaximumThreads );
    m_ThreadPool = new BS::thread_pool( num_threads );
    // UI component - commented out for minimal build (OpenGL context)
    // m_GLContextManager = new GL_CONTEXT_MANAGER();
    m_GLContextManager = nullptr;
}