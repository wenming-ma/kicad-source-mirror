#include <QWidget>
#include <QTreeWidget>
#include <QApplication>
#include "widgets/ui_common.h"
#include <marker_base.h>
#include <eda_draw_frame.h>
#include <rc_item.h>
#include <rc_json_schema.h>
#include <eda_item.h>
#include <base_units.h>

#define QT_TREEWIDGET_WINDOW_PADDING 6


QString RC_ITEM::GetErrorMessage() const
{
    if( m_errorMessage.isEmpty() )
        return GetErrorText();
    else
        return m_errorMessage;
}


static QString showCoord( UNITS_PROVIDER* aUnitsProvider, const VECTOR2I& aPos )
{
    return QString( "@(%1, %2)" ).arg(
                             aUnitsProvider->MessageTextFromValue( aPos.x ),
                             aUnitsProvider->MessageTextFromValue( aPos.y ) );
}


void RC_ITEM::AddItem( EDA_ITEM* aItem )
{
    m_ids.push_back( aItem->m_Uuid );
}


void RC_ITEM::SetItems( const EDA_ITEM* aItem, const EDA_ITEM* bItem,
                        const EDA_ITEM* cItem, const EDA_ITEM* dItem )
{
    m_ids.clear();

    if( aItem )
        m_ids.push_back( aItem->m_Uuid );

    if( bItem )
        m_ids.push_back( bItem->m_Uuid );

    if( cItem )
        m_ids.push_back( cItem->m_Uuid );

    if( dItem )
        m_ids.push_back( dItem->m_Uuid );
}


QString RC_ITEM::getSeverityString( SEVERITY aSeverity )
{
    QString severity;

    switch( aSeverity )
    {
    case RPT_SEVERITY_ERROR:     severity = "error";     break;
    case RPT_SEVERITY_WARNING:   severity = "warning";   break;
    case RPT_SEVERITY_ACTION:    severity = "action";    break;
    case RPT_SEVERITY_INFO:      severity = "info";      break;
    case RPT_SEVERITY_EXCLUSION: severity = "exclusion"; break;
    case RPT_SEVERITY_DEBUG:     severity = "debug";     break;
    default:;
    };

    return severity;
}


QString RC_ITEM::ShowReport( UNITS_PROVIDER* aUnitsProvider, SEVERITY aSeverity,
                              const std::map<KIID, EDA_ITEM*>& aItemMap ) const
{
    QString severity = getSeverityString( aSeverity );

    if( m_parent && m_parent->IsExcluded() )
        severity += " (excluded)";

    EDA_ITEM* mainItem = nullptr;
    EDA_ITEM* auxItem = nullptr;

    auto ii = aItemMap.find( GetMainItemID() );

    if( ii != aItemMap.end() )
        mainItem = ii->second;

    ii = aItemMap.find( GetAuxItemID() );

    if( ii != aItemMap.end() )
        auxItem = ii->second;

    // Note: some customers machine-process these.  So:
    // 1) don't translate
    // 2) try not to re-order or change syntax
    // 3) report settings key (which should be more stable) in addition to message

    QString msg;

    if( mainItem && auxItem )
    {
        msg = QString( "[%1]: %2\n    %3; %4\n    %5: %6\n    %7: %8\n" )
                    .arg( GetSettingsKey(),
                          GetErrorMessage(),
                          GetViolatingRuleDesc(),
                          severity,
                          showCoord( aUnitsProvider, mainItem->GetPosition()),
                          mainItem->GetItemDescription( aUnitsProvider, true ),
                          showCoord( aUnitsProvider, auxItem->GetPosition()),
                          auxItem->GetItemDescription( aUnitsProvider, true ) );
    }
    else if( mainItem )
    {
        msg = QString( "[%1]: %2\n    %3; %4\n    %5: %6\n" )
                    .arg( GetSettingsKey(),
                          GetErrorMessage(),
                          GetViolatingRuleDesc(),
                          severity,
                          showCoord( aUnitsProvider, mainItem->GetPosition()),
                          mainItem->GetItemDescription( aUnitsProvider, true ) );
    }
    else
    {
        msg = QString( "[%1]: %2\n    %3; %4\n" )
                    .arg( GetSettingsKey(),
                          GetErrorMessage(),
                          GetViolatingRuleDesc(),
                          severity );
    }

    if( m_parent && m_parent->IsExcluded() && !m_parent->GetComment().isEmpty() )
        msg += QString( "    %1\n" ).arg( m_parent->GetComment() );

    return msg;
}


