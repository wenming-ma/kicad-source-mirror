// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#include <eda_base_frame.h>
#include <eda_pattern_match.h>
#include <kiface_base.h>
#include <kiplatform/ui.h>
#include <lib_tree_model_adapter.h>
#include <project/project_file.h>
#include <settings/app_settings.h>
#include <widgets/kiui_common.h>
#include <QStringList>
#include <QRegularExpression>
#include <QApplication>
#include <QPainter>
#include <QTreeView>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QFontMetrics>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <string_utils.h>
#include <i18n_utility.h>


static const int kDataViewIndent = 20;


class LIB_TREE_RENDERER : public QStyledItemDelegate
{
public:
    LIB_TREE_RENDERER() :
            m_canvasItem( false )
    {}

    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const override
    {
        QFontMetrics fm( option.font );
        return QSize( option.rect.width(), fm.height() + 2 );
    }

    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const override
    {
        QStyledItemDelegate::paint( painter, option, index );

        if( m_canvasItem )
        {
            QPoint points[6];
            points[0] = option.rect.topLeft();
            points[1] = option.rect.topRight() + QPoint( -4, 0 );
            points[2] = option.rect.topRight() + QPoint( 0, option.rect.height() / 2 );
            points[3] = option.rect.bottomRight() + QPoint( -4, 1 );
            points[4] = option.rect.bottomLeft() + QPoint( 0, 1 );
            points[5] = option.rect.topLeft();

            QPen pen( KIPLATFORM::UI::IsDarkTheme() ? Qt::white : Qt::black );
            painter->setPen( pen );
            painter->drawPolyline( points, 6 );
        }
    }

private:
    bool    m_canvasItem;
    QString m_text;
};


QModelIndex LIB_TREE_MODEL_ADAPTER::ToItem( const LIB_TREE_NODE* aNode ) const
{
    return createIndex( 0, 0, const_cast<void*>( static_cast<void const*>( aNode ) ) );
}


LIB_TREE_NODE* LIB_TREE_MODEL_ADAPTER::ToNode( const QModelIndex& aItem )
{
    return static_cast<LIB_TREE_NODE*>( aItem.internalPointer() );
}


LIB_TREE_MODEL_ADAPTER::LIB_TREE_MODEL_ADAPTER( EDA_BASE_FRAME* aParent,
                                                const QString& aPinnedKey,
                                                APP_SETTINGS_BASE::LIB_TREE& aSettingsStruct ) :
        m_widget( nullptr ),
        m_parent( aParent ),
        m_cfg( aSettingsStruct ),
        m_sort_mode( BEST_MATCH ),
        m_show_units( true ),
        m_preselect_unit( 0 ),
        m_freeze( 0 ),
        m_filter( nullptr )
{
    // Default column widths.  Do not translate these names.
    m_colWidths[ QString(_HKI( "Item" )) ] = 300;
    m_colWidths[ QString(_HKI( "Description" )) ] = 600;

    m_availableColumns = { QString(_HKI( "Item" )), QString(_HKI( "Description" )) };

    for( const auto& pair : m_cfg.column_widths )
    {
        const QString& key = pair.first;
        int value = pair.second;
        m_colWidths[key] = value;
    }

    m_shownColumns.clear();
    for( const QString& col : m_cfg.columns )
        m_shownColumns.push_back( col );

    if( m_shownColumns.empty() )
        m_shownColumns = { QString(_HKI( "Item" )), QString(_HKI( "Description" )) };

    if( m_shownColumns[0] != QString(_HKI( "Item" )) )
        m_shownColumns.insert( m_shownColumns.begin(), QString(_HKI( "Item" )) );
}


LIB_TREE_MODEL_ADAPTER::~LIB_TREE_MODEL_ADAPTER()
{}


std::vector<QString> LIB_TREE_MODEL_ADAPTER::GetOpenLibs() const
{
    std::vector<QString> openLibs;
    QModelIndex          rootItem;
    QModelIndexList      children;

    GetChildren( rootItem, children );

    for( const QModelIndex& child : children )
    {
        if( m_widget->isExpanded( child ) )
            openLibs.emplace_back( ToNode( child )->m_LibId.GetUniStringLibNickname() );
    }

    return openLibs;
}


