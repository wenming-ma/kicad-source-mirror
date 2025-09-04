// QT_TRANSFORMATION_COMPLETED

#ifndef PROPERTY_MGR_H
#define PROPERTY_MGR_H

#include <QString>
#include <QHash>

#include <functional>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>
#include <eda_units.h>

class PROPERTY_BASE;
class TYPE_CAST_BASE;
class ORIGIN_TRANSFORMS;
class INSPECTABLE;
class COMMIT;

///< Unique type identifier
using TYPE_ID = size_t;

using PROPERTY_LIST = std::vector<PROPERTY_BASE*>;

using PROPERTY_SET = std::set<std::pair<size_t, QString>>;

template<typename ValueType>
using PROPERTY_MAP = std::map<std::pair<size_t, QString>, ValueType>;

using PROPERTY_FUNCTOR_MAP = PROPERTY_MAP<std::function<bool( INSPECTABLE* )>>;

using PROPERTY_DISPLAY_ORDER = std::map<PROPERTY_BASE*, int>;

using PROPERTY_LISTENER = std::function<void( INSPECTABLE*, PROPERTY_BASE*, COMMIT* )>;

class PROPERTY_COMMIT_HANDLER
{
public:
    PROPERTY_COMMIT_HANDLER( COMMIT* aCommit );

    ~PROPERTY_COMMIT_HANDLER();
};

class PROPERTY_MANAGER
{
public:
    static PROPERTY_MANAGER& Instance()
    {
        static PROPERTY_MANAGER pm;
        return pm;
    }

    void RegisterType( TYPE_ID aType, const QString& aName );

    const QString& ResolveType( TYPE_ID aType ) const;

    PROPERTY_BASE* GetProperty( TYPE_ID aType, const QString& aProperty ) const;

    const PROPERTY_LIST& GetProperties( TYPE_ID aType ) const;

    const PROPERTY_DISPLAY_ORDER& GetDisplayOrder( TYPE_ID aType ) const;

    const std::vector<QString>& GetGroupDisplayOrder( TYPE_ID aType ) const;

    const void* TypeCast( const void* aSource, TYPE_ID aBase, TYPE_ID aTarget ) const;

    void* TypeCast( void* aSource, TYPE_ID aBase, TYPE_ID aTarget ) const
    {
        return const_cast<void*>( TypeCast( (const void*) aSource, aBase, aTarget ) );
    }

    PROPERTY_BASE& AddProperty( PROPERTY_BASE* aProperty, const QString& aGroup = QString() );

    PROPERTY_BASE& ReplaceProperty( size_t aBase, const QString& aName, PROPERTY_BASE* aNew,
                                    const QString& aGroup = QString() );

    void AddTypeCast( TYPE_CAST_BASE* aCast );

    void InheritsAfter( TYPE_ID aDerived, TYPE_ID aBase );

    void Mask( TYPE_ID aDerived, TYPE_ID aBase, const QString& aName );

    void OverrideAvailability( TYPE_ID aDerived, TYPE_ID aBase, const QString& aName,
                               std::function<bool( INSPECTABLE* )> aFunc );

    void OverrideWriteability( TYPE_ID aDerived, TYPE_ID aBase, const QString& aName,
                               std::function<bool( INSPECTABLE* )> aFunc );

    bool IsAvailableFor( TYPE_ID aItemClass, PROPERTY_BASE* aProp, INSPECTABLE* aItem );

    bool IsWriteableFor( TYPE_ID aItemClass, PROPERTY_BASE* aProp, INSPECTABLE* aItem );

    bool IsOfType( TYPE_ID aDerived, TYPE_ID aBase ) const;

    void Rebuild();

    struct CLASS_INFO
    {
        QString name;
        TYPE_ID type;
        std::vector<PROPERTY_BASE*> properties;
    };

    typedef std::vector<CLASS_INFO> CLASSES_INFO;

    CLASSES_INFO GetAllClasses();

    std::vector<TYPE_ID> GetMatchingClasses( PROPERTY_BASE* aProperty );

    void PropertyChanged( INSPECTABLE* aObject, PROPERTY_BASE* aProperty );

    void RegisterListener( TYPE_ID aType, PROPERTY_LISTENER aListenerFunc )
    {
        m_listeners[aType].emplace_back( aListenerFunc );
    }

    void UnregisterListeners( TYPE_ID aType )
    {
        m_listeners[aType].clear();
    }

private:
    PROPERTY_MANAGER() :
            m_dirty( false ),
            m_managedCommit( nullptr )
    {
    }

    friend class PROPERTY_COMMIT_HANDLER;

    struct CLASS_DESC
    {
        CLASS_DESC( TYPE_ID aId )
            : m_id( aId )
        {
            m_groupDisplayOrder.emplace_back( QString() );
            m_groups.insert( QString() );
        }

        const TYPE_ID m_id;

        std::vector<std::reference_wrapper<CLASS_DESC>> m_bases;

        std::map<QString, std::unique_ptr<PROPERTY_BASE>> m_ownProperties;

        std::map<TYPE_ID, std::unique_ptr<TYPE_CAST_BASE>> m_typeCasts;

        PROPERTY_SET m_maskedBaseProperties;

        PROPERTY_FUNCTOR_MAP m_availabilityOverrides;

        PROPERTY_FUNCTOR_MAP m_writeabilityOverrides;

        std::vector<PROPERTY_BASE*> m_allProperties;

        PROPERTY_DISPLAY_ORDER m_displayOrder;

        std::vector<QString> m_groupDisplayOrder;

        std::vector<PROPERTY_BASE*> m_ownDisplayOrder;

        std::set<QString> m_groups;

        PROPERTY_SET m_replaced;

        void rebuild();

        void collectPropsRecur( PROPERTY_LIST& aResult, PROPERTY_SET& aReplaced,
                                PROPERTY_DISPLAY_ORDER& aDisplayOrder,
                                PROPERTY_SET& aMasked ) const;
    };

    CLASS_DESC& getClass( TYPE_ID aTypeId );

    std::unordered_map<TYPE_ID, QString> m_classNames;

    std::unordered_map<TYPE_ID, CLASS_DESC> m_classes;

    bool m_dirty;

    std::map<TYPE_ID, std::vector<PROPERTY_LISTENER>> m_listeners;

    COMMIT* m_managedCommit;
};

#define REGISTER_TYPE(x) PROPERTY_MANAGER::Instance().RegisterType(TYPE_HASH(x), TYPE_NAME(x))

#endif /* PROPERTY_MGR_H */
