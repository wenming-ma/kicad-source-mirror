
#ifndef CLASS_NETCLASS_H
#define CLASS_NETCLASS_H

#include <optional>
#include <QString>

#include <gal/color4d.h>
#include <kicommon.h>
#include <macros_swig.h>
#include <stroke_params.h>
#include <api/serializable.h>

using KIGFX::COLOR4D;

DECL_SET_FOR_SWIG( STRINGSET, QString )

// A collection of nets and the parameters used to route or test these nets
class KICOMMON_API NETCLASS : public SERIALIZABLE
{
public:
    static const char Default[];

    NETCLASS( const QString& aName, bool aInitWithDefaults = true );

    ~NETCLASS(){};

    NETCLASS( const NETCLASS& ) = delete;
    NETCLASS& operator=( const NETCLASS& ) = delete;

    bool operator==( const NETCLASS& other ) const;

    QString GetClass() const
    {
        return QStringLiteral( "NETCLASS" );
    }

    //void Serialize( google::protobuf::Any &aContainer ) const override;
    //bool Deserialize( const google::protobuf::Any &aContainer ) override;

    void ResetParents();
    void ResetParameters();

    const std::vector<NETCLASS*>& GetConstituentNetclasses() const;
    void SetConstituentNetclasses( std::vector<NETCLASS*>&& constituents );

    bool ContainsNetclassWithName( const QString& netclass ) const;
    bool IsDefault() const { return m_isDefault; }

    void SetName( const QString& aName )
    {
        m_Name = aName;

        if( aName == Default )
            m_isDefault = true;
    }

    const QString GetName() const;
    const QString GetHumanReadableName() const;

    const QString& GetDescription() const  { return m_Description; }
    void  SetDescription( const QString& aDesc ) { m_Description = aDesc; }

    bool    HasClearance() const { return (bool) m_Clearance; }
    int     GetClearance() const { return m_Clearance.value_or(-1); }
    std::optional<int> GetClearanceOpt() const { return m_Clearance; }
    void    SetClearance( int aClearance )  { m_Clearance = aClearance; }
    void    SetClearance( std::optional<int> aClearance ) { m_Clearance = aClearance; }
    void    SetClearanceParent( NETCLASS* parent) { m_clearanceParent = parent; }
    NETCLASS* GetClearanceParent() const { return m_clearanceParent; }

    bool    HasTrackWidth() const { return (bool) m_TrackWidth; }
    int     GetTrackWidth() const           { return m_TrackWidth.value_or( -1 ); }
    std::optional<int> GetTrackWidthOpt() const { return m_TrackWidth; }
    void    SetTrackWidth( int aWidth )     { m_TrackWidth = aWidth; }
    void    SetTrackWidth( std::optional<int> aWidth ) { m_TrackWidth = aWidth; }
    void    SetTrackWidthParent( NETCLASS* parent) { m_trackWidthParent = parent; }
    NETCLASS* GetTrackWidthParent() const { return m_trackWidthParent; }

    bool    HasViaDiameter() const          { return (bool) m_ViaDia; }
    int     GetViaDiameter() const          { return m_ViaDia.value_or( -1 ); }
    std::optional<int> GetViaDiameterOpt() const { return m_ViaDia; }
    void    SetViaDiameter( int aDia )      { m_ViaDia = aDia; }
    void    SetViaDiameter( std::optional<int> aDia ) { m_ViaDia = aDia; }
    void    SetViaDiameterParent( NETCLASS* parent) { m_viaDiameterParent = parent; }
    NETCLASS* GetViaDiameterParent() const { return m_viaDiameterParent; }

    int     HasViaDrill() const             { return (bool) m_ViaDrill; }
    int     GetViaDrill() const             { return m_ViaDrill.value_or( -1 ); }
    std::optional<int> GetViaDrillOpt() const { return m_ViaDrill; }
    void    SetViaDrill( int aSize )        { m_ViaDrill = aSize; }
    void    SetViaDrill( std::optional<int> aSize ) { m_ViaDrill = aSize; }
    void    SetViaDrillParent( NETCLASS* parent) { m_viaDrillParent = parent; }
    NETCLASS* GetViaDrillParent() const { return m_viaDrillParent; }

