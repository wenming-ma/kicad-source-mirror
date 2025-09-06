
#ifndef KICAD_PROJECT_LOCAL_SETTINGS_H
#define KICAD_PROJECT_LOCAL_SETTINGS_H

#include <layer_ids.h>
#include <project/board_project_settings.h>
#include <project/sch_project_settings.h>
#include <settings/json_settings.h>
#include <wildcards_and_files_ext.h>
#include <settings/app_settings.h>
#include <vector>
#include <set>

class PROJECT;

struct KICOMMON_API PROJECT_FILE_STATE
{
    std::string fileName;
    bool open;
    struct WINDOW_STATE window;
};


class KICOMMON_API PROJECT_LOCAL_SETTINGS : public JSON_SETTINGS
{
public:
    PROJECT_LOCAL_SETTINGS( PROJECT* aProject, const QString& aFilename );

    virtual ~PROJECT_LOCAL_SETTINGS() {}

    bool MigrateFromLegacy( QSettings* aLegacyConfig ) override;

    bool SaveAs( const QString& aDirectory, const QString& aFile );

    bool SaveToFile( const QString& aDirectory = "", bool aForce = false ) override;

    void SetProject( PROJECT* aProject ) { m_project = aProject; }

    void SaveFileState( const QString& aFileName, const WINDOW_SETTINGS* aWindowCfg, bool aOpen );

    const PROJECT_FILE_STATE* GetFileState( const QString& aFileName );

    void ClearFileState();

    bool ShouldAutoSave() const { return !m_wasMigrated && !m_isFutureFormat; }

protected:
    QString getFileExt() const override
    {
        return QString::fromStdString( FILEEXT::ProjectLocalSettingsFileExtension );
    }

    QString getLegacyFileExt() const override
    {
        return QStringLiteral( "NO_SUCH_FILE_EXTENSION" );
    }

public:


    /// File based state
    std::vector<PROJECT_FILE_STATE> m_files;

    std::vector<std::string>       m_OpenJobSets;


    /// The board layers that are turned on for viewing (@see PCB_LAYER_ID)
    LSET m_VisibleLayers;

    /// The GAL layers (aka items) that are turned on for viewing (@see GAL_LAYER_ID)
    GAL_SET m_VisibleItems;

    /// The current (active) board layer for editing
    PCB_LAYER_ID m_ActiveLayer;

    /// The name of a LAYER_PRESET that is currently activated (or blank if none)
    std::string m_ActiveLayerPreset;

    /// The current contrast mode
    HIGH_CONTRAST_MODE m_ContrastModeDisplay;

    /// The current net color mode
    NET_COLOR_MODE m_NetColorMode;

    /// The state of the net inspector panel
    PANEL_NET_INSPECTOR_SETTINGS m_NetInspectorPanel;

    /// The current setting for whether to automatically adjust track widths to match
    bool m_AutoTrackWidth;

    /// How zones are drawn
    ZONE_DISPLAY_MODE m_ZoneDisplayMode;

    double m_TrackOpacity;     ///< Opacity override for all tracks
    double m_ViaOpacity;       ///< Opacity override for all types of via
    double m_PadOpacity;       ///< Opacity override for SMD pads and PTH
    double m_ZoneOpacity;      ///< Opacity override for filled zones
    double m_ShapeOpacity;     ///< Opacity override for graphic shapes
    double m_ImageOpacity;     ///< Opacity override for user images

    std::vector<std::string> m_HiddenNets;
    std::set<std::string> m_HiddenNetclasses;

    /// State of the selection filter widgets
    PCB_SELECTION_FILTER_OPTIONS m_PcbSelectionFilter;
    SCH_SELECTION_FILTER_OPTIONS m_SchSelectionFilter;

    // Upstream git repo info
    std::string m_GitRepoUsername;
    std::string m_GitRepoType;
    std::string m_GitSSHKey;

private:
    /// A link to the owning project
    PROJECT* m_project;

    bool m_wasMigrated;
};

#endif
