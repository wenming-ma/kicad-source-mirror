// QT_TRANSFORMATION_COMPLETED

#ifndef PROPERTY_H
#define PROPERTY_H

#include <origin_transforms.h>
#include <properties/color4d_variant.h>
#include <properties/eda_angle_variant.h>
#include <properties/property_validator.h>

#include <QVariant>
#include <QString>
#include <QPixmap>
#include <QFont>
#include <QValidator>
#include <QMetaType>

#ifdef DEBUG
#include <QDebug>
#endif

#include <functional>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <algorithm>
#include "std_optional_variants.h"

// Specialize std::hash for QString to use with std::unordered_map
namespace std {
    template<>
    struct hash<QString>
    {
        std::size_t operator()(const QString& s) const noexcept
        {
            return qHash(s);
        }
    };
}

class QStandardItem;
class INSPECTABLE;
class PROPERTY_BASE;

template<typename T>
class ENUM_MAP;

// Common property types
enum PROPERTY_DISPLAY
{
    PT_DEFAULT,    // Default property for a given type
    PT_SIZE,       // Size expressed in distance units (mm/inch)
    PT_AREA,       // Area expressed in distance units-squared (mm/inch)
    PT_COORD,      // Coordinate expressed in distance units (mm/inch)
    PT_DEGREE,     // Angle expressed in degrees
    PT_DECIDEGREE, // Angle expressed in decidegrees
    PT_RATIO
};

// Macro to generate unique identifier for a type
#define TYPE_HASH( x ) typeid( x ).hash_code()
#define TYPE_NAME( x ) typeid( x ).name()

template<typename Owner, typename T>
class GETTER_BASE
{
public:
    virtual ~GETTER_BASE() {}

    virtual T operator()( const Owner* aOwner ) const = 0;
};

template<typename Owner, typename T, typename FuncType>
class GETTER : public GETTER_BASE<Owner, T>
{
public:
    GETTER( FuncType aFunc )
        : m_func( aFunc )
    {
        Q_ASSERT( m_func );
    }

    T operator()( const Owner* aOwner ) const override
    {
        return ( aOwner->*m_func )();
    }

private:
    FuncType m_func;
};

template<typename Owner, typename T>
class SETTER_BASE
{
public:
    virtual ~SETTER_BASE() {}

    virtual void operator()( Owner* aOwner, T aValue ) = 0;
};

template<typename Owner, typename T, typename FuncType>
class SETTER : public SETTER_BASE<Owner, T>
{
public:
    SETTER( FuncType aFunc )
        : m_func( aFunc )
    {
        Q_ASSERT( m_func );
    }

    void operator()( Owner* aOwner, T aValue ) override
    {
        ( aOwner->*m_func )( aValue );
    }

private:
    FuncType m_func;
};

#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 5266 ) // 'const' qualifier on return type has no effect
#endif

template<typename Owner, typename T, typename Base = Owner>
class METHOD
{
public:
    static GETTER_BASE<Owner, T>* Wrap( T (Base::*aFunc)() )
    {
        return new GETTER<Owner, T, T (Base::*)()>( aFunc );
    }

    constexpr static GETTER_BASE<Owner, T>* Wrap( const T (Base::*aFunc)() )
    {
        return new GETTER<Owner, T, T (Base::*)()>( aFunc );
    }

    constexpr static GETTER_BASE<Owner, T>* Wrap( const T& (Base::*aFunc)() )
    {
        return new GETTER<Owner, T, const T& (Base::*)()>( aFunc );
    }

    constexpr static GETTER_BASE<Owner, T>* Wrap( T (Base::*aFunc)() const )
    {
        return new GETTER<Owner, T, T (Base::*)() const>( aFunc );
    }

    constexpr static GETTER_BASE<Owner, T>* Wrap( const T (Base::*aFunc)() const )
    {
        return new GETTER<Owner, T, T (Base::*)() const>( aFunc );
    }

    constexpr static GETTER_BASE<Owner, T>* Wrap( const T& (Base::*aFunc)() const )
    {
        return new GETTER<Owner, T, const T& (Base::*)() const>( aFunc );
    }

    constexpr static SETTER_BASE<Owner, T>* Wrap( void (Base::*aFunc)( T ) )
    {
        return aFunc ? new SETTER<Owner, T, void (Base::*)( T )>( aFunc ) : nullptr;
    }

