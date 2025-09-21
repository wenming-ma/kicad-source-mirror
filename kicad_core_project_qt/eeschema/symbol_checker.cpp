
#include <vector>
#include <QString>
#include <eda_draw_frame.h>
#include <lib_symbol.h>
#include <sch_shape.h>
#include <macros.h>

// helper function to sort pins by pin num
static bool sort_by_pin_number( const SCH_PIN* ref, const SCH_PIN* tst );


static void CheckLibSymbolGraphics( LIB_SYMBOL* aSymbol, std::vector<QString>& aMessages,
                                    UNITS_PROVIDER* aUnitsProvider );


void CheckDuplicatePins( LIB_SYMBOL* aSymbol, std::vector<QString>& aMessages,
                         UNITS_PROVIDER* aUnitsProvider )
{
    QString              msg;
    std::vector<SCH_PIN*> pinList = aSymbol->GetPins();

    // Test for duplicates:
    // Sort pins by pin num, so 2 duplicate pins
    // (pins with the same number) will be consecutive in list
    sort( pinList.begin(), pinList.end(), sort_by_pin_number );

    for( unsigned ii = 1; ii < pinList.size(); ii++ )
    {
        SCH_PIN* pin  = pinList[ii - 1];
        SCH_PIN* next = pinList[ii];

        if( pin->GetNumber() != next->GetNumber() )
            continue;

        // Pins are not duplicated only if they are in different body styles
        // (but GetBodyStyle() == 0 means common to all body styles)
        if( pin->GetBodyStyle() != 0 && next->GetBodyStyle() != 0 )
        {
            if( pin->GetBodyStyle() != next->GetBodyStyle() )
                continue;
        }

        QString pinName;
        QString nextName;

        if( pin->GetName() != "~"  && !pin->GetName().isEmpty() )
            pinName = " '" + pin->GetName() + "'";

        if( next->GetName() != "~"  && !next->GetName().isEmpty() )
            nextName = " '" + next->GetName() + "'";

        if( aSymbol->HasAlternateBodyStyle() && next->GetBodyStyle() )
        {
            if( pin->GetUnit() == 0 || next->GetUnit() == 0 )
            {
                msg = QString::asprintf( _( "<b>Duplicate pin %s</b> %s at location <b>(%s, %s)</b>"
                               " conflicts with pin %s%s at location <b>(%s, %s)</b>"
                               " in %s body style." ),
                            next->GetNumber().toStdString().c_str(),
                            nextName.toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( next->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -next->GetPosition().y ).toStdString().c_str(),
                            pin->GetNumber().toStdString().c_str(),
                            pin->GetName().toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                            SCH_ITEM::GetBodyStyleDescription( pin->GetBodyStyle() ).toLower().toStdString().c_str() );
            }
            else
            {
                msg = QString::asprintf( _( "<b>Duplicate pin %s</b> %s at location <b>(%s, %s)</b>"
                               " conflicts with pin %s%s at location <b>(%s, %s)</b>"
                               " in units %s and %s of %s body style." ),
                            next->GetNumber().toStdString().c_str(),
                            nextName.toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( next->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -next->GetPosition().y ).toStdString().c_str(),
                            pin->GetNumber().toStdString().c_str(),
                            pinName.toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                            aSymbol->GetUnitReference( next->GetUnit() ).toStdString().c_str(),
                            aSymbol->GetUnitReference( pin->GetUnit() ).toStdString().c_str(),
                            SCH_ITEM::GetBodyStyleDescription( pin->GetBodyStyle() ).toLower().toStdString().c_str() );
            }
        }
        else
        {
            if( pin->GetUnit() == 0 || next->GetUnit() == 0 )
            {
                msg = QString::asprintf( _( "<b>Duplicate pin %s</b> %s at location <b>(%s, %s)</b>"
                               " conflicts with pin %s%s at location <b>(%s, %s)</b>." ),
                            next->GetNumber().toStdString().c_str(),
                            nextName.toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( next->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -next->GetPosition().y ).toStdString().c_str(),
                            pin->GetNumber().toStdString().c_str(),
                            pinName.toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str() );
            }
            else
            {
                msg = QString::asprintf( _( "<b>Duplicate pin %s</b> %s at location <b>(%s, %s)</b>"
                               " conflicts with pin %s%s at location <b>(%s, %s)</b>"
                               " in units %s and %s." ),
                            next->GetNumber().toStdString().c_str(),
                            nextName.toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( next->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -next->GetPosition().y ).toStdString().c_str(),
                            pin->GetNumber().toStdString().c_str(),
                            pinName.toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                            aSymbol->GetUnitReference( next->GetUnit() ).toStdString().c_str(),
                            aSymbol->GetUnitReference( pin->GetUnit() ).toStdString().c_str() );
            }
        }

        msg += "<br><br>";
        aMessages.push_back( msg );
    }
}


