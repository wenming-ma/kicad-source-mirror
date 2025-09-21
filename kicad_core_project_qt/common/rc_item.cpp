// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#include <QWidget>
#include <QTreeView>
#include <QApplication>
#include <widgets/kitepui_common.h>
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


KIID RC_TREE_MODEL::ToUUID( const QModelIndex& aIndex )
{
    const RC_TREE_NODE* node = RC_TREE_MODEL::ToNode( aIndex );

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


RC_TREE_MODEL::RC_TREE_MODEL( EDA_DRAW_FRAME* aParentFrame, QTreeView* aView ) :
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
        QModelIndex currentIndex = m_view->currentIndex();
        RC_TREE_NODE* selectedNode = ToNode( currentIndex );
        selectedRcItem = selectedNode ? selectedNode->m_RcItem : nullptr;

        // Clear selection before rebuilding to avoid issues with deleted items
        m_view->clearSelection();
    }

    beginResetModel();

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
    endResetModel();

    // Model reset will clear the view
    m_view->setHeaderHidden( true );

    ExpandAll();

    // Qt provides better scroll position handling, but we'll restore selection similarly
    if( selectedRcItem )
    {
        for( size_t i = 0; i < m_tree.size(); ++i )
        {
            if( m_tree[i]->m_RcItem == selectedRcItem )
            {
                QModelIndex idx = index( i, 0 );
                m_view->setCurrentIndex( idx );
                m_view->scrollTo( idx );
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
    for( size_t i = 0; i < m_tree.size(); ++i )
        m_view->expand( index( i, 0 ) );
}


bool RC_TREE_MODEL::hasChildren( const QModelIndex& aParent ) const
{
    if( !aParent.isValid() )    // Root has children (the tree)
        return !m_tree.empty();
        
    RC_TREE_NODE* node = ToNode( aParent );
    return node && !node->m_Children.empty();
}


QModelIndex RC_TREE_MODEL::parent( const QModelIndex& aChild ) const
{
    if( !aChild.isValid() )
        return QModelIndex();
        
    RC_TREE_NODE* node = ToNode( aChild );
    if( !node || !node->m_Parent )
        return QModelIndex();
        
    // Find parent's row in its parent's children
    RC_TREE_NODE* parent = node->m_Parent;
    RC_TREE_NODE* grandparent = parent->m_Parent;
    
    if( !grandparent )  // Parent is a top-level node
    {
        for( size_t i = 0; i < m_tree.size(); ++i )
        {
            if( m_tree[i] == parent )
                return createIndex( i, 0, parent );
        }
    }
    else  // Parent is a child node
    {
        for( size_t i = 0; i < grandparent->m_Children.size(); ++i )
        {
            if( grandparent->m_Children[i] == parent )
                return createIndex( i, 0, parent );
        }
    }
    
    return QModelIndex();
}


int RC_TREE_MODEL::rowCount( const QModelIndex& aParent ) const
{
    if( !aParent.isValid() )
        return m_tree.size();
        
    RC_TREE_NODE* node = ToNode( aParent );
    return node ? node->m_Children.size() : 0;
}

int RC_TREE_MODEL::columnCount( const QModelIndex& aParent ) const
{
    return 1;  // Single column tree
}

QModelIndex RC_TREE_MODEL::index( int row, int column, const QModelIndex& parent ) const
{
    if( column != 0 || row < 0 )
        return QModelIndex();
        
    if( !parent.isValid() )  // Top level
    {
        if( row < (int)m_tree.size() )
            return createIndex( row, column, m_tree[row] );
    }
    else
    {
        RC_TREE_NODE* parentNode = ToNode( parent );
        if( parentNode && row < (int)parentNode->m_Children.size() )
            return createIndex( row, column, parentNode->m_Children[row] );
    }
    
    return QModelIndex();
}


QVariant RC_TREE_MODEL::data( const QModelIndex& aIndex, int aRole ) const
{
    if( !aIndex.isValid() || !m_view->updatesEnabled() )
        return QVariant();

    const RC_TREE_NODE*            node = ToNode( aIndex );
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
    
    if( aRole == Qt::DisplayRole )
        return msg;
    else if( aRole == Qt::FontRole || aRole == Qt::ForegroundRole )
    {
        QFont font;
        QColor textColor;
        if( GetAttr( aIndex, 0, font, textColor ) )
        {
            if( aRole == Qt::FontRole )
                return font;
            else
                return textColor;
        }
    }
    
    return QVariant();
}


bool RC_TREE_MODEL::GetAttr( const QModelIndex&     aIndex,
                             unsigned int            aCol,
                             QFont&                  aFont,
                             QColor&                 aTextColor ) const
{
    if( !aIndex.isValid() || !m_view->updatesEnabled() )
        return false;

    const RC_TREE_NODE* node = ToNode( aIndex );

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

    // Find the index for this node
    QModelIndex markerIndex;
    for( size_t i = 0; i < m_tree.size(); ++i )
    {
        if( m_tree[i] == aNode )
        {
            markerIndex = index( i, 0 );
            break;
        }
    }
    
    if( !markerIndex.isValid() )
        return;

    // Emit dataChanged signal for the node and its children
    emit dataChanged( markerIndex, markerIndex );

    for( size_t i = 0; i < aNode->m_Children.size(); ++i )
    {
        QModelIndex childIndex = index( i, 0, markerIndex );
        emit dataChanged( childIndex, childIndex );
    }

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
            beginInsertRows( markerIndex, aNode->m_Children.size(), aNode->m_Children.size() );
            commentNode = new RC_TREE_NODE( aNode, rcItem, RC_TREE_NODE::COMMENT );
            aNode->m_Children.push_back( commentNode );
            endInsertRows();
        }
        else if( commentNode && !needsCommentNode )
        {
            beginRemoveRows( markerIndex, aNode->m_Children.size() - 1, aNode->m_Children.size() - 1 );
            aNode->m_Children.erase( aNode->m_Children.end() - 1 );
            delete commentNode;
            endRemoveRows();
        }
    }
}


