
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05
#include "generators_mgr.h"


GENERATORS_MGR& GENERATORS_MGR::Instance()
{
    static GENERATORS_MGR pm;
    return pm;
}


void GENERATORS_MGR::Register( const QString& aTypeStr, const QString& aName,
                               std::function<PCB_GENERATOR*( void )> aCreateFunc )
{
    Q_ASSERT( !aName.isEmpty() );
    Q_ASSERT( !aTypeStr.isEmpty() );
    Q_ASSERT( aCreateFunc );

    ENTRY ent;
    ent.m_createFunc = aCreateFunc;
    ent.m_type = aTypeStr;
    ent.m_displayName = aName;
    m_registry.emplace( aTypeStr, ent );
}


PCB_GENERATOR* GENERATORS_MGR::CreateFromType( const QString& aTypeStr )
{
    auto it = m_registry.find( aTypeStr );

    if( it == m_registry.end() )
    {
        return nullptr;
    }

    ENTRY& entry = it->second;

    return entry.m_createFunc();
}