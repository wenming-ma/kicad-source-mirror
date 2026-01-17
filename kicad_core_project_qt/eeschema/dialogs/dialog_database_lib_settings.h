
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#ifndef KICAD_DIALOG_DATABASE_LIB_SETTINGS_H
#define KICAD_DIALOG_DATABASE_LIB_SETTINGS_H

#include "dialog_database_lib_settings_base.h"

class SCH_IO_DATABASE;

class DIALOG_DATABASE_LIB_SETTINGS : public DIALOG_DATABASE_LIB_SETTINGS_BASE
{
public:
    DIALOG_DATABASE_LIB_SETTINGS( QWidget* aParent, SCH_IO_DATABASE* aPlugin );

    virtual ~DIALOG_DATABASE_LIB_SETTINGS() {}

    bool TransferDataFromWindow() override;
    bool TransferDataToWindow() override;

protected:
    void OnDSNSelected() override;
    void OnConnectionStringSelected() override;
    void OnBtnTest() override;
    void OnBtnReloadConfig() override;

private:
    bool hasPotentiallyValidConfig();

    SCH_IO_DATABASE* m_plugin;
};

#endif //KICAD_DIALOG_DATABASE_LIB_SETTINGS_H