void RC_ITEM::GetJsonViolation( RC_JSON::VIOLATION& aViolation, UNITS_PROVIDER* aUnitsProvider,
                                SEVERITY aSeverity,
                                const std::map<KIID, EDA_ITEM*>& aItemMap ) const
{
    QString severity = getSeverityString( aSeverity );

    aViolation.severity = severity;
    aViolation.description = GetErrorMessage();
    aViolation.type = GetSettingsKey();
    aViolation.excluded = ( m_parent && m_parent->IsExcluded() );

    EDA_ITEM* mainItem = nullptr;
    EDA_ITEM* auxItem = nullptr;

    auto ii = aItemMap.find( GetMainItemID() );

    if( ii != aItemMap.end() )
        mainItem = ii->second;

    ii = aItemMap.find( GetAuxItemID() );

    if( ii != aItemMap.end() )
        auxItem = ii->second;

    if( mainItem )
    {
        RC_JSON::AFFECTED_ITEM item;
        item.description = mainItem->GetItemDescription( aUnitsProvider, true );
        item.uuid = mainItem->m_Uuid.AsString();
        item.pos.x = EDA_UNIT_UTILS::UI::ToUserUnit( aUnitsProvider->GetIuScale(),
                                                     aUnitsProvider->GetUserUnits(),
                                                     mainItem->GetPosition().x );
        item.pos.y = EDA_UNIT_UTILS::UI::ToUserUnit( aUnitsProvider->GetIuScale(),
                                                     aUnitsProvider->GetUserUnits(),
                                                     mainItem->GetPosition().y );
        aViolation.items.emplace_back( item );
    }

    if( auxItem )
    {
        RC_JSON::AFFECTED_ITEM item;
        item.description = auxItem->GetItemDescription( aUnitsProvider, true );
        item.uuid = auxItem->m_Uuid.AsString();
        item.pos.x = EDA_UNIT_UTILS::UI::ToUserUnit( aUnitsProvider->GetIuScale(),
                                                     aUnitsProvider->GetUserUnits(),
                                                     auxItem->GetPosition().x );
        item.pos.y = EDA_UNIT_UTILS::UI::ToUserUnit( aUnitsProvider->GetIuScale(),
                                                     aUnitsProvider->GetUserUnits(),
                                                     auxItem->GetPosition().y );
        aViolation.items.emplace_back( item );
    }
}


KIID RC_TREE_MODEL::ToUUID( QTreeWidgetItem* aItem )
{
    const RC_TREE_NODE* node = RC_TREE_MODEL::ToNode( aItem );

    if( node && node->m_RcItem )
    {
        const std::shared_ptr<RC_ITEM> rc_item = node->m_RcItem;

        switch( node->m_Type )
        {
        case RC_TREE_NODE::MARKER:
        case RC_TREE_NODE::COMMENT:
            // rc_item->GetParent() can be null, if the parent is not existing
            // when a RC item has no corresponding ERC/DRC marker
            if( rc_item->GetParent() )
                return rc_item->GetParent()->GetUUID();

            break;

        case RC_TREE_NODE::MAIN_ITEM: return rc_item->GetMainItemID();
        case RC_TREE_NODE::AUX_ITEM:  return rc_item->GetAuxItemID();
        case RC_TREE_NODE::AUX_ITEM2: return rc_item->GetAuxItem2ID();
        case RC_TREE_NODE::AUX_ITEM3: return rc_item->GetAuxItem3ID();
        }
    }

    return niluuid;
}


