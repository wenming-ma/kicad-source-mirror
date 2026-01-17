
#ifndef _DIALOG_MIGRATE_SETTINGS_H
#define _DIALOG_MIGRATE_SETTINGS_H

// #include "dialog_migrate_settings_base.h"


class SETTINGS_MANAGER;


class KICOMMON_API DIALOG_MIGRATE_SETTINGS // : public DIALOG_MIGRATE_SETTINGS_BASE
{
public:
    DIALOG_MIGRATE_SETTINGS( SETTINGS_MANAGER* aManager );

    ~DIALOG_MIGRATE_SETTINGS();

    // bool TransferDataToWindow() override;

    // bool TransferDataFromWindow() override;

protected:

    // void OnPrevVerSelected();

    // void OnPathChanged();

    // void OnPathDefocused();

    // void OnChoosePath();

    // void OnDefaultSelected();

private:

    bool validatePath();

    void showPathError( bool aShow = true );

    SETTINGS_MANAGER* m_manager;
};

#endif
