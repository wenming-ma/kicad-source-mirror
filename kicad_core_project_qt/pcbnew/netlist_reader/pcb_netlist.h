
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05
#ifndef PCB_NETLIST_H
#define PCB_NETLIST_H

#include <boost/ptr_container/ptr_vector.hpp>
#include <QString>
#include <QStringList>
#include <json_common.h>
#include <unordered_set>

#include <lib_id.h>
#include <footprint.h>
#include <ctl_flags.h>


class REPORTER;


/**
 * Used to store the component pin name to net name (and pin function)
 * associations stored in a netlist.
 */
class COMPONENT_NET
{
public:
    COMPONENT_NET() {}

    COMPONENT_NET( const QString& aPinName, const QString& aNetName,
                   const QString& aPinFunction, const QString& aPinType ) :
        m_pinName( aPinName ),
        m_netName( aNetName ),
        m_pinFunction( aPinFunction ),
        m_pinType( aPinType )
    {
    }

    const QString& GetPinName() const { return m_pinName; }
    const QString& GetNetName() const { return m_netName; }
    const QString& GetPinFunction() const { return m_pinFunction; }
    const QString& GetPinType() const { return m_pinType; }

    bool IsValid() const { return !m_pinName.isEmpty(); }

    bool operator <( const COMPONENT_NET& aNet ) const
    {
        return m_pinName < aNet.m_pinName;
    }

    int Format( OUTPUTFORMATTER* aOut, int aNestLevel, int aCtl );

private:
    QString m_pinName;
    QString m_netName;
    QString m_pinFunction;
    QString m_pinType;
};


typedef std::vector< COMPONENT_NET > COMPONENT_NETS;

/**
 * Store all of the related footprint information found in a netlist.
 */
class COMPONENT
{
public:
    COMPONENT( const LIB_ID&            aFPID,
               const QString&          aReference,
               const QString&          aValue,
               const KIID_PATH&         aPath,
               const std::vector<KIID>& aKiids )
    {
        m_fpid             = aFPID;
        m_reference        = aReference;
        m_value            = aValue;
        m_pinCount         = 0;
        m_path             = aPath;
        m_kiids            = aKiids;
    }

    virtual ~COMPONENT() { };

    void AddNet( const QString& aPinName, const QString& aNetName, const QString& aPinFunction,
                 const QString& aPinType )
    {
        m_nets.emplace_back( aPinName, aNetName, aPinFunction, aPinType );
    }

    unsigned GetNetCount() const { return m_nets.size(); }

    const COMPONENT_NET& GetNet( unsigned aIndex ) const { return m_nets[aIndex]; }

    const COMPONENT_NET& GetNet( const QString& aPinName ) const;

    void ClearNets() { m_nets.clear(); }

    void SortPins() { sort( m_nets.begin(), m_nets.end() ); }

    void SetName( const QString& aName ) { m_name = aName;}
    const QString& GetName() const { return m_name; }

    void SetLibrary( const QString& aLibrary ) { m_library = aLibrary; }
    const QString& GetLibrary() const { return m_library; }

    void SetReference( const QString& aReference ) { m_reference = aReference; }
    const QString& GetReference() const { return m_reference; }

    void SetValue( const QString& aValue ) { m_value = aValue; }
    const QString& GetValue() const { return m_value; }

    void SetFields( nlohmann::ordered_map<QString, QString>& aFields )
    {
        m_fields = std::move( aFields );
    }
    const nlohmann::ordered_map<QString, QString>& GetFields() const { return m_fields; }

    void SetProperties( std::map<QString, QString>& aProps )
    {
        m_properties = std::move( aProps );
    }
    const std::map<QString, QString>& GetProperties() const { return m_properties; }

    void SetFPID( const LIB_ID& aFPID ) { m_fpid = aFPID;  }
    const LIB_ID& GetFPID() const { return m_fpid; }

    void SetAltFPID( const LIB_ID& aFPID ) { m_altFpid = aFPID; }
    const LIB_ID& GetAltFPID() const { return m_altFpid; }

    const KIID_PATH& GetPath() const { return m_path; }

    const std::vector<KIID>& GetKIIDs() const { return m_kiids; }

    void SetFootprintFilters( const QStringList& aFilters ) { m_footprintFilters = aFilters; }
    const QStringList& GetFootprintFilters() const { return m_footprintFilters; }

    void SetPinCount( int aPinCount ) { m_pinCount = aPinCount; }
    int GetPinCount() const { return m_pinCount; }

    FOOTPRINT* GetFootprint( bool aRelease = false )
    {
        return ( aRelease ) ? m_footprint.release() : m_footprint.get();
    }

    void SetFootprint( FOOTPRINT* aFootprint );

    bool IsLibSource( const QString& aLibrary, const QString& aName ) const
    {
        return aLibrary == m_library && aName == m_name;
    }

    void Format( OUTPUTFORMATTER* aOut, int aNestLevel, int aCtl );

    void SetHumanReadablePath( const QString& aPath ) { m_humanReadablePath = aPath; }
    const QString& GetHumanReadablePath() const { return m_humanReadablePath; }

