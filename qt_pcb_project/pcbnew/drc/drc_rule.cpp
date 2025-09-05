

#include <board.h>
#include <board_item.h>
#include <drc/drc_rule.h>
#include <drc/drc_rule_condition.h>


DRC_RULE::DRC_RULE() :
        m_Unary( false ),
        m_Implicit( false ),
        m_ImplicitItemId( 0 ),
        m_LayerCondition( LSET::AllLayersMask() ),
        m_Condition( nullptr ),
        m_Severity( RPT_SEVERITY_UNDEFINED )
{
}


DRC_RULE::DRC_RULE( const QString& aName ) :
        m_Unary( false ),
        m_Implicit( false ),
        m_ImplicitItemId( 0 ),
        m_Name( aName ),
        m_LayerCondition( LSET::AllLayersMask() ),
        m_Condition( nullptr ),
        m_Severity( RPT_SEVERITY_UNDEFINED )
{
}


DRC_RULE::~DRC_RULE()
{
    delete m_Condition;
}


void DRC_RULE::AddConstraint( DRC_CONSTRAINT& aConstraint )
{
    aConstraint.SetParentRule( this );
    m_Constraints.push_back( aConstraint );
}


std::optional<DRC_CONSTRAINT> DRC_RULE::FindConstraint( DRC_CONSTRAINT_T aType )
{
    for( DRC_CONSTRAINT& c : m_Constraints)
    {
        if( c.m_Type == aType )
            return c;
    }

    return std::optional<DRC_CONSTRAINT>();
}