void LIB_TREE_MODEL_ADAPTER::OpenLibs( const std::vector<QString>& aLibs )
{
    m_widget->setUpdatesEnabled( false );

    for( const QString& lib : aLibs )
    {
        QModelIndex item = FindItem( LIB_ID( lib, QString() ) );

        if( item.isValid() )
            m_widget->expand( item );
    }

    m_widget->setUpdatesEnabled( true );
}


void LIB_TREE_MODEL_ADAPTER::SaveSettings()
{
    if( m_widget )
    {
        m_cfg.columns.clear();
        for( const QString& col : GetShownColumns() )
            m_cfg.columns.push_back( col );
        m_cfg.column_widths.clear();

        for( const auto& pair : m_colNameMap )
        {
            const QString& colName = pair.first;
            QHeaderView* headerView = pair.second;
            if( headerView )
                m_cfg.column_widths[colName] = headerView->sectionSize( 0 );
        }

        m_cfg.open_libs.clear();
        for( const QString& lib : GetOpenLibs() )
            m_cfg.open_libs.push_back( lib );
    }
}


void LIB_TREE_MODEL_ADAPTER::ShowUnits( bool aShow )
{
    m_show_units = aShow;
}


void LIB_TREE_MODEL_ADAPTER::SetPreselectNode( const LIB_ID& aLibId, int aUnit )
{
    m_preselect_lib_id = aLibId;
    m_preselect_unit = aUnit;
}


LIB_TREE_NODE_LIBRARY& LIB_TREE_MODEL_ADAPTER::DoAddLibraryNode( const QString& aNodeName,
                                                                 const QString& aDesc,
                                                                 bool pinned )
{
    LIB_TREE_NODE_LIBRARY& lib_node = m_tree.AddLib( aNodeName, aDesc );

    lib_node.m_Pinned = pinned;

    return lib_node;
}


LIB_TREE_NODE_LIBRARY& LIB_TREE_MODEL_ADAPTER::DoAddLibrary( const QString& aNodeName,
                                                             const QString& aDesc,
                                                             const std::vector<LIB_TREE_ITEM*>& aItemList,
                                                             bool pinned, bool presorted )
{
    LIB_TREE_NODE_LIBRARY& lib_node = DoAddLibraryNode( aNodeName, aDesc, pinned );

    for( LIB_TREE_ITEM* item: aItemList )
        lib_node.AddItem( item );

    lib_node.AssignIntrinsicRanks( presorted );

    return lib_node;
}


void LIB_TREE_MODEL_ADAPTER::RemoveGroup( bool aRecentGroup, bool aPlacedGroup )
{
    m_tree.RemoveGroup( aRecentGroup, aPlacedGroup );
}


void LIB_TREE_MODEL_ADAPTER::UpdateSearchString( const QString& aSearch, bool aState )
{
    {
        m_widget->setUpdatesEnabled( false );

        // Clear selection to avoid issues with deleted rows
        m_widget->clearSelection();

        // Collapse tree before search to avoid issues with expanded elements
        if( !aState && !aSearch.isNull() && m_tree.m_Children.size() )
        {
            for( std::unique_ptr<LIB_TREE_NODE>& child: m_tree.m_Children )
                m_widget->collapse( ToItem( &*child ) );
        }

        // DO NOT REMOVE THE FREEZE/THAW. This freeze/thaw is a flag for this model adapter
        // that tells it when it shouldn't trust any of the data in the model. When set, it will
        // not return invalid data to the UI, since this invalid data can cause crashes.
        // This is different than the update locker, which locks the UI aspects only.
        Freeze();
        beginResetModel();

        // Don't cause KiCad to hang if someone accidentally pastes the PCB or schematic into
        // the search box.
        constexpr int MAX_TERMS = 100;

        QStringList                                        tokens = aSearch.split( QRegularExpression( "\\s+" ), Qt::SkipEmptyParts );
        std::vector<std::unique_ptr<EDA_COMBINED_MATCHER>> termMatchers;

        for( const QString& token : tokens )
        {
            if( termMatchers.size() >= MAX_TERMS )
                break;
            QString term = token.toLower();
            termMatchers.emplace_back( std::make_unique<EDA_COMBINED_MATCHER>( term, CTX_LIBITEM ) );
        }

        m_tree.UpdateScore( termMatchers, m_filter );

        m_tree.SortNodes( m_sort_mode == BEST_MATCH );
        endResetModel();
        Thaw();

        m_widget->setUpdatesEnabled( true );
    }

    const LIB_TREE_NODE* firstMatch = ShowResults();

    if( firstMatch )
    {
        QModelIndex item = ToItem( firstMatch );
        m_widget->selectionModel()->select( item, QItemSelectionModel::ClearAndSelect );

        // Make sure the *parent* item is visible. The selected item is the first (shown) child
        // of the parent. So it's always right below the parent, and this way the user can also
        // see what library the selected part belongs to, without having a case where the selection
        // is off the screen (unless the window is a single row high, which is unlikely).
        {
            QModelIndex parent = item.parent();

            if( parent.isValid() )
                m_widget->scrollTo( parent );
        }

        m_widget->scrollTo( item );
    }
}


