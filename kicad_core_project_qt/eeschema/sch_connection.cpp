
#include <regex>
#include <QString>
#include <QStringList>

#include <connection_graph.h>
#include <sch_symbol.h>
#include <sch_pin.h>
#include <sch_screen.h>
#include <project/net_settings.h>
#include <advanced_config.h>
#include <string_utils.h>

#include <sch_connection.h>
#include <boost/algorithm/string/join.hpp>

/**
 *
 * Buses can be defined in multiple ways. A bus vector consists of a prefix and
 * a numeric range of suffixes:
 *
 *     BUS_NAME[M..N]
 *
 * For example, the bus A[3..0] will contain nets A3, A2, A1, and A0.
 * The BUS_NAME is required.  M and N must be integers but do not need to be in
 * any particular order -- A[0..3] produces the same result.
 *
 * Like net names, bus names cannot contain whitespace.
 *
 * A bus group is just a grouping of signals, separated by spaces, some
 * of which may be bus vectors.  Bus groups can have names, but do not need to.
 *
 *     MEMORY{A[15..0] D[7..0] RW CE OE}
 *
 * In named bus groups, the net names are expanded as <BUS_NAME>.<NET_NAME>
 * In the above example, the nets would be named like MEMORY.A15, MEMORY.D0, etc.
 *
 *     {USB_DP USB_DN}
 *
 * In the above example, the bus is unnamed and so the underlying net names are
 * just USB_DP and USB_DN.
 *
 */

SCH_CONNECTION::SCH_CONNECTION( SCH_ITEM* aParent, const SCH_SHEET_PATH& aPath ) :
        m_sheet( aPath ),
        m_local_sheet( aPath ),
        m_parent( aParent ),
        m_driver( nullptr ),
        m_graph( nullptr )
{
    Reset();
}


SCH_CONNECTION::SCH_CONNECTION( CONNECTION_GRAPH* aGraph ) :
        m_sheet( SCH_SHEET_PATH() ),
        m_local_sheet( SCH_SHEET_PATH() ),
        m_parent( nullptr ),
        m_driver( nullptr ),
        m_graph( aGraph )
{
    Reset();
}


SCH_CONNECTION::SCH_CONNECTION( SCH_CONNECTION& aOther ) :
        m_parent( nullptr ),
        m_driver( nullptr )
{
    Reset();
    Clone( aOther );
}


bool SCH_CONNECTION::operator==( const SCH_CONNECTION& aOther ) const
{
    // NOTE: Not comparing m_dirty or net/bus/subgraph codes
    if( ( aOther.m_driver == m_driver ) &&
        ( aOther.m_type == m_type ) &&
        ( aOther.m_name == m_name ) &&
        ( aOther.m_sheet == m_sheet ) )
    {
        return true;
    }

    return false;
}


void SCH_CONNECTION::SetDriver( SCH_ITEM* aItem )
{
    m_driver = aItem;

    recacheName();

    for( const std::shared_ptr<SCH_CONNECTION>& member : m_members )
        member->SetDriver( aItem );
}


void SCH_CONNECTION::SetSheet( const SCH_SHEET_PATH& aSheet )
{
    m_sheet       = aSheet;
    m_local_sheet = aSheet;

    recacheName();

    for( const std::shared_ptr<SCH_CONNECTION>& member : m_members )
        member->SetSheet( aSheet );
}


bool SCH_CONNECTION::operator!=( const SCH_CONNECTION& aOther ) const
{
    return !( aOther == *this );
}


