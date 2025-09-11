// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05
#ifndef __PNS_SIZES_SETTINGS_H
#define __PNS_SIZES_SETTINGS_H

#include <map>
#include <optional>
#include <QString>

#include "pcb_track.h" // for VIATYPE_T

class BOARD;
class BOARD_DESIGN_SETTINGS;

namespace PNS {

class ITEM;

class SIZES_SETTINGS
{
public:
    SIZES_SETTINGS() :
            m_clearance( 0 ),
            m_minClearance( 0 ),
            m_trackWidth( 155000 ),
            m_trackWidthIsExplicit( true ),
            m_boardMinTrackWidth( 0 ),
            m_viaType( VIATYPE::THROUGH ),
            m_viaDiameter( 600000 ),
            m_viaDrill( 250000 ),
            m_diffPairWidth( 125000 ),
            m_diffPairGap( 180000 ),
            m_diffPairViaGap( 180000 ),
            m_diffPairViaGapSameAsTraceGap( true ),
            m_holeToHole( 0 ),
            m_diffPairHoleToHole( 0 )
    {};

    ~SIZES_SETTINGS() {};

    void ClearLayerPairs();
    void AddLayerPair( int aL1, int aL2 );

    // The working clearance, from the current net to anything else (without knowing the net of the
    // other object).  Specific clearance to another object may be resolved differently.
    int Clearance() const { return m_clearance; }
    void SetClearance( int aClearance ) { m_clearance = aClearance; }

    // The absolute minimum clearance from the board
    int MinClearance() const { return m_minClearance; }
    void SetMinClearance( int aClearance ) { m_minClearance = aClearance; }

    int TrackWidth() const { return m_trackWidth; }
    void SetTrackWidth( int aWidth ) { m_trackWidth = aWidth; }

    bool TrackWidthIsExplicit() const { return m_trackWidthIsExplicit; }
    void SetTrackWidthIsExplicit( bool aIsExplicit ) { m_trackWidthIsExplicit = aIsExplicit; }

    int BoardMinTrackWidth() const { return m_boardMinTrackWidth; }
    void SetBoardMinTrackWidth( int aWidth ) { m_boardMinTrackWidth = aWidth; }

    int DiffPairWidth() const { return m_diffPairWidth; }
    int DiffPairGap() const { return m_diffPairGap; }

    int DiffPairViaGap() const
    {
        return m_diffPairViaGapSameAsTraceGap ? m_diffPairGap : m_diffPairViaGap;
    }

    bool DiffPairViaGapSameAsTraceGap() const { return m_diffPairViaGapSameAsTraceGap; }

    void SetDiffPairWidth( int aWidth ) { m_diffPairWidth = aWidth; }
    void SetDiffPairGap( int aGap ) { m_diffPairGap = aGap; }
    void SetDiffPairViaGapSameAsTraceGap ( bool aEnable )
    {
        m_diffPairViaGapSameAsTraceGap = aEnable;
    }

    void SetDiffPairViaGap( int aGap ) { m_diffPairViaGap = aGap; }

    int ViaDiameter() const { return m_viaDiameter; }
    void SetViaDiameter( int aDiameter ) { m_viaDiameter = aDiameter; }

    int ViaDrill() const { return m_viaDrill; }
    void SetViaDrill( int aDrill ) { m_viaDrill = aDrill; }

    std::optional<int> PairedLayer( int aLayerId )
    {
        if( m_layerPairs.find(aLayerId) == m_layerPairs.end() )
            return std::optional<int>();

        return m_layerPairs[aLayerId];
    }

    int GetLayerTop() const;
    int GetLayerBottom() const;

    void SetHoleToHole( int aHoleToHole ) { m_holeToHole = aHoleToHole; }
    int GetHoleToHole() const { return m_holeToHole; }

    void SetViaType( VIATYPE aViaType ) { m_viaType = aViaType; }
    VIATYPE ViaType() const { return m_viaType; }

    QString GetClearanceSource() const { return m_clearanceSource; }
    void SetClearanceSource( const QString& aSource ) { m_clearanceSource = aSource; }

    QString GetWidthSource() const { return m_widthSource; }
    void SetWidthSource( const QString& aSource ) { m_widthSource = aSource; }

    QString GetDiffPairWidthSource() const { return m_diffPairWidthSource; }
    void SetDiffPairWidthSource( const QString& aSource ) { m_diffPairWidthSource = aSource; }

    QString GetDiffPairGapSource() const { return m_diffPairGapSource; }
    void SetDiffPairGapSource( const QString& aSource ) { m_diffPairGapSource = aSource; }

    void SetDiffPairHoleToHole( int aHoleToHole ) { m_diffPairHoleToHole = aHoleToHole; }
    int GetDiffPairHoleToHole() const { return m_diffPairHoleToHole; }

private:
    int     m_clearance;
    int     m_minClearance;
    int     m_trackWidth;
    bool    m_trackWidthIsExplicit;
    int     m_boardMinTrackWidth;

    VIATYPE m_viaType;
    int     m_viaDiameter;
    int     m_viaDrill;

    int     m_diffPairWidth;
    int     m_diffPairGap;
    int     m_diffPairViaGap;
    bool    m_diffPairViaGapSameAsTraceGap;

    int     m_holeToHole;
    int     m_diffPairHoleToHole;

    std::map<int, int> m_layerPairs;

    QString m_clearanceSource;
    QString m_widthSource;
    QString m_diffPairWidthSource;
    QString m_diffPairGapSource;
};

}

#endif // __PNS_SIZES_SETTINGS_H
