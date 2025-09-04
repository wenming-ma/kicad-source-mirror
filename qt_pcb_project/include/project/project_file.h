#ifndef KICAD_PROJECT_FILE_H
#define KICAD_PROJECT_FILE_H

#include <QString>
#include <QStringList>
#include <QSettings>
#include <common.h> // needed for QString hash template
#include <kiid.h>
#include <project/board_project_settings.h>
#include <settings/json_settings.h>
#include <settings/nested_settings.h>

class BOARD_DESIGN_SETTINGS;
class ERC_SETTINGS;
class NET_SETTINGS;
class LAYER_PAIR_SETTINGS;
class SCHEMATIC_SETTINGS;
class TEMPLATES;

typedef std::pair<KIID, QString> FILE_INFO_PAIR;

enum LAST_PATH_TYPE : unsigned int
{
    LAST_PATH_FIRST = 0,
    LAST_PATH_NETLIST = LAST_PATH_FIRST,
    LAST_PATH_STEP,
    LAST_PATH_IDF,
    LAST_PATH_VRML,
    LAST_PATH_SPECCTRADSN,
    LAST_PATH_GENCAD,
    LAST_PATH_POS_FILES,
    LAST_PATH_SVG,
    LAST_PATH_PLOT,
    LAST_PATH_2581,
    LAST_PATH_ODBPP,

    LAST_PATH_SIZE
};

class KICOMMON_API PROJECT_FILE : public JSON_SETTINGS
{
public:
    PROJECT_FILE( const QString& aFullPath );

    virtual ~PROJECT_FILE() = default;

    virtual bool MigrateFromLegacy( QSettings* aCfg ) override;

    bool SaveToFile( const QString& aDirectory = "", bool aForce = false ) override;

    bool SaveAs( const QString& aDirectory, const QString& aFile );

    void SetProject( PROJECT* aProject )
    {
        m_project = aProject;
    }

    std::vector<FILE_INFO_PAIR>& GetSheets()
    {
        return m_sheets;
    }

    std::vector<FILE_INFO_PAIR>& GetBoards()
    {
        return m_boards;
    }

    std::shared_ptr<NET_SETTINGS>& NetSettings()
    {
        return m_NetSettings;
    }

    bool ShouldAutoSave() const { return !m_wasMigrated && !m_isFutureFormat; }

protected:
    QString getFileExt() const override;

    QString getLegacyFileExt() const override;

public:
    // The list of pinned symbol libraries
    std::vector<QString> m_PinnedSymbolLibs;

    // The list of pinned footprint libraries
    std::vector<QString> m_PinnedFootprintLibs;

    // The list of pinned design block libraries
    std::vector<QString> m_PinnedDesignBlockLibs;

    std::map<QString, QString> m_TextVars;

    // Schematic ERC settings: lifecycle managed by SCHEMATIC
    ERC_SETTINGS* m_ErcSettings;

    // Schematic editing and misc settings: lifecycle managed by SCHEMATIC
    SCHEMATIC_SETTINGS* m_SchematicSettings;

    // Legacy parameters LibDir and LibName, for importing old projects
    QString m_LegacyLibDir;

    QStringList m_LegacyLibNames;

    // List of equivalence (equ) files used in the project
    std::vector<QString> m_EquivalenceFiles;

    // Drawing sheet file
    QString m_BoardDrawingSheetFile;

    // MRU path storage
    QString m_PcbLastPath[LAST_PATH_SIZE];

    // Board design settings for this project's board. Owned by the BOARD; may be null if a board isn't loaded: be careful
    BOARD_DESIGN_SETTINGS* m_BoardSettings;

    // Net settings for this project (owned here)
    std::shared_ptr<NET_SETTINGS> m_NetSettings;


    std::vector<LAYER_PRESET>     m_LayerPresets;   // List of stored layer presets
    std::vector<VIEWPORT>         m_Viewports;      // List of stored viewports (pos + zoom)
    std::vector<VIEWPORT3D>       m_Viewports3D;    // List of stored 3D viewports (view matrixes)
    std::vector<LAYER_PAIR_INFO>  m_LayerPairInfos; // Layer pair list for the board

    struct IP2581_BOM             m_IP2581Bom;      // IPC-2581 BOM settings

private:
    bool migrateSchema1To2();
    bool migrateSchema2To3();

    // An list of schematic sheets in this project
    std::vector<FILE_INFO_PAIR> m_sheets;

    // A list of board files in this project
    std::vector<FILE_INFO_PAIR> m_boards;

    // A link to the owning PROJECT
    PROJECT* m_project;

    bool m_wasMigrated;
};

void to_json( nlohmann::json& aJson, const FILE_INFO_PAIR& aPair );

void from_json( const nlohmann::json& aJson, FILE_INFO_PAIR& aPair );

#endif