RC_TREE_MODEL::RC_TREE_MODEL( EDA_DRAW_FRAME* aParentFrame, QTreeWidget* aView ) :
        m_editFrame( aParentFrame ),
        m_view( aView ),
        m_severities( 0 ),
        m_rcItemsProvider( nullptr )
{
}


RC_TREE_MODEL::~RC_TREE_MODEL()
{
    for( RC_TREE_NODE* topLevelNode : m_tree )
        delete topLevelNode;
}


void RC_TREE_MODEL::rebuildModel( std::shared_ptr<RC_ITEMS_PROVIDER> aProvider, int aSeverities )
{
    m_view->setUpdatesEnabled( false );

    std::shared_ptr<RC_ITEM> selectedRcItem = nullptr;

    if( m_view )
    {
        RC_TREE_NODE* selectedNode = ToNode( m_view->currentItem() );
        selectedRcItem = selectedNode ? selectedNode->m_RcItem : nullptr;

        // Clear selection before rebuilding to avoid issues with deleted items
        m_view->clearSelection();
    }

    BeforeReset();

    m_rcItemsProvider = std::move( aProvider );

    if( aSeverities != m_severities )
        m_severities = aSeverities;

    if( m_rcItemsProvider )
        m_rcItemsProvider->SetSeverities( m_severities );

    for( RC_TREE_NODE* topLevelNode : m_tree )
        delete topLevelNode;

    m_tree.clear();

    // QTreeWidget can handle large lists better than wxDataView
    int count = 0;

    if( m_rcItemsProvider )
        count = std::min( 1000, m_rcItemsProvider->GetCount() );

    for( int i = 0; i < count; ++i )
    {
        std::shared_ptr<RC_ITEM> rcItem = m_rcItemsProvider->GetItem( i );

        m_tree.push_back( new RC_TREE_NODE( nullptr, rcItem, RC_TREE_NODE::MARKER ) );
        RC_TREE_NODE* n = m_tree.back();

        if( rcItem->GetMainItemID() != niluuid )
            n->m_Children.push_back( new RC_TREE_NODE( n, rcItem, RC_TREE_NODE::MAIN_ITEM ) );

        if( rcItem->GetAuxItemID() != niluuid )
            n->m_Children.push_back( new RC_TREE_NODE( n, rcItem, RC_TREE_NODE::AUX_ITEM ) );

        if( rcItem->GetAuxItem2ID() != niluuid )
            n->m_Children.push_back( new RC_TREE_NODE( n, rcItem, RC_TREE_NODE::AUX_ITEM2 ) );

        if( rcItem->GetAuxItem3ID() != niluuid )
            n->m_Children.push_back( new RC_TREE_NODE( n, rcItem, RC_TREE_NODE::AUX_ITEM3 ) );

        if( MARKER_BASE* marker = rcItem->GetParent() )
        {
            if( marker->IsExcluded() && !marker->GetComment().isEmpty() )
                n->m_Children.push_back( new RC_TREE_NODE( n, rcItem, RC_TREE_NODE::COMMENT ) );
        }
    }

    // Must be called after a significant change of items to force the
    // tree model to reread all of them, repopulating itself entirely.
    AfterReset();

    m_view->clear();
    m_view->setHeaderHidden( true );

    ExpandAll();

    // Qt provides better scroll position handling, but we'll restore selection similarly
    if( selectedRcItem )
    {
        for( RC_TREE_NODE* candidate : m_tree )
        {
            if( candidate->m_RcItem == selectedRcItem )
            {
                QTreeWidgetItem* item = ToItem( candidate );
                m_view->setCurrentItem( item );
                m_view->scrollToItem( item );
                break;
            }
        }
    }

    m_view->setUpdatesEnabled( true );
}


void RC_TREE_MODEL::Update( std::shared_ptr<RC_ITEMS_PROVIDER> aProvider, int aSeverities )
{
    rebuildModel( aProvider, aSeverities );
}


