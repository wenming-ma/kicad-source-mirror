
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#include <cstdio>
#include <memory>
#include <mutex>
#include <QString>
#include <QRegularExpression>
#include <QtCore>
#include <i18n_utility.h>
#include <board.h>
#include <footprint.h>
#include <lset.h>
#include <board_connected_item.h>
#include <pcbexpr_evaluator.h>
#include <drc/drc_engine.h>

/* --------------------------------------------------------------------------------------------
 * Specialized Expression References
 */

BOARD_ITEM* PCBEXPR_VAR_REF::GetObject( const LIBEVAL::CONTEXT* aCtx ) const
{
    Q_ASSERT( dynamic_cast<const PCBEXPR_CONTEXT*>( aCtx ) );

    const PCBEXPR_CONTEXT* ctx = static_cast<const PCBEXPR_CONTEXT*>( aCtx );
    BOARD_ITEM*            item  = ctx->GetItem( m_itemIndex );
    return item;
}


class PCBEXPR_LAYER_VALUE : public LIBEVAL::VALUE
{
public:
    PCBEXPR_LAYER_VALUE( PCB_LAYER_ID aLayer ) :
        LIBEVAL::VALUE( LayerName( aLayer ) ),
        m_layer( aLayer )
    {};

    virtual bool EqualTo( LIBEVAL::CONTEXT* aCtx, const VALUE* b ) const override
    {
        // For boards with user-defined layer names there will be 2 entries for each layer
        // in the ENUM_MAP: one for the canonical layer name and one for the user layer name.
        // We need to check against both.

        auto&    layerMap = ENUM_MAP<PCB_LAYER_ID>::Instance().Choices();
        const QString& layerName = b->AsString();
        BOARD*          board = static_cast<PCBEXPR_CONTEXT*>( aCtx )->GetBoard();

        {
            std::shared_lock<std::shared_mutex> readLock( board->m_CachesMutex );

            auto i = board->m_LayerExpressionCache.find( layerName );

            if( i != board->m_LayerExpressionCache.end() )
                return i->second.Contains( m_layer );
        }

        LSET mask;

        for( unsigned ii = 0; ii < layerMap.count(); ++ii )
        {
            const QString& entry = layerMap[ii];

            if( QRegularExpression::fromWildcard( layerName, Qt::CaseInsensitive ).match( entry ).hasMatch() )
                mask.set( ToLAYER_ID( ENUM_MAP<PCB_LAYER_ID>::Instance().ToEnum( entry ) ) );
        }

        {
            std::unique_lock<std::shared_mutex> writeLock( board->m_CachesMutex );
            board->m_LayerExpressionCache[ layerName ] = mask;
        }

        return mask.Contains( m_layer );
    }

protected:
    PCB_LAYER_ID m_layer;
};


class PCBEXPR_PINTYPE_VALUE : public LIBEVAL::VALUE
{
public:
    PCBEXPR_PINTYPE_VALUE( const QString& aPinTypeName ) :
            LIBEVAL::VALUE( aPinTypeName )
    {};

    bool EqualTo( LIBEVAL::CONTEXT* aCtx, const VALUE* b ) const override
    {
        const QString& thisStr = AsString();
        const QString& otherStr = b->AsString();

        // Case insensitive
        if( thisStr.compare( otherStr, Qt::CaseInsensitive ) == 0 )
            return true;

        // Wildcards
        if( QRegularExpression::fromWildcard( otherStr, Qt::CaseInsensitive ).match( thisStr ).hasMatch() )
            return true;

        // Handle cases where the netlist token is different from the EEschema token
        QString altStr;

        if( thisStr == "tri_state" )
            altStr = "Tri-state";
        else if( thisStr == "power_in" )
            altStr = "Power input";
        else if( thisStr == "power_out" )
            altStr = "Power output";
        else if( thisStr == "no_connect" )
            altStr = "Unconnected";

        if( !altStr.isEmpty() )
        {
            // Case insensitive
            if( altStr.compare( otherStr, Qt::CaseInsensitive ) == 0 )
                return true;

            // Wildcards
            if( QRegularExpression::fromWildcard( otherStr, Qt::CaseInsensitive ).match( altStr ).hasMatch() )
                return true;
        }

        return false;
    }
};


