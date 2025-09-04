
#include <kiface_base.h>
#include <hotkeys_basic.h>
#include <id.h>
#include <string_utils.h>
#include <eda_base_frame.h>
#include <eda_draw_frame.h>
#include <wildcards_and_files_ext.h>
#include <paths.h>

#include <tool/tool_manager.h>
#include "dialogs/dialog_hotkey_list.h"
#include <QString>
#include <QChar>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QRegularExpression>
#include <tool/tool_action.h>


struct hotkey_name_descr
{
    const QChar* m_Name;
    int          m_KeyCode;
};


#define KEY_NON_FOUND -1
static struct hotkey_name_descr hotkeyNameList[] =
{
    { "F1",            WXK_F1                 },
    { "F2",            WXK_F2                 },
    { "F3",            WXK_F3                 },
    { "F4",            WXK_F4                 },
    { "F5",            WXK_F5                 },
    { "F6",            WXK_F6                 },
    { "F7",            WXK_F7                 },
    { "F8",            WXK_F8                 },
    { "F9",            WXK_F9                 },
    { "F10",           WXK_F10                },
    { "F11",           WXK_F11                },
    { "F12",           WXK_F12                },
    { "F13",           WXK_F13                },
    { "F14",           WXK_F14                },
    { "F15",           WXK_F15                },
    { "F16",           WXK_F16                },
    { "F17",           WXK_F17                },
    { "F18",           WXK_F18                },
    { "F19",           WXK_F19                },
    { "F20",           WXK_F20                },
    { "F21",           WXK_F21                },
    { "F22",           WXK_F22                },
    { "F23",           WXK_F23                },
    { "F24",           WXK_F24                },

    { "Esc",           WXK_ESCAPE             },
    { "Del",           WXK_DELETE             },
    { "Tab",           WXK_TAB                },
    { "Back",          WXK_BACK               },
    { "Ins",           WXK_INSERT             },

    { "Home",          WXK_HOME               },
    { "End",           WXK_END                },
    { "PgUp",          WXK_PAGEUP             },
    { "PgDn",          WXK_PAGEDOWN           },

    { "Up",            WXK_UP                 },
    { "Down",          WXK_DOWN               },
    { "Left",          WXK_LEFT               },
    { "Right",         WXK_RIGHT              },

    { "Return",        WXK_RETURN             },

    { "Space",         WXK_SPACE              },

    { "Num Pad 0",     WXK_NUMPAD0            },
    { "Num Pad 1",     WXK_NUMPAD1            },
    { "Num Pad 2",     WXK_NUMPAD2            },
    { "Num Pad 3",     WXK_NUMPAD3            },
    { "Num Pad 4",     WXK_NUMPAD4            },
    { "Num Pad 5",     WXK_NUMPAD5            },
    { "Num Pad 6",     WXK_NUMPAD6            },
    { "Num Pad 7",     WXK_NUMPAD7            },
    { "Num Pad 8",     WXK_NUMPAD8            },
    { "Num Pad 9",     WXK_NUMPAD9            },
    { "Num Pad +",     WXK_NUMPAD_ADD         },
    { "Num Pad -",     WXK_NUMPAD_SUBTRACT    },
    { "Num Pad *",     WXK_NUMPAD_MULTIPLY    },
    { "Num Pad /",     WXK_NUMPAD_DIVIDE      },
    { "Num Pad .",     WXK_NUMPAD_SEPARATOR   },
    { "Num Pad Enter", WXK_NUMPAD_ENTER       },
    { "Num Pad F1",    WXK_NUMPAD_F1          },
    { "Num Pad F2",    WXK_NUMPAD_F2          },
    { "Num Pad F3",    WXK_NUMPAD_F3          },
    { "Num Pad F4",    WXK_NUMPAD_F4          },

    { "",              0                      },

    { "Click",         PSEUDO_WXK_CLICK       },
    { "DblClick",      PSEUDO_WXK_DBLCLICK    },
    { "Wheel",         PSEUDO_WXK_WHEEL       },

    { "",              KEY_NON_FOUND          }
};


#ifdef __WXMAC__
#define USING_MAC_CMD
#endif

#ifdef USING_MAC_CMD
#define MODIFIER_CTRL       "Cmd+"
#define MODIFIER_ALT        "Option+"
#else
#define MODIFIER_CTRL       "Ctrl+"
#define MODIFIER_ALT        "Alt+"
#endif
#define MODIFIER_CMD_MAC    "Cmd+"
#define MODIFIER_CTRL_BASE  "Ctrl+"
#define MODIFIER_SHIFT      "Shift+"


