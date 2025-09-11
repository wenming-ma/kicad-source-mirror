
#ifndef CONDITIONAL_MENU_H
#define CONDITIONAL_MENU_H

#include <tool/selection_conditions.h>
#include <tool/action_menu.h>
#include <list>

class PCB_SELECTION_TOOL;
class TOOL_ACTION;
class TOOL_INTERACTIVE;

enum class BITMAPS : unsigned int;


class CONDITIONAL_MENU : public ACTION_MENU
{
public:
    ///< Constant to indicate that we do not care about an #ENTRY location in the menu.
    static const int ANY_ORDER = -1;

    CONDITIONAL_MENU( TOOL_INTERACTIVE* aTool );

    ACTION_MENU* create() const override;

    void AddItem( const TOOL_ACTION& aAction, const SELECTION_CONDITION& aCondition,
                  int aOrder = ANY_ORDER );

    void AddItem( int aId, const QString& aText, const QString& aTooltip, BITMAPS aIcon,
                  const SELECTION_CONDITION& aCondition, int aOrder = ANY_ORDER );

    void AddCheckItem( const TOOL_ACTION& aAction, const SELECTION_CONDITION& aCondition,
                       int aOrder = ANY_ORDER );

    void AddCheckItem( int aId, const QString& aText, const QString& aTooltip, BITMAPS aIcon,
                       const SELECTION_CONDITION& aCondition, int aOrder = ANY_ORDER );

    void AddMenu( ACTION_MENU* aMenu,
                  const SELECTION_CONDITION& aCondition = SELECTION_CONDITIONS::ShowAlways,
                  int aOrder = ANY_ORDER );

    void AddSeparator( int aOrder = ANY_ORDER );

    void AddSeparator( const SELECTION_CONDITION& aCondition, int aOrder = ANY_ORDER );

    void Evaluate( const SELECTION& aSelection );

     void Resolve();

private:
    ///< Helper class to organize menu entries.
    class ENTRY
    {
    public:
        ENTRY( const TOOL_ACTION* aAction, SELECTION_CONDITION aCondition, int aOrder,
               bool aCheckmark ) :
            m_type( ACTION ),
            m_icon( static_cast<BITMAPS>( 0 ) ),
            m_condition( std::move( aCondition ) ),
            m_order( aOrder ),
            m_isCheckmarkEntry( aCheckmark )
        {
            m_data.action = aAction;
        }

        ENTRY( ACTION_MENU* aMenu, SELECTION_CONDITION aCondition, int aOrder ) :
            m_type( MENU ),
            m_icon( static_cast<BITMAPS>( 0 ) ),
            m_condition( std::move( aCondition ) ),
            m_order( aOrder ),
            m_isCheckmarkEntry( false )
        {
            m_data.menu = aMenu;
        }

        ENTRY( const QAction& aItem, BITMAPS aBitmap,
               SELECTION_CONDITION aCondition, int aOrder, bool aCheckmark ) :
            m_type( QTITEM ),
            m_icon( aBitmap ),
            m_condition( std::move( aCondition ) ),
            m_order( aOrder ),
            m_isCheckmarkEntry( aCheckmark )
        {
            m_data.qtItem = new QAction( nullptr );
            m_data.qtItem->setObjectName( aItem.objectName() );
            m_data.qtItem->setText( aItem.text() );
            m_data.qtItem->setToolTip( aItem.toolTip() );
            m_data.qtItem->setCheckable( aItem.isCheckable() );
        }

        // Separator
        ENTRY( SELECTION_CONDITION aCondition, int aOrder ) :
            m_type( SEPARATOR ),
            m_icon( static_cast<BITMAPS>( 0 ) ),
            m_data(),
            m_condition( std::move( aCondition ) ),
            m_order( aOrder ),
            m_isCheckmarkEntry( false )
        {
        }

        ENTRY( const ENTRY& aEntry );

        ~ENTRY();

        enum ENTRY_TYPE {
            ACTION,
            MENU,
            QTITEM,
            SEPARATOR
        };

        inline ENTRY_TYPE Type() const
        {
            return m_type;
        }

        inline BITMAPS GetIcon() const
        {
            return m_icon;
        }

        inline const TOOL_ACTION* Action() const
        {
            assert( m_type == ACTION );
            return m_data.action;
        }

        inline ACTION_MENU* Menu() const
        {
            assert( m_type == MENU );
            return m_data.menu;
        }

        inline QAction* qtItem() const
        {
            assert( m_type == QTITEM );
            return m_data.qtItem;
        }

        inline bool IsCheckmarkEntry() const
        {
            return m_isCheckmarkEntry;
        }

        inline const SELECTION_CONDITION& Condition() const
        {
            return m_condition;
        }

        inline int Order() const
        {
            return m_order;
        }

        inline void SetOrder( int aOrder )
        {
            m_order = aOrder;
        }

    private:
        ENTRY_TYPE m_type;
        BITMAPS m_icon;

        // This class owns the qtItem object and needs to create, copy and delete it accordingly
        // But it does not own the action nor menu item
        union {
            const TOOL_ACTION* action;
            ACTION_MENU*       menu;
            QAction*           qtItem;
        } m_data;

        SELECTION_CONDITION m_condition;
        int m_order;
        bool m_isCheckmarkEntry;
    };

    void addEntry( ENTRY aEntry );
    std::list<ENTRY> m_entries;
};

#endif /* CONDITIONAL_MENU_H */