class PCBEXPR_NETCLASS_VALUE : public LIBEVAL::VALUE
{
public:
    PCBEXPR_NETCLASS_VALUE( BOARD_CONNECTED_ITEM* aItem ) :
            LIBEVAL::VALUE( QString() ),
            m_item( aItem )
    {};

    const QString& AsString() const override
    {
        const_cast<PCBEXPR_NETCLASS_VALUE*>( this )->Set( m_item->GetEffectiveNetClass()->GetName() );
        return LIBEVAL::VALUE::AsString();
    }

    bool EqualTo( LIBEVAL::CONTEXT* aCtx, const VALUE* b ) const override
    {
        if( const PCBEXPR_NETCLASS_VALUE* bValue = dynamic_cast<const PCBEXPR_NETCLASS_VALUE*>( b ) )
            return *( m_item->GetEffectiveNetClass() ) == *( bValue->m_item->GetEffectiveNetClass() );

        if( b->GetType() == LIBEVAL::VT_STRING )
        {
            // Test constituent net class names. The effective net class name (e.g. CLASS1,CLASS2,OTHER_CLASS) is
            // tested in the fallthrough condition.
            for( const NETCLASS* nc : m_item->GetEffectiveNetClass()->GetConstituentNetclasses() )
            {
                const QString& ncName = nc->GetName();

                if( b->StringIsWildcard() )
                {
                    if( WildCompareString( b->AsString(), ncName, false ) )
                        return true;
                }
                else
                {
                    if( ncName.compare( b->AsString(), Qt::CaseInsensitive ) == 0 )
                        return true;
                }
            }
        }

        return LIBEVAL::VALUE::EqualTo( aCtx, b );
    }

    bool NotEqualTo( LIBEVAL::CONTEXT* aCtx, const LIBEVAL::VALUE* b ) const override
    {
        if( const PCBEXPR_NETCLASS_VALUE* bValue = dynamic_cast<const PCBEXPR_NETCLASS_VALUE*>( b ) )
            return *( m_item->GetEffectiveNetClass() ) != *( bValue->m_item->GetEffectiveNetClass() );

        if( b->GetType() == LIBEVAL::VT_STRING )
        {
            // Test constituent net class names
            bool isInConstituents = false;

            for( const NETCLASS* nc : m_item->GetEffectiveNetClass()->GetConstituentNetclasses() )
            {
                const QString& ncName = nc->GetName();

                if( b->StringIsWildcard() )
                {
                    if( WildCompareString( b->AsString(), ncName, false ) )
                    {
                        isInConstituents = true;
                        break;
                    }
                }
                else
                {
                    if( ncName.compare( b->AsString(), Qt::CaseInsensitive ) == 0 )
                    {
                        isInConstituents = true;
                        break;
                    }
                }
            }

            // Test effective net class name
            const bool isFullName = LIBEVAL::VALUE::EqualTo( aCtx, b );

            return !isInConstituents && !isFullName;
        }

        return LIBEVAL::VALUE::NotEqualTo( aCtx, b );
    }

protected:
    BOARD_CONNECTED_ITEM* m_item;
};


class PCBEXPR_COMPONENT_CLASS_VALUE : public LIBEVAL::VALUE
{
public:
    PCBEXPR_COMPONENT_CLASS_VALUE( BOARD_ITEM* aItem ) :
            LIBEVAL::VALUE( QString() ),
            m_item( dynamic_cast<FOOTPRINT*>( aItem ) )
    {};

    const QString& AsString() const override
    {
        if( !m_item )
            return LIBEVAL::VALUE::AsString();

        if( const COMPONENT_CLASS* compClass = m_item->GetComponentClass() )
            const_cast<PCBEXPR_COMPONENT_CLASS_VALUE*>( this )->Set( compClass->GetFullName() );

        return LIBEVAL::VALUE::AsString();
    }