void SCH_CONNECTION::ConfigureFromLabel( const QString& aLabel )
{
    m_members.clear();

    m_name         = aLabel;
    m_local_name   = aLabel;
    m_local_prefix = m_prefix;

    QString prefix;
    std::vector<QString> members;

    QString unescaped = UnescapeString( aLabel );

    if( NET_SETTINGS::ParseBusVector( unescaped, &prefix, &members ) )
    {
        m_type = CONNECTION_TYPE::BUS;
        m_vector_prefix = prefix;

        long i = 0;

        for( const QString& vector_member : members )
        {
            auto member            = std::make_shared<SCH_CONNECTION>( m_parent, m_sheet );
            member->m_type         = CONNECTION_TYPE::NET;
            member->m_prefix       = m_prefix;
            member->m_local_name   = vector_member;
            member->m_local_prefix = m_prefix;
            member->m_vector_index = i++;
            member->SetName( vector_member );
            member->SetGraph( m_graph );
            m_members.push_back( member );
        }
    }
    else if( NET_SETTINGS::ParseBusGroup( unescaped, &prefix, &members ) )
    {
        m_type       = CONNECTION_TYPE::BUS_GROUP;
        m_bus_prefix = prefix;

        // Named bus groups generate a net prefix, unnamed ones don't
        if( !prefix.isEmpty() )
            prefix += ".";

        for( const QString& group_member : members )
        {
            // Handle alias inside bus group member list
            if( auto alias = m_graph->GetBusAlias( group_member ) )
            {
                for( const QString& alias_member : alias->Members() )
                {
                    auto member = std::make_shared< SCH_CONNECTION >( m_parent, m_sheet );
                    member->SetPrefix( prefix );
                    member->SetGraph( m_graph );
                    member->ConfigureFromLabel( EscapeString( alias_member, CTX_NETNAME ) );
                    m_members.push_back( member );
                }
            }
            else
            {
                auto member = std::make_shared< SCH_CONNECTION >( m_parent, m_sheet );
                member->SetPrefix( prefix );
                member->SetGraph( m_graph );
                member->ConfigureFromLabel( group_member );
                m_members.push_back( member );
            }
        }
    }
    else
    {
        m_type = CONNECTION_TYPE::NET;
    }

    recacheName();
}


void SCH_CONNECTION::Reset()
{
    m_type = CONNECTION_TYPE::NONE;
    m_name.clear();
    m_local_name.clear();
    m_local_prefix.clear();
    m_cached_name.clear();
    m_cached_name_with_path.clear();
    m_prefix.clear();
    m_bus_prefix.clear();
    m_suffix.clear();
    m_lastDriver = m_driver;
    m_driver = nullptr;
    m_members.clear();
    m_dirty = true;
    m_net_code = 0;
    m_bus_code = 0;
    m_subgraph_code = 0;
    m_vector_start = 0;
    m_vector_end = 0;
    m_vector_index = 0;
    m_vector_prefix.clear();
}


void SCH_CONNECTION::Clone( const SCH_CONNECTION& aOther )
{
    m_graph = aOther.m_graph;

    // Note: m_lastDriver is not cloned as it needs to be the last driver of *this* connection
    m_driver = aOther.Driver();
    m_sheet  = aOther.Sheet();

    // Note: m_local_sheet is not cloned
    m_name   = aOther.m_name;
    m_type   = aOther.m_type;

    // Note: m_local_name is not cloned if not set yet
    if( m_local_name.isEmpty() )
    {
        m_local_name   = aOther.LocalName();
        m_local_prefix = aOther.Prefix();
    }

    m_prefix       = aOther.Prefix();

    // m_bus_prefix is not cloned; only used for local names
    m_suffix       = aOther.Suffix();
    m_net_code     = aOther.NetCode();
    m_bus_code     = aOther.BusCode();
    m_vector_start = aOther.VectorStart();
    m_vector_end   = aOther.VectorEnd();

    // Note: m_vector_index is not cloned
    m_vector_prefix = aOther.VectorPrefix();

    // Note: subgraph code isn't cloned, it should remain with the original object

    // Handle vector bus members: make sure local names are preserved where possible
    const std::vector<std::shared_ptr<SCH_CONNECTION>>& otherMembers = aOther.Members();

    if( m_type == CONNECTION_TYPE::BUS && aOther.Type() == CONNECTION_TYPE::BUS )
    {
        if( m_members.empty() )
        {
            m_members = otherMembers;
        }
        else
        {
            size_t cloneLimit = std::min( m_members.size(), otherMembers.size() );

            for( size_t i = 0; i < cloneLimit; ++i )
                m_members[i]->Clone( *otherMembers[i] );
        }
    }
    else if( m_type == CONNECTION_TYPE::BUS_GROUP && aOther.Type() == CONNECTION_TYPE::BUS_GROUP )
    {
        if( m_members.empty() )
        {
            m_members = otherMembers;
        }
        else
        {
            // TODO: refactor this once we support deep nesting
            for( std::shared_ptr<SCH_CONNECTION>& member : m_members )
            {
                auto it = std::find_if( otherMembers.begin(), otherMembers.end(),
                                        [&]( const std::shared_ptr<SCH_CONNECTION>& aTest )
                                        {
                                            return aTest->LocalName() == member->LocalName();
                                        } );

                if( it != otherMembers.end() )
                    member->Clone( **it );
            }
        }
    }

    m_type = aOther.Type();

    recacheName();
}


