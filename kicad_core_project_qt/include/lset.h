
#ifndef LSET_H
#define LSET_H

#include <layer_ids.h>
#include <base_set.h>

class LSEQ;
class LAYER_RANGE;

// LSET is a set of PCB_LAYER_IDs
class KICOMMON_API LSET : public BASE_SET
{
public:

    // Create an empty (cleared) set
    LSET() : BASE_SET( PCB_LAYER_ID_COUNT ) {} // all bits are set to zero in BASE_SET()

    LSET( const BASE_SET& aOther ) : BASE_SET( aOther ) {}

    LSET( std::initializer_list<PCB_LAYER_ID> aList );

    LSET( const std::vector<PCB_LAYER_ID>& aList );

    LSET( const LSEQ& aSeq );

    LSET( const LAYER_RANGE& aRange );

    LSET( unsigned long __val ) = delete;

    // See if the layer set contains a PCB layer
    bool Contains( PCB_LAYER_ID aLayer ) const
    {
        // At the moment, LSET cannot store negative layers, but PCB_LAYER_ID can contain them
        if( aLayer < 0 )
           return false;

        try
        {
            return test( aLayer );
        }
        catch( std::out_of_range& )
        {
            return false;
        }
    }

    // Return the fixed name association with aLayerId
    static QString Name( PCB_LAYER_ID aLayerId );

    // Return the layer number from a layer name
    static int NameToLayer( QString& aName );

    // Return true if aLayer is between aStart and aEnd, inclusive
    static bool IsBetween( PCB_LAYER_ID aStart, PCB_LAYER_ID aEnd, PCB_LAYER_ID aLayer );

    // Return a complete set of internal copper layers
    static const LSET& InternalCuMask();

    // Return a complete set of all top assembly layers
    static const LSET& FrontAssembly();

    // Return a complete set of all bottom assembly layers
    static const LSET& BackAssembly();

    // Return a mask holding the requested number of Cu PCB_LAYER_IDs
    static LSET AllCuMask( int aCuLayerCount );

    // return AllCuMask( MAX_CU_LAYERS )
    static LSET AllCuMask();

    // Return a mask holding the Front and Bottom layers
    static const LSET& ExternalCuMask();

    // Return a mask holding all layer minus CU layers
    static LSET AllNonCuMask();

    static const LSET& AllLayersMask();

    // Return a mask holding all technical layers (no CU layer) on front side
    static const LSET& FrontTechMask();

    // Return a mask holding technical layers used in a board fabrication
    static const LSET& FrontBoardTechMask();

    // Return a mask holding all technical layers (no CU layer) on back side
    static const LSET& BackTechMask();

    // Return a mask holding technical layers used in a board fabrication
    static const LSET& BackBoardTechMask();

    // Return a mask holding all technical layers (no CU layer) on both side
    static const LSET& AllTechMask();

    // Return a mask holding board technical layers (no CU layer) on both side
    static const LSET& AllBoardTechMask();

    // Return a mask holding all technical layers and the external CU layer on front side
    static const LSET& FrontMask();

    // Return a mask holding all technical layers and the external CU layer on back side
    static const LSET& BackMask();

    static const LSET& SideSpecificMask();

    static const LSET& UserMask();

    // Return a mask holding all layers which are physically realized
    static const LSET& PhysicalLayersMask();

    // Return a mask with the requested number of user defined layers
    static LSET UserDefinedLayersMask( int aUserDefinedLayerCount = MAX_USER_DEFINED_LAYERS );

    // Layers which are not allowed within footprint definitions

    static const LSET& ForbiddenFootprintLayers();

    // Return a sequence of copper layers from front to back
    LSEQ CuStack() const;

    // Return the technical and user layers in the order shown in layer widget
    LSEQ TechAndUserUIOrder() const;

    // Return the copper, technical and user layers in the order shown in layer widget
    LSEQ UIOrder() const;

    // Return an LSEQ from the union of this LSET and a desired sequence
    LSEQ Seq( const LSEQ& aSequence ) const;

    // Return a LSEQ from this LSET in ascending PCB_LAYER_ID order
    LSEQ Seq() const;

    // Generate a sequence of layers that represent a top to bottom stack
    LSEQ SeqStackupTop2Bottom( PCB_LAYER_ID aSelectedLayer = UNDEFINED_LAYER ) const;

    // Return the sequence that is typical for a bottom-to-top stack-up
    LSEQ SeqStackupForPlotting() const;

    // Execute a function on each layer of the LSET
    void RunOnLayers( const std::function<void( PCB_LAYER_ID )>& aFunction ) const
    {
        for( size_t ii = 0; ii < size(); ++ii )
        {
            if( test( ii ) )
                aFunction( PCB_LAYER_ID( ii ) );
        }
    }

    // Find the first set PCB_LAYER_ID
    PCB_LAYER_ID ExtractLayer() const;


    // Flip the layers in this set
    LSET& FlipStandardLayers( int aCopperLayersCount = 0 );

    // Return the number of layers between aStart and aEnd, inclusive
    static int LayerCount( PCB_LAYER_ID aStart, PCB_LAYER_ID aEnd, int aCopperLayerCount );

    // Clear the copper layers in this set
    LSET& ClearCopperLayers();

    // Clear the non-copper layers in this set
    LSET& ClearNonCopperLayers();

    // Clear the user defined layers in this set
    LSET& ClearUserDefinedLayers();

#ifndef SWIG
    // Custom iterator to iterate over all set bits
    class KICOMMON_API all_set_layers_iterator : public BASE_SET::set_bits_iterator
    {
    public:
        all_set_layers_iterator( const BASE_SET& set, size_t index ) :
                BASE_SET::set_bits_iterator( set, index )
        {
        }

        PCB_LAYER_ID operator*() const
        {
            return PCB_LAYER_ID( BASE_SET::set_bits_iterator::operator*() );
        }

        all_set_layers_iterator& operator++()
        {
            BASE_SET::set_bits_iterator::operator++();
            return *this;
        }
    };

    all_set_layers_iterator begin() const { return all_set_layers_iterator( *this, 0 ); }
    all_set_layers_iterator end() const { return all_set_layers_iterator( *this, size() ); }

    // Custom iterators for Copper and Non-Copper layers
    class KICOMMON_API copper_layers_iterator : public BASE_SET::set_bits_iterator
    {
    public:
        copper_layers_iterator( const BASE_SET& set, size_t index );
        PCB_LAYER_ID            operator*() const;
        copper_layers_iterator& operator++();

    private:
        void advance_to_next_set_copper_bit();
        void next_copper_layer();
    };

    class KICOMMON_API non_copper_layers_iterator : public BASE_SET::set_bits_iterator
    {
    public:
        non_copper_layers_iterator( const BASE_SET& set, size_t index );
        PCB_LAYER_ID                operator*() const;
        non_copper_layers_iterator& operator++();

    private:
        void advance_to_next_set_non_copper_bit();
    };

    copper_layers_iterator copper_layers_begin() const;
    copper_layers_iterator copper_layers_end() const;
    non_copper_layers_iterator non_copper_layers_begin() const;
    non_copper_layers_iterator non_copper_layers_end() const;

#endif

};
#endif // LSET_H