    bool EqualTo( LIBEVAL::CONTEXT* aCtx, const VALUE* b ) const override
    {
        if( const PCBEXPR_COMPONENT_CLASS_VALUE* bValue = dynamic_cast<const PCBEXPR_COMPONENT_CLASS_VALUE*>( b ) )
        {
            if( !m_item || !bValue->m_item )
                return LIBEVAL::VALUE::EqualTo( aCtx, b );

            const COMPONENT_CLASS* aClass = m_item->GetComponentClass();
            const COMPONENT_CLASS* bClass = bValue->m_item->GetComponentClass();

            // Note this depends on COMPONENT_CLASS_MANAGER maintaining ownership
            // of all unique component class objects
            return aClass == bClass;
        }

        if( b->GetType() == LIBEVAL::VT_STRING )
        {
            // Test constituent component class names. The effective component class name
            // (e.g. CLASS1,CLASS2,OTHER_CLASS) is tested in the fallthrough condition.
            for( const COMPONENT_CLASS* cc : m_item->GetComponentClass()->GetConstituentClasses() )
            {
                const QString& ccName = cc->GetFullName();

                if( b->StringIsWildcard() )
                {
                    if( WildCompareString( b->AsString(), ccName, false ) )
                        return true;
                }
                else
                {
                    if( ccName.compare( b->AsString(), Qt::CaseInsensitive ) == 0 )
                        return true;
                }
            }
        }

        return LIBEVAL::VALUE::EqualTo( aCtx, b );
    }

    bool NotEqualTo( LIBEVAL::CONTEXT* aCtx, const LIBEVAL::VALUE* b ) const override
    {
        if( const PCBEXPR_COMPONENT_CLASS_VALUE* bValue = dynamic_cast<const PCBEXPR_COMPONENT_CLASS_VALUE*>( b ) )
        {
            if( !m_item || !bValue->m_item )
                return LIBEVAL::VALUE::NotEqualTo( aCtx, b );

            const COMPONENT_CLASS* aClass = m_item->GetComponentClass();
            const COMPONENT_CLASS* bClass = bValue->m_item->GetComponentClass();

            // Note this depends on COMPONENT_CLASS_MANAGER maintaining ownership
            // of all unique component class objects
            return aClass != bClass;
        }

        if( b->GetType() == LIBEVAL::VT_STRING )
        {
            // Test constituent component class names
            bool isInConstituents = false;

            for( const COMPONENT_CLASS* cc : m_item->GetComponentClass()->GetConstituentClasses() )
            {
                const QString& ccName = cc->GetFullName();

                if( b->StringIsWildcard() )
                {
                    if( WildCompareString( b->AsString(), ccName, false ) )
                    {
                        isInConstituents = true;
                        break;
                    }
                }
                else
                {
                    if( ccName.compare( b->AsString(), Qt::CaseInsensitive ) == 0 )
                    {
                        isInConstituents = true;
                        break;
                    }
                }
            }

            // Test effective component class name
            const bool isFullName = LIBEVAL::VALUE::EqualTo( aCtx, b );

            return !isInConstituents && !isFullName;
        }

        return LIBEVAL::VALUE::NotEqualTo( aCtx, b );
    }

protected:
    FOOTPRINT* m_item;
};


class PCBEXPR_NET_VALUE : public LIBEVAL::VALUE
{
public:
    PCBEXPR_NET_VALUE( BOARD_CONNECTED_ITEM* aItem ) :
            LIBEVAL::VALUE( QString() ),
            m_item( aItem )
    {};

    const QString& AsString() const override
    {
        const_cast<PCBEXPR_NET_VALUE*>( this )->Set( m_item->GetNetname() );
        return LIBEVAL::VALUE::AsString();
    }

    bool EqualTo( LIBEVAL::CONTEXT* aCtx, const VALUE* b ) const override
    {
        if( const PCBEXPR_NET_VALUE* bValue = dynamic_cast<const PCBEXPR_NET_VALUE*>( b ) )
            return m_item->GetNetCode() == bValue->m_item->GetNetCode();
        else
            return LIBEVAL::VALUE::EqualTo( aCtx, b );
    }

    bool NotEqualTo( LIBEVAL::CONTEXT* aCtx, const LIBEVAL::VALUE* b ) const override
    {
        if( const PCBEXPR_NET_VALUE* bValue = dynamic_cast<const PCBEXPR_NET_VALUE*>( b ) )
            return m_item->GetNetCode() != bValue->m_item->GetNetCode();
        else
            return LIBEVAL::VALUE::NotEqualTo( aCtx, b );
    }

protected:
    BOARD_CONNECTED_ITEM* m_item;
};