    constexpr static SETTER_BASE<Owner, T>* Wrap( void (Base::*aFunc)( T& ) )
    {
        return aFunc ? new SETTER<Owner, T, void (Base::*)( T& )>( aFunc ) : nullptr;
    }

    constexpr static SETTER_BASE<Owner, T>* Wrap( void (Base::*aFunc)( const T& ) )
    {
        return aFunc ? new SETTER<Owner, T, void (Base::*)( const T& )>( aFunc ) : nullptr;
    }

    METHOD() = delete;
};

#if defined( _MSC_VER )
#pragma warning( pop )
#endif

class PROPERTY_BASE
{
private:
    ///< Used to generate unique IDs.  Must come up front so it's initialized before ctor.

public:
    PROPERTY_BASE( const QString& aName, PROPERTY_DISPLAY aDisplay = PT_DEFAULT,
                   ORIGIN_TRANSFORMS::COORD_TYPES_T aCoordType = ORIGIN_TRANSFORMS::NOT_A_COORD ) :
            m_name( aName ),
            m_display( aDisplay ),
            m_coordType( aCoordType ),
            m_hideFromPropertiesManager( false ),
            m_hideFromLibraryEditors( false ),
            m_hideFromDesignEditors( false ),
            m_hideFromRulesEditor( false ),
            m_availFunc( [](INSPECTABLE*)->bool { return true; } ),
            m_writeableFunc( [](INSPECTABLE*)->bool { return true; } ),
            m_validator( NullValidator )
    {
    }

    virtual ~PROPERTY_BASE()
    {
    }

    const QString& Name() const { return m_name; }

    virtual const QStringList& Choices() const
    {
        static QStringList empty;
        return empty;
    }

    virtual void SetChoices( const QStringList& aChoices )
    {
        Q_ASSERT(false); // only possible for PROPERTY_ENUM
    }
    virtual bool HasChoices() const
    {
        return false;
    }

    bool Available( INSPECTABLE* aObject ) const
    {
        return m_availFunc( aObject );
    }

    PROPERTY_BASE& SetAvailableFunc( std::function<bool(INSPECTABLE*)> aFunc )
    {
        m_availFunc = std::move( aFunc );
        return *this;
    }

    virtual bool Writeable( INSPECTABLE* aObject ) const
    {
        return m_writeableFunc( aObject );
    }

    PROPERTY_BASE& SetWriteableFunc( std::function<bool(INSPECTABLE*)> aFunc )
    {
        m_writeableFunc = std::move( aFunc );
        return *this;
    }

    virtual size_t OwnerHash() const = 0;

    virtual size_t BaseHash() const = 0;

    virtual size_t TypeHash() const = 0;

    PROPERTY_DISPLAY Display() const { return m_display; }
    PROPERTY_BASE& SetDisplay( PROPERTY_DISPLAY aDisplay ) { m_display = aDisplay; return *this; }

    ORIGIN_TRANSFORMS::COORD_TYPES_T CoordType() const { return m_coordType; }
    PROPERTY_BASE& SetCoordType( ORIGIN_TRANSFORMS::COORD_TYPES_T aType )
    {
        m_coordType = aType;
        return *this;
    }

    bool IsHiddenFromPropertiesManager() const { return m_hideFromPropertiesManager; }
    PROPERTY_BASE& SetIsHiddenFromPropertiesManager( bool aHide = true )
    {
        m_hideFromPropertiesManager = aHide;
        return *this;
    }

    bool IsHiddenFromRulesEditor() const { return m_hideFromRulesEditor; }
    PROPERTY_BASE& SetIsHiddenFromRulesEditor( bool aHide = true )
    {
        m_hideFromRulesEditor = aHide;
        return *this;
    }

    bool IsHiddenFromLibraryEditors() const { return m_hideFromLibraryEditors; }
    PROPERTY_BASE& SetIsHiddenFromLibraryEditors( bool aIsHidden = true )
    {
        m_hideFromLibraryEditors = aIsHidden;
        return *this;
    }

    bool IsHiddenFromDesignEditors() const { return m_hideFromDesignEditors; }
    PROPERTY_BASE& SetIsHiddenFromDesignEditors( bool aIsHidden = true )
    {
        m_hideFromDesignEditors = aIsHidden;
        return *this;
    }