QString KeyNameFromKeyCode( int aKeycode, bool* aIsFound )
{
    QString keyname, modifier, fullkeyname;
    int      ii;
    bool     found = false;

    if( aKeycode == WXK_CONTROL )
        return QString( MODIFIER_CTRL ).section( '+', 0, 0 );
    else if( aKeycode == WXK_RAW_CONTROL )
        return QString( MODIFIER_CTRL_BASE ).section( '+', 0, 0 );
    else if( aKeycode == WXK_SHIFT )
        return QString( MODIFIER_SHIFT ).section( '+', 0, 0 );
    else if( aKeycode == WXK_ALT )
        return QString( MODIFIER_ALT ).section( '+', 0, 0 );

    if( (aKeycode & MD_CTRL) != 0 )
        modifier += MODIFIER_CTRL;

    if( (aKeycode & MD_ALT) != 0 )
        modifier += MODIFIER_ALT;

    if( (aKeycode & MD_SHIFT) != 0 )
        modifier += MODIFIER_SHIFT;

    aKeycode &= ~( MD_CTRL | MD_ALT | MD_SHIFT );

    if( (aKeycode > ' ') && (aKeycode < 0x7F ) )
    {
        found   = true;
        keyname += (QChar)aKeycode;
    }
    else
    {
        for( ii = 0; ; ii++ )
        {
            if( hotkeyNameList[ii].m_KeyCode == KEY_NON_FOUND ) // End of list
            {
                keyname = "<unknown>";
                break;
            }

            if( hotkeyNameList[ii].m_KeyCode == aKeycode )
            {
                keyname = hotkeyNameList[ii].m_Name;
                found   = true;
                break;
            }
        }
    }

    if( aIsFound )
        *aIsFound = found;

    fullkeyname = modifier + keyname;
    return fullkeyname;
}


QString AddHotkeyName( const QString& aText, int aHotKey, HOTKEY_ACTION_TYPE aStyle )
{
    QString msg = aText;
    QString keyname = KeyNameFromKeyCode( aHotKey );

    if( !keyname.isEmpty() )
    {
        switch( aStyle )
        {
        case IS_HOTKEY:
        {
            if( aHotKey != 0 )
            {
                msg += "\t" + keyname;
            }
            break;
        }
        case IS_COMMENT:
        {
            msg += " (" + keyname + ")";
            break;
        }
        }
    }

#ifdef USING_MAC_CMD
    // On OSX, the modifier equivalent to the Ctrl key of PCs
    // is the Cmd key, but in code we should use Ctrl as prefix in menus
    msg.Replace( MODIFIER_CMD_MAC, MODIFIER_CTRL_BASE );
#endif

    return msg;
}


int KeyCodeFromKeyName( const QString& keyname )
{
    int ii, keycode = KEY_NON_FOUND;

    QString key = keyname;
    QString prefix;
    int modifier = 0;

    while( true )
    {
        prefix.clear();

        if( key.startsWith( MODIFIER_CTRL_BASE ) )
        {
            modifier |= MD_CTRL;
            prefix = MODIFIER_CTRL_BASE;
        }
        else if( key.startsWith( MODIFIER_CMD_MAC ) )
        {
            modifier |= MD_CTRL;
            prefix = MODIFIER_CMD_MAC;
        }
        else if( key.startsWith( MODIFIER_ALT ) )
        {
            modifier |= MD_ALT;
            prefix = MODIFIER_ALT;
        }
        else if( key.startsWith( MODIFIER_SHIFT ) )
        {
            modifier |= MD_SHIFT;
            prefix = MODIFIER_SHIFT;
        }
        else
        {
            break;
        }

        if( !prefix.isEmpty() )
            key.remove( 0, prefix.length() );
    }

    if( (key.length() == 1) && (key[0] > ' ') && (key[0] < 0x7F) )
    {
        keycode = key[0];
        keycode += modifier;
        return keycode;
    }

    for( ii = 0; hotkeyNameList[ii].m_KeyCode != KEY_NON_FOUND; ii++ )
    {
        if( key.compare( hotkeyNameList[ii].m_Name, Qt::CaseInsensitive ) == 0 )
        {
            keycode = hotkeyNameList[ii].m_KeyCode + modifier;
            break;
        }
    }

    return keycode;
}


void DisplayHotkeyList( EDA_BASE_FRAME* aParent )
{
    DIALOG_LIST_HOTKEYS dlg( aParent );
    dlg.ShowModal();
}