void LIB_TREE_MODEL_ADAPTER::AttachTo( QTreeView* aTreeView )
{
    m_widget = aTreeView;
    aTreeView->setIndentation( kDataViewIndent );
    aTreeView->setModel( this );
    recreateColumns();
}


void LIB_TREE_MODEL_ADAPTER::recreateColumns()
{
    // Clear existing columns in header
    m_widget->header()->hide();

    m_columns.clear();
    m_colIdxMap.clear();
    m_colNameMap.clear();

    // The Item column is always shown
    doAddColumn( QString("Item") );

    for( const QString& colName : m_shownColumns )
    {
        if( !m_colNameMap.count( colName ) )
            doAddColumn( colName, colName == QString("Description") );
    }

    m_widget->header()->show();
}


void LIB_TREE_MODEL_ADAPTER::resortTree()
{
    Freeze();
    beginResetModel();

    m_tree.SortNodes( m_sort_mode == BEST_MATCH );

    endResetModel();
    Thaw();
}


void LIB_TREE_MODEL_ADAPTER::PinLibrary( LIB_TREE_NODE* aTreeNode )
{
    m_parent->Prj().PinLibrary( aTreeNode->m_LibId.GetLibNickname(), getLibType() );
    aTreeNode->m_Pinned = true;

    resortTree();
    m_widget->scrollTo( ToItem( aTreeNode ) );
}


void LIB_TREE_MODEL_ADAPTER::UnpinLibrary( LIB_TREE_NODE* aTreeNode )
{
    m_parent->Prj().UnpinLibrary( aTreeNode->m_LibId.GetLibNickname(), getLibType() );
    aTreeNode->m_Pinned = false;

    resortTree();
    // Keep focus at top when unpinning
}


void LIB_TREE_MODEL_ADAPTER::ShowChangedLanguage()
{
    recreateColumns();

    for( const std::unique_ptr<LIB_TREE_NODE>& lib: m_tree.m_Children )
    {
        if( lib->m_IsRecentlyUsedGroup )
            lib->m_Name = QString( "-- " + _( "Recently Used" ) + " --" );
        else if( lib->m_IsAlreadyPlacedGroup )
            lib->m_Name = QString( "-- " + _( "Already Placed" ) + " --" );
    }
}


QHeaderView* LIB_TREE_MODEL_ADAPTER::doAddColumn( const QString& aHeader, bool aTranslate )
{
    QString translatedHeader = aTranslate ? QObject::tr( aHeader.toUtf8() ) : aHeader;

    // The extent of the text doesn't take into account the space on either side
    // in the header, so artificially pad it
    QFontMetrics fm( m_widget->font() );
    QSize headerMinWidth = QSize( fm.horizontalAdvance( translatedHeader + "MMM" ), fm.height() );

    if( !m_colWidths.count( aHeader ) || m_colWidths[aHeader] < headerMinWidth.width() )
        m_colWidths[aHeader] = headerMinWidth.width();

    int index = (int) m_columns.size();

    // Set header data for the model
    setHeaderData( index, Qt::Horizontal, translatedHeader, Qt::DisplayRole );

    QHeaderView* header = m_widget->header();
    header->setSectionResizeMode( index, QHeaderView::Interactive );
    header->resizeSection( index, m_colWidths[aHeader] );
    header->setMinimumSectionSize( headerMinWidth.width() );

    m_columns.emplace_back( header );
    m_colNameMap[aHeader] = header;
    m_colIdxMap[index] = aHeader;

    return header;
}


