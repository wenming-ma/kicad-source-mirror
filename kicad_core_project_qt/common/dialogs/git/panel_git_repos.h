
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef PANEL_GIT_REPOS_H
#define PANEL_GIT_REPOS_H

#include <QtWidgets/QWidget>
#include <git/panel_git_repos_base.h>
#include <widgets/qt_grid.h>

class PANEL_GIT_REPOS : public PANEL_GIT_REPOS_BASE
{
public:
    PANEL_GIT_REPOS( QWidget* parent );
    ~PANEL_GIT_REPOS() override;

    void ResetPanel() override;

    bool TransferDataFromWindow() override;
    bool TransferDataToWindow() override;

    enum COLS
    {
        COL_ACTIVE = 0,
        COL_NAME,
        COL_PATH,
        COL_STATUS,
        COL_AUTH_TYPE,
        COL_USERNAME,
        COL_PASSWORD,
        COL_SSH_KEY,
        COL_SSH_PATH
    };

private:
    void onDefaultClick() override;
    void onEnableGitClick() override;

};

#endif // PANEL_GIT_REPOS_H