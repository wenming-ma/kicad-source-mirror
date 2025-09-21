
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef PANEL_DATA_COLLECTION_H
#define PANEL_DATA_COLLECTION_H

#include "panel_data_collection_base.h"


class COMMON_SETTINGS;
class PAGED_DIALOG;


class PANEL_DATA_COLLECTION : public PANEL_DATA_COLLECTION_BASE
{
public:
    PANEL_DATA_COLLECTION( QWidget* aParent );

    void ResetPanel() override;

protected:
    bool TransferDataFromWindow() override;
    bool TransferDataToWindow() override;

    virtual void OnResetIdClick( QEvent& aEvent ) override;

private:
    void applySettingsToPanel();
};

#endif //PANEL_DATA_COLLECTION_H
