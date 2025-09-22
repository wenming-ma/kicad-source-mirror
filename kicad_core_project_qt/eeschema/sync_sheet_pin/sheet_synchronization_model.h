
#ifndef SHEET_SYNCHRONIZATION_MODEL_H
#define SHEET_SYNCHRONIZATION_MODEL_H

#include <sch_sheet_path.h>
#include <memory>
#include <optional>
#include <list>
#include <QAbstractListModel>
#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QList>

class SHEET_SYNCHRONIZATION_ITEM;
using SHEET_SYNCHRONIZATION_ITE_PTR = std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM>;
using SHEET_SYNCHRONIZATION_ITEM_LIST = std::vector<SHEET_SYNCHRONIZATION_ITE_PTR>;

class SHEET_SYNCHRONIZATION_NOTIFIER;
class SHEET_SYNCHRONIZATION_AGENT;
class SCH_SHEET;
class SCH_SHEET_PATH;

class SHEET_SYNCHRONIZATION_MODEL : public QAbstractListModel
{
public:
    enum SHEET_SYNCHRONIZATION_COL
    {
        NAME,
        SHAPE,
        COL_COUNT
    };

    enum
    {
        HIRE_LABEL,
        SHEET_PIN,
        ASSOCIATED,
        MODEL_COUNT
    };

    static QString GetColName( int col )
    {
        switch( col )
        {
        case NAME: return "Name";
        case SHAPE: return "Shape";
        default: return {};
        }
    }


    SHEET_SYNCHRONIZATION_MODEL( SHEET_SYNCHRONIZATION_AGENT& aAgent, SCH_SHEET* aSheet,
                                 SCH_SHEET_PATH& aPath );
    ~SHEET_SYNCHRONIZATION_MODEL() override;

    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;

    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole ) override;

    // Attribute handling moved to data() method with custom roles

    void RemoveItems( QList<QModelIndex> const& aItems );

    /**
     * Add a new item, the notifiers are notified.
     */
    bool AppendNewItem( std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM> aItem );

    /**
     * Just append item to the list, the notifiers are not notified.
     */
    bool AppendItem( std::shared_ptr<SHEET_SYNCHRONIZATION_ITEM> aItem );

    SHEET_SYNCHRONIZATION_ITEM_LIST TakeItems( QList<QModelIndex> const& aItems );

    SHEET_SYNCHRONIZATION_ITE_PTR TakeItem( QModelIndex const& aItem );

    SHEET_SYNCHRONIZATION_ITE_PTR GetSynchronizationItem( unsigned aIndex ) const;

    SHEET_SYNCHRONIZATION_ITE_PTR GetSynchronizationItem( QModelIndex const& aItem ) const;

    void OnRowSelected( std::optional<unsigned> aRow );

    void UpdateItems( SHEET_SYNCHRONIZATION_ITEM_LIST aItems );

    void AddNotifier( std::shared_ptr<SHEET_SYNCHRONIZATION_NOTIFIER> aNotifier );

    void DoNotify();

    bool HasSelectedIndex() const { return m_selectedIndex.has_value(); }

    std::optional<unsigned int> GetSelectedIndex() const { return m_selectedIndex; }

    int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
    int columnCount( const QModelIndex& parent = QModelIndex() ) const override;


private:
    SHEET_SYNCHRONIZATION_ITEM_LIST                            m_items;
    std::optional<unsigned>                                    m_selectedIndex;
    std::list<std::shared_ptr<SHEET_SYNCHRONIZATION_NOTIFIER>> m_notifiers;
    SHEET_SYNCHRONIZATION_AGENT&                               m_agent;
    SCH_SHEET*                                                 m_sheet;
    SCH_SHEET_PATH                                             m_path;
};

#endif