LIBEVAL::VALUE* PCBEXPR_VAR_REF::GetValue( LIBEVAL::CONTEXT* aCtx )
{
    PCBEXPR_CONTEXT* context = static_cast<PCBEXPR_CONTEXT*>( aCtx );

    if( m_type == LIBEVAL::VT_NULL )
        return LIBEVAL::VALUE::MakeNullValue();

    if( m_itemIndex == 2 )
        return new PCBEXPR_LAYER_VALUE( context->GetLayer() );

    BOARD_ITEM* item = GetObject( aCtx );

    if( !item )
        return new LIBEVAL::VALUE();

    auto it = m_matchingTypes.find( TYPE_HASH( *item ) );

    if( it == m_matchingTypes.end() )
    {
        // Don't force user to type "A.Type == 'via' && A.Via_Type == 'buried'" when the
        // simpler "A.Via_Type == 'buried'" is perfectly clear.  Instead, return an undefined
        // value when the property doesn't appear on a particular object.

        return new LIBEVAL::VALUE();
    }
    else
    {
        if( m_type == LIBEVAL::VT_NUMERIC )
        {
            if( m_isOptional )
            {
                std::optional<int> val = item->Get<std::optional<int>>( it->second );

                if( val.has_value() )
                    return new LIBEVAL::VALUE( static_cast<double>( val.value() ) );

                return LIBEVAL::VALUE::MakeNullValue();
            }

            return new LIBEVAL::VALUE( static_cast<double>( item->Get<int>( it->second ) ) );
        }
        else if( m_type == LIBEVAL::VT_NUMERIC_DOUBLE )
        {
            if( m_isOptional )
            {
                std::optional<double> val = item->Get<std::optional<double>>( it->second );

                if( val.has_value() )
                    return new LIBEVAL::VALUE( val.value() );

                return LIBEVAL::VALUE::MakeNullValue();
            }

            return new LIBEVAL::VALUE( item->Get<double>( it->second ) );
        }
        else
        {
            QString str;

            if( !m_isEnum )
            {
                str = item->Get<QString>( it->second );

                if( it->second->Name() == "Pin Type" )
                    return new PCBEXPR_PINTYPE_VALUE( str );
                else
                    return new LIBEVAL::VALUE( str );
            }
            else
            {
                const QVariant& any = item->Get( it->second );
                PCB_LAYER_ID layer;

                if( it->second->Name() == "Layer"
                        || it->second->Name() == "Layer Top"
                        || it->second->Name() == "Layer Bottom" )
                {
                    if( any.canConvert<PCB_LAYER_ID>() )
                    {
                        layer = any.value<PCB_LAYER_ID>();
                        return new PCBEXPR_LAYER_VALUE( layer );
                    }
                    else if( any.canConvert<QString>() )
                    {
                        str = any.toString();
                        return new PCBEXPR_LAYER_VALUE( context->GetBoard()->GetLayerID( str ) );
                    }
                }
                else
                {
                    if( any.canConvert<QString>() )
                    {
                        str = any.toString();
                        return new LIBEVAL::VALUE( str );
                    }
                }
            }

            return new LIBEVAL::VALUE();
        }
    }
}


LIBEVAL::VALUE* PCBEXPR_NETCLASS_REF::GetValue( LIBEVAL::CONTEXT* aCtx )
{
    BOARD_CONNECTED_ITEM* item = dynamic_cast<BOARD_CONNECTED_ITEM*>( GetObject( aCtx ) );

    if( !item )
        return new LIBEVAL::VALUE();

    return new PCBEXPR_NETCLASS_VALUE( item );
}


LIBEVAL::VALUE* PCBEXPR_COMPONENT_CLASS_REF::GetValue( LIBEVAL::CONTEXT* aCtx )
{
    BOARD_ITEM* item = dynamic_cast<BOARD_ITEM*>( GetObject( aCtx ) );

    if( !item || item->Type() != PCB_FOOTPRINT_T )
        return new LIBEVAL::VALUE();

    return new PCBEXPR_COMPONENT_CLASS_VALUE( item );
}


LIBEVAL::VALUE* PCBEXPR_NETNAME_REF::GetValue( LIBEVAL::CONTEXT* aCtx )
{
    BOARD_CONNECTED_ITEM* item = dynamic_cast<BOARD_CONNECTED_ITEM*>( GetObject( aCtx ) );

    if( !item )
        return new LIBEVAL::VALUE();

    return new PCBEXPR_NET_VALUE( item );
}