void LIB_TREE_MODEL_ADAPTER::addColumnIfNecessary( const QString& aHeader )
{
    if( m_colNameMap.count( aHeader ) )
        return;

    // Columns will be created later
    m_colNameMap[aHeader] = nullptr;
    m_availableColumns.emplace_back( aHeader );
}


void LIB_TREE_MODEL_ADAPTER::SetShownColumns( const std::vector<QString>& aColumnNames )
{
    bool recreate = m_shownColumns != aColumnNames;

    m_shownColumns = aColumnNames;

    if( recreate && m_widget )
        recreateColumns();
}


LIB_ID LIB_TREE_MODEL_ADAPTER::GetAliasFor( const QModelIndex& aSelection ) const
{
    const LIB_TREE_NODE* node = ToNode( aSelection );
    return node ? node->m_LibId : LIB_ID();
}


int LIB_TREE_MODEL_ADAPTER::GetUnitFor( const QModelIndex& aSelection ) const
{
    const LIB_TREE_NODE* node = ToNode( aSelection );
    return node ? node->m_Unit : 0;
}


LIB_TREE_NODE::TYPE LIB_TREE_MODEL_ADAPTER::GetTypeFor( const QModelIndex& aSelection ) const
{
    const LIB_TREE_NODE* node = ToNode( aSelection );
    return node ? node->m_Type : LIB_TREE_NODE::TYPE::INVALID;
}


LIB_TREE_NODE* LIB_TREE_MODEL_ADAPTER::GetTreeNodeFor( const QModelIndex& aSelection ) const
{
    return ToNode( aSelection );
}


int LIB_TREE_MODEL_ADAPTER::GetItemCount() const
{
    int n = 0;

    for( const std::unique_ptr<LIB_TREE_NODE>& lib: m_tree.m_Children )
        n += lib->m_Children.size();

    return n;
}


QModelIndex LIB_TREE_MODEL_ADAPTER::FindItem( const LIB_ID& aLibId )
{
    for( std::unique_ptr<LIB_TREE_NODE>& lib: m_tree.m_Children )
    {
        if( lib->m_Name != aLibId.GetUniStringLibNickname() )
            continue;

        // if part name is not specified, return the library node
        if( aLibId.GetLibItemName().empty() )
            return ToItem( lib.get() );

        for( std::unique_ptr<LIB_TREE_NODE>& alias: lib->m_Children )
        {
            if( alias->m_Name == aLibId.GetUniStringLibItemName() )
                return ToItem( alias.get() );
        }

        break;  // could not find the part in the requested library
    }

    return QModelIndex();
}


QModelIndex LIB_TREE_MODEL_ADAPTER::GetCurrentDataViewItem()
{
    return FindItem( m_preselect_lib_id );
}


unsigned int LIB_TREE_MODEL_ADAPTER::GetChildren( const QModelIndex&  aItem,
                                                  QModelIndexList&    aChildren ) const
{
    const LIB_TREE_NODE* node = ( aItem.isValid() ? ToNode( aItem ) : &m_tree );
    unsigned int         count = 0;

    if( node->m_Type == LIB_TREE_NODE::TYPE::ROOT
            || node->m_Type == LIB_TREE_NODE::TYPE::LIBRARY
            || ( m_show_units && node->m_Type == LIB_TREE_NODE::TYPE::ITEM ) )
    {
        for( std::unique_ptr<LIB_TREE_NODE> const& child: node->m_Children )
        {
            if( child->m_Score > 0 )
            {
                aChildren.append( ToItem( &*child ) );
                ++count;
            }
        }
    }

    return count;
}


void LIB_TREE_MODEL_ADAPTER::FinishTreeInitialization()
{
    QHeaderView* header_view     = m_widget->header();
    size_t       idx             = 0;
    int          totalWidth      = 0;
    QString      header;

    for( ; idx < m_columns.size() - 1; idx++ )
    {
        Q_ASSERT( m_colIdxMap.count( idx ) );

        header = m_colIdxMap[idx];

        Q_ASSERT( m_colWidths.count( header ) );

        header_view->resizeSection( idx, m_colWidths[header] );
        totalWidth += header_view->sectionSize( idx );
    }

    int remainingWidth = m_widget->size().width() - totalWidth;
    header = m_colIdxMap[idx];

    header_view->resizeSection( idx, std::max( m_colWidths[header], remainingWidth ) );
}