void RC_TREE_MODEL::ExpandAll()
{
    for( RC_TREE_NODE* topLevelNode : m_tree )
        m_view->expandItem( ToItem( topLevelNode ) );
}


bool RC_TREE_MODEL::IsContainer( QTreeWidgetItem* const aItem ) const
{
    if( ToNode( aItem ) == nullptr )    // must be tree root...
        return true;
    else
        return ToNode( aItem )->m_Type == RC_TREE_NODE::MARKER;
}


QTreeWidgetItem* RC_TREE_MODEL::GetParent( QTreeWidgetItem* const aItem ) const
{
    return ToItem( ToNode( aItem)->m_Parent );
}


unsigned int RC_TREE_MODEL::GetChildren( QTreeWidgetItem* const aItem,
                                         QVector<QTreeWidgetItem*>&  aChildren ) const
{
    const RC_TREE_NODE* node = ToNode( aItem );
    const std::vector<RC_TREE_NODE*>& children = node ? node->m_Children : m_tree;

    for( const RC_TREE_NODE* child: children )
        aChildren.push_back( ToItem( child ) );

    return children.size();
}


void RC_TREE_MODEL::GetValue( QString&                aText,
                              QTreeWidgetItem*        aItem,
                              unsigned int            aCol ) const
{
    if( !aItem || !m_view->updatesEnabled() )
        return;

    const RC_TREE_NODE*            node = ToNode( aItem );
    const std::shared_ptr<RC_ITEM> rcItem = node->m_RcItem;
    MARKER_BASE*                   marker = rcItem->GetParent();
    EDA_ITEM*                      item = nullptr;
    QString                        msg;

    switch( node->m_Type )
    {
    case RC_TREE_NODE::MARKER:
        if( marker )
        {
            SEVERITY severity = marker->GetSeverity();

            if( severity == RPT_SEVERITY_EXCLUSION )
            {
                if( m_editFrame->GetSeverity( rcItem->GetErrorCode() ) == RPT_SEVERITY_WARNING )
                    msg = "Excluded warning: ";
                else
                    msg = "Excluded error: ";
            }
            else if( severity == RPT_SEVERITY_WARNING )
            {
                msg = "Warning: ";
            }
            else
            {
                msg = "Error: ";
            }
        }

        msg += rcItem->GetErrorMessage();
        break;

    case RC_TREE_NODE::MAIN_ITEM:
        if( marker && marker->GetMarkerType() == MARKER_BASE::MARKER_DRAWING_SHEET )
            msg = "Drawing Sheet";
        else
            item = m_editFrame->GetItem( rcItem->GetMainItemID() );

        break;

    case RC_TREE_NODE::AUX_ITEM:
        item = m_editFrame->GetItem( rcItem->GetAuxItemID() );
        break;

    case RC_TREE_NODE::AUX_ITEM2:
        item = m_editFrame->GetItem( rcItem->GetAuxItem2ID() );
        break;

    case RC_TREE_NODE::AUX_ITEM3:
        item = m_editFrame->GetItem( rcItem->GetAuxItem3ID() );
        break;

    case RC_TREE_NODE::COMMENT:
        if( marker )
            msg = marker->GetComment();

        break;
    }

    if( item )
        msg += item->GetItemDescription( m_editFrame, true );

    msg.replace( "\n", " " );
    aText = msg;
}


bool RC_TREE_MODEL::GetAttr( QTreeWidgetItem*        aItem,
                             unsigned int            aCol,
                             QFont&                  aFont,
                             QColor&                 aTextColor ) const
{
    if( !aItem || !m_view->updatesEnabled() )
        return false;

    const RC_TREE_NODE* node = ToNode( aItem );

    bool ret = false;
    bool heading = node->m_Type == RC_TREE_NODE::MARKER;

    if( heading )
    {
        aFont.setBold( true );
        ret = true;
    }

    if( node->m_RcItem->GetParent()
            && node->m_RcItem->GetParent()->GetSeverity() == RPT_SEVERITY_EXCLUSION )
    {
        QColor textColour = QApplication::palette().color( QPalette::Text );
        double brightness = ( textColour.red() + textColour.green() + textColour.blue() ) / (3.0 * 255.0);

        if( brightness > 0.5 )
        {
            int lightness = static_cast<int>( brightness * ( heading ? 50 : 60 ) );
            aTextColor = textColour.lighter( lightness );
        }
        else
        {
            aTextColor = textColour.lighter( heading ? 170 : 165 );
        }

        aFont.setItalic( true );   // Strikethrough would be better, if Qt supported it better
        ret = true;
    }

    return ret;
}


