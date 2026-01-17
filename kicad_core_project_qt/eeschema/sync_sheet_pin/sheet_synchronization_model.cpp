
#include "sheet_synchronization_model.h"
#include "sheet_synchronization_item.h"
#include "sheet_synchronization_notifier.h"
#include "sheet_synchronization_agent.h"

#include <sch_label.h>
#include <sch_sheet_pin.h>
#include <QColor>
#include <QVariant>
#include <QString>
#include <QIcon>
#include <QFont>
#include "i18n_utility.h"


// sch_label.cpp
extern QString getElectricalTypeLabel( LABEL_FLAG_SHAPE aType );


SHEET_SYNCHRONIZATION_MODEL::SHEET_SYNCHRONIZATION_MODEL( SHEET_SYNCHRONIZATION_AGENT& aAgent,
                                                          SCH_SHEET*                   aSheet,
                                                          SCH_SHEET_PATH&              aPath ) :
        m_selectedIndex( std::optional<unsigned>() ),
        m_agent( aAgent ),
        m_sheet( aSheet ),
        m_path( aPath )
{
}


SHEET_SYNCHRONIZATION_MODEL::~SHEET_SYNCHRONIZATION_MODEL() = default;


QVariant SHEET_SYNCHRONIZATION_MODEL::data( const QModelIndex& index, int role ) const
{
    if( !index.isValid() || index.row() >= static_cast<int>(m_items.size()) )
        return QVariant();

    const std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM>& item = m_items[index.row()];

    if( role == Qt::DisplayRole )
    {
        switch( index.column() )
        {
        case NAME:
            return item->GetName();
        case SHAPE:
            return getElectricalTypeLabel( static_cast<LABEL_FLAG_SHAPE>( item->GetShape() ) );
        }
    }
    else if( role == Qt::DecorationRole && index.column() == NAME )
    {
        return item->GetBitmap();
    }
    else if( role == Qt::FontRole )
    {
        if( m_selectedIndex.has_value() && static_cast<unsigned>(index.row()) == m_selectedIndex )
        {
            QFont font;
            font.setBold( true );
            return font;
        }
    }

    return QVariant();
}


bool SHEET_SYNCHRONIZATION_MODEL::setData( const QModelIndex& index, const QVariant& value, int role )
{
    Q_UNUSED( index )
    Q_UNUSED( value )
    Q_UNUSED( role )

    return false;
}


int SHEET_SYNCHRONIZATION_MODEL::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent )
    return static_cast<int>(m_items.size());
}

int SHEET_SYNCHRONIZATION_MODEL::columnCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent )
    return COL_COUNT;
}


void SHEET_SYNCHRONIZATION_MODEL::RemoveItems( QList<QModelIndex> const& aItems )
{
    if( aItems.empty() )
        return;

    for( const auto& item : TakeItems( aItems ) )
        m_agent.RemoveItem( *item, m_sheet, m_path );

    DoNotify();
}


bool SHEET_SYNCHRONIZATION_MODEL::AppendNewItem( std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM> aItem )
{
    m_items.push_back( std::move( aItem ) );
    beginResetModel();
    endResetModel();
    DoNotify();
    return true;
}


bool SHEET_SYNCHRONIZATION_MODEL::AppendItem( std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM> aItem )
{
    m_items.push_back( std::move( aItem ) );
    beginResetModel();
    endResetModel();
    return true;
}


SHEET_SYNCHRONIZATION_ITEM_LIST
SHEET_SYNCHRONIZATION_MODEL::TakeItems( QList<QModelIndex> const& aItems )
{
    if( aItems.size() == 1 )
        return { TakeItem( aItems[0] ) };

    std::set<unsigned>              rowsToBeRemove;
    SHEET_SYNCHRONIZATION_ITEM_LIST items_remain;
    SHEET_SYNCHRONIZATION_ITEM_LIST items_token;

    for( const auto& item : aItems )
    {
        if( item.isValid() )
        {
            unsigned int idx = static_cast<unsigned int>(item.row());
            rowsToBeRemove.insert( idx );
        }
    }

    for( unsigned i = 0; i < m_items.size(); i++ )
    {
        if( rowsToBeRemove.find( i ) == rowsToBeRemove.end() )
        {
            items_remain.push_back( m_items[i] );
        }
        else
        {
            items_token.push_back( m_items[i] );
        }
    }

    UpdateItems( std::move( items_remain ) );
    OnRowSelected( {} );
    return items_token;
}


SHEET_SYNCHRONIZATION_ITE_PTR SHEET_SYNCHRONIZATION_MODEL::TakeItem( QModelIndex const& aItem )
{
    const unsigned int row = static_cast<unsigned int>(aItem.row());

    if( row + 1 > m_items.size() )
        return {};

    std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM> item = m_items[row];
    m_items.erase( m_items.begin() + row );
    OnRowSelected( {} );
    beginResetModel();
    endResetModel();
    return item;
}


SHEET_SYNCHRONIZATION_ITE_PTR
SHEET_SYNCHRONIZATION_MODEL::GetSynchronizationItem( unsigned aIndex ) const
{
    if( aIndex < m_items.size() )
        return m_items[aIndex];

    return {};
}


SHEET_SYNCHRONIZATION_ITE_PTR
SHEET_SYNCHRONIZATION_MODEL::GetSynchronizationItem( QModelIndex const& aItem ) const
{
    return GetSynchronizationItem( static_cast<unsigned>(aItem.row()) );
}


void SHEET_SYNCHRONIZATION_MODEL::OnRowSelected( std::optional<unsigned> aRow )
{
    m_selectedIndex = aRow;

    if( aRow.has_value() && m_items.size() > *aRow )
    {
        if( QModelIndex item = index( *aRow, 0 ); item.isValid() )
            emit dataChanged( item, item );
    }
}


void SHEET_SYNCHRONIZATION_MODEL::UpdateItems( SHEET_SYNCHRONIZATION_ITEM_LIST aItems )
{
    m_items = std::move( aItems );
    beginResetModel();
    endResetModel();
}


void SHEET_SYNCHRONIZATION_MODEL::AddNotifier(
        std::shared_ptr<SHEET_SYNCHRONIZATION_NOTIFIER> aNotifier )
{
    m_notifiers.push_back( std::move( aNotifier ) );
}


void SHEET_SYNCHRONIZATION_MODEL::DoNotify()
{
    for( const auto& notifier : m_notifiers )
        notifier->Notify();
}

