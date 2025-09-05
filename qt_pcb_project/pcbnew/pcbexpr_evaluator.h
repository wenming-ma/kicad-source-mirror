

#ifndef PCBEXPR_EVALUATOR_H
#define PCBEXPR_EVALUATOR_H

#include <unordered_map>
#include <QString>
#include <QStringList>

#include <properties/property.h>
#include <properties/property_mgr.h>

#include <libeval_compiler/libeval_compiler.h>

class BOARD;
class BOARD_ITEM;

class PCBEXPR_VAR_REF;

class PCBEXPR_UCODE final : public LIBEVAL::UCODE
{
public:
    PCBEXPR_UCODE() {};
    virtual ~PCBEXPR_UCODE() {};

    virtual std::unique_ptr<LIBEVAL::VAR_REF> CreateVarRef( const QString& aVar,
                                                            const QString& aField ) override;
    virtual LIBEVAL::FUNC_CALL_REF CreateFuncCall( const QString& aName ) override;
};


class PCBEXPR_CONTEXT : public LIBEVAL::CONTEXT
{
public:
    PCBEXPR_CONTEXT( int aConstraint = 0, PCB_LAYER_ID aLayer = F_Cu ) :
            m_constraint( aConstraint ),
            m_layer( aLayer )
    {
        m_items[0] = nullptr;
        m_items[1] = nullptr;
    }

    void SetItems( BOARD_ITEM* a, BOARD_ITEM* b = nullptr )
    {
        m_items[0] = a;
        m_items[1] = b;
    }

    BOARD* GetBoard() const;

    int GetConstraint() const              { return m_constraint; }
    BOARD_ITEM* GetItem( int index ) const { return m_items[index]; }
    PCB_LAYER_ID GetLayer() const          { return m_layer; }

private:
    int          m_constraint;
    BOARD_ITEM*  m_items[2];
    PCB_LAYER_ID m_layer;
};


class PCBEXPR_VAR_REF : public LIBEVAL::VAR_REF
{
public:
    PCBEXPR_VAR_REF( int aItemIndex ) :
            m_itemIndex( aItemIndex ),
            m_type( LIBEVAL::VT_UNDEFINED ),
            m_isEnum( false ),
            m_isOptional( false )
    {}

    ~PCBEXPR_VAR_REF() {};

    void SetIsEnum( bool s ) { m_isEnum = s; }
    bool IsEnum() const { return m_isEnum; }

    void SetIsOptional( bool s = true ) { m_isOptional = s; }
    bool IsOptional() const { return m_isOptional; }

    void SetType( LIBEVAL::VAR_TYPE_T type ) { m_type = type; }
    LIBEVAL::VAR_TYPE_T GetType() const override { return m_type; }

    void AddAllowedClass( TYPE_ID type_hash, PROPERTY_BASE* prop )
    {
        m_matchingTypes[type_hash] = prop;
    }

    LIBEVAL::VALUE* GetValue( LIBEVAL::CONTEXT* aCtx ) override;

    BOARD_ITEM* GetObject( const LIBEVAL::CONTEXT* aCtx ) const;

private:
    std::unordered_map<TYPE_ID, PROPERTY_BASE*> m_matchingTypes;
    int                                         m_itemIndex;
    LIBEVAL::VAR_TYPE_T                         m_type;
    bool                                        m_isEnum;
    bool                                        m_isOptional;
};


// "Object code" version of a netclass reference (for performance).
class PCBEXPR_NETCLASS_REF : public PCBEXPR_VAR_REF
{
public:
    PCBEXPR_NETCLASS_REF( int aItemIndex ) :
            PCBEXPR_VAR_REF( aItemIndex )
    {
        SetType( LIBEVAL::VT_STRING );
    }

    LIBEVAL::VALUE* GetValue( LIBEVAL::CONTEXT* aCtx ) override;
};


