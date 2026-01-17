

#ifndef SCH_NAVIGATE_TOOL_H
#define SCH_NAVIGATE_TOOL_H

#include <sch_base_frame.h>
#include <tools/sch_tool_base.h>
#include <status_popup.h>
#include <sch_sheet_path.h>
#include <list>
#include <QObject>

class SCH_EDIT_FRAME;

/**
 * Handle actions specific to the schematic editor.
 */
class SCH_NAVIGATE_TOOL : public QObject, public SCH_TOOL_BASE<SCH_EDIT_FRAME>
{
    Q_OBJECT

public:
    SCH_NAVIGATE_TOOL() : SCH_TOOL_BASE<SCH_EDIT_FRAME>( "eeschema.NavigateTool" ) {}

    ~SCH_NAVIGATE_TOOL() { }

    ///< Reset navigation history. Must be done when schematic changes
    void ResetHistory();
    ///< Remove deleted pages from history. Must be done when schematic
    // hierarchy changes.
    void CleanHistory();

    ///< Enter sheet provided in aEvent
    int ChangeSheet( const TOOL_EVENT& aEvent );
    ///< Enter selected sheet
    int EnterSheet( const TOOL_EVENT& aEvent );
    ///< Return to parent sheet. Synonymous with Up
    int LeaveSheet( const TOOL_EVENT& aEvent );
    ///< Navigate up in sheet hierarchy
    int Up( const TOOL_EVENT& aEvent );
    ///< Navigate forward in sheet history
    int Forward( const TOOL_EVENT& aEvent );
    ///< Navigate back in sheet history
    int Back( const TOOL_EVENT& aEvent );
    ///< Navigate to previous sheet by numeric sheet number
    int Previous( const TOOL_EVENT& aEvent );
    ///< Navigate to next sheet by numeric sheet number
    int Next( const TOOL_EVENT& aEvent );

    void HypertextCommand( const QString& aHref );

    bool CanGoBack();
    bool CanGoForward();
    bool CanGoUp();
    bool CanGoPrevious();
    bool CanGoNext();

public:
    static QString g_BackLink;

private:
    ///< Set up handlers for various events.
    void setTransitions() override;
    ///< Clear history after this nav index and pushes aPath to history
    void pushToHistory( const SCH_SHEET_PATH& aPath );
    ///< Change current sheet to aPath and handle history, zooming, etc.
    void changeSheet( const SCH_SHEET_PATH& aPath );

private:
    std::list<SCH_SHEET_PATH>           m_navHistory;
    std::list<SCH_SHEET_PATH>::iterator m_navIndex;
};


#endif // SCH_NAVIGATE_TOOL_H
