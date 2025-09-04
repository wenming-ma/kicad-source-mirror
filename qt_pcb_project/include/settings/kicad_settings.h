// QT_TRANSFORMATION_COMPLETED

#ifndef _KICAD_SETTINGS_H
#define _KICAD_SETTINGS_H

#include <settings/app_settings.h>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QVector>
#define PCM_DEFAULT_REPOSITORY_URL "https://repository.kicad.org/repository.json"


class KICOMMON_API KICAD_SETTINGS : public APP_SETTINGS_BASE
{
public:
    KICAD_SETTINGS();

    virtual ~KICAD_SETTINGS() {}

    virtual bool MigrateFromLegacy( void* aLegacyConfig ) override;

    int m_LeftWinWidth;


    QVector<QString> m_OpenProjects;

    QString m_lastDesignBlockLibDir;

    QVector<std::pair<QString, QString>> m_PcmRepositories;
    QString                               m_PcmLastDownloadDir;

    // This controls background update check for PCM.
    // It is set according to m_updateCheck on first start.
    bool m_PcmUpdateCheck;
    // Auto add libs to global table
    bool m_PcmLibAutoAdd;
    // Auto remove libs
    bool m_PcmLibAutoRemove;
    // Generated library nickname prefix
    QString m_PcmLibPrefix;

    bool     m_KiCadUpdateCheck;
    QString m_lastUpdateCheckTime;
    QString m_lastReceivedUpdate;

    // Last position of the template window
    QPoint m_TemplateWindowPos;
    // Last size of the template window
    QSize m_TemplateWindowSize;

protected:
    virtual std::string getLegacyFrameName() const override { return "KicadFrame"; }
};

#endif