    void SetComponentClassNames( const std::unordered_set<QString>& aClassNames )
    {
        m_componentClassNames = aClassNames;
    }

    std::unordered_set<QString>& GetComponentClassNames() { return m_componentClassNames; }

private:
    std::vector<COMPONENT_NET>   m_nets;  ///< list of nets shared by the component pins

    QStringList                  m_footprintFilters;
    int                          m_pinCount;
    QString                      m_reference;
    QString                      m_value;

    // human-readable hierarchical sheet path (e.g. /root/block0/sheet1)
    QString                      m_humanReadablePath;

    /// A fully specified path to the component (but not the component: [ sheetUUID, sheetUUID, .. ]
    KIID_PATH                    m_path;

    /// A vector of possible KIIDs corresponding to all units in a symbol
    std::vector<KIID>            m_kiids;

    /// The name of the component in #m_library used when it was placed on the schematic.
    QString                      m_name;

    /// The name of the component library where #m_name was found.
    QString                      m_library;

    /// The #LIB_ID of the footprint assigned to the component.
    LIB_ID                       m_fpid;

    /// The alt LIB_ID of the footprint, when there are 2 different assigned footprints,
    /// One from the netlist, the other from the .cmp file.
    /// this one is a copy of the netlist footprint assignment
    LIB_ID                       m_altFpid;

    /// The #FOOTPRINT loaded for #m_FPID.
    std::unique_ptr<FOOTPRINT>   m_footprint;

    /// Component-specific properties found in the netlist.
    std::map<QString, QString> m_properties;

    /// Component-specific user fields found in the netlist.
    nlohmann::ordered_map<QString, QString> m_fields;

    /// Component classes for this footprint
    std::unordered_set<QString> m_componentClassNames;

    static COMPONENT_NET         m_emptyNet;
};


typedef boost::ptr_vector< COMPONENT > COMPONENTS;


/**
 * Store information read from a netlist along with the flags used to update the NETLIST in the
 * #BOARD.
 */
class NETLIST
{
public:
    NETLIST() :
        m_findByTimeStamp( false ),
        m_replaceFootprints( false )
    {
    }

    /**
     * @return true if there are no components in the netlist.
     */
    bool IsEmpty() const { return m_components.empty(); }

    /**
     * Remove all components from the netlist.
     */
    void Clear() { m_components.clear(); }

    /**
     * @return the number of components in the netlist.
     */
    unsigned GetCount() const { return m_components.size(); }

    /**
     * Return the #COMPONENT at \a aIndex.
     *
     * @param aIndex the index in #m_components to fetch.
     * @return a pointer to the #COMPONENT at \a Index.
     */
    COMPONENT* GetComponent( unsigned aIndex ) { return &m_components[ aIndex ]; }

    /**
     * Add \a aComponent to the NETLIST.
     *
     * @note If \a aComponent already exists in the NETLIST, \a aComponent is deleted
     *       to prevent memory leaks.  An assertion is raised in debug builds.
     *
     * @param aComponent is the COMPONENT to save to the NETLIST.
     */
    void AddComponent( COMPONENT* aComponent );

    /**
     * Return a #COMPONENT by \a aReference.
     *
     * @param aReference is the reference designator the #COMPONENT.
     * @return a pointer to the #COMPONENT that matches \a aReference if found.  Otherwise NULL.
     */
    COMPONENT* GetComponentByReference( const QString& aReference );

    /**
     * Return a #COMPONENT by \a aPath.
     *
     * @param aPath is the KIID_PATH [ sheetUUID, .., compUUID ] of the #COMPONENT.
     * @return a pointer to the #COMPONENT that matches \a aPath if found.  Otherwise NULL.
     */
    COMPONENT* GetComponentByPath( const KIID_PATH& aPath );

    void SortByFPID();
    void SortByReference();

    void SetFindByTimeStamp( bool aFindByTimeStamp ) { m_findByTimeStamp = aFindByTimeStamp; }
    bool IsFindByTimeStamp() const { return m_findByTimeStamp; }

    void SetReplaceFootprints( bool aReplace ) { m_replaceFootprints = aReplace; }
    bool GetReplaceFootprints() const { return m_replaceFootprints; }

    /**
     * @return true if any component with a footprint link is found.
     */
    bool AnyFootprintsLinked() const;

    void Format( const char* aDocName, OUTPUTFORMATTER* aOut, int aNestLevel, int aCtl = 0 );

#define CTL_FOR_CVPCB    (CTL_OMIT_NETS | CTL_OMIT_FILTERS | CTL_OMIT_EXTRA)

    void FormatCvpcbNetlist( OUTPUTFORMATTER* aOut )
    {
        Format( "cvpcb_netlist", aOut, 0, CTL_FOR_CVPCB );
    }

private:
    COMPONENTS m_components;          // Components found in the netlist.

    bool       m_findByTimeStamp;     // Associate components by KIID (or refdes if false)
    bool       m_replaceFootprints;   // Update footprints to match footprints defined in netlist
};


#endif   // PCB_NETLIST_H