    bool    HasuViaDiameter() const         { return (bool) m_uViaDia; }
    int     GetuViaDiameter() const         { return m_uViaDia.value_or( -1 ); }
    std::optional<int> GetuViaDiameterOpt() const { return m_uViaDia; }
    void    SetuViaDiameter( int aSize )    { m_uViaDia = aSize; }
    void    SetuViaDiameter( std::optional<int> aSize ) { m_uViaDia = aSize; }
    void    SetuViaDiameterParent( NETCLASS* parent) { m_uViaDiaParent = parent; }
    NETCLASS* GetuViaDiameterParent() const { return m_uViaDiaParent; }

    bool    HasuViaDrill() const            { return (bool) m_uViaDrill; }
    int     GetuViaDrill() const            { return m_uViaDrill.value_or( -1 ); }
    std::optional<int> GetuViaDrillOpt() const { return m_uViaDrill; }
    void    SetuViaDrill( int aSize )       { m_uViaDrill = aSize; }
    void    SetuViaDrill( std::optional<int> aSize ) { m_uViaDrill = aSize; }
    void    SetuViaDrillParent( NETCLASS* parent) { m_uViaDrillParent = parent; }
    NETCLASS* GetuViaDrillParent() const { return m_uViaDrillParent; }

    bool    HasDiffPairWidth() const        { return (bool) m_diffPairWidth; }
    int     GetDiffPairWidth() const        { return m_diffPairWidth.value_or( -1 ); }
    std::optional<int> GetDiffPairWidthOpt() const { return m_diffPairWidth; }
    void    SetDiffPairWidth( int aSize )   { m_diffPairWidth = aSize; }
    void    SetDiffPairWidth( std::optional<int> aSize ) { m_diffPairWidth = aSize; }
    void    SetDiffPairWidthParent( NETCLASS* parent) { m_diffPairWidthParent = parent; }
    NETCLASS* GetDiffPairWidthParent() const { return m_diffPairWidthParent; }

    bool    HasDiffPairGap() const          { return (bool) m_diffPairGap; }
    int     GetDiffPairGap() const          { return m_diffPairGap.value_or( -1 ); }
    std::optional<int> GetDiffPairGapOpt() const { return m_diffPairGap; }
    void    SetDiffPairGap( int aSize )     { m_diffPairGap = aSize; }
    void    SetDiffPairGap( std::optional<int> aSize ) { m_diffPairGap = aSize; }
    void    SetDiffPairGapParent( NETCLASS* parent) { m_diffPairGapParent = parent; }
    NETCLASS* GetDiffPairGapParent() const { return m_diffPairGapParent; }

    bool    HasDiffPairViaGap() const       { return (bool) m_diffPairViaGap; }
    int     GetDiffPairViaGap() const       { return m_diffPairViaGap.value_or( -1 ); }
    std::optional<int> GetDiffPairViaGapOpt() const { return m_diffPairViaGap; }
    void    SetDiffPairViaGap( int aSize )  { m_diffPairViaGap = aSize; }
    void    SetDiffPairViaGap( std::optional<int> aSize ) { m_diffPairViaGap = aSize; }
    void    SetDiffPairViaGapParent( NETCLASS* parent) { m_diffPairViaGapParent = parent; }
    NETCLASS* GetDiffPairViaGapParent() const { return m_diffPairViaGapParent; }

    bool    HasPcbColor() const { return m_isDefault ? false : m_pcbColor != COLOR4D::UNSPECIFIED; }
    COLOR4D GetPcbColor( bool aIsForSave = false ) const
    {
        // If we are saving netclases, return the underlying color (which may be set from an old
        // schematic with a default color set - this allows us to roll back the no-default-colors
        // changes later if required)
        if( aIsForSave || !m_isDefault )
            return m_pcbColor;

        return COLOR4D::UNSPECIFIED;
    }
    void    SetPcbColor( const COLOR4D& aColor ) { m_pcbColor = aColor; }
    void    SetPcbColorParent( NETCLASS* parent) { m_pcbColorParent = parent; }
    NETCLASS* GetPcbColorParent() const { return m_pcbColorParent; }

    bool    HasWireWidth() const            { return (bool) m_wireWidth; }
    int     GetWireWidth() const            { return m_wireWidth.value_or( -1 ); }
    std::optional<int> GetWireWidthOpt() const { return m_wireWidth; }
    void    SetWireWidth( int aWidth )      { m_wireWidth = aWidth; }
    void    SetWireWidth( std::optional<int> aWidth ) { m_wireWidth = aWidth; }
    void    SetWireWidthParent( NETCLASS* parent) { m_wireWidthParent = parent; }
    NETCLASS* GetWireWidthParent() const { return m_wireWidthParent; }

    bool    HasBusWidth() const             { return (bool) m_busWidth; }
    int     GetBusWidth() const             { return m_busWidth.value_or( -1 ); }
    std::optional<int> GetBusWidthOpt() const { return m_busWidth; }
    void    SetBusWidth( int aWidth )       { m_busWidth = aWidth; }
    void    SetBusWidth( std::optional<int> aWidth ) { m_busWidth = aWidth; }
    void    SetBusWidthParent( NETCLASS* parent) { m_busWidthParent = parent; }
    NETCLASS* GetBusWidthParent() const { return m_busWidthParent; }

    COLOR4D GetSchematicColor( bool aIsForSave = false ) const
    {
        // If we are saving netclases, return the underlying color (which may be set from an old
        // schematic with a default color set - this allows us to roll back the no-default-colors
        // changes later if required)
        if( aIsForSave || !m_isDefault )
            return m_schematicColor;

        return COLOR4D::UNSPECIFIED;
    }
    void    SetSchematicColor( COLOR4D aColor ) { m_schematicColor = aColor; }
    void    SetSchematicColorParent( NETCLASS* parent) { m_schematicColorParent = parent; }
    NETCLASS* GetSchematicColorParent() const { return m_schematicColorParent; }

    bool    HasLineStyle() const            { return (bool) m_lineStyle; }
    int     GetLineStyle() const            { return m_lineStyle.value_or( 0 ); }
    std::optional<int> GetLineStyleOpt() const { return m_lineStyle; }
    void    SetLineStyle( int aStyle )      { m_lineStyle = aStyle; }
    void    SetLineStyle( std::optional<int> aStyle ) { m_lineStyle = aStyle; }
    void    SetLineStyleParent( NETCLASS* parent) { m_lineStyleParent = parent; }
    NETCLASS* GetLineStyleParent() const { return m_lineStyleParent; }

    void    SetPriority( int aPriority )    { m_Priority = aPriority; }
    int     GetPriority() const             { return m_Priority; }

protected:
    bool m_isDefault;

    std::vector<NETCLASS*> m_constituents;

    QString m_Name;
    int     m_Priority;
    QString m_Description;

    std::optional<int> m_Clearance;

    std::optional<int> m_TrackWidth;
    std::optional<int> m_ViaDia;
    std::optional<int> m_ViaDrill;

    std::optional<int> m_uViaDia;
    std::optional<int> m_uViaDrill;

    std::optional<int> m_diffPairWidth;
    std::optional<int> m_diffPairGap;
    std::optional<int> m_diffPairViaGap;

    std::optional<int> m_wireWidth;
    std::optional<int> m_busWidth;
    COLOR4D            m_schematicColor;
    std::optional<int> m_lineStyle;

    COLOR4D            m_pcbColor;

    // The NETCLASS providing each parameter
    NETCLASS* m_clearanceParent;
    NETCLASS* m_trackWidthParent;
    NETCLASS* m_viaDiameterParent;
    NETCLASS* m_viaDrillParent;
    NETCLASS* m_uViaDiaParent;
    NETCLASS* m_uViaDrillParent;
    NETCLASS* m_diffPairWidthParent;
    NETCLASS* m_diffPairGapParent;
    NETCLASS* m_diffPairViaGapParent;
    NETCLASS* m_pcbColorParent;
    NETCLASS* m_wireWidthParent;
    NETCLASS* m_busWidthParent;
    NETCLASS* m_schematicColorParent;
    NETCLASS* m_lineStyleParent;
};

#endif  // CLASS_NETCLASS_H