    QString Group() const { return m_group; }
    PROPERTY_BASE& SetGroup( const QString& aGroup ) { m_group = aGroup; return *this; }

    PROPERTY_BASE& SetValidator( PROPERTY_VALIDATOR_FN&& aValidator )
    {
        m_validator = aValidator;
        return *this;
    }

    VALIDATOR_RESULT Validate( const QVariant&& aValue, EDA_ITEM* aItem )
    {
        return m_validator( std::move( aValue ), aItem );
    }

    static VALIDATOR_RESULT NullValidator( const QVariant&& aValue, EDA_ITEM* aItem )
    {
        return std::nullopt;
    }

protected:
    template<typename T>
    void set( void* aObject, T aValue )
    {
        QVariant a = QVariant::fromValue(aValue);

        // QVariant will be type "long" even if the property is supposed to be
        // unsigned.  Let's trust that we're coming from the property grid where
        // we used a UInt editor.
        if( std::is_same<T, QVariant>::value )
        {
            QVariant var = static_cast<QVariant>( aValue );
            QVariant pv = getter( aObject );

            if( pv.canConvert<unsigned>() )
            {
                a = static_cast<unsigned>( var.toInt() );
            }
            else if( pv.canConvert<std::optional<int>>() )
            {
                auto* data = static_cast<STD_OPTIONAL_INT_VARIANT_DATA*>( var.data() );
                a = QVariant::fromValue(data->Value());
            }
            else if( pv.canConvert<std::optional<double>>() )
            {
                auto* data = static_cast<STD_OPTIONAL_DOUBLE_VARIANT_DATA*>( var.data() );
                a = QVariant::fromValue(data->Value());
            }
            else if( pv.canConvert<EDA_ANGLE>() )
            {
                EDA_ANGLE_VARIANT_DATA* ad = static_cast<EDA_ANGLE_VARIANT_DATA*>( var.data() );
                a = QVariant::fromValue(ad->Angle());
            }
            else if( pv.canConvert<KIGFX::COLOR4D>() )
            {
                COLOR4D_VARIANT_DATA* cd = static_cast<COLOR4D_VARIANT_DATA*>( var.data() );
                a = QVariant::fromValue(cd->Color());
            }
        }

        setter( aObject, a );
    }

    template<typename T>
    T get( const void* aObject ) const
    {
        QVariant a = getter( aObject );

        // We don't currently have a bool type, so change it to a numeric
        if( a.userType() == QMetaType::Bool )
            a = a.toBool() ? 1 : 0;

        if ( !( std::is_enum<T>::value && a.canConvert<int>() ) && !a.canConvert<T>() )
            throw std::invalid_argument( "Invalid requested type" );

        return a.value<T>();
    }

private:
    virtual void setter( void* aObject, QVariant& aValue ) = 0;
    virtual QVariant getter( const void* aObject ) const = 0;

private:
    const QString m_name;

    PROPERTY_DISPLAY m_display;

    ORIGIN_TRANSFORMS::COORD_TYPES_T m_coordType;

    bool m_hideFromPropertiesManager;
    bool m_hideFromLibraryEditors;
    bool m_hideFromDesignEditors;
    bool m_hideFromRulesEditor;

    QString m_group;

    std::function<bool(INSPECTABLE*)> m_availFunc;
    std::function<bool(INSPECTABLE*)> m_writeableFunc;

    PROPERTY_VALIDATOR_FN m_validator;

    friend class INSPECTABLE;
};


template<typename Owner, typename T, typename Base = Owner>
class PROPERTY : public PROPERTY_BASE
{
public:
    using BASE_TYPE = typename std::decay<T>::type;

    template<typename SetType, typename GetType>
    PROPERTY( const QString& aName,
              void ( Base::*aSetter )( SetType ),
              GetType( Base::*aGetter )(),
              PROPERTY_DISPLAY aDisplay = PT_DEFAULT,
              ORIGIN_TRANSFORMS::COORD_TYPES_T aCoordType = ORIGIN_TRANSFORMS::NOT_A_COORD ) :
        PROPERTY( aName,
                  METHOD<Owner, T, Base>::Wrap( aSetter ),
                  METHOD<Owner, T, Base>::Wrap( aGetter ),
                  aDisplay, aCoordType )
    {
    }