bool SCH_CONNECTION::IsDriver() const
{
    Q_ASSERT( Parent() );

    switch( Parent()->Type() )
    {
    case SCH_LABEL_T:
    case SCH_GLOBAL_LABEL_T:
    case SCH_HIER_LABEL_T:
    case SCH_SHEET_PIN_T:
    case SCH_SHEET_T:
        return true;

    case SCH_PIN_T:
    {
        const SCH_PIN* pin = static_cast<const SCH_PIN*>( Parent() );

        if( const SCH_SYMBOL* symbol = dynamic_cast<const SCH_SYMBOL*>( pin->GetParentSymbol() ) )
        {
            // Only annotated symbols should drive nets.
            return pin->IsGlobalPower() || symbol->IsAnnotated( &m_sheet );
        }

        return true;
    }

    default:
        return false;
    }
}


bool SCH_CONNECTION::HasDriverChanged() const
{
    return m_driver != m_lastDriver;
}


void SCH_CONNECTION::ClearDriverChanged()
{
    m_lastDriver = m_driver;
}



QString SCH_CONNECTION::Name( bool aIgnoreSheet ) const
{
    Q_ASSERT( !m_cached_name.isEmpty() );
    return aIgnoreSheet ? m_cached_name : m_cached_name_with_path;
}


QString SCH_CONNECTION::GetNetName() const
{
    QString retv;

    if( m_graph )
    {
        CONNECTION_SUBGRAPH* subgraph = m_graph->GetSubgraphForItem( m_parent );

        if( subgraph )
            retv = subgraph->GetNetName();
    }

    return retv;
}


void SCH_CONNECTION::recacheName()
{
    m_cached_name = m_name.isEmpty() ? QString( "<NO NET>" )
                                     : QString( m_prefix ) + m_name + m_suffix;

    bool prepend_path = true;

    if( !Parent() || m_type == CONNECTION_TYPE::NONE )
        prepend_path = false;

    if( m_driver )
    {
        switch( m_driver->Type() )
        {
        case SCH_GLOBAL_LABEL_T:
        case SCH_PIN_T:
            // Pins are either power connections or belong to a uniquely-annotated
            // symbol, so they don't need a path if they are driving the subgraph.
            prepend_path = false;
            break;

        default:
            break;
        }
    }

    m_cached_name_with_path = prepend_path ? m_sheet.PathHumanReadable() + m_cached_name
                                           : m_cached_name;
}


void SCH_CONNECTION::SetPrefix( const QString& aPrefix )
{
    m_prefix = aPrefix;

    recacheName();

    for( const std::shared_ptr<SCH_CONNECTION>& m : Members() )
        m->SetPrefix( aPrefix );
}


void SCH_CONNECTION::SetSuffix( const QString& aSuffix )
{
    m_suffix = aSuffix;

    recacheName();

    for( const std::shared_ptr<SCH_CONNECTION>& m : Members() )
        m->SetSuffix( aSuffix );
}


