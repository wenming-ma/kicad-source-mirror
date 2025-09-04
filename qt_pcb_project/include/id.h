// QT_TRANSFORMATION_COMPLETED

// Common command IDs shared by more than one of the KiCad applications.
// Only place command IDs used in base window class event tables or shared
// across multiple applications such as the zoom, grid, and language IDs.


#ifndef ID_H_
#define ID_H_

#include <QtCore/QObject>

// Define room for IDs, for each sub application
#define ROOM_FOR_KICADMANAGER 50
#define ROOM_FOR_3D_VIEWER 100
#define ROOM_FOR_PANEL_PREV_MODEL 50


// IDs range for menuitems file history:
// The default range file history size is 9 (compatible with default Qt range).
#define DEFAULT_FILE_HISTORY_SIZE 9
#define MAX_FILE_HISTORY_SIZE 99

enum main_id
{
    ID_LOAD_FILE = 10000,  // Qt custom ID range start
    ID_NEW_BOARD,
    ID_SAVE_BOARD,
    ID_SAVE_BOARD_AS,
    ID_AUTO_SAVE_TIMER,

    // ID for menuitems used in our file history management,
    // when we do not use Qt file ID (restricted to 9 items)
    ID_FILE,
    ID_FILE1,
    ID_FILEMAX = ID_FILE + MAX_FILE_HISTORY_SIZE,
    ID_FILE_LIST_EMPTY,
    ID_FILE_LIST_CLEAR,

    ID_PREFERENCES_RESET_PANEL,

    ID_GEN_PLOT,
    ID_GEN_PLOT_PS,
    ID_GEN_PLOT_HPGL,
    ID_GEN_PLOT_GERBER,
    ID_GEN_PLOT_SVG,
    ID_GEN_PLOT_DXF,
    ID_GEN_PLOT_PDF,

    // id for toolbars
    ID_H_TOOLBAR,
    ID_V_TOOLBAR,
    ID_OPT_TOOLBAR,
    ID_AUX_TOOLBAR,

    ID_LANGUAGE_CHOICE,
    ID_LANGUAGE_DANISH,
    ID_LANGUAGE_DEFAULT,
    ID_LANGUAGE_ENGLISH,
    ID_LANGUAGE_FRENCH,
    ID_LANGUAGE_FINNISH,
    ID_LANGUAGE_HEBREW,
    ID_LANGUAGE_SPANISH,
    ID_LANGUAGE_SPANISH_MEXICAN,
    ID_LANGUAGE_GERMAN,
    ID_LANGUAGE_GREEK,
    ID_LANGUAGE_NORWEGIAN,
    ID_LANGUAGE_RUSSIAN,
    ID_LANGUAGE_PORTUGUESE,
    ID_LANGUAGE_PORTUGUESE_BRAZILIAN,
    ID_LANGUAGE_TURKISH,
    ID_LANGUAGE_INDONESIAN,
    ID_LANGUAGE_ITALIAN,
    ID_LANGUAGE_SLOVENIAN,
    ID_LANGUAGE_SLOVAK,
    ID_LANGUAGE_HUNGARIAN,
    ID_LANGUAGE_POLISH,
    ID_LANGUAGE_CZECH,
    ID_LANGUAGE_KOREAN,
    ID_LANGUAGE_CATALAN,
    ID_LANGUAGE_CHINESE_SIMPLIFIED,
    ID_LANGUAGE_CHINESE_TRADITIONAL,
    ID_LANGUAGE_DUTCH,
    ID_LANGUAGE_JAPANESE,
    ID_LANGUAGE_BULGARIAN,
    ID_LANGUAGE_LATVIAN,
    ID_LANGUAGE_LITHUANIAN,
    ID_LANGUAGE_VIETNAMESE,
    ID_LANGUAGE_SERBIAN,
    ID_LANGUAGE_THAI,
    ID_LANGUAGE_SWEDISH,
    ID_LANGUAGE_UKRANIAN,
    ID_LANGUAGE_CHOICE_END,

    ID_ON_ZOOM_SELECT,

    ID_ON_GRID_SELECT,

    // Popup Menu (mouse Right button) (id consecutifs)

    /* Reserve IDs for popup menu zoom levels.  If you need more
     * levels of zoom, change ID_POPUP_ZOOM_LEVEL_END.  Note that more
     * than 15 entries in a context submenu may get too large to display
     * cleanly.  Add any additional popup zoom IDs above here or the
     * zoom event handler will not work properly.
     */
    ID_POPUP_ZOOM_LEVEL_START,
    ID_POPUP_ZOOM_LEVEL_END = ID_POPUP_ZOOM_LEVEL_START + 99,

    ID_POPUP_GRID_START,
    ID_POPUP_GRID_END = ID_POPUP_ZOOM_LEVEL_START + 99,

    // KiFace server for standalone operation
    ID_EDA_SOCKET_EVENT_SERV,
    ID_EDA_SOCKET_EVENT,

    // IDs specifics to a sub-application (Eeschema, Kicad manager....) start here
    //
    // We reserve here Ids for each sub-application, to avoid duplicate IDs
    // between them.
    // mainly we experienced issues related to Qt update events when 2 (or more) QFrames
    // share the same ID in menus, mainly in menubars/toolbars
    // The reason is the fact Qt propagates the update events to all parent windows
    // to find update event functions matching the menuitem IDs found when activate a
    // menu in the first frame.

    // Reserve ROOM_FOR_KICADMANAGER IDs, for Kicad manager
    // Change it if this count is too small.
    ID_KICAD_MANAGER_START,
    ID_KICAD_MANAGER_END = ID_KICAD_MANAGER_START + ROOM_FOR_KICADMANAGER,

    // Reserve ROOM_FOR_KICADMANAGER IDs, for Kicad manager
    // Change it if this count is too small.
    ID_KICAD_3D_VIEWER_START,
    ID_KICAD_3D_VIEWER_END = ID_KICAD_3D_VIEWER_START + ROOM_FOR_3D_VIEWER,

    ID_KICAD_PANEL_PREV_MODEL_START,
    ID_KICAD_PANEL_PREV_MODEL_END = ID_KICAD_PANEL_PREV_MODEL_START + ROOM_FOR_PANEL_PREV_MODEL,

    // Reserve ID for popup menus, when we need to know a menu item is inside a popup menu
    ID_POPUP_MENU_START,

    // The extra here need to minimum be larger than MAX_BUS_UNFOLD_MENU_ITEMS +
    // MAX_UNIT_COUNT_PER_PACKAGE.
    // These values are stored in eeschema_id.h
    ID_POPUP_MENU_END = ID_POPUP_MENU_START + 2048,

    ID_END_LIST
};

#endif  // ID_H_
