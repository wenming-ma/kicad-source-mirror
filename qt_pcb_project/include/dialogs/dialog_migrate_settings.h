
#ifndef _DIALOG_MIGRATE_SETTINGS_H
#define _DIALOG_MIGRATE_SETTINGS_H

#include "dialogs/dialog_migrate_settings_base.h"

class SETTINGS_MANAGER;

class KICOMMON_API DIALOG_MIGRATE_SETTINGS : public DIALOG_MIGRATE_SETTINGS_BASE
{
    Q_OBJECT

public:
    DIALOG_MIGRATE_SETTINGS( SETTINGS_MANAGER* aManager );

    ~DIALOG_MIGRATE_SETTINGS();
    
    bool TransferDataToWindow();
    bool TransferDataFromWindow();

protected slots:
    void OnPrevVerSelected() override;
    void OnPathChanged() override;
    void OnPathDefocused() override;
    void OnChoosePath() override;
    void OnDefaultSelected() override;

private:

    bool validatePath();

    void showPathError( bool aShow = true );

    SETTINGS_MANAGER* m_manager;
};

#endif
