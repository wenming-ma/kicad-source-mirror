#ifndef PANEL_PACKAGES_AND_SETTINGS_H_
#define PANEL_PACKAGES_AND_SETTINGS_H_

#include "panel_packages_and_updates_base.h"

class PANEL_PACKAGES_AND_UPDATES : public PANEL_PACKAGES_AND_UPDATES_BASE
{
public:
    PANEL_PACKAGES_AND_UPDATES( QWidget* parent );

    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;
};

#endif // PANEL_PACKAGES_AND_SETTINGS_H_
