
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef __CONNECTIVITY_DATA_H
#define __CONNECTIVITY_DATA_H

#include <core/typeinfo.h>
#include <core/spinlock.h>

#include <memory>
#include <mutex>
#include <vector>
#include <QString>

#include <math/vector2d.h>
#include <geometry/shape_poly_set.h>
#include <project/net_settings.h>
#include <zone.h>

class FROM_TO_CACHE;
class CN_CLUSTER;
class CN_CONNECTIVITY_ALGO;
class CN_EDGE;
class BOARD;
class BOARD_COMMIT;
class BOARD_CONNECTED_ITEM;
class BOARD_ITEM;
class ZONE;
class RN_DATA;
class RN_NET;
class PCB_TRACK;
class PCB_VIA;
class PAD;
class FOOTPRINT;
class PROGRESS_REPORTER;


struct CN_DISJOINT_NET_ENTRY
{
    int net;
    BOARD_CONNECTED_ITEM *a, *b;
    VECTOR2I anchorA, anchorB;
};


struct RN_DYNAMIC_LINE
{
    int netCode;
    VECTOR2I a, b;
};


/**
 * Controls how nets are propagated through clusters
 */
enum class PROPAGATE_MODE
{
    SKIP_CONFLICTS,     ///< Clusters with conflicting drivers are not updated (default)
    RESOLVE_CONFLICTS   ///< Clusters with conflicting drivers are updated to the most popular net
};


// a wrapper class encompassing the connectivity computation algorithm and the
class CONNECTIVITY_DATA
{
public:
    CONNECTIVITY_DATA();
    ~CONNECTIVITY_DATA();

    CONNECTIVITY_DATA( std::shared_ptr<CONNECTIVITY_DATA> aGlobalConnectivity,
                       const std::vector<BOARD_ITEM*>& aLocalItems, bool                               aSkipRatsnestUpdate = false );

    // Builds the connectivity database for the board aBoard
    bool Build( BOARD* aBoard, PROGRESS_REPORTER* aReporter = nullptr );

    // Builds the connectivity database for a set of items aItems
    void Build( std::shared_ptr<CONNECTIVITY_DATA>& aGlobalConnectivity,
                const std::vector<BOARD_ITEM*>& aLocalItems );

    // Adds an item to the connectivity data
    bool Add( BOARD_ITEM* aItem );

    // Removes an item from the connectivity data
    bool Remove( BOARD_ITEM* aItem );

    // Updates the connectivity data for an item
    bool Update( BOARD_ITEM* aItem );

    // Moves the connectivity list anchors. N.B., this does not move the bounding
    // boxes for the RTree, so the use of this function will invalidate the
    // connectivity data for uses other than the dynamic ratsnest
    void Move( const VECTOR2I& aDelta );

    // Erases the connectivity database
    void ClearRatsnest();

    // Returns the total number of nets in the connectivity database
    int GetNetCount() const;

    // Returns the ratsnest, expressed as a set of graph edges for a given net
    RN_NET* GetRatsnestForNet( int aNet );

    // Propagates the net codes from the source pads to the tracks/vias
    void PropagateNets( BOARD_COMMIT* aCommit = nullptr );

    // Fill the isolate islands list for each layer of each zone. Isolated islands are individual
    // polygons in a zone fill that don't connect to a net
    void FillIsolatedIslandsMap( std::map<ZONE*, std::map<PCB_LAYER_ID, ISOLATED_ISLANDS>>& aMap,
                                 bool aConnectivityAlreadyRebuilt = false );

    // Updates the ratsnest for the board
    void RecalculateRatsnest( BOARD_COMMIT* aCommit = nullptr );

    unsigned int GetUnconnectedCount( bool aVisibileOnly ) const;

    bool IsConnectedOnLayer( const BOARD_CONNECTED_ITEM* aItem, int aLayer,
                             const std::initializer_list<KICAD_T>& aTypes = {} ) const;

    unsigned int GetNodeCount( int aNet = -1 ) const;

    unsigned int GetPadCount( int aNet = -1 ) const;

    const std::vector<PCB_TRACK*> GetConnectedTracks( const BOARD_CONNECTED_ITEM* aItem ) const;

    const std::vector<PAD*> GetConnectedPads( const BOARD_CONNECTED_ITEM* aItem ) const;

    void GetConnectedPads( const BOARD_CONNECTED_ITEM* aItem, std::set<PAD*>* pads ) const;

    void GetConnectedPadsAndVias( const BOARD_CONNECTED_ITEM* aItem, std::vector<PAD*>* pads,
                                  std::vector<PCB_VIA*>* vias );

    // Returns a list of items connected to a source item aItem at position aAnchor
    // with an optional maximum distance from the defined anchor
    const std::vector<BOARD_CONNECTED_ITEM*>
    GetConnectedItemsAtAnchor( const BOARD_CONNECTED_ITEM* aItem, const VECTOR2I& aAnchor,
                               const std::vector<KICAD_T>& aTypes, const int& aMaxError = 0 ) const;

    void RunOnUnconnectedEdges( std::function<bool( CN_EDGE& )> aFunc );

    bool TestTrackEndpointDangling( PCB_TRACK* aTrack, bool aIgnoreTracksInPads,
                                    VECTOR2I* aPos = nullptr ) const;

    // Erases the temporary, selection-based ratsnest (i.e. the ratsnest lines that pcbnew
    // displays when moving an item/set of items)
    void ClearLocalRatsnest();

    // Hides the temporary, selection-based ratsnest lines
    void HideLocalRatsnest();

    // Calculates the temporary (usually selection-based) ratsnest for the set of aItems
    void ComputeLocalRatsnest( const std::vector<BOARD_ITEM*>& aItems,
                               const CONNECTIVITY_DATA* aDynamicData,
                               VECTOR2I aInternalOffset = { 0, 0 } );

    const std::vector<RN_DYNAMIC_LINE>& GetLocalRatsnest() const { return m_dynamicRatsnest; }

    // Returns a list of items connected to a source item aItem
    const std::vector<BOARD_CONNECTED_ITEM*>
    GetConnectedItems( const BOARD_CONNECTED_ITEM* aItem, const std::vector<KICAD_T>& aTypes,
                       bool aIgnoreNetcodes = false ) const;

    // Returns the list of items that belong to a certain net
    const std::vector<BOARD_CONNECTED_ITEM*>
    GetNetItems( int aNetCode, const std::vector<KICAD_T>& aTypes ) const;

    void BlockRatsnestItems( const std::vector<BOARD_ITEM*>& aItems );

    std::shared_ptr<CN_CONNECTIVITY_ALGO> GetConnectivityAlgo() const { return m_connAlgo; }

    KISPINLOCK& GetLock() { return m_lock; }

    void MarkItemNetAsDirty( BOARD_ITEM* aItem );
    void RemoveInvalidRefs();

    void SetProgressReporter( PROGRESS_REPORTER* aReporter );

    const NET_SETTINGS* GetNetSettings() const;

    bool            HasNetNameForNetCode( int nc ) const { return m_netcodeMap.count( nc ) > 0; }
    const QString& GetNetNameForNetCode( int nc ) const { return m_netcodeMap.at( nc ); }

    // Refresh the map of netcodes to net names
    void RefreshNetcodeMap( BOARD* aBoard );

#ifndef SWIG
    const std::vector<CN_EDGE> GetRatsnestForItems( const std::vector<BOARD_ITEM*>& aItems );

    const std::vector<CN_EDGE> GetRatsnestForPad( const PAD* aPad );

    const std::vector<CN_EDGE> GetRatsnestForComponent( FOOTPRINT* aComponent,
                                                        bool aSkipInternalConnections = false );
#endif

    std::shared_ptr<FROM_TO_CACHE> GetFromToCache() { return m_fromToCache; }

private:

    // Updates the ratsnest for the board without locking the connectivity mutex
    void internalRecalculateRatsnest( BOARD_COMMIT* aCommit = nullptr );
    void updateRatsnest();

    void addRatsnestCluster( const std::shared_ptr<CN_CLUSTER>& aCluster );

private:
    std::shared_ptr<CN_CONNECTIVITY_ALGO> m_connAlgo;

    std::shared_ptr<FROM_TO_CACHE>  m_fromToCache;
    std::vector<RN_DYNAMIC_LINE>    m_dynamicRatsnest;
    std::vector<RN_NET*>            m_nets;

    // Used to suppress ratsnest calculations on dynamic ratsnests
    bool                            m_skipRatsnestUpdate;

    KISPINLOCK                      m_lock;

    PROGRESS_REPORTER*              m_progressReporter;

    // Used to get netclass data when drawing ratsnests
    std::weak_ptr<NET_SETTINGS> m_netSettings;

    /// Used to map netcode to net name
    std::map<int, QString> m_netcodeMap;
};

#endif
