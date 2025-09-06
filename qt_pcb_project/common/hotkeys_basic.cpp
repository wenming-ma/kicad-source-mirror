
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
#include <tool/tool_event.h>
#include <QKeyEvent>


struct hotkey_name_descr
{
    const char* m_Name;
    int          m_KeyCode;
};


#define KEY_NON_FOUND -1
static struct hotkey_name_descr hotkeyNameList[] =
{
    { "F1",            Qt::Key_F1             },
    { "F2",            Qt::Key_F2             },
    { "F3",            Qt::Key_F3             },
    { "F4",            Qt::Key_F4             },
    { "F5",            Qt::Key_F5             },
    { "F6",            Qt::Key_F6             },
    { "F7",            Qt::Key_F7             },
    { "F8",            Qt::Key_F8             },
    { "F9",            Qt::Key_F9             },
    { "F10",           Qt::Key_F10            },
    { "F11",           Qt::Key_F11            },
    { "F12",           Qt::Key_F12            },
    { "F13",           Qt::Key_F13            },
    { "F14",           Qt::Key_F14            },
    { "F15",           Qt::Key_F15            },
    { "F16",           Qt::Key_F16            },
    { "F17",           Qt::Key_F17            },
    { "F18",           Qt::Key_F18            },
    { "F19",           Qt::Key_F19            },
    { "F20",           Qt::Key_F20            },
    { "F21",           Qt::Key_F21            },
    { "F22",           Qt::Key_F22            },
    { "F23",           Qt::Key_F23            },
    { "F24",           Qt::Key_F24            },

    { "Esc",           Qt::Key_Escape         },
    { "Del",           Qt::Key_Delete         },
    { "Tab",           Qt::Key_Tab            },
    { "Back",          Qt::Key_Backspace      },
    { "Ins",           Qt::Key_Insert         },

    { "Home",          Qt::Key_Home           },
    { "End",           Qt::Key_End            },
    { "PgUp",          Qt::Key_PageUp         },
    { "PgDn",          Qt::Key_PageDown       },

    { "Up",            Qt::Key_Up             },
    { "Down",          Qt::Key_Down           },
    { "Left",          Qt::Key_Left           },
    { "Right",         Qt::Key_Right          },

    { "Return",        Qt::Key_Return         },

    { "Space",         Qt::Key_Space          },

    { "Num Pad 0",     Qt::Key_0              },
    { "Num Pad 1",     Qt::Key_1              },
    { "Num Pad 2",     Qt::Key_2              },
    { "Num Pad 3",     Qt::Key_3              },
    { "Num Pad 4",     Qt::Key_4              },
    { "Num Pad 5",     Qt::Key_5              },
    { "Num Pad 6",     Qt::Key_6              },
    { "Num Pad 7",     Qt::Key_7              },
    { "Num Pad 8",     Qt::Key_8              },
    { "Num Pad 9",     Qt::Key_9              },
    { "Num Pad +",     Qt::Key_Plus           },
    { "Num Pad -",     Qt::Key_Minus          },
    { "Num Pad *",     Qt::Key_Asterisk       },
    { "Num Pad /",     Qt::Key_Slash          },
    { "Num Pad .",     Qt::Key_Period         },
    { "Num Pad Enter", Qt::Key_Enter          },
    { "Num Pad F1",    Qt::Key_F1             },
    { "Num Pad F2",    Qt::Key_F2             },
    { "Num Pad F3",    Qt::Key_F3             },
    { "Num Pad F4",    Qt::Key_F4             },

    { "",              0                      },

    { "Click",         PSEUDO_QT_CLICK       },
    { "DblClick",      PSEUDO_QT_DBLCLICK    },
    { "Wheel",         PSEUDO_QT_WHEEL       },

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

    if( aKeycode == Qt::Key_Control )
        return QString( MODIFIER_CTRL ).section( '+', 0, 0 );
    else if( aKeycode == Qt::Key_Control )
        return QString( MODIFIER_CTRL_BASE ).section( '+', 0, 0 );
    else if( aKeycode == Qt::Key_Shift )
        return QString( MODIFIER_SHIFT ).section( '+', 0, 0 );
    else if( aKeycode == Qt::Key_Alt )
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
    msg.replace( MODIFIER_CMD_MAC, MODIFIER_CTRL_BASE );
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

    if( (key.length() == 1) && (key[0].unicode() > ' ') && (key[0].unicode() < 0x7F) )
    {
        keycode = key[0].unicode();
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
        fileName = userSettingsPath + "/user." + QString::fromStdString(FILEEXT::HotkeyFileExtension);
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
    QString fileName = userSettingsPath + "/user." + QString::fromStdString(FILEEXT::HotkeyFileExtension);

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
    QString fileName = userSettingsPath + "/" + aFilename + "." + QString::fromStdString(FILEEXT::HotkeyFileExtension);

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


