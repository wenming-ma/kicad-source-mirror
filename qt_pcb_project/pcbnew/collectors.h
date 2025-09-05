
#ifndef COLLECTORS_H
#define COLLECTORS_H

// This module contains a number of COLLECTOR implementations which are used
// to augment the functionality of class PCB_EDIT_FRAME.

#include <collector.h>
#include <layer_ids.h>              // LAYER_COUNT, layer defs
#include <lset.h>
#include <view/view.h>
#include <board_item.h>

// An abstract base class whose derivatives may be passed to a GENERAL_COLLECTOR telling it what
// should be collected (aside from HitTest()ing and KICAD_T scanTypes, which are provided to the
// GENERAL_COLLECTOR through attributes or arguments separately).
// This class introduces the notion of layer locking.
class COLLECTORS_GUIDE
{
public:
    virtual ~COLLECTORS_GUIDE() {}

    // @return true if the given layer is visible, else false.
    virtual bool IsLayerVisible( PCB_LAYER_ID layer ) const = 0;

    // @return the preferred layer for HitTest()ing.
    virtual PCB_LAYER_ID GetPreferredLayer() const = 0;

    virtual bool IgnoreLockedItems() const = 0;
    virtual bool IncludeSecondary() const = 0;
    virtual bool IgnoreFPTextOnBack() const = 0;
    virtual bool IgnoreFPTextOnFront() const = 0;
    virtual bool IgnoreFootprintsOnBack() const = 0;
    virtual bool IgnoreFootprintsOnFront() const = 0;
    virtual bool IgnorePadsOnBack() const = 0;
    virtual bool IgnorePadsOnFront() const = 0;
    virtual bool IgnoreThroughHolePads() const = 0;
    virtual bool IgnorePads() const
    {
        return IgnorePadsOnFront() && IgnorePadsOnBack() && IgnoreThroughHolePads();
    }

    virtual bool IgnoreFPValues() const = 0;
    virtual bool IgnoreFPReferences() const = 0;
    virtual bool IgnoreThroughVias() const = 0;
    virtual bool IgnoreBlindBuriedVias() const = 0;
    virtual bool IgnoreMicroVias() const = 0;
    virtual bool IgnoreTracks() const = 0;
    virtual bool IgnoreZoneFills() const = 0;
    virtual bool IgnoreNoNets() const = 0;

    virtual int Accuracy() const = 0;

    virtual double OnePixelInIU() const = 0;
};



// Collect BOARD_ITEM objects.
// All this object really does is override the [] operator and return a BOARD_ITEM instead
// of a EDA_ITEM. Derive all board collector objects from this class instead of the base
// COLLECTOR object.
class PCB_COLLECTOR : public COLLECTOR
{
public:
    // Overload the COLLECTOR::operator[](int) to return a BOARD_ITEM instead of an EDA_ITEM.
    BOARD_ITEM* operator[]( int ndx ) const override
    {
        if( (unsigned)ndx < (unsigned)GetCount() )
            return (BOARD_ITEM*) m_list[ ndx ];

        return nullptr;
    }
};


// Used when the right click button is pressed, or when the select tool is in effect.
// This class can be used by window classes such as PCB_EDIT_FRAME.
class GENERAL_COLLECTOR : public PCB_COLLECTOR
{
protected:
    // A place to hold collected objects which don't match precisely the search
    // criteria, but would be acceptable if nothing else is found.
    // "2nd" choice, which will be appended to the end of COLLECTOR's prime
    // "list" at the end of the search.
    std::vector<EDA_ITEM*>      m_List2nd;

    // Determine which items are to be collected by Inspect().
    const COLLECTORS_GUIDE*     m_Guide;

public:

    static const std::vector<KICAD_T> AllBoardItems;
    static const std::vector<KICAD_T> Zones;
    static const std::vector<KICAD_T> BoardLevelItems;
    static const std::vector<KICAD_T> Footprints;
    static const std::vector<KICAD_T> PadsOrTracks;
    static const std::vector<KICAD_T> FootprintItems;
    static const std::vector<KICAD_T> Tracks;
    static const std::vector<KICAD_T> Dimensions;
    static const std::vector<KICAD_T> DraggableItems;

    GENERAL_COLLECTOR() :
            m_Guide( nullptr )
    {
        SetScanTypes( AllBoardItems );
    }

    void Empty2nd()
    {
        m_List2nd.clear();
    }

