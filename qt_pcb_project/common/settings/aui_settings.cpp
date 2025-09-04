#include <settings/aui_settings.h>
#include <json_common.h>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QSizePolicy>


void to_json( nlohmann::json& aJson, const QPoint& aPoint )
{
    aJson = nlohmann::json
            {
                { "x", aPoint.x() },
                { "y", aPoint.y() }
            };
}


void from_json( const nlohmann::json& aJson, QPoint& aPoint )
{
    aPoint.setX( aJson.at( "x" ).get<int>() );
    aPoint.setY( aJson.at( "y" ).get<int>() );
}


bool operator<( const QPoint& aLhs, const QPoint& aRhs )
{
    int xDelta = aLhs.x() - aRhs.x();

    if( xDelta < 0 )
        return true;

    if( ( xDelta == 0 ) && (aLhs.y() < aRhs.y() ) )
        return true;

    return false;
}


void to_json( nlohmann::json& aJson, const QSize& aSize )
{
    aJson = nlohmann::json
            {
                { "width", aSize.width() },
                { "height", aSize.height() }
            };
}


void from_json( const nlohmann::json& aJson, QSize& aSize )
{
    aSize.setWidth( aJson.at( "width" ).get<int>() );
    aSize.setHeight( aJson.at( "height" ).get<int>() );
}


bool operator<( const QSize& aLhs, const QSize& aRhs )
{
    int xDelta = aLhs.width() - aRhs.width();

    if( xDelta < 0 )
        return true;

    if( ( xDelta == 0 ) && (aLhs.height() < aRhs.height() ) )
        return true;

    return false;
}


void to_json( nlohmann::json& aJson, const QRect& aRect )
{
    aJson = nlohmann::json
            {
                { "position", aRect.topLeft() },
                { "size", aRect.size() }
            };
}


void from_json( const nlohmann::json& aJson, QRect& aRect )
{
    aRect.setTopLeft( aJson.at( "position" ).get<QPoint>() );
    aRect.setSize( aJson.at( "size" ).get<QSize>() );
}


bool operator<( const QRect& aLhs, const QRect& aRhs )
{
    if( aLhs.size() <  aRhs.size() )
        return true;

    if( aLhs.topLeft() < aRhs.topLeft() )
        return true;

    return false;
}


void to_json( nlohmann::json& aJson, const QDockWidget& aPaneInfo )
{
    aJson = nlohmann::json
    {
        { "name", aPaneInfo.objectName() },
        { "caption", aPaneInfo.windowTitle() },
        { "state", static_cast<int>(aPaneInfo.isFloating() ? 1 : 0) },
        { "dock_direction", 0 },
        { "dock_layer", 0 },
        { "dock_row", 0 },
        { "dock_pos", 0 },
        { "dock_proportion", 0 },
        { "best_size", aPaneInfo.sizeHint() },
        { "min_size", aPaneInfo.minimumSize() },
        { "max_size", aPaneInfo.maximumSize() },
        { "floating_pos", aPaneInfo.pos() },
        { "floating_size", aPaneInfo.size() },
        { "rect", aPaneInfo.geometry() }
    };
}


void from_json( const nlohmann::json& aJson, QDockWidget& aPaneInfo )
{
    aPaneInfo.setObjectName( aJson.at( "name" ).get<QString>() );
    aPaneInfo.setWindowTitle( aJson.at( "caption" ).get<QString>() );
    int state = aJson.at( "state" ).get<int>();
    aPaneInfo.setFloating( state != 0 );
    
    aPaneInfo.setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    aPaneInfo.setMinimumSize( aJson.at( "min_size" ).get<QSize>() );
    aPaneInfo.setMaximumSize( aJson.at( "max_size" ).get<QSize>() );
    
    QPoint floatingPos = aJson.at( "floating_pos" ).get<QPoint>();
    QSize floatingSize = aJson.at( "floating_size" ).get<QSize>();
    QRect rect = aJson.at( "rect" ).get<QRect>();
    
    if( aPaneInfo.isFloating() )
    {
        aPaneInfo.move( floatingPos );
        aPaneInfo.resize( floatingSize );
    }
    else
    {
        aPaneInfo.setGeometry( rect );
    }
}


bool operator<( const QDockWidget& aLhs, const QDockWidget& aRhs )
{
    if( aLhs.objectName() < aRhs.objectName() )
        return true;

    if( aLhs.windowTitle() < aRhs.windowTitle() )
        return true;

    int lhsState = static_cast<int>(aLhs.isFloating() ? 1 : 0);
    int rhsState = static_cast<int>(aRhs.isFloating() ? 1 : 0);
    if( lhsState < rhsState )
        return true;

    if( aLhs.sizeHint() < aRhs.sizeHint() )
        return true;

    if( aLhs.minimumSize() < aRhs.minimumSize() )
        return true;

    if( aLhs.maximumSize() < aRhs.maximumSize() )
        return true;

    if( aLhs.pos() < aRhs.pos() )
        return true;

    if( aLhs.size() < aRhs.size() )
        return true;

    if( aLhs.geometry() < aRhs.geometry() )
        return true;

    return false;
}


bool operator==( const QDockWidget& aLhs, const QDockWidget& aRhs )
{
    if( aLhs.objectName() != aRhs.objectName() )
        return false;

    if( aLhs.windowTitle() != aRhs.windowTitle() )
        return false;

    if( aLhs.isFloating() != aRhs.isFloating() )
        return false;

    if( aLhs.sizeHint() != aRhs.sizeHint() )
        return false;

    if( aLhs.minimumSize() != aRhs.minimumSize() )
        return false;

    if( aLhs.maximumSize() != aRhs.maximumSize() )
        return false;

    if( aLhs.pos() != aRhs.pos() )
        return false;

    if( aLhs.size() != aRhs.size() )
        return false;

    if( aLhs.geometry() != aRhs.geometry() )
        return false;

    return true;
}
