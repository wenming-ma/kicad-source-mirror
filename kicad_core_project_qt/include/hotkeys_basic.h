// QT_TRANSFORMATION_COMPLETED

#ifndef  HOTKEYS_BASIC_H
#define  HOTKEYS_BASIC_H

#include <QString>
#include <map>
#include <vector>

#define EESCHEMA_HOTKEY_NAME QStringLiteral( "Eeschema" )
#define PCBNEW_HOTKEY_NAME QStringLiteral( "PcbNew" )

// A define to allow translation of Hot Key message Info in hotkey help menu
// We do not want to use the _( x ) usual macro from Qt, which calls tr(),
// because the English string is used in key file configuration
// The translated string is used only when displaying the help window.
// Therefore translation tools have to use the "_" and the "_HKI" prefix to extract
// strings to translate
#include <i18n_utility.h>       // _HKI definition

class TOOL_ACTION;
class TOOL_MANAGER;
class EDA_BASE_FRAME;


/*
 * Keep these out of the ASCII range, and out of the Qt Key range
 */
#define PSEUDO_QT_CLICK    400
#define PSEUDO_QT_DBLCLICK 401
#define PSEUDO_QT_WHEEL    402

int KeyCodeFromKeyName( const QString& keyname );

QString KeyNameFromKeyCode( int aKeycode, bool* aIsFound = nullptr );

enum HOTKEY_ACTION_TYPE
{
    IS_HOTKEY,
    IS_COMMENT
};

QString AddHotkeyName( const QString& aText, int aHotKey, HOTKEY_ACTION_TYPE aStyle = IS_HOTKEY );

void DisplayHotkeyList( EDA_BASE_FRAME* aFrame );

void ReadHotKeyConfig( const QString&                              aFileName,
                       std::map<std::string, std::pair<int, int>>& aHotKeys );

void ReadHotKeyConfigIntoActions( const QString& aFileName, std::vector<TOOL_ACTION*>& aActions );

int WriteHotKeyConfig( const std::vector<TOOL_ACTION*>& aActions );

int ReadLegacyHotkeyConfigFile( const QString& aFilename, std::map<std::string, int>& aMap );

int ReadLegacyHotkeyConfig( const QString& aAppname, std::map<std::string, int>& aMap );

#endif // HOTKEYS_BASIC_H