void RC_TREE_MODEL::ValueChanged( RC_TREE_NODE* aNode )
{
    if( aNode->m_Type != RC_TREE_NODE::MARKER )
    {
        ValueChanged( aNode->m_Parent );
        return;
    }

    QTreeWidgetItem* markerItem = ToItem( aNode );

    // Qt automatically updates the display when we change item text or properties
    m_view->update( m_view->indexFromItem( markerItem ) );

    for( const RC_TREE_NODE* child : aNode->m_Children )
        m_view->update( m_view->indexFromItem( ToItem( child ) ) );

    // Comment items can come and go depending on exclusion state and comment content.
    const std::shared_ptr<RC_ITEM> rcItem = aNode->m_RcItem;
    MARKER_BASE*                   marker = rcItem ? rcItem->GetParent() : nullptr;

    if( marker )
    {
        bool          needsCommentNode = marker->IsExcluded() && !marker->GetComment().isEmpty();
        RC_TREE_NODE* commentNode = aNode->m_Children.empty() ? nullptr : aNode->m_Children.back();

        if( commentNode && commentNode->m_Type != RC_TREE_NODE::COMMENT )
            commentNode = nullptr;

        if( needsCommentNode && !commentNode )
        {
            commentNode = new RC_TREE_NODE( aNode, rcItem, RC_TREE_NODE::COMMENT );
            QTreeWidgetItem* newItem = ToItem( commentNode );

            aNode->m_Children.push_back( commentNode );
            markerItem->addChild( newItem );
        }
        else if( commentNode && !needsCommentNode )
        {
            QTreeWidgetItem* itemToDelete = ToItem( commentNode );
            
            aNode->m_Children.erase( aNode->m_Children.end() - 1 );
            markerItem->removeChild( itemToDelete );
            delete itemToDelete;
        }
    }
}


void RC_TREE_MODEL::DeleteCurrentItem( bool aDeep )
{
    DeleteItems( true, true, aDeep );
}


