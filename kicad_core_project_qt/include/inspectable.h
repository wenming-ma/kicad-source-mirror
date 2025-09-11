
#ifndef INSPECTABLE_H
#define INSPECTABLE_H

#include <QVariant>

#include <properties/property_mgr.h>
#include <properties/property.h>

#include <optional>

class INSPECTABLE
{
public:
    virtual ~INSPECTABLE()
    {
    }

    bool Set( PROPERTY_BASE* aProperty, QVariant& aValue, bool aNotify = true )
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        void* object = propMgr.TypeCast( this, TYPE_HASH( *this ), aProperty->OwnerHash() );

        if( object )
        {
            aProperty->setter( object, aValue );

            if( aNotify )
                propMgr.PropertyChanged( this, aProperty );
        }

        return object != nullptr;
    }

    template<typename T>
    bool Set( PROPERTY_BASE* aProperty, T aValue, bool aNotify = true )
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        void* object = propMgr.TypeCast( this, TYPE_HASH( *this ), aProperty->OwnerHash() );

        if( object )
        {
            aProperty->set<T>( object, aValue );

            if( aNotify )
                propMgr.PropertyChanged( this, aProperty );
        }

        return object != nullptr;
    }

    template<typename T>
    bool Set( const QString& aProperty, T aValue, bool aNotify = true )
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        TYPE_ID thisType = TYPE_HASH( *this );
        PROPERTY_BASE* prop = propMgr.GetProperty( thisType, aProperty );
        void* object = nullptr;

        if( prop )
        {
            object = propMgr.TypeCast( this, thisType, prop->OwnerHash() );

            if( object )
            {
                prop->set<T>( object, aValue );

                if( aNotify )
                    propMgr.PropertyChanged( this, prop );
            }
        }

        return object != nullptr;
    }

    QVariant Get( PROPERTY_BASE* aProperty ) const
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        const void* object = propMgr.TypeCast( this, TYPE_HASH( *this ), aProperty->OwnerHash() );
        return object ? aProperty->getter( object ) : QVariant();
    }

    template<typename T>
    T Get( PROPERTY_BASE* aProperty ) const
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        const void* object = propMgr.TypeCast( this, TYPE_HASH( *this ), aProperty->OwnerHash() );

        if( !object )
            throw std::runtime_error( "Could not cast INSPECTABLE to the requested type" );

        return aProperty->get<T>( object );
    }

    template<typename T>
    std::optional<T> Get( const QString& aProperty ) const
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        TYPE_ID thisType = TYPE_HASH( *this );
        PROPERTY_BASE* prop = propMgr.GetProperty( thisType, aProperty );
        std::optional<T> ret;

        if( prop )
        {
            const void* object = propMgr.TypeCast( this, thisType, prop->OwnerHash() );

            if( object )
                ret = prop->get<T>( object );
        }

        return ret;
    }
};

#endif /* INSPECTABLE_H */
