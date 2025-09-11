
#ifndef BIN_MOD_H_
#define BIN_MOD_H_

#include <QString>

#include <search_stack.h>

class APP_SETTINGS_BASE;

struct BIN_MOD
{
    BIN_MOD( const char* aName );
    ~BIN_MOD();

    void Init();
    void End();

    void InitSettings( APP_SETTINGS_BASE* aPtr ) { m_config = aPtr; }

    const char*         m_name;

    APP_SETTINGS_BASE*  m_config;
    QString             m_help_file;

    SEARCH_STACK        m_search;
};

#endif // BIN_MOD_H_