void LIB_TREE_MODEL_ADAPTER::RefreshTree()
{
    // Yes, this is an enormous hack.  But it works on all platforms, it doesn't suffer
    // the On^2 sorting issues that ItemChanged() does on OSX, and it doesn't lose the
    // user's scroll position (which re-attaching or deleting/re-inserting columns does).
    static int walk = 1;

    std::vector<int> widths;

    QHeaderView* header_view = m_widget->header();
    for( int i = 0; i < m_columns.size(); ++i )
        widths.emplace_back( header_view->sectionSize( i ) );

    Q_ASSERT( widths.size() );

    // Only use the widths read back if they are non-zero.
    // GTK returns the displayed width of the column, which is not calculated immediately
    if( widths[0] > 0 )
    {
        size_t i = 0;

        for( const auto& pair : m_colNameMap )
        {
            const QString& colName = pair.first;
            m_colWidths[ colName ] = widths[i++];
        }
    }

    auto colIt = m_colWidths.begin();

    colIt->second += walk;
    colIt++;

    if( colIt != m_colWidths.end() )
        colIt->second -= walk;

    QHeaderView* header_view2 = m_widget->header();
    for( const auto& colPair : m_colNameMap )
    {
        const QString& colName = colPair.first;
        QHeaderView* colPtr = colPair.second;

        if( colPtr == m_columns[0] )
            continue;

        Q_ASSERT( m_colWidths.count( colName ) );
        // Find the column index for this column name
        for( const auto& idxPair : m_colIdxMap )
        {
            unsigned idx = idxPair.first;
            const QString& name = idxPair.second;

            if( name == colName )
            {
                header_view2->resizeSection( idx, m_colWidths[colName] );
                break;
            }
        }
    }

    walk = -walk;
}


bool LIB_TREE_MODEL_ADAPTER::hasChildren( const QModelIndex& parent ) const
{
    LIB_TREE_NODE* node = ToNode( parent );
    return node ? node->m_Children.size() > 0 : true;
}


int LIB_TREE_MODEL_ADAPTER::rowCount( const QModelIndex& parent ) const
{
    const LIB_TREE_NODE* node = ( parent.isValid() ? ToNode( parent ) : &m_tree );

    if( node->m_Type == LIB_TREE_NODE::TYPE::ROOT
            || node->m_Type == LIB_TREE_NODE::TYPE::LIBRARY
            || ( m_show_units && node->m_Type == LIB_TREE_NODE::TYPE::ITEM ) )
    {
        int count = 0;
        for( const std::unique_ptr<LIB_TREE_NODE>& child: node->m_Children )
        {
            if( child->m_Score > 0 )
                ++count;
        }
        return count;
    }

    return 0;
}


int LIB_TREE_MODEL_ADAPTER::columnCount( const QModelIndex& parent ) const
{
    return m_columns.size();
}


QModelIndex LIB_TREE_MODEL_ADAPTER::parent( const QModelIndex& child ) const
{
    if( m_freeze || !child.isValid() )
        return QModelIndex();

    LIB_TREE_NODE* node   = ToNode( child );
    LIB_TREE_NODE* parent_node = node ? node->m_Parent : nullptr;

    // QAbstractItemModel has no root node, but rather top-level elements have
    // an invalid (null) parent.
    if( !node || !parent_node || parent_node->m_Type == LIB_TREE_NODE::TYPE::ROOT )
        return QModelIndex();
    else
        return ToItem( parent_node );
}


QModelIndex LIB_TREE_MODEL_ADAPTER::index( int row, int column, const QModelIndex& parent ) const
{
    if( !hasIndex( row, column, parent ) )
        return QModelIndex();

    const LIB_TREE_NODE* parent_node = ( parent.isValid() ? ToNode( parent ) : &m_tree );

    int current_row = 0;
    for( const std::unique_ptr<LIB_TREE_NODE>& child: parent_node->m_Children )
    {
        if( child->m_Score > 0 )
        {
            if( current_row == row )
                return createIndex( row, column, child.get() );
            ++current_row;
        }
    }

    return QModelIndex();
}