void ReadHotKeyConfig( const QString&                              aFileName,
                       std::map<std::string, std::pair<int, int>>& aHotKeys )
{
    QString fileName = aFileName;

    if( fileName.isEmpty() )
    {
        QString userSettingsPath = PATHS::GetUserSettingsPath();
        fileName = userSettingsPath + "/user." + FILEEXT::HotkeyFileExtension;
    }

    if( !QFile::exists( fileName ) )
        return;

    QFile file( fileName );

    if( !file.open( QIODevice::ReadOnly ) )
        return;

    QTextStream stream( &file );
    QString input = stream.readAll();
    input.replace( "\r\n", "\n" );
    QStringList lines = input.split( "\n", Qt::SkipEmptyParts );

    for( const QString& line : lines )
    {
        QStringList tokens = line.split( "\t" );
        
        if( tokens.size() >= 3 )
        {
            QString cmdName = tokens[0];
            QString primary = tokens[1];
            QString secondary = tokens[2];
            
            if( !cmdName.isEmpty() )
                aHotKeys[cmdName.toStdString()] = std::pair<int, int>(
                        KeyCodeFromKeyName( primary ), KeyCodeFromKeyName( secondary ) );
        }
    }
}


void ReadHotKeyConfigIntoActions( const QString& aFileName, std::vector<TOOL_ACTION*>& aActions )
{
    std::map<std::string, std::pair<int, int>> hotkeys;

    // Read the existing config (all hotkeys)
    ReadHotKeyConfig( aFileName, hotkeys );

    // Set each tool action hotkey to the config file hotkey if present
    for( TOOL_ACTION* action : aActions )
    {
        if( hotkeys.find( action->GetName() ) != hotkeys.end() )
        {
            std::pair<int, int> keys = hotkeys[action->GetName()];
            action->SetHotKey( keys.first, keys.second );
        }
    }
}


int WriteHotKeyConfig( const std::vector<TOOL_ACTION*>& aActions )
{
    std::map<std::string, std::pair<int, int>> hotkeys;
    QString userSettingsPath = PATHS::GetUserSettingsPath();
    QString fileName = userSettingsPath + "/user." + FILEEXT::HotkeyFileExtension;

    ReadHotKeyConfig( fileName, hotkeys );

    for( const TOOL_ACTION* action : aActions )
        hotkeys[ action->GetName() ] = std::pair<int, int>( action->GetHotKey(),
                                                            action->GetHotKeyAlt() );

    QFile outFile( fileName );
    if( outFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QTextStream txtStream( &outFile );
        
        for( const std::pair<const std::string, std::pair<int, int>>& entry : hotkeys )
            txtStream << QString::fromStdString( entry.first )
                << "\t" << KeyNameFromKeyCode( entry.second.first )
                << "\t" << KeyNameFromKeyCode( entry.second.second ) << "\n";
        
        outFile.close();
    }

    return 1;
}


int ReadLegacyHotkeyConfig( const QString& aAppname, std::map<std::string, int>& aMap )
{
    if( aAppname == LIB_EDIT_FRAME_NAME || aAppname == SCH_EDIT_FRAME_NAME )
    {
        return ReadLegacyHotkeyConfigFile( EESCHEMA_HOTKEY_NAME, aMap );
    }
    else if( aAppname == PCB_EDIT_FRAME_NAME || aAppname == FOOTPRINT_EDIT_FRAME_NAME )
    {
        return ReadLegacyHotkeyConfigFile( PCBNEW_HOTKEY_NAME, aMap );
    }

    return ReadLegacyHotkeyConfigFile( aAppname, aMap );
}


int ReadLegacyHotkeyConfigFile( const QString& aFilename, std::map<std::string, int>& aMap )
{
    QString userSettingsPath = PATHS::GetUserSettingsPath();
    QString fileName = userSettingsPath + "/" + aFilename + "." + FILEEXT::HotkeyFileExtension;

    if( !QFile::exists( fileName ) )
        return 0;

    QFile cfgfile( fileName );

    if( !cfgfile.open( QIODevice::ReadOnly ) )
        return 0;

    QTextStream stream( &cfgfile );
    QString data = stream.readAll();

    if( data.startsWith( "Keys=" ) )
    {
        data = data.mid( 5 );
        data.replace( "\\n", "\n" );
    }

    QStringList lines = data.split( QRegularExpression( "[\r\n]" ), Qt::SkipEmptyParts );
    
    for( const QString& line : lines )
    {
        QStringList tokens = line.split( QRegularExpression( "\\s+" ), Qt::SkipEmptyParts );
        
        if( tokens.isEmpty() )
            continue;
            
        QString line_type = tokens[0];
        
        if( line_type[0] == '#' )
            continue;
            
        if( line_type[0] == '[' )
            continue;
            
        if( line_type == "$Endlist" )
            break;
            
        if( line_type != "shortcut" )
            continue;
            
        QStringList quotedTokens = line.split( '\"', Qt::SkipEmptyParts );
        if( quotedTokens.size() >= 3 )
        {
            QString keyname = quotedTokens[0].trimmed().split( QRegularExpression( "\\s+" ) ).last();
            QString fctname = quotedTokens[1];
            
            aMap[ fctname.toStdString() ] = KeyCodeFromKeyName( keyname );
        }
    }

    cfgfile.close();
    return 1;
}


