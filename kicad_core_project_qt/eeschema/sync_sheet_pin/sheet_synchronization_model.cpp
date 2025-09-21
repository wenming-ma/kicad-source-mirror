
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


// sch_label.cpp
extern QString getElectricalTypeLabel( LABEL_FLAG_SHAPE aType );


SHEET_SYNCHRONIZATION_MODEL::SHEET_SYNCHRONIZATION_MODEL( SHEET_SYNCHRONIZATION_AGENT& aAgent,
                                                          SCH_SHEET*                   aSheet,
                                                          SCH_SHEET_PATH&              aPath ) :
        m_selectedIndex( std::optional<unsigned>() ),
        m_agent( aAgent ),
        m_sheet( aSheet ),
        m_path( std::move( aPath ) )
{
}


SHEET_SYNCHRONIZATION_MODEL::~SHEET_SYNCHRONIZATION_MODEL() = default;


void SHEET_SYNCHRONIZATION_MODEL::GetValueByRow( QVariant& aVariant, unsigned row,
                                                 unsigned col ) const
{
    const std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM>& item = m_items[row];

    switch( col )
    {
    case NAME:
        aVariant = QVariant::fromValue(QPair<QString, QIcon>(item->GetName(), item->GetBitmap()));
        break;
    case SHAPE:
        aVariant = getElectricalTypeLabel( static_cast<LABEL_FLAG_SHAPE>( item->GetShape() ) );
        break;
    }
}


bool SHEET_SYNCHRONIZATION_MODEL::SetValueByRow( const QVariant& aVariant, unsigned row,
                                                 unsigned col )
{
    Q_UNUSED( aVariant )
    Q_UNUSED( row )
    Q_UNUSED( col )

    return {};
}


bool SHEET_SYNCHRONIZATION_MODEL::GetAttrByRow( unsigned row, unsigned int col,
                                                QModelIndex& attr ) const
{
    if( m_selectedIndex.has_value() && row == m_selectedIndex )
    {
        // Note: In Qt, font attributes are typically handled through QStandardItemModel or custom delegate
        // This functionality needs to be handled in the view layer
        return true;
    }

    return false;
}


void SHEET_SYNCHRONIZATION_MODEL::RemoveItems( QModelIndexList const& aItems )
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
    Reset( GetCount() );
    DoNotify();
    return true;
}


bool SHEET_SYNCHRONIZATION_MODEL::AppendItem( std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM> aItem )
{
    m_items.push_back( std::move( aItem ) );
    Reset( GetCount() );
    return true;
}


SHEET_SYNCHRONIZATION_ITEM_LIST
SHEET_SYNCHRONIZATION_MODEL::TakeItems( QModelIndexList const& aItems )
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
            unsigned int idx = item.row();
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
    const unsigned int row = aItem.row();

    if( row + 1 > m_items.size() )
        return {};

    std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM> item = m_items[row];
    m_items.erase( m_items.begin() + row );
    OnRowSelected( {} );
    Reset( GetCount() );
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
    return GetSynchronizationItem( aItem.row() );
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
    Reset( GetCount() );
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

unsigned int SHEET_SYNCHRONIZATION_MODEL::GetCount() const
{
    return m_items.size();
}