    template<typename SetType, typename GetType>
    PROPERTY( const QString& aName,
              void ( Base::*aSetter )( SetType ),
              GetType( Base::*aGetter )() const,
              PROPERTY_DISPLAY aDisplay = PT_DEFAULT,
              ORIGIN_TRANSFORMS::COORD_TYPES_T aCoordType = ORIGIN_TRANSFORMS::NOT_A_COORD ) :
        PROPERTY( aName,
                  METHOD<Owner, T, Base>::Wrap( aSetter ),
                  METHOD<Owner, T, Base>::Wrap( aGetter ),
                  aDisplay, aCoordType )
    {
    }

    size_t OwnerHash() const override
    {
        return m_ownerHash;
    }

    size_t BaseHash() const override
    {
        return m_baseHash;
    }

    size_t TypeHash() const override
    {
        return m_typeHash;
    }

    bool Writeable( INSPECTABLE* aObject ) const override
    {
        return m_setter && PROPERTY_BASE::Writeable( aObject );
    }

protected:
    PROPERTY( const QString& aName,
              SETTER_BASE<Owner, T>* s,
              GETTER_BASE<Owner, T>* g,
              PROPERTY_DISPLAY aDisplay, ORIGIN_TRANSFORMS::COORD_TYPES_T aCoordType ) :
        PROPERTY_BASE( aName, aDisplay, aCoordType ),
        m_setter( s ),
        m_getter( g ),
        m_ownerHash( TYPE_HASH( Owner ) ),
        m_baseHash( TYPE_HASH( Base ) ),
        m_typeHash( TYPE_HASH( BASE_TYPE ) )
    {
    }

    virtual ~PROPERTY() {}

    virtual void setter( void* obj, QVariant& v ) override
    {
        Q_ASSERT( m_setter );

        if( !v.canConvert<T>() )
            throw std::invalid_argument( "Invalid type requested" );

        Owner* o = reinterpret_cast<Owner*>( obj );
        BASE_TYPE value = v.value<BASE_TYPE>();
        (*m_setter)( o, value );
    }

    virtual QVariant getter( const void* obj ) const override
    {
        const Owner* o = reinterpret_cast<const Owner*>( obj );
        QVariant res = QVariant::fromValue((*m_getter)( o ));
        return res;
    }

    std::unique_ptr<SETTER_BASE<Owner, T>> m_setter;
    std::unique_ptr<GETTER_BASE<Owner, T>> m_getter;
    const size_t m_ownerHash;
    const size_t m_baseHash;
    const size_t m_typeHash;
};


template<typename Owner, typename T, typename Base = Owner>
class PROPERTY_ENUM : public PROPERTY<Owner, T, Base>
{
public:
    template<typename SetType, typename GetType>
    PROPERTY_ENUM( const QString& aName,
                   void ( Base::*aSetter )( SetType ),
                   GetType( Base::*aGetter )(),
                   PROPERTY_DISPLAY aDisplay = PT_DEFAULT ) :
          PROPERTY<Owner, T, Base>( aName,
                                    METHOD<Owner, T, Base>::Wrap( aSetter ),
                                    METHOD<Owner, T, Base>::Wrap( aGetter ),
                                    aDisplay )
    {
        if ( std::is_enum<T>::value )
        {
            m_choices = ENUM_MAP<T>::Instance().Choices();
            Q_ASSERT_X( m_choices.size() > 0, "PROPERTY_ENUM", "No enum choices defined" );
        }
    }

    template<typename SetType, typename GetType>
    PROPERTY_ENUM( const QString& aName,
                   void ( Base::*aSetter )( SetType ),
                   GetType( Base::*aGetter )() const,
                   PROPERTY_DISPLAY aDisplay = PT_DEFAULT,
                   ORIGIN_TRANSFORMS::COORD_TYPES_T aCoordType = ORIGIN_TRANSFORMS::NOT_A_COORD ) :
            PROPERTY<Owner, T, Base>( aName,
                                      METHOD<Owner, T, Base>::Wrap( aSetter ),
                                      METHOD<Owner, T, Base>::Wrap( aGetter ),
                                      aDisplay, aCoordType )
    {
        if ( std::is_enum<T>::value )
        {
            m_choices = ENUM_MAP<T>::Instance().Choices();
            Q_ASSERT_X( m_choices.size() > 0, "PROPERTY_ENUM", "No enum choices defined" );
        }
    }

