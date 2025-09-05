

#include <QString>
#include <board_item.h>
#include <reporter.h>
#include <drc/drc_rule_condition.h>
#include <pcbexpr_evaluator.h>


DRC_RULE_CONDITION::DRC_RULE_CONDITION( const QString& aExpression ) :
    m_expression( aExpression ),
    m_ucode ( nullptr )
{
}


DRC_RULE_CONDITION::~DRC_RULE_CONDITION()
{
}


bool DRC_RULE_CONDITION::EvaluateFor( const BOARD_ITEM* aItemA, const BOARD_ITEM* aItemB,
                                      int aConstraint, PCB_LAYER_ID aLayer, REPORTER* aReporter )
{
    if( GetExpression().isEmpty() )
        return true;

    if( !m_ucode )
    {
        if( aReporter )
            aReporter->Report( _( "ERROR in expression." ) );

        return false;
    }

    PCBEXPR_CONTEXT ctx( aConstraint, aLayer );

    if( aReporter )
    {
        ctx.SetErrorCallback(
                [&]( const QString& aMessage, int aOffset )
                {
                    aReporter->Report( QStringLiteral( "ERROR: " ) + aMessage );
                } );
    }

    BOARD_ITEM* a = const_cast<BOARD_ITEM*>( aItemA );
    BOARD_ITEM* b = const_cast<BOARD_ITEM*>( aItemB );

    ctx.SetItems( a, b );

    if( m_ucode->Run( &ctx )->AsDouble() != 0.0 )
    {
        return true;
    }
    else if( aItemB )   // Conditions are commutative
    {
        ctx.SetItems( b, a );

        if( m_ucode->Run( &ctx )->AsDouble() != 0.0 )
            return true;
    }

    return false;
}


bool DRC_RULE_CONDITION::Compile( REPORTER* aReporter, int aSourceLine, int aSourceOffset )
{
    PCBEXPR_COMPILER compiler( new PCBEXPR_UNIT_RESOLVER() );

    if( aReporter )
    {
        compiler.SetErrorCallback(
                [&]( const QString& aMessage, int aOffset )
                {
                    QString rest;
                    int index = aMessage.indexOf( '|' );
                    QString first = index >= 0 ? aMessage.left( index ) : aMessage;
                    if( index >= 0 )
                        rest = aMessage.mid( index + 1 );
                    QString msg = QString::asprintf( "ERROR: <a href='%d:%d'>%s</a>%s",
                                                     aSourceLine,
                                                     aSourceOffset + aOffset,
                                                     qPrintable( first ),
                                                     qPrintable( rest ) );

                    aReporter->Report( msg, RPT_SEVERITY_ERROR );
                } );
    }

    m_ucode = std::make_unique<PCBEXPR_UCODE>();

    PCBEXPR_CONTEXT preflightContext( 0, F_Cu );

    bool ok = compiler.Compile( GetExpression().toUtf8().data(), m_ucode.get(), &preflightContext );
    return ok;
}