QVariant LIB_TREE_MODEL_ADAPTER::data( const QModelIndex& index, int role ) const
{
    if( !index.isValid() || IsFrozen() )
        return QVariant();

    LIB_TREE_NODE* node = ToNode( index );
    if( !node )
        return QVariant();

    if( role == Qt::DisplayRole )
    {
        QString valueStr;
        int aCol = index.column();

        switch( aCol )
        {
        case NAME_COL:
            if( node->m_Pinned )
                valueStr = GetPinningSymbol() + UnescapeString( node->m_Name );
            else
                valueStr = UnescapeString( node->m_Name );

            break;

        default:
            if( m_colIdxMap.count( aCol ) )
            {
                const QString& key = m_colIdxMap.at( aCol );

                if( key == "Description" )
                    valueStr = UnescapeString( node->m_Desc );
                else if( node->m_Fields.count( key ) )
                    valueStr = UnescapeString( node->m_Fields.at( key ) );
                else
                    valueStr = QString();
            }

            break;
        }

        valueStr.replace( "\n", " " ); // Clear line breaks

        return valueStr;
    }
    else if( role == Qt::FontRole )
    {
        if( node->m_Type == LIB_TREE_NODE::TYPE::ITEM && !node->m_IsRoot && index.column() == 0 )
        {
            QFont font;
            font.setItalic( true );
            return font;
        }
    }

    return QVariant();
}


void recursiveDescent( LIB_TREE_NODE& aNode, const std::function<int( const LIB_TREE_NODE* )>& f )
{
    for( std::unique_ptr<LIB_TREE_NODE>& node: aNode.m_Children )
    {
        int r = f( node.get() );

        if( r == 0 )
            break;
        else if( r == -1 )
            continue;

        recursiveDescent( *node, f );
    }
}


void LIB_TREE_MODEL_ADAPTER::expandAncestors( const QModelIndex& index )
{
    QModelIndex parent = index.parent();
    while( parent.isValid() )
    {
        m_widget->expand( parent );
        parent = parent.parent();
    }
}


const LIB_TREE_NODE* LIB_TREE_MODEL_ADAPTER::ShowResults()
{
    const LIB_TREE_NODE* firstMatch = nullptr;

    // Expand parents of leaf nodes with some level of matching
    recursiveDescent( m_tree,
            [&]( const LIB_TREE_NODE* n )
            {
                if( n->m_Type == LIB_TREE_NODE::TYPE::ITEM && n->m_Score > 1 )
                {
                    if( !firstMatch )
                        firstMatch = n;
                    else if( n->m_Score > firstMatch->m_Score )
                        firstMatch = n;

                    expandAncestors( ToItem( n ) );
                }

                return 1; // keep going to expand ancestors of all found items
            } );

    // If no matches, find and show the preselect node
    if( !firstMatch && m_preselect_lib_id.IsValid() )
    {
        recursiveDescent( m_tree,
                [&]( const LIB_TREE_NODE* n )
                {
                    // Don't match the recent and already placed libraries
                    if( n->m_Name.startsWith( "-- " ) )
                        return -1; // Skip this node and its children

                    if( n->m_Type == LIB_TREE_NODE::TYPE::ITEM
                              && ( n->m_Children.empty() || !m_preselect_unit )
                              && m_preselect_lib_id == n->m_LibId )
                    {
                        firstMatch = n;
                        expandAncestors( ToItem( n ) );
                        return 0;
                    }
                    else if( n->m_Type == LIB_TREE_NODE::TYPE::UNIT
                              && ( m_preselect_unit && m_preselect_unit == n->m_Unit )
                              && m_preselect_lib_id == n->m_Parent->m_LibId )
                    {
                        firstMatch = n;
                        expandAncestors( ToItem( n ) );
                        return 0;
                    }

                    return 1;
                } );
    }

    // If still no matches expand a single library if there is only one
    if( !firstMatch )
    {
        int libraries = 0;

        for( const std::unique_ptr<LIB_TREE_NODE>& child : m_tree.m_Children )
        {
            if( !child->m_Name.startsWith( "-- " ) )
                 libraries++;
        }

        if( libraries != 1 )
            return nullptr;

        recursiveDescent( m_tree,
                [&]( const LIB_TREE_NODE* n )
                {
                    if( n->m_Type == LIB_TREE_NODE::TYPE::ITEM )
                    {
                        firstMatch = n;
                        expandAncestors( ToItem( n ) );
                        return 0;
                    }

                    return 1;
                } );
    }

    return firstMatch;
}