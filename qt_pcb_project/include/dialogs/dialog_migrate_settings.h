
#ifndef _DIALOG_MIGRATE_SETTINGS_H
#define _DIALOG_MIGRATE_SETTINGS_H

#include <QDialog>

class SETTINGS_MANAGER;

class KICOMMON_API DIALOG_MIGRATE_SETTINGS : public QDialog
{
    Q_OBJECT

public:
    DIALOG_MIGRATE_SETTINGS( SETTINGS_MANAGER* aManager );

    ~DIALOG_MIGRATE_SETTINGS();

protected slots:
    void OnPrevVerSelected();
    void OnPathChanged();
    void OnPathDefocused();
    void OnChoosePath();
    void OnDefaultSelected();

private:

    bool validatePath();

    void showPathError( bool aShow = true );

    SETTINGS_MANAGER* m_manager;
};

#endif
