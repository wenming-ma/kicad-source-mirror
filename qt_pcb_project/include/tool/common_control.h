// QT_TRANSFORMATION_COMPLETED

#ifndef COMMON_CONTROL_H
#define COMMON_CONTROL_H

#include <tool/tool_interactive.h>
#include <QString>
#include <QObject>

class EDA_BASE_FRAME;

class COMMON_CONTROL : public TOOL_INTERACTIVE
{
    Q_OBJECT
public:
    COMMON_CONTROL() :
        TOOL_INTERACTIVE( "common.SuiteControl" ),
        m_frame( nullptr )
    { }

    ~COMMON_CONTROL() override { }

    void Reset( RESET_REASON aReason ) override;

    int OpenPreferences( const TOOL_EVENT& aEvent );
    int ConfigurePaths( const TOOL_EVENT& aEvent );
    int ShowLibraryTable( const TOOL_EVENT& aEvent );

    int ShowPlayer( const TOOL_EVENT& aEvent );
    int ShowProjectManager( const TOOL_EVENT& aEvent );

    int ShowHelp( const TOOL_EVENT& aEvent );
    int About( const TOOL_EVENT& aEvent );
    int ListHotKeys( const TOOL_EVENT& aEvent );
    int GetInvolved( const TOOL_EVENT& aEvent );
    int Donate( const TOOL_EVENT& aEvent );
    int ReportBug( const TOOL_EVENT& aEvent );

    void setTransitions() override;

private:
    EDA_BASE_FRAME* m_frame;

    static QString m_bugReportUrl;
    static QString m_bugReportTemplate;
};

#endif