    void setter( void* obj, QVariant& v ) override
    {
        Q_ASSERT( PROPERTY<Owner, T, Base>::m_setter );
        Owner* o = reinterpret_cast<Owner*>( obj );

        if( v.canConvert<T>() )
        {
            T value = v.value<T>();
            (*PROPERTY<Owner, T, Base>::m_setter)( o, value );
        }
        else if (v.canConvert<int>() )
        {
            int value = v.toInt();
            (*PROPERTY<Owner, T, Base>::m_setter)( o, static_cast<T>( value ) );
        }
        else
        {
            throw std::invalid_argument( "Invalid type requested" );
        }
    }

    QVariant getter( const void* obj ) const override
    {
        const Owner* o = reinterpret_cast<const Owner*>( obj );
        QVariant res = QVariant::fromValue(static_cast<T>( (*PROPERTY<Owner, T, Base>::m_getter)( o ) ));
        return res;
    }

    const QStringList& Choices() const override
    {
        return m_choices.size() > 0 ? m_choices : ENUM_MAP<T>::Instance().Choices();
    }

    void SetChoices( const QStringList& aChoices ) override
    {
        m_choices = aChoices;
    }

    bool HasChoices() const override
    {
        return Choices().size() > 0;
    }

protected:
    QStringList m_choices;
};


class TYPE_CAST_BASE
{
public:
    virtual ~TYPE_CAST_BASE() {}
    virtual void* operator()( void* aPointer ) const = 0;
    virtual const void* operator()( const void* aPointer ) const = 0;
    virtual size_t BaseHash() const = 0;
    virtual size_t DerivedHash() const = 0;
};


template<typename Base, typename Derived>
class TYPE_CAST : public TYPE_CAST_BASE
{
public:
    TYPE_CAST()
    {
    }

    void* operator()( void* aPointer ) const override
    {
        Base* base = reinterpret_cast<Base*>( aPointer );
        return static_cast<Derived*>( base );
    }

    const void* operator()( const void* aPointer ) const override
    {
        const Base* base = reinterpret_cast<const Base*>( aPointer );
        return static_cast<const Derived*>( base );
    }

    size_t BaseHash() const override
    {
        return TYPE_HASH( Base );
    }

    size_t DerivedHash() const override
    {
        return TYPE_HASH( Derived );
    }
};


template<typename T>
class ENUM_MAP
{
public:
    static ENUM_MAP<T>& Instance()
    {
        static ENUM_MAP<T> inst;
        return inst;
    }

    ENUM_MAP& Map( T aValue, const QString& aName )
    {
        m_choices.append( aName );
        m_reverseMap[ aName ] = aValue;
        return *this;
    }

    ENUM_MAP& Undefined( T aValue )
    {
        m_undefined = aValue;
        return *this;
    }

    const QString& ToString( T value ) const
    {
        static const QString s_undef = "UNDEFINED";

        auto it = std::find_if(m_reverseMap.begin(), m_reverseMap.end(),
                               [value](const auto& pair) { return pair.second == value; });

        if( it != m_reverseMap.end() )
            return it->first;
        else
            return s_undef;
    }

    bool IsValueDefined( T value ) const
    {
        auto it = std::find_if(m_reverseMap.begin(), m_reverseMap.end(),
                               [value](const auto& pair) { return pair.second == value; });

        return it != m_reverseMap.end();
    }

    T ToEnum( const QString value )
    {
        if( m_reverseMap.count( value ) )
            return m_reverseMap[ value ];
        else
            return m_undefined;
    }

    QStringList& Choices()
    {
        return m_choices;
    }

private:
    QStringList                     m_choices;
    std::unordered_map<QString, T>  m_reverseMap;
    T                               m_undefined;

    ENUM_MAP()
    {
    }
};


// Helper macros to handle enum types - Qt requires Q_DECLARE_METATYPE for custom types
#define DECLARE_ENUM_TO_QVARIANT( type )                                                   \
    Q_DECLARE_METATYPE( type )

#define IMPLEMENT_ENUM_TO_QVARIANT( type ) // No implementation needed for Qt

#define ENUM_TO_QVARIANT( type )                                                           \
    DECLARE_ENUM_TO_QVARIANT( type )

// Macro to define read-only fields (no setter method available)
#define NO_SETTER( owner, type ) ( ( void ( owner::* )( type ) ) nullptr )
#endif /* PROPERTY_H */
