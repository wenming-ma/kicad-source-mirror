
#ifndef PIN_TYPE_H_
#define PIN_TYPE_H_

#include <QStringList>
#include <vector>

enum class BITMAPS : unsigned int;

/**
 * The symbol library pin object electrical types used in ERC tests.
 */
enum class ELECTRICAL_PINTYPE
{
    PT_INPUT,         ///< usual pin input: must be connected
    PT_OUTPUT,        ///< usual output
    PT_BIDI,          ///< input or output (like port for a microprocessor)
    PT_TRISTATE,      ///< tri state bus pin

    /// pin for passive symbols: must be connected, and can be connected to any pin.
    PT_PASSIVE,
    PT_NIC,           ///< not internally connected (may be connected to anything)
    PT_UNSPECIFIED,   ///< unknown electrical properties: creates always a warning when connected
    PT_POWER_IN,      ///< power input (GND, VCC for ICs). Must be connected to a power output.
    PT_POWER_OUT,     ///< output of a regulator: intended to be connected to power input pins
    PT_OPENCOLLECTOR, ///< pin type open collector
    PT_OPENEMITTER,   ///< pin type open emitter
    PT_NC,            ///< not connected (must be left open)

    PT_LAST_OPTION = PT_NC, ///< sentinel value, set to last usable enum option
    PT_INHERIT
};

#define ELECTRICAL_PINTYPES_TOTAL ( static_cast<int>( ELECTRICAL_PINTYPE::PT_LAST_OPTION ) + 1 )

enum class GRAPHIC_PINSHAPE
{
    LINE,
    INVERTED,
    CLOCK,
    INVERTED_CLOCK,
    INPUT_LOW,
    CLOCK_LOW,
    OUTPUT_LOW,
    FALLING_EDGE_CLOCK,
    NONLOGIC,

    LAST_OPTION = NONLOGIC, ///< this is the sentinel value, must be set to last enum value
    INHERIT
};

#define GRAPHIC_PINSHAPES_TOTAL ( static_cast<int>( GRAPHIC_PINSHAPE::LAST_OPTION ) + 1 )

/**
 *  The symbol library pin object orientations.
 */
enum class PIN_ORIENTATION
{
    /**
     * The pin extends rightwards from the connection point.
     * Probably on the left side of the symbol.
     * x---|
     */
    PIN_RIGHT,

    /**
     * The pin extends leftwards from the connection point:
     * Probably on the right side of the symbol.
     * |---x
     */
    PIN_LEFT,

    /**
     * The pin extends upwards from the connection point:
     * Probably on the bottom side of the symbol.
     *  ___
     *   |
     *   x
     */
    PIN_UP,

    /**
     * The pin extends downwards from the connection:
     * Probably on the top side of the symbol.
     *   x
     *  _|_
     */
    PIN_DOWN,
    INHERIT
};


// UI

QString PinShapeGetText( GRAPHIC_PINSHAPE shape );
BITMAPS PinShapeGetBitmap( GRAPHIC_PINSHAPE shape );

QString ElectricalPinTypeGetText( ELECTRICAL_PINTYPE );
BITMAPS ElectricalPinTypeGetBitmap( ELECTRICAL_PINTYPE );

QString PinOrientationName( PIN_ORIENTATION aOrientation );
BITMAPS PinOrientationGetBitmap( PIN_ORIENTATION aOrientation );
PIN_ORIENTATION PinOrientationCode( size_t index );
int PinOrientationIndex( PIN_ORIENTATION code );

const QStringList& PinTypeNames();
const std::vector<BITMAPS>& PinTypeIcons();

const QStringList& PinShapeNames();
const std::vector<BITMAPS>& PinShapeIcons();

const QStringList& PinOrientationNames();
const std::vector<BITMAPS>& PinOrientationIcons();

#endif
