
#ifndef TOOL_BASE_H
#define TOOL_BASE_H

#include <cassert>
#include <functional>
#include <string>
#include <QtCore/QDebug>

class EDA_ITEM;
class TOOL_EVENT;
class TOOL_MANAGER;
class TOOLS_HOLDER;

namespace KIGFX
{
class VIEW;
class VIEW_CONTROLS;
}

enum TOOL_TYPE
{
    /// Tool that interacts with the user
    INTERACTIVE = 0x01,

    /// Tool that runs in the background without any user intervention
    BATCH       = 0x02
};

/// Unique identifier for tools
typedef int TOOL_ID;

using TOOL_STATE_FUNC = std::function<int(const TOOL_EVENT&)>;


// Base abstract interface for all kinds of tools

class TOOL_BASE
{
public:
    TOOL_BASE( TOOL_TYPE aType, TOOL_ID aId, const std::string& aName = std::string( "" ) ) :
        m_type( aType ),
        m_toolId( aId ),
        m_toolName( aName ),
        m_toolMgr( nullptr ) {};

    virtual ~TOOL_BASE() {};

    /// Determine the reason of reset for a tool.
    enum RESET_REASON
    {
        RUN,                ///< Tool is invoked after being inactive
        MODEL_RELOAD,       ///< Model changes (the sheet for a schematic)
        SUPERMODEL_RELOAD,  ///< For schematics, the entire schematic changed, not just the sheet
        GAL_SWITCH,         ///< Rendering engine changes
        REDRAW,             ///< Full drawing refresh
        SHUTDOWN            ///< Tool is being shut down
    };

    // Init() is called once upon a registration of the tool.
    // Returns true if the initialization went fine, false otherwise.
    virtual bool Init()
    {
        return true;
    }

    // Bring the tool to a known, initial state.
    // If the tool claimed anything from the model or the view, it must release it when reset.
    // aReason contains information about the reason of tool reset.
    virtual void Reset( RESET_REASON aReason ) = 0;

    // Return the type of the tool
    TOOL_TYPE GetType() const
    {
        return m_type;
    }

    // Return the unique identifier of the tool.
    // The identifier is set by an instance of TOOL_MANAGER.
    TOOL_ID GetId() const
    {
        return m_toolId;
    }

    // Return the name of the tool.
    // Tool names are expected to obey the format: application.ToolName (eg. pcbnew.InteractiveSelection).
    const std::string& GetName() const
    {
        return m_toolName;
    }

    // Return the instance of TOOL_MANAGER that takes care of the tool.
    // Returns instance of the TOOL_MANAGER or NULL if there is no associated tool manager.
    TOOL_MANAGER* GetManager() const
    {
        return m_toolMgr;
    }

    //TOOL_SETTINGS& GetAdapter();

    bool IsToolActive() const;

protected:
    friend class TOOL_MANAGER;

    // Set the TOOL_MANAGER the tool will belong to.
    // Called by TOOL_MANAGER::RegisterTool()
    void attachManager( TOOL_MANAGER* aManager );

    // Returns the instance of VIEW object used in the application. It allows tools to draw.
    KIGFX::VIEW* getView() const;

    // Return the instance of VIEW_CONTROLS object used in the application.
    // It allows tools to read & modify user input and its settings (eg. show cursor, enable
    // snapping to grid, etc.).
    KIGFX::VIEW_CONTROLS* getViewControls() const;

    // Return the application window object, casted to requested user type
    template <typename T>
    T* getEditFrame() const
    {
#if !defined( QA_TEST )   // Dynamic casts give the linker a seizure in the test framework
        Q_ASSERT( dynamic_cast<T*>( getToolHolderInternal() ) );
#endif
        return static_cast<T*>( getToolHolderInternal() );
    }

    // Return the model object if it matches the requested type
    template <typename T>
    T* getModel() const
    {
        EDA_ITEM* m = getModelInternal();
#if !defined( QA_TEST )   // Dynamic casts give the linker a seizure in the test framework
        Q_ASSERT( dynamic_cast<T*>( m ) );
#endif
        return static_cast<T*>( m );
    }

private:
    // hide the implementation to avoid spreading half of kicad headers to the tools
    // that may not need them at all!
    EDA_ITEM* getModelInternal() const;
    TOOLS_HOLDER* getToolHolderInternal() const;

protected:
    TOOL_TYPE     m_type;
    TOOL_ID       m_toolId;       ///< Unique id, assigned by a TOOL_MANAGER instance.

    /// Names are expected to obey the format application.ToolName (eg.
    /// pcbnew.InteractiveSelection).
    std::string   m_toolName;
    TOOL_MANAGER* m_toolMgr;
};

#endif  // TOOL_BASE_H
