
#ifndef SYMBOL_EDITOR_CONTROL_H
#define SYMBOL_EDITOR_CONTROL_H

#include <sch_base_frame.h>
#include <tools/sch_tool_base.h>
#include <QObject>

class SYMBOL_EDIT_FRAME;

/**
 * Handle actions for the various symbol editor and viewers.
 */
class SYMBOL_EDITOR_CONTROL : public QObject, public SCH_TOOL_BASE<SCH_BASE_FRAME>
{
public:
    SYMBOL_EDITOR_CONTROL() :
            SCH_TOOL_BASE<SCH_BASE_FRAME>( "eeschema.SymbolLibraryControl" )
    { }

    /// @copydoc TOOL_INTERACTIVE::Init()
    bool Init() override;

    int AddLibrary( const TOOL_EVENT& aEvent );
    int AddSymbol( const TOOL_EVENT& aEvent );
    int EditSymbol( const TOOL_EVENT& aEvent );
    int EditLibrarySymbol( const TOOL_EVENT& aEvent );

    int Save( const TOOL_EVENT& aEvt );
    int Revert( const TOOL_EVENT& aEvent );

    int CutCopyDelete( const TOOL_EVENT& aEvent );
    int DuplicateSymbol( const TOOL_EVENT& aEvent );
    int RenameSymbol( const TOOL_EVENT& newName );
    int ExportSymbol( const TOOL_EVENT& aEvent );
    int OpenDirectory( const TOOL_EVENT& aEvent );
    int OpenWithTextEditor( const TOOL_EVENT& aEvent );
    int ExportView( const TOOL_EVENT& aEvent );
    int ExportSymbolAsSVG( const TOOL_EVENT& aEvent );
    int AddSymbolToSchematic( const TOOL_EVENT& aEvent );

    int OnDeMorgan( const TOOL_EVENT& aEvent );

    int ShowElectricalTypes( const TOOL_EVENT& aEvent );
    int ShowPinNumbers( const TOOL_EVENT& aEvent );
    int ToggleProperties( const TOOL_EVENT& aEvent );
    int ToggleSyncedPinsMode( const TOOL_EVENT& aEvent );

    int ToggleHiddenPins( const TOOL_EVENT& aEvent );
    int ToggleHiddenFields( const TOOL_EVENT& aEvent );
    int TogglePinAltIcons( const TOOL_EVENT& aEvent );

    int ChangeUnit( const TOOL_EVENT& aEvent );

    int DdAddLibrary( const TOOL_EVENT& aEvent );

private:
    ///< Set up handlers for various events.
    void setTransitions() override;
};


#endif // SYMBOL_EDITOR_CONTROL_H