void SCH_CONNECTION::AppendInfoToMsgPanel( std::vector<MSG_PANEL_ITEM>& aList ) const
{
    QString msg, group_name, members;
    std::vector<QString> group_members;

    aList.emplace_back( "Connection Name", UnescapeString( Name() ) );

    if( std::shared_ptr<BUS_ALIAS> alias = m_graph->GetBusAlias( m_name ) )
    {
        msg = QString( "Bus Alias %1 Members" ).arg( m_name );
        aList.emplace_back( msg, boost::algorithm::join( alias->Members(), " " ) );
    }
    else if( NET_SETTINGS::ParseBusGroup( m_name, &group_name, &group_members ) )
    {
        for( const QString& group_member : group_members )
        {
            if( std::shared_ptr<BUS_ALIAS> group_alias = m_graph->GetBusAlias( group_member ) )
            {
                msg = QString( "Bus Alias %1 Members" ).arg( group_alias->GetName() );
                aList.emplace_back( msg, boost::algorithm::join( group_alias->Members(), " " ) );
            }
        }
    }

#if defined(DEBUG)
    // These messages are not flagged as translatable, because they are only debug messages

    if( IsBus() )
        aList.emplace_back( "Bus Code", QString::asprintf( "%d", m_bus_code ) );

    aList.emplace_back( "Subgraph Code", QString::asprintf( "%d", m_subgraph_code ) );

    if( SCH_ITEM* driver = Driver() )
    {
        UNITS_PROVIDER unitsProvider( schIUScale, EDA_UNITS::MM );

        msg = QString::asprintf( "%s at %p",
                    driver->GetItemDescription( &unitsProvider, false ).toStdString().c_str(),
                    driver );
        aList.emplace_back( "Connection Source", msg );
    }
#endif
}


bool SCH_CONNECTION::IsBusLabel( const QString& aLabel )
{
    const QString& unescaped = UnescapeString( aLabel );

    return NET_SETTINGS::ParseBusVector( unescaped, nullptr, nullptr )
                || NET_SETTINGS::ParseBusGroup( unescaped, nullptr, nullptr );
}


bool SCH_CONNECTION::MightBeBusLabel( const QString& aLabel )
{
    // Weak heuristic for performance reasons.  Stronger test will be used for connectivity
    QString label = UnescapeString( aLabel );

    return label.contains( "[" ) || label.contains( "{" );
}


const std::vector< std::shared_ptr< SCH_CONNECTION > > SCH_CONNECTION::AllMembers() const
{
    std::vector< std::shared_ptr< SCH_CONNECTION > > ret( m_members );

    for( const std::shared_ptr<SCH_CONNECTION>& member : m_members )
    {
        if( member->IsBus() )
            ret.insert( ret.end(), member->Members().begin(), member->Members().end() );
    }

    return ret;
}


static bool isSuperSubOverbar( QChar c )
{
    return c == '_' || c == '^' || c == '~';
};


QString SCH_CONNECTION::PrintBusForUI( const QString& aGroup )
{
    size_t   groupLen = aGroup.length();
    size_t   i = 0;
    QString ret;

    // Parse prefix
    //
    for( ; i < groupLen; ++i )
    {
        if( isSuperSubOverbar( aGroup[i] ) && i + 1 < groupLen && aGroup[i+1] == '{' )
        {
            i++;
            continue;
        }
        else if( aGroup[i] == '}' )
        {
            continue;
        }

        ret += aGroup[i];

        if( aGroup[i] == '{' )
            break;
    }

    // Parse members
    //
    i++;  // '{' character

    for( ; i < groupLen; ++i )
    {
        if( isSuperSubOverbar( aGroup[i] ) && i + 1 < groupLen && aGroup[i+1] == '{' )
        {
            i++;
            continue;
        }
        else if( aGroup[i] == '}' )
        {
            continue;
        }

        ret += aGroup[i];

        if( aGroup[i] == '}' )
            break;
    }

    return ret;
}


bool SCH_CONNECTION::IsSubsetOf( SCH_CONNECTION* aOther ) const
{
    if( !aOther->IsBus() )
        return false;

    for( const std::shared_ptr<SCH_CONNECTION>& member : aOther->Members() )
    {
        if( member->FullLocalName() == FullLocalName() )
            return true;
    }

    return false;
}


bool SCH_CONNECTION::IsMemberOfBus( SCH_CONNECTION* aOther ) const
{
    if( !aOther->IsBus() )
        return false;

    QString me = Name( true );

    for( const std::shared_ptr<SCH_CONNECTION>& m : aOther->Members() )
    {
        if( m->Name( true ) == me )
            return true;
    }

    return false;
}