LIBEVAL::VALUE* PCBEXPR_TYPE_REF::GetValue( LIBEVAL::CONTEXT* aCtx )
{
    BOARD_ITEM* item = GetObject( aCtx );

    if( !item )
        return new LIBEVAL::VALUE();

    return new LIBEVAL::VALUE( ENUM_MAP<KICAD_T>::Instance().ToString( item->Type() ) );
}


LIBEVAL::FUNC_CALL_REF PCBEXPR_UCODE::CreateFuncCall( const QString& aName )
{
    PCBEXPR_BUILTIN_FUNCTIONS& registry = PCBEXPR_BUILTIN_FUNCTIONS::Instance();

    return registry.Get( aName.toLower() );
}


std::unique_ptr<LIBEVAL::VAR_REF> PCBEXPR_UCODE::CreateVarRef( const QString& aVar,
                                                               const QString& aField )
{
    PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
    std::unique_ptr<PCBEXPR_VAR_REF> vref;

    if( aVar.compare( "null", Qt::CaseInsensitive ) == 0 )
    {
        vref = std::make_unique<PCBEXPR_VAR_REF>( 0 );
        vref->SetType( LIBEVAL::VT_NULL );
        return std::move(vref);
    }

    // Check for a couple of very common cases and compile them straight to "object code".

    if( aField.compare( "NetClass", Qt::CaseInsensitive ) == 0 )
    {
        if( aVar == "A" )
            return std::make_unique<PCBEXPR_NETCLASS_REF>( 0 );
        else if( aVar == "B" )
            return std::make_unique<PCBEXPR_NETCLASS_REF>( 1 );
        else
            return nullptr;
    }
    else if( aField.compare( "ComponentClass", Qt::CaseInsensitive ) == 0 )
    {
        if( aVar == "A" )
            return std::make_unique<PCBEXPR_COMPONENT_CLASS_REF>( 0 );
        else if( aVar == "B" )
            return std::make_unique<PCBEXPR_COMPONENT_CLASS_REF>( 1 );
        else
            return nullptr;
    }
    else if( aField.compare( "NetName", Qt::CaseInsensitive ) == 0 )
    {
        if( aVar == "A" )
            return std::make_unique<PCBEXPR_NETNAME_REF>( 0 );
        else if( aVar == "B" )
            return std::make_unique<PCBEXPR_NETNAME_REF>( 1 );
        else
            return nullptr;
    }
    else if( aField.compare( "Type", Qt::CaseInsensitive ) == 0 )
    {
        if( aVar == "A" )
            return std::make_unique<PCBEXPR_TYPE_REF>( 0 );
        else if( aVar == "B" )
            return std::make_unique<PCBEXPR_TYPE_REF>( 1 );
        else
            return nullptr;
    }

    if( aVar == "A" || aVar == "AB" )
        vref = std::make_unique<PCBEXPR_VAR_REF>( 0 );
    else if( aVar == "B" )
        vref = std::make_unique<PCBEXPR_VAR_REF>( 1 );
    else if( aVar == "L" )
        vref = std::make_unique<PCBEXPR_VAR_REF>( 2 );
    else
        return nullptr;

    if( aField.length() == 0 ) // return reference to base object
        return std::move(vref);

    QString field( aField );
    field.replace( "_",  " " );

    for( const PROPERTY_MANAGER::CLASS_INFO& cls : propMgr.GetAllClasses() )
    {
        if( propMgr.IsOfType( cls.type, TYPE_HASH( BOARD_ITEM ) ) )
        {
            PROPERTY_BASE* prop = propMgr.GetProperty( cls.type, field );

            if( prop )
            {
                vref->AddAllowedClass( cls.type, prop );

                if( prop->TypeHash() == TYPE_HASH( int ) )
                {
                    vref->SetType( LIBEVAL::VT_NUMERIC );
                }
                else if( prop->TypeHash() == TYPE_HASH( std::optional<int> ) )
                {
                    vref->SetType( LIBEVAL::VT_NUMERIC );
                    vref->SetIsOptional();
                }
                else if( prop->TypeHash() == TYPE_HASH( double ) )
                {
                    vref->SetType( LIBEVAL::VT_NUMERIC_DOUBLE );
                }
                else if( prop->TypeHash() == TYPE_HASH( std::optional<double> ) )
                {
                    vref->SetType( LIBEVAL::VT_NUMERIC_DOUBLE );
                    vref->SetIsOptional();
                }
                else if( prop->TypeHash() == TYPE_HASH( bool ) )
                {
                    vref->SetType( LIBEVAL::VT_NUMERIC );
                }
                else if( prop->TypeHash() == TYPE_HASH( QString ) )
                {
                    vref->SetType( LIBEVAL::VT_STRING );
                }
                else if ( prop->HasChoices() )
                {   // it's an enum, we treat it as string
                    vref->SetType( LIBEVAL::VT_STRING );
                    vref->SetIsEnum( true );
                }
                else
                {
                    QString msg = QString::asprintf( "PCBEXPR_UCODE::createVarRef: Unknown "
                                                     "property type %s from %s.",
                                                     cls.name.data(),
                                                     field.toStdString().c_str() );
                    Q_ASSERT_X( false, "PCBEXPR_UCODE::createVarRef", msg.toStdString().c_str() );
                }
            }
        }
    }

    if( vref->GetType() == LIBEVAL::VT_UNDEFINED )
        vref->SetType( LIBEVAL::VT_PARSE_ERROR );

    return std::move(vref);
}