void RC_TREE_MODEL::DeleteCurrentItem( bool aDeep )
{
    DeleteItems( true, true, aDeep );
}


void RC_TREE_MODEL::DeleteItems( bool aCurrentOnly, bool aIncludeExclusions, bool aDeep )
{
    RC_TREE_NODE* current_node = m_view ? ToNode( m_view->currentIndex() ) : nullptr;
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
        for( size_t i = 0; i < m_tree.size(); ++i )
        {
            QModelIndex idx = index( i, 0 );
            if( m_view->isExpanded( idx ) )
                expanded.push_back( m_tree[i] );
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
            beginRemoveRows( QModelIndex(), i, i );
            
            for( RC_TREE_NODE* child : m_tree[i]->m_Children )
            {
                to_delete.push_back( child );
            }

            m_tree[i]->m_Children.clear();
            to_delete.push_back( m_tree[i] );
            m_tree.erase( m_tree.begin() + i );
            
            endRemoveRows();
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
            // Node may not exist in current tree after deletion
            for( size_t i = 0; i < m_tree.size(); ++i )
            {
                if( m_tree[i] == node )
                {
                    m_view->expand( index( i, 0 ) );
                    break;
                }
            }
        }

        QModelIndex selIndex = index( lastGood, 0 );
        m_view->setCurrentIndex( selIndex );

        // Qt automatically emits selection changed signals
    }

    for( RC_TREE_NODE* item : to_delete )
        delete( item );

    if( m_view )
        m_view->setUpdatesEnabled( true );
}


void RC_TREE_MODEL::PrevMarker()
{
    RC_TREE_NODE* currentNode = ToNode( m_view->currentIndex() );
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
    {
        for( size_t i = 0; i < m_tree.size(); ++i )
        {
            if( m_tree[i] == prevMarker )
            {
                m_view->setCurrentIndex( index( i, 0 ) );
                break;
            }
        }
    }
}


void RC_TREE_MODEL::NextMarker()
{
    RC_TREE_NODE* currentNode = ToNode( m_view->currentIndex() );

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
    {
        for( size_t i = 0; i < m_tree.size(); ++i )
        {
            if( m_tree[i] == nextMarker )
            {
                m_view->setCurrentIndex( index( i, 0 ) );
                break;
            }
        }
    }
}


void RC_TREE_MODEL::SelectMarker( const MARKER_BASE* aMarker )
{
    if( !m_view->updatesEnabled() )
        return;

    for( size_t i = 0; i < m_tree.size(); ++i )
    {
        if( m_tree[i]->m_RcItem->GetParent() == aMarker )
        {
            m_view->setCurrentIndex( index( i, 0 ) );
            return;
        }
    }
}


void RC_TREE_MODEL::CenterMarker( const MARKER_BASE* aMarker )
{
    if( !m_view->updatesEnabled() )
        return;

    for( size_t i = 0; i < m_tree.size(); ++i )
    {
        if( m_tree[i]->m_RcItem->GetParent() == aMarker )
        {
            m_view->scrollTo( index( i, 0 ) );
            return;
        }
    }
}