/**
 * Check a library symbol to find incorrect settings.
 *
 *  - Pins not on a valid grid
 *  - Pins duplicated
 *  - Conflict with pins at same location
 *  - Incorrect Power Symbols
 *  - illegal reference prefix (cannot ends by a digit or a '?')
 *
 * @param aSymbol is the library symbol to check.
 * @param aMessages is a room to store error messages.
 * @param aGridForPins (in IU) is the grid to test pin positions ( >= 25 mils )
 * should be 25, 50 or 100 mils (converted to IUs).
 * @param aUnitsProvider a frame to format coordinates in messages.
 */
void CheckLibSymbol( LIB_SYMBOL* aSymbol, std::vector<QString>& aMessages,
                     int aGridForPins, UNITS_PROVIDER* aUnitsProvider )
{
    if( !aSymbol )
        return;

    QString msg;

    // Test reference prefix validity:
    // if the symbol is saved in a library, the prefix should not ends by a digit or a '?'
    // but it is acceptable if the symbol is saved to a schematic.
    QString reference_base = aSymbol->GetReferenceField().GetText();

    if( reference_base.isEmpty() )
    {
        aMessages.push_back( _( "<b>Warning: reference is empty</b><br><br>" ) );
    }
    else
    {
        QString illegal_end( "0123456789?" );
        QChar last_char = reference_base.at(reference_base.length() - 1);

        if( illegal_end.indexOf( last_char ) != -1 )
        {
            msg = QString::asprintf( _( "<b>Warning: reference prefix</b><br>prefix ending by '%s' can create"
                           " issues if saved in a symbol library" ),
                        illegal_end.toStdString().c_str() );
            msg += "<br><br>";
            aMessages.push_back( msg );
        }
    }

    CheckDuplicatePins( aSymbol, aMessages, aUnitsProvider );

    std::vector<SCH_PIN*> pinList = aSymbol->GetPins();
    sort( pinList.begin(), pinList.end(), sort_by_pin_number );

    // The minimal grid size allowed to place a pin is 25 mils
    // the best grid size is 50 mils, but 25 mils is still usable
    // this is because all aSymbols are using a 50 mils grid to place pins, and therefore
    // the wires must be on the 50 mils grid
    // So raise an error if a pin is not on a 25 (or bigger :50 or 100) mils grid
    const int min_grid_size = schIUScale.MilsToIU( 25 );
    const int clamped_grid_size = ( aGridForPins < min_grid_size ) ? min_grid_size : aGridForPins;

    // Test for a valid power aSymbol.
    // A valid power aSymbol has only one unit, no alternate body styles and one pin.
    // And this pin should be PT_POWER_IN (invisible to be automatically connected)
    // or PT_POWER_OUT for a power flag
    if( aSymbol->IsPower() )
    {
        if( aSymbol->GetUnitCount() != 1 )
        {
            msg = QString::fromUtf8( _( "<b>A Power Symbol should have only one unit</b><br><br>" ) );
            aMessages.push_back( msg );
        }

        if( aSymbol->HasAlternateBodyStyle() )
        {
            msg = QString::fromUtf8( _( "<b>A Power Symbol should not have DeMorgan variants</b><br><br>" ) );
            aMessages.push_back( msg );
        }

        if( pinList.size() != 1 )
        {
            msg = QString::fromUtf8( _( "<b>A Power Symbol should have only one pin</b><br><br>" ) );
            aMessages.push_back( msg );
        }

        SCH_PIN* pin = pinList[0];

        if( pin->GetType() != ELECTRICAL_PINTYPE::PT_POWER_IN
                && pin->GetType() != ELECTRICAL_PINTYPE::PT_POWER_OUT )
        {
            msg = QString::fromUtf8( _( "<b>Suspicious Power Symbol</b><br>"
                           "Only an input or output power pin has meaning<br><br>" ) );
            aMessages.push_back( msg );
        }

        if( pin->GetType() == ELECTRICAL_PINTYPE::PT_POWER_IN && !pin->IsVisible() )
        {
            msg = QString::fromUtf8( _( "<b>Suspicious Power Symbol</b><br>"
                           "Invisible input power pins are no longer required<br><br>" ) );
            aMessages.push_back( msg );
        }
    }


    for( SCH_PIN* pin : pinList )
    {
        QString pinName = pin->GetName();

        if( pinName.isEmpty() || pinName == "~" )
            pinName = "";
        else
            pinName = "'" + pinName + "'";

        if( !aSymbol->IsPower()
                && pin->GetType() == ELECTRICAL_PINTYPE::PT_POWER_IN
                && !pin->IsVisible() )
        {
            // hidden power pin
            if( aSymbol->HasAlternateBodyStyle() && pin->GetBodyStyle() )
            {
                if( aSymbol->GetUnitCount() <= 1 )
                {
                    msg = QString::asprintf( _( "Info: <b>Hidden power pin %s</b> %s at location <b>(%s, %s)</b>"
                                   " in %s body style." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                                SCH_ITEM::GetBodyStyleDescription( pin->GetBodyStyle() ).toLower().toStdString().c_str() );
                }
                else
                {
                    msg = QString::asprintf( _( "Info: <b>Hidden power pin %s</b> %s at location <b>(%s, %s)</b>"
                                   " in unit %c of %s body style." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                                'A' + pin->GetUnit() - 1,
                                SCH_ITEM::GetBodyStyleDescription( pin->GetBodyStyle() ).toLower().toStdString().c_str() );
                }
            }
            else
            {
                if( aSymbol->GetUnitCount() <= 1 )
                {
                    msg = QString::asprintf( _( "Info: <b>Hidden power pin %s</b> %s at location <b>"
                                   "(%s, %s)</b>." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str() );
                }
                else
                {
                    msg = QString::asprintf( _( "Info: <b>Hidden power pin %s</b> %s at location <b>(%s, %s)</b>"
                                   " in unit %c." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                                'A' + pin->GetUnit() - 1 );
                }
            }

            msg += "<br>";
            msg += _( "(Hidden power pins will drive their pin names on to any connected nets.)" );
            msg += "<br><br>";
            aMessages.push_back( msg );
        }

        if( ( (pin->GetPosition().x % clamped_grid_size) != 0 )
                || ( (pin->GetPosition().y % clamped_grid_size) != 0 ) )
        {
            // pin is off grid
            msg.clear();

            if( aSymbol->HasAlternateBodyStyle() && pin->GetBodyStyle() )
            {
                if( aSymbol->GetUnitCount() <= 1 )
                {
                    msg = QString::asprintf( _( "<b>Off grid pin %s</b> %s at location <b>(%s, %s)</b>"
                                   " of %s body style." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                                SCH_ITEM::GetBodyStyleDescription( pin->GetBodyStyle() ).toLower().toStdString().c_str() );
                }
                else
                {
                    msg = QString::asprintf( _( "<b>Off grid pin %s</b> %s at location <b>(%s, %s)</b>"
                                   " in unit %c of %s body style." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                                'A' + pin->GetUnit() - 1,
                                SCH_ITEM::GetBodyStyleDescription( pin->GetBodyStyle() ).toLower().toStdString().c_str() );
                 }
            }
            else
            {
                if( aSymbol->GetUnitCount() <= 1 )
                {
                    msg = QString::asprintf( _( "<b>Off grid pin %s</b> %s at location <b>(%s, %s)</b>." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str() );
                }
                else
                {
                    msg = QString::asprintf( _( "<b>Off grid pin %s</b> %s at location <b>(%s, %s)</b>"
                                   " in unit %c." ),
                                pin->GetNumber().toStdString().c_str(),
                                pinName.toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( pin->GetPosition().x ).toStdString().c_str(),
                                aUnitsProvider->MessageTextFromValue( -pin->GetPosition().y ).toStdString().c_str(),
                                'A' + pin->GetUnit() - 1 );
                }
            }

            msg += "<br><br>";
            aMessages.push_back( msg );
        }
    }

    CheckLibSymbolGraphics( aSymbol, aMessages,  aUnitsProvider );
}


void CheckLibSymbolGraphics( LIB_SYMBOL* aSymbol, std::vector<QString>& aMessages,
                             UNITS_PROVIDER* aUnitsProvider )
{
    if( !aSymbol )
        return;

    QString msg;

    for( const SCH_ITEM& item : aSymbol->GetDrawItems() )
    {
        if( item.Type() != SCH_SHAPE_T )
            continue;

        const SCH_SHAPE* shape = static_cast<const SCH_SHAPE*>( &item );

        switch( shape->GetShape() )
        {
        case SHAPE_T::ARC:
            break;

        case SHAPE_T::CIRCLE:
            if( shape->GetRadius() <= 0 )
            {
                msg = QString::asprintf( _( "<b>Graphic circle has radius = 0</b> at location "
                             "<b>(%s, %s)</b>." ),
                            aUnitsProvider->MessageTextFromValue(shape->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -shape->GetPosition().y ).toStdString().c_str() );
                msg += "<br>";
                aMessages.push_back( msg );
            }
            break;

        case SHAPE_T::RECTANGLE:
            if( shape->GetPosition() == shape->GetEnd() )
            {
                msg = QString::asprintf( _( "<b>Graphic rectangle has size 0</b> at location <b>(%s, %s)</b>." ),
                            aUnitsProvider->MessageTextFromValue(shape->GetPosition().x ).toStdString().c_str(),
                            aUnitsProvider->MessageTextFromValue( -shape->GetPosition().y ).toStdString().c_str() );
                msg += "<br>";
                aMessages.push_back( msg );
            }
            break;

        case SHAPE_T::POLY:
            break;

        case SHAPE_T::BEZIER:
            break;

        default:
            UNIMPLEMENTED_FOR( shape->SHAPE_T_asString() );
        }
    }
}


bool sort_by_pin_number( const SCH_PIN* ref, const SCH_PIN* tst )
{
    // Use number as primary key
    int test = ref->GetNumber().Cmp( tst->GetNumber() );

    // Use DeMorgan variant as secondary key
    if( test == 0 )
        test = ref->GetBodyStyle() - tst->GetBodyStyle();

    // Use unit as tertiary key
    if( test == 0 )
        test = ref->GetUnit() - tst->GetUnit();

    return test < 0;
}
