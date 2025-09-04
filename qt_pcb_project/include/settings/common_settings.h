// QT_TRANSFORMATION_COMPLETED

#ifndef _COMMON_SETTINGS_H
#define _COMMON_SETTINGS_H

#include <settings/environment.h>
#include <settings/json_settings.h>
#include <QString>
#include <vector>


enum class MOUSE_DRAG_ACTION
{
    // WARNING: these are encoded as integers in the file, so don't change their values.
    DRAG_ANY = -2,
    DRAG_SELECTED,
    SELECT,
    ZOOM,
    PAN,
    NONE
};

enum class ICON_THEME
{
    LIGHT,
    DARK,
    AUTO
};


class KICOMMON_API COMMON_SETTINGS : public JSON_SETTINGS
{
public:
    struct APPEARANCE
    {
        bool       show_scrollbars;
        double     canvas_scale;
        ICON_THEME icon_theme;
        bool       use_icons_in_menus;
        bool       apply_icon_scale_to_fonts;
        double     hicontrast_dimming_factor;
        int        text_editor_zoom;
        int        toolbar_icon_size;
        bool       grid_striping;
    };

    struct AUTO_BACKUP
    {
        bool   enabled;
        bool   backup_on_autosave;
        int    limit_total_files;
        int    limit_daily_files;
        int    min_interval;
        unsigned long long limit_total_size;
    };

    struct ENVIRONMENT
    {
        ENV_VAR_MAP vars;
    };

    struct INPUT
    {
        bool focus_follow_sch_pcb;
        bool auto_pan;
        int  auto_pan_acceleration;
        bool center_on_zoom;
        bool immediate_actions;
        bool warp_mouse_on_move;
        bool horizontal_pan;
        bool hotkey_feedback;

        bool zoom_acceleration;
        int  zoom_speed;
        bool zoom_speed_auto;

        int scroll_modifier_zoom;
        int scroll_modifier_pan_h;
        int scroll_modifier_pan_v;

        MOUSE_DRAG_ACTION drag_left;
        MOUSE_DRAG_ACTION drag_middle;
        MOUSE_DRAG_ACTION drag_right;

        bool reverse_scroll_zoom;
        bool reverse_scroll_pan_h;
    };

    struct GRAPHICS
    {
        int cairo_aa_mode;
        int opengl_aa_mode;
    };

    struct SESSION
    {
        bool remember_open_files;
        std::vector<QString> pinned_symbol_libs;
        std::vector<QString> pinned_fp_libs;
        std::vector<QString> pinned_design_block_libs;
    };

    struct SYSTEM
    {
        int autosave_interval;
        QString text_editor;
        QString file_explorer;
        int file_history_size;
        QString language;
        QString pdf_viewer_name;
        bool use_system_pdf_viewer;
        QString working_dir;
        int clear_3d_cache_interval;
    };

    struct DO_NOT_SHOW_AGAIN
    {
        bool zone_fill_warning;
        bool env_var_overwrite_warning;
        bool scaled_3d_models_warning;
        bool data_collection_prompt;
        bool update_check_prompt;
    };

    struct NETCLASS_PANEL
    {
        int sash_pos;
        QString eeschema_visible_columns;
        QString pcbnew_visible_columns;
    };

    struct PACKAGE_MANAGER
    {
        int sash_pos;
    };

    struct GIT_REPOSITORY
    {
        QString name;
        QString path;
        QString authType;
        QString username;
        QString ssh_path;
        bool     active;
        bool     checkValid;
    };

    struct GIT
    {
        std::vector<GIT_REPOSITORY> repositories;
        bool                    enableGit;
        int                     updatInterval;
        bool                    useDefaultAuthor;
        QString                 authorName;
        QString                 authorEmail;
    };

    struct API
    {
        QString python_interpreter;
        bool enable_server;
    };

    COMMON_SETTINGS();

    virtual ~COMMON_SETTINGS() {}

    virtual bool MigrateFromLegacy( void* aLegacyConfig ) override;

    void InitializeEnvironment();

private:
    bool migrateSchema0to1();
    bool migrateSchema1to2();
    bool migrateSchema2to3();

    struct LEGACY_3D_SEARCH_PATH
    {
        QString m_Alias;       // alias to the base path
        QString m_Pathvar;     // base path as stored in the config file
        QString m_Pathexp;     // expanded base path
        QString m_Description; // description of the aliased path
    };

    static bool getLegacy3DHollerith( const std::string& aString, size_t& aIndex,
                                      QString& aResult );
    bool readLegacy3DResolverCfg( const QString& aPath,
                                  std::vector<LEGACY_3D_SEARCH_PATH>& aSearchPaths );

public:
    APPEARANCE m_Appearance;

    AUTO_BACKUP m_Backup;

    ENVIRONMENT m_Env;

    INPUT m_Input;

    GRAPHICS m_Graphics;

    SESSION m_Session;

    SYSTEM m_System;

    DO_NOT_SHOW_AGAIN m_DoNotShowAgain;

    NETCLASS_PANEL m_NetclassPanel;

    PACKAGE_MANAGER m_PackageManager;

    GIT m_Git;

    API m_Api;
};

#endif