// "Object code" version of a component class reference (for performance).
class PCBEXPR_COMPONENT_CLASS_REF : public PCBEXPR_VAR_REF
{
public:
    PCBEXPR_COMPONENT_CLASS_REF( int aItemIndex ) : PCBEXPR_VAR_REF( aItemIndex )
    {
        SetType( LIBEVAL::VT_STRING );
    }

    LIBEVAL::VALUE* GetValue( LIBEVAL::CONTEXT* aCtx ) override;
};


// "Object code" version of a netname reference (for performance).
class PCBEXPR_NETNAME_REF : public PCBEXPR_VAR_REF
{
public:
    PCBEXPR_NETNAME_REF( int aItemIndex ) :
            PCBEXPR_VAR_REF( aItemIndex )
    {
        SetType( LIBEVAL::VT_STRING );
    }

    LIBEVAL::VALUE* GetValue( LIBEVAL::CONTEXT* aCtx ) override;
};


class PCBEXPR_TYPE_REF : public PCBEXPR_VAR_REF
{
public:
    PCBEXPR_TYPE_REF( int aItemIndex ) :
            PCBEXPR_VAR_REF( aItemIndex )
    {
        SetType( LIBEVAL::VT_STRING );
    }

    LIBEVAL::VALUE* GetValue( LIBEVAL::CONTEXT* aCtx ) override;
};


class PCBEXPR_BUILTIN_FUNCTIONS
{
public:
    PCBEXPR_BUILTIN_FUNCTIONS();

    static PCBEXPR_BUILTIN_FUNCTIONS& Instance()
    {
        static PCBEXPR_BUILTIN_FUNCTIONS self;
        return self;
    }

    LIBEVAL::FUNC_CALL_REF Get( const QString& name )
    {
        return m_funcs[ name ];
    }

    const QStringList GetSignatures() const
    {
        return m_funcSigs;
    }

    void RegisterFunc( const QString& funcSignature, LIBEVAL::FUNC_CALL_REF funcPtr )
    {
        QString funcName = funcSignature.section( '(', 0, 0 );
        m_funcs[std::string( funcName.toLower().toStdString() )] = std::move( funcPtr );
        m_funcSigs.append( funcSignature );
    }

    void RegisterAllFunctions();

private:
    std::map<QString, LIBEVAL::FUNC_CALL_REF> m_funcs;

    QStringList m_funcSigs;
};


class PCBEXPR_UNIT_RESOLVER : public LIBEVAL::UNIT_RESOLVER
{
public:
    const std::vector<QString>& GetSupportedUnits() const override;

    QString GetSupportedUnitsMessage() const override;

    double Convert( const QString& aString, int unitId ) const override;
};


class PCBEXPR_UNITLESS_RESOLVER : public LIBEVAL::UNIT_RESOLVER
{
public:
    const std::vector<QString>& GetSupportedUnits() const override;

    double Convert( const QString& aString, int unitId ) const override;
};


class PCBEXPR_COMPILER : public LIBEVAL::COMPILER
{
public:
    PCBEXPR_COMPILER( LIBEVAL::UNIT_RESOLVER* aUnitResolver );
};


class PCBEXPR_EVALUATOR
{
public:
    PCBEXPR_EVALUATOR( LIBEVAL::UNIT_RESOLVER* aUnitResolver );
    ~PCBEXPR_EVALUATOR();

    bool Evaluate( const QString& aExpr );
    int  Result() const { return m_result; }

    void SetErrorCallback( std::function<void( const QString& aMessage, int aOffset )> aCallback )
    {
        m_compiler.SetErrorCallback( aCallback );
    }

    bool IsErrorPending() const { return m_errorStatus.pendingError; }
    const LIBEVAL::ERROR_STATUS& GetError() const { return m_errorStatus; }

private:
    int  m_result;

    PCBEXPR_COMPILER      m_compiler;
    PCBEXPR_UCODE         m_ucode;
    LIBEVAL::ERROR_STATUS m_errorStatus;
};

#endif
