
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#include <tool/grid_menu.h>
#include <id.h>
#include <eda_draw_frame.h>
#include <settings/app_settings.h>
#include <tool/actions.h>
#include <bitmaps.h>
#include <base_units.h>
#include <i18n_utility.h>
#include <QAction>
#include <QStringList>

using namespace std::placeholders;

GRID_MENU::GRID_MENU( EDA_DRAW_FRAME* aParent ) :
        ACTION_MENU( true ),
        m_parent( aParent )
{
    UpdateTitle();
    SetIcon( BITMAPS::grid_select );
    update();
}


OPT_TOOL_EVENT GRID_MENU::eventHandler( QAction* aAction )
{
    OPT_TOOL_EVENT event( ACTIONS::gridPreset.MakeEvent() );
    event->SetParameter<int>( aAction->data().toInt() - ID_POPUP_GRID_START );
    return event;
}


void GRID_MENU::UpdateTitle()
{
    SetTitle( _( "Grid" ) );
}


void GRID_MENU::update()
{
    APP_SETTINGS_BASE* settings = m_parent->config();
    unsigned int       current = settings->m_Window.grid.last_size_idx + ID_POPUP_GRID_START;
    QStringList        gridsList;
    int                i = ID_POPUP_GRID_START;

    GRID_MENU::BuildChoiceList( &gridsList, settings, m_parent );

    while( actions().count() > 0 )
        removeAction( actions().at( 0 ) );

    Add( ACTIONS::gridOrigin );
    addSeparator();

    for( const QString& grid : gridsList )
    {
        int idx = i++;
        Add( grid, QString(), idx, BITMAPS::INVALID_BITMAP, true )->setChecked( idx == (int) current );
    }
}


void GRID_MENU::BuildChoiceList( QStringList* aGridsList, APP_SETTINGS_BASE* aCfg,
                                 EDA_DRAW_FRAME* aParent )
{
    QString      msg;
    EDA_IU_SCALE scale = aParent->GetIuScale();
    EDA_UNITS    primaryUnit;
    EDA_UNITS    secondaryUnit;

    aParent->GetUnitPair( primaryUnit, secondaryUnit );

    for( GRID& gridSize : aCfg->m_Window.grid.grids )
    {
        QString name;

        if( !gridSize.name.isEmpty() )
            name = gridSize.name + ": ";

        msg = QString::asprintf( "%s%s (%s)", name.toStdString().c_str(),
                                gridSize.MessageText( scale, primaryUnit, true ).toStdString().c_str(),
                                gridSize.MessageText( scale, secondaryUnit, true ).toStdString().c_str() );

        aGridsList->append( msg );
    }
}
