
#include <bitmaps.h>
#include <tool/conditional_menu.h>
#include <tool/action_menu.h>
#include <tool/selection.h>
#include <kiface_base.h>
#include <widgets/ui_common.h>


CONDITIONAL_MENU::CONDITIONAL_MENU( TOOL_INTERACTIVE* aTool ) :
        ACTION_MENU( true, aTool )
{
}


ACTION_MENU* CONDITIONAL_MENU::create() const
{
    CONDITIONAL_MENU* clone = new CONDITIONAL_MENU( m_tool );
    clone->m_entries = m_entries;
    return clone;
}


void CONDITIONAL_MENU::AddItem( const TOOL_ACTION& aAction, const SELECTION_CONDITION& aCondition,
                                int aOrder )
{
    Q_ASSERT( aAction.GetId() > 0 ); // Check if action was previously registered in ACTION_MANAGER
    addEntry( ENTRY( &aAction, aCondition, aOrder, false ) );
}


void CONDITIONAL_MENU::AddCheckItem( const TOOL_ACTION& aAction,
                                     const SELECTION_CONDITION& aCondition, int aOrder )
{
    Q_ASSERT( aAction.GetId() > 0 ); // Check if action was previously registered in ACTION_MANAGER
    addEntry( ENTRY( &aAction, aCondition, aOrder, true ) );
}


void CONDITIONAL_MENU::AddItem( int aId, const QString& aText, const QString& aTooltip,
                                BITMAPS aIcon, const SELECTION_CONDITION& aCondition,
                                int aOrder )
{
    QAction item( nullptr );
    item.setObjectName( QString::number( aId ) );
    item.setText( aText );
    item.setToolTip( aTooltip );
    item.setCheckable( false );

    if( !!aIcon )
        KIUI::AddBitmapToAction( &item, KiBitmap( aIcon ) );

    addEntry( ENTRY( item, aIcon, aCondition, aOrder, false ) );
}


void CONDITIONAL_MENU::AddCheckItem( int aId, const QString& aText, const QString& aTooltip,
                                     BITMAPS aIcon, const SELECTION_CONDITION& aCondition,
                                     int aOrder )
{
    QAction item( nullptr );
    item.setObjectName( QString::number( aId ) );
    item.setText( aText );
    item.setToolTip( aTooltip );
    item.setCheckable( true );

    if( !!aIcon )
        KIUI::AddBitmapToAction( &item, KiBitmap( aIcon ) );

    addEntry( ENTRY( item, aIcon, aCondition, aOrder, true ) );
}


void CONDITIONAL_MENU::AddMenu( ACTION_MENU* aMenu, const SELECTION_CONDITION& aCondition,
                                int aOrder )
{
    addEntry( ENTRY( aMenu, aCondition, aOrder ) );
}


void CONDITIONAL_MENU::AddSeparator( int aOrder )
{
    addEntry( ENTRY( SELECTION_CONDITIONS::ShowAlways, aOrder ) );
}


void CONDITIONAL_MENU::AddSeparator( const SELECTION_CONDITION& aCondition, int aOrder )
{
    addEntry( ENTRY( aCondition, aOrder ) );
}


SELECTION g_resolveDummySelection;


void CONDITIONAL_MENU::Resolve()
{
    Evaluate( g_resolveDummySelection );
    UpdateAll();

    runOnSubmenus(
            [] ( ACTION_MENU* aMenu )
            {
                CONDITIONAL_MENU* conditionalMenu = dynamic_cast<CONDITIONAL_MENU*>( aMenu );

                if( conditionalMenu )
                    conditionalMenu->Resolve();
            } );
}


void CONDITIONAL_MENU::Evaluate( const SELECTION& aSelection )
{
    Clear();

    // We try to avoid adding useless separators (when no menuitems between separators)
    int menu_count = 0;     // number of menus since the latest separator

    for( const ENTRY& entry : m_entries )
    {
        const SELECTION_CONDITION& cond = entry.Condition();
        bool                       result;
        QAction*                   menuItem = nullptr;

        try
        {
            result = cond( aSelection );
        }
        catch( std::exception& )
        {
            continue;
        }

        if( !result )
            continue;

        switch( entry.Type() )
        {
        case ENTRY::ACTION:
            Add( *entry.Action(), entry.IsCheckmarkEntry() );
            menu_count++;
            break;

        case ENTRY::MENU:
            entry.Menu()->UpdateTitle();
            Add( entry.Menu()->Clone() );
            menu_count++;
            break;

        case ENTRY::QTITEM:
            menuItem = new QAction( this );
            menuItem->setObjectName( entry.qtItem()->objectName() );
            menuItem->setText( QCoreApplication::translate( "menu", entry.qtItem()->text().toUtf8().constData() ) );
            menuItem->setToolTip( QCoreApplication::translate( "menu", entry.qtItem()->toolTip().toUtf8().constData() ) );
            menuItem->setCheckable( entry.qtItem()->isCheckable() );

            if( !!entry.GetIcon() )
                KIUI::AddBitmapToAction( menuItem, KiBitmap( entry.GetIcon() ) );

            // the QAction must be added only after the bitmap is set:
            addAction( menuItem );

            menu_count++;
            break;

        case ENTRY::SEPARATOR:
            if( menu_count )
                AppendSeparator();

            menu_count = 0;
            break;

        default:
            Q_ASSERT( false );
            break;
        }
    }

    // Recursively call Evaluate on all the submenus that are CONDITIONAL_MENUs to ensure
    // they are updated. This is also required on GTK to make sure the menus have the proper
    // size when created.
    runOnSubmenus(
            [&aSelection]( ACTION_MENU* aMenu )
            {
                CONDITIONAL_MENU* conditionalMenu = dynamic_cast<CONDITIONAL_MENU*>( aMenu );

                if( conditionalMenu )
                    conditionalMenu->Evaluate( aSelection );
            } );
}


void CONDITIONAL_MENU::addEntry( ENTRY aEntry )
{
    if( aEntry.Order() < 0 )        // Any order, so give it any order number
        aEntry.SetOrder( m_entries.size() );

    std::list<ENTRY>::iterator it = m_entries.begin();

    // Find the right spot for the entry
    while( it != m_entries.end() && it->Order() <= aEntry.Order() )
        ++it;

    m_entries.insert( it, aEntry );
}


CONDITIONAL_MENU::ENTRY::ENTRY( const ENTRY& aEntry )
{
    m_type = aEntry.m_type;
    m_icon = aEntry.m_icon;

    switch( aEntry.m_type )
    {
    case ACTION:
        m_data.action = aEntry.m_data.action;
        break;

    case MENU:
        m_data.menu = aEntry.m_data.menu;
        break;

    case QTITEM:
        // We own the qtItem, so we need to make a new one for the new object
        m_data.qtItem = new QAction( nullptr );
        m_data.qtItem->setObjectName( aEntry.m_data.qtItem->objectName() );
        m_data.qtItem->setText( aEntry.m_data.qtItem->text() );
        m_data.qtItem->setToolTip( aEntry.m_data.qtItem->toolTip() );
        m_data.qtItem->setCheckable( aEntry.m_data.qtItem->isCheckable() );
        break;

    case SEPARATOR:
        break; //No data to copy
    }

    m_condition        = aEntry.m_condition;
    m_order            = aEntry.m_order;
    m_isCheckmarkEntry = aEntry.m_isCheckmarkEntry;
}


CONDITIONAL_MENU::ENTRY::~ENTRY()
{
    if( QTITEM == m_type )
        delete m_data.qtItem;
}

