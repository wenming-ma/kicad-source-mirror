#ifndef GL_CONTEXT_MANAGER_H
#define GL_CONTEXT_MANAGER_H

#include <kicommon.h>
#include <gal/gal.h>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtGui/QOpenGLContext>
#include <mutex>
#include <map>

class KICOMMON_API GL_CONTEXT_MANAGER
{
public:

    GL_CONTEXT_MANAGER() : m_glCtx( nullptr ) {}

    QOpenGLContext* CreateCtx( QOpenGLWidget* aCanvas, const QOpenGLContext* aOther = nullptr );

    void DestroyCtx( QOpenGLContext* aContext );

    void DeleteAll();

    void LockCtx( QOpenGLContext* aContext, QOpenGLWidget* aCanvas );

    void UnlockCtx( QOpenGLContext* aContext );

    QOpenGLContext* GetCurrentCtx() const
    {
        return m_glCtx;
    }

    QOpenGLWidget* GetCurrentCanvas() const
    {
        auto it = m_glContexts.find( m_glCtx );
        return it != m_glContexts.end() ? it->second : nullptr;
    }

    template<typename Func, typename... Args>
    auto RunWithoutCtxLock( Func&& aFunction, Args&&... args )
    {
        QOpenGLContext* currentCtx = GetCurrentCtx();
        QOpenGLWidget* currentCanvas = GetCurrentCanvas();
        UnlockCtx( currentCtx );

        if constexpr (std::is_void_v<decltype(aFunction(std::forward<Args>(args)...))>)
        {
            std::forward<Func>(aFunction)(std::forward<Args>(args)...);
            LockCtx( currentCtx, currentCanvas );
            return;
        }
        else
        {
            auto result = std::forward<Func>(aFunction)(std::forward<Args>(args)...);
            LockCtx( currentCtx, currentCanvas );
            return result;
        }
    }

private:
    std::map<QOpenGLContext*, QOpenGLWidget*> m_glContexts;

    QOpenGLContext* m_glCtx;

    std::mutex m_glCtxMutex;
};

#endif /* GL_CONTEXT_MANAGER_H */