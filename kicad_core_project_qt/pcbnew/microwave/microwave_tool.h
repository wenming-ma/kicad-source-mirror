
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05
#ifndef TOOLS_MICROWAVE_TOOL_H
#define TOOLS_MICROWAVE_TOOL_H

#include <tools/pcb_tool_base.h>

#include <tool/tool_menu.h>
#include <QString>

// Microwave shapes that are created as board footprints when the user requests them.
enum class MICROWAVE_FOOTPRINT_SHAPE
{
    GAP,
    STUB,
    STUB_ARC,
    FUNCTION_SHAPE,
};

/**
 * Parameters for construction of a microwave inductor
 */
struct MICROWAVE_INDUCTOR_PATTERN
{
public:
    VECTOR2I m_Start;
    VECTOR2I m_End;
    int      m_Length;       // full length trace.
    int      m_Width;        // Trace width.
};


/**
 * Tool responsible for adding microwave features to PCBs
 */
class MICROWAVE_TOOL : public PCB_TOOL_BASE
{
public:
    MICROWAVE_TOOL();
    ~MICROWAVE_TOOL();

    ///< React to model/view changes
    void Reset( RESET_REASON aReason ) override;

    ///< Bind handlers to corresponding TOOL_ACTIONs
    void setTransitions() override;

private:
    ///< Main interactive tool
    int addMicrowaveFootprint( const TOOL_EVENT& aEvent );

    ///< Create an inductor between the two points
    void createInductorBetween( const VECTOR2I& aStart, const VECTOR2I& aEnd );

    ///< Draw a microwave inductor interactively
    int drawMicrowaveInductor( const TOOL_EVENT& aEvent );

    /**
     * Create a footprint "GAP" or "STUB" used in micro wave designs.
     *
     * This footprint has 2 pads:
     * PAD_ATTRIB::SMD, rectangular, H size = V size = current track width.
     * the "gap" is isolation created between this 2 pads
     *
     * @param aComponentShape is the component to create.
     * @return the new footprint.
     */
    FOOTPRINT* createFootprint( MICROWAVE_FOOTPRINT_SHAPE aFootprintShape );

    FOOTPRINT* createPolygonShape();

    /**
     * Create an S-shaped coil footprint for microwave applications.
     */
    FOOTPRINT* createMicrowaveInductor( MICROWAVE_INDUCTOR_PATTERN& aPattern,
                                        QString& aErrorMessage );

    /**
     * Create a basic footprint for micro wave applications.
     *
     * The default pad settings are:
     *  PAD_ATTRIB::SMD, rectangular, H size = V size = current track width.
     *
     * @param aValue is the text value.
     * @param aTextSize is the size of ref and value texts ( <= 0 to use board default values ).
     * @param aPadCount is number of pads.
     * @return the new footprint.
     */
    FOOTPRINT* createBaseFootprint( const QString& aValue, int aTextSize, int aPadCount );
};


#endif // TOOLS_MICROWAVE_TOOL_H
