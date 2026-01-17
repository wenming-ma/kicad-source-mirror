
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05
#ifndef GENERATORS_MGR_H_
#define GENERATORS_MGR_H_

#include <cstdint>
#include <config.h>
#include <vector>
#include <map>
#include <memory>
#include <QString>

class PCB_GENERATOR;


///< Unique type identifier
using TYPE_ID = size_t;

/**
 * A factory which returns an instance of a #PCB_GENERATOR.
 */
class GENERATORS_MGR
{
public:
    struct ENTRY
    {
        QString                               m_type;
        QString                               m_displayName;
        std::function<PCB_GENERATOR*( void )> m_createFunc;
    };

    static GENERATORS_MGR& Instance();

    void Register( const QString& aTypeStr, const QString& aName,
                   std::function<PCB_GENERATOR*( void )> aCreateFunc );

    PCB_GENERATOR* CreateFromType( const QString& aTypeStr );

    // Static helper to register a generator.
    // T must define static members GENERATOR_TYPE and DISPLAY_NAME
    template <typename T>
    struct REGISTER
    {
        REGISTER()
        {
            GENERATORS_MGR::Instance().Register( T::GENERATOR_TYPE, T::DISPLAY_NAME,
                                                 []
                                                 {
                                                     return new T;
                                                 } );
        }
    };

private:
    std::map<QString, ENTRY> m_registry;
};


#endif // GENERATORS_MGR_H_
