#ifndef HOTKEY_STORE__H
#define HOTKEY_STORE__H

#include <hotkeys_basic.h>
#include <tool/tool_action.h>
#include <QVector>
#include <QString>

class TOOL_MANAGER;


struct HOTKEY
{
    QVector<TOOL_ACTION*> m_Actions;
    int                       m_EditKeycode;
    int                       m_EditKeycodeAlt;

    HOTKEY() :
            m_EditKeycode( 0 ),
            m_EditKeycodeAlt( 0 )
    { }

    HOTKEY( TOOL_ACTION* aAction ) :
            m_EditKeycode( aAction->GetHotKey() ),
            m_EditKeycodeAlt( aAction->GetHotKeyAlt() )
    {
        m_Actions.push_back( aAction );
    }
};


struct HOTKEY_SECTION
{
    QString            m_SectionName;
    QVector<HOTKEY> m_HotKeys;
};


class HOTKEY_STORE
{
public:

    HOTKEY_STORE();

    void Init( QVector<TOOL_ACTION*> aActionsList, bool aIncludeReadOnlyCmds );

    static QString GetAppName( TOOL_ACTION* aAction );
    static QString GetSectionName( TOOL_ACTION* aAction );

    QVector<HOTKEY_SECTION>& GetSections();

    void SaveAllHotkeys();

    void ResetAllHotkeysToDefault();

    void ResetAllHotkeysToOriginal();

    bool CheckKeyConflicts( TOOL_ACTION* aAction, long aKey, HOTKEY** aConflict );

private:
    QVector<TOOL_MANAGER*>  m_toolManagers;
    QVector<HOTKEY_SECTION> m_hk_sections;
};

#endif // HOTKEY_STORE__H