    void Append2nd( EDA_ITEM* item )
    {
        m_List2nd.push_back( item );
    }

    // Record which COLLECTORS_GUIDE to use.
    void SetGuide( const COLLECTORS_GUIDE* aGuide ) { m_Guide = aGuide; }

    const COLLECTORS_GUIDE* GetGuide() const { return m_Guide; }

    // The examining function within the INSPECTOR which is passed to the Iterate function.
    // Search and collect all the objects which match the test data.
    INSPECT_RESULT Inspect( EDA_ITEM* aTestItem, void* aTestData )  override;

    // Scan a BOARD_ITEM using this class's Inspector method, which does the collection.
    void Collect( BOARD_ITEM* aItem, const std::vector<KICAD_T>& aScanList,
                  const VECTOR2I& aRefPos, const COLLECTORS_GUIDE& aGuide );
};


// A general implementation of a COLLECTORS_GUIDE. One of its constructors is
// entitled to grab information from the program's global preferences.
class GENERAL_COLLECTORS_GUIDE : public COLLECTORS_GUIDE
{
public:

    // Constructor that grabs stuff from global preferences and uses reasonable defaults.
    GENERAL_COLLECTORS_GUIDE( LSET aVisibleLayerMask, PCB_LAYER_ID aPreferredLayer,
                              KIGFX::VIEW* aView )
    {
        static const VECTOR2I one( 1, 1 );

        m_preferredLayer            = aPreferredLayer;
        m_visibleLayers             = aVisibleLayerMask;
        m_ignoreLockedItems         = false;

#if defined(USE_MATCH_LAYER)
        m_includeSecondary          = false;
#else
        m_includeSecondary          = true;
#endif

        m_ignoreFPTextOnBack        = true;
        m_ignoreFPTextOnFront       = false;
        m_ignoreFootprintsOnBack    = true; // !Show_footprints_Cmp;
        m_ignoreFootprintsOnFront   = false;

        m_ignorePadsOnFront         = false;
        m_ignorePadsOnBack          = false;
        m_ignoreThroughHolePads     = false;

        m_ignoreFPValues            = false;
        m_ignoreFPReferences        = false;

        m_ignoreThroughVias         = false;
        m_ignoreBlindBuriedVias     = false;
        m_ignoreMicroVias           = false;
        m_ignoreTracks              = false;
        m_ignoreZoneFills           = true;
        m_ignoreNoNets              = false;

        m_onePixelInIU = abs( aView->ToWorld( one, false ).x );
        m_accuracy = KiROUND( 5 * m_onePixelInIU );
    }

    bool IsLayerVisible( PCB_LAYER_ID aLayerId ) const override
    {
        return m_visibleLayers[aLayerId];
    }
    void SetLayerVisible( PCB_LAYER_ID aLayerId, bool isVisible )
    {
        m_visibleLayers.set( aLayerId, isVisible );
    }
    void SetLayerVisibleBits( LSET aLayerBits ) { m_visibleLayers = aLayerBits; }

    PCB_LAYER_ID GetPreferredLayer() const override    { return m_preferredLayer; }
    void SetPreferredLayer( PCB_LAYER_ID aLayer )      { m_preferredLayer = aLayer; }

    bool IgnoreLockedItems() const override         { return m_ignoreLockedItems; }
    void SetIgnoreLockedItems( bool ignore )        { m_ignoreLockedItems = ignore; }

    bool IncludeSecondary() const override { return m_includeSecondary; }
    void SetIncludeSecondary( bool include ) { m_includeSecondary = include; }

    bool IgnoreFPTextOnBack() const override { return m_ignoreFPTextOnBack; }
    void SetIgnoreFPTextOnBack( bool ignore ) { m_ignoreFPTextOnBack = ignore; }

    bool IgnoreFPTextOnFront() const override { return m_ignoreFPTextOnFront; }
    void SetIgnoreFPTextOnFront( bool ignore ) { m_ignoreFPTextOnFront = ignore; }

    bool IgnoreFootprintsOnBack() const override { return m_ignoreFootprintsOnBack; }
    void SetIgnoreFootprintsOnBack( bool ignore ) { m_ignoreFootprintsOnBack = ignore; }

    bool IgnoreFootprintsOnFront() const override { return m_ignoreFootprintsOnFront; }
    void SetIgnoreFootprintsOnFront( bool ignore ) { m_ignoreFootprintsOnFront = ignore; }