BOARD* PCBEXPR_CONTEXT::GetBoard() const
{
    if( m_items[0] )
        return m_items[0]->GetBoard();

    return nullptr;
}


/* --------------------------------------------------------------------------------------------
 * Unit Resolvers
 */

const std::vector<QString>& PCBEXPR_UNIT_RESOLVER::GetSupportedUnits() const
{
    static const std::vector<QString> pcbUnits = { "mil", "mm", "in",
                                                   "deg" };

    return pcbUnits;
}


QString PCBEXPR_UNIT_RESOLVER::GetSupportedUnitsMessage() const
{
    return _( "must be mm, in, or mil" );
}


double PCBEXPR_UNIT_RESOLVER::Convert( const QString& aString, int unitId ) const
{
    double v = aString.toDouble();

    switch( unitId )
    {
    case 0: return EDA_UNIT_UTILS::UI::DoubleValueFromString( pcbIUScale, EDA_UNITS::MILS, aString );
    case 1: return EDA_UNIT_UTILS::UI::DoubleValueFromString( pcbIUScale, EDA_UNITS::MM, aString );
    case 2: return EDA_UNIT_UTILS::UI::DoubleValueFromString( pcbIUScale, EDA_UNITS::INCH, aString );
    default: return v;
    }
};


const std::vector<QString>& PCBEXPR_UNITLESS_RESOLVER::GetSupportedUnits() const
{
    static const std::vector<QString> emptyUnits;

    return emptyUnits;
}


double PCBEXPR_UNITLESS_RESOLVER::Convert( const QString& aString, int unitId ) const
{
    return aString.toDouble();
};


PCBEXPR_COMPILER::PCBEXPR_COMPILER( LIBEVAL::UNIT_RESOLVER* aUnitResolver )
{
    m_unitResolver.reset( aUnitResolver );
}


/* --------------------------------------------------------------------------------------------
 * PCB Expression Evaluator
 */

PCBEXPR_EVALUATOR::PCBEXPR_EVALUATOR( LIBEVAL::UNIT_RESOLVER* aUnitResolver ) :
    m_result( 0 ),
    m_compiler( aUnitResolver ),
    m_ucode(),
    m_errorStatus()
{
}


PCBEXPR_EVALUATOR::~PCBEXPR_EVALUATOR()
{
}


bool PCBEXPR_EVALUATOR::Evaluate( const QString& aExpr )
{
    PCBEXPR_UCODE    ucode;
    PCBEXPR_CONTEXT  preflightContext( NULL_CONSTRAINT, F_Cu );

    if( !m_compiler.Compile( aExpr.toUtf8().data(), &ucode, &preflightContext ) )
        return false;

    PCBEXPR_CONTEXT  evaluationContext( NULL_CONSTRAINT, F_Cu );
    LIBEVAL::VALUE*  result = ucode.Run( &evaluationContext );

    if( result->GetType() == LIBEVAL::VT_NUMERIC )
        m_result = KiROUND( result->AsDouble() );

    return true;
}