void RC_TREE_MODEL::DeleteItems( bool aCurrentOnly, bool aIncludeExclusions, bool aDeep )
{
    RC_TREE_NODE* current_node = m_view ? ToNode( m_view->currentItem() ) : nullptr;
    const std::shared_ptr<RC_ITEM> current_item = current_node ? current_node->m_RcItem : nullptr;

    // Keep a vector of elements to free after Qt is definitely done accessing them
    std::vector<RC_TREE_NODE*> to_delete;
    std::vector<RC_TREE_NODE*> expanded;

    if( aCurrentOnly && !current_item )
    {
        QApplication::beep();
        return;
    }

    // Qt preserves expanded state better, but we'll track it anyway
    if( m_view && aCurrentOnly )
    {
        for( RC_TREE_NODE* node : m_tree )
        {
            if( m_view->isItemExpanded( ToItem( node ) ) )
                expanded.push_back( node );
        }
    }

    int  lastGood = -1;
    bool itemDeleted = false;

    if( m_view )
    {
        m_view->clearSelection();
        QApplication::processEvents();
        m_view->setUpdatesEnabled( false );
    }

    if( !m_rcItemsProvider )
        return;

    for( int i = m_rcItemsProvider->GetCount() - 1; i >= 0; --i )
    {
        std::shared_ptr<RC_ITEM> rcItem = m_rcItemsProvider->GetItem( i );
        MARKER_BASE*             marker = rcItem->GetParent();
        bool                     excluded = false;

        if( marker && marker->GetSeverity() == RPT_SEVERITY_EXCLUSION )
            excluded = true;

        if( aCurrentOnly && itemDeleted && lastGood >= 0 )
            break;

        if( aCurrentOnly && rcItem != current_item )
        {
            lastGood = i;
            continue;
        }

        if( excluded && !aIncludeExclusions )
            continue;

        if( i < (int) m_tree.size() )   // Careful; tree is truncated for large datasets
        {
            QTreeWidgetItem*    markerItem = ToItem( m_tree[i] );
            QTreeWidgetItem*    parentItem = ToItem( m_tree[i]->m_Parent );

            for( RC_TREE_NODE* child : m_tree[i]->m_Children )
            {
                to_delete.push_back( child );
            }

            m_tree[i]->m_Children.clear();
            
            // Remove from tree widget
            if( parentItem )
                parentItem->removeChild( markerItem );
            else
                m_view->takeTopLevelItem( m_view->indexOfTopLevelItem( markerItem ) );

            to_delete.push_back( m_tree[i] );
            m_tree.erase( m_tree.begin() + i );
        }

        // Only deep delete the current item here; others will be done by the caller, which
        // can more efficiently delete all markers on the board.
        m_rcItemsProvider->DeleteItem( i, aDeep && aCurrentOnly );

        if( lastGood > i )
            lastGood--;

        itemDeleted = true;
    }

    if( m_view && aCurrentOnly && lastGood >= 0 )
    {
        for( RC_TREE_NODE* node : expanded )
        {
            QTreeWidgetItem* item = ToItem( node );

            if( item )
                m_view->expandItem( item );
        }

        QTreeWidgetItem* selItem = ToItem( m_tree[ lastGood ] );
        m_view->setCurrentItem( selItem );

        // Qt automatically emits selection changed signals
    }

    for( RC_TREE_NODE* item : to_delete )
        delete( item );

    if( m_view )
        m_view->setUpdatesEnabled( true );
}


void RC_TREE_MODEL::PrevMarker()
{
    RC_TREE_NODE* currentNode = ToNode( m_view->currentItem() );
    RC_TREE_NODE* prevMarker = nullptr;

    while( currentNode && currentNode->m_Type != RC_TREE_NODE::MARKER )
        currentNode = currentNode->m_Parent;

    for( RC_TREE_NODE* candidate : m_tree )
    {
        if( candidate == currentNode )
            break;
        else
            prevMarker = candidate;
    }

    if( prevMarker )
        m_view->setCurrentItem( ToItem( prevMarker ) );
}


void RC_TREE_MODEL::NextMarker()
{
    RC_TREE_NODE* currentNode = ToNode( m_view->currentItem() );

    while( currentNode && currentNode->m_Type != RC_TREE_NODE::MARKER )
        currentNode = currentNode->m_Parent;

    RC_TREE_NODE* nextMarker = nullptr;
    bool          trigger = currentNode == nullptr;

    for( RC_TREE_NODE* candidate : m_tree )
    {
        if( candidate == currentNode )
        {
            trigger = true;
        }
        else if( trigger )
        {
            nextMarker = candidate;
            break;
        }
    }

    if( nextMarker )
        m_view->setCurrentItem( ToItem( nextMarker ) );
}


void RC_TREE_MODEL::SelectMarker( const MARKER_BASE* aMarker )
{
    if( !m_view->updatesEnabled() )
        return;

    for( RC_TREE_NODE* candidate : m_tree )
    {
        if( candidate->m_RcItem->GetParent() == aMarker )
        {
            m_view->setCurrentItem( ToItem( candidate ) );
            return;
        }
    }
}


void RC_TREE_MODEL::CenterMarker( const MARKER_BASE* aMarker )
{
    if( !m_view->updatesEnabled() )
        return;

    for( RC_TREE_NODE* candidate : m_tree )
    {
        if( candidate->m_RcItem->GetParent() == aMarker )
        {
            m_view->scrollToItem( ToItem( candidate ) );
            return;
        }
    }
}