    bool IgnorePadsOnBack() const override { return m_ignorePadsOnBack; }
    void SetIgnorePadsOnBack(bool ignore) { m_ignorePadsOnBack = ignore; }

    bool IgnorePadsOnFront() const override { return m_ignorePadsOnFront; }
    void SetIgnorePadsOnFront(bool ignore) { m_ignorePadsOnFront = ignore; }

    bool IgnoreThroughHolePads() const override { return m_ignoreThroughHolePads; }
    void SetIgnoreThroughHolePads(bool ignore) { m_ignoreThroughHolePads = ignore; }

    bool IgnoreFPValues() const override { return m_ignoreFPValues; }
    void SetIgnoreFPValues( bool ignore) { m_ignoreFPValues = ignore; }

    bool IgnoreFPReferences() const override { return m_ignoreFPReferences; }
    void SetIgnoreFPReferences( bool ignore) { m_ignoreFPReferences = ignore; }

    bool IgnoreThroughVias() const override { return m_ignoreThroughVias; }
    void SetIgnoreThroughVias( bool ignore ) { m_ignoreThroughVias = ignore; }

    bool IgnoreBlindBuriedVias() const override { return m_ignoreBlindBuriedVias; }
    void SetIgnoreBlindBuriedVias( bool ignore ) { m_ignoreBlindBuriedVias = ignore; }

    bool IgnoreMicroVias() const override { return m_ignoreMicroVias; }
    void SetIgnoreMicroVias( bool ignore ) { m_ignoreMicroVias = ignore; }

    bool IgnoreTracks() const override { return m_ignoreTracks; }
    void SetIgnoreTracks( bool ignore ) { m_ignoreTracks = ignore; }

    bool IgnoreZoneFills() const override { return m_ignoreZoneFills; }
    void SetIgnoreZoneFills( bool ignore ) { m_ignoreZoneFills = ignore; }

    bool IgnoreNoNets() const override { return m_ignoreNoNets; }
    void SetIgnoreNoNets( bool ignore ) { m_ignoreNoNets = ignore; }

    int  Accuracy() const override { return m_accuracy; }
    void SetAccuracy( int aValue ) { m_accuracy = aValue; }

    double OnePixelInIU() const override { return m_onePixelInIU; }

private:
    PCB_LAYER_ID m_preferredLayer;
    LSET    m_visibleLayers;
    bool    m_ignoreLockedItems;
    bool    m_includeSecondary;
    bool    m_ignoreFPTextOnBack;
    bool    m_ignoreFPTextOnFront;
    bool    m_ignoreFootprintsOnBack;
    bool    m_ignoreFootprintsOnFront;
    bool    m_ignorePadsOnFront;
    bool    m_ignorePadsOnBack;
    bool    m_ignoreThroughHolePads;
    bool    m_ignoreFPValues;
    bool    m_ignoreFPReferences;
    bool    m_ignoreThroughVias;
    bool    m_ignoreBlindBuriedVias;
    bool    m_ignoreMicroVias;
    bool    m_ignoreTracks;
    bool    m_ignoreZoneFills;
    bool    m_ignoreNoNets;
    double  m_onePixelInIU;
    int     m_accuracy;
};


// Collect all BOARD_ITEM objects of a given set of KICAD_T type(s).
class PCB_TYPE_COLLECTOR : public PCB_COLLECTOR
{
public:

    INSPECT_RESULT Inspect( EDA_ITEM* testItem, void* testData ) override;
    void Collect( BOARD_ITEM* aBoard, const std::vector<KICAD_T>& aTypes );
};


// Collect all BOARD_ITEM objects on a given layer.
// This only uses the primary object layer for comparison.
class PCB_LAYER_COLLECTOR : public PCB_COLLECTOR
{
public:
    PCB_LAYER_COLLECTOR( PCB_LAYER_ID aLayerId = UNDEFINED_LAYER ) :
        m_layer_id( aLayerId )
    { }

    void SetLayerId( PCB_LAYER_ID aLayerId ) { m_layer_id = aLayerId; }

    INSPECT_RESULT Inspect( EDA_ITEM* testItem, void* testData ) override;
    void Collect( BOARD_ITEM* aBoard, const std::vector<KICAD_T>& aTypes );

private:
    PCB_LAYER_ID m_layer_id;
};

#endif // COLLECTORS_H
