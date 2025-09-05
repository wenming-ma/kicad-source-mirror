
#ifndef KICAD_NET_SETTINGS_H
#define KICAD_NET_SETTINGS_H

#include <vector>
#include <set>
#include <memory>
#include <map>

#include <netclass.h>
#include <settings/nested_settings.h>
#include <eda_pattern_match.h>

class KICOMMON_API NET_SETTINGS : public NESTED_SETTINGS
{
public:
    NET_SETTINGS( JSON_SETTINGS* aParent, const std::string& aPath );

    virtual ~NET_SETTINGS();

    bool operator==( const NET_SETTINGS& aOther ) const;

    bool operator!=( const NET_SETTINGS& aOther ) const { return !operator==( aOther ); }

    Sets the default netclass for the project
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void SetDefaultNetclass( std::shared_ptr<NETCLASS> netclass );

    Gets the default netclass for the project
    std::shared_ptr<NETCLASS> GetDefaultNetclass();

    Determines if the given netclass exists
    bool HasNetclass( const QString& netclassName ) const;

    Sets the given netclass
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void SetNetclass( const QString& netclassName, std::shared_ptr<NETCLASS>& netclass );

    Sets all netclass
    /// Calling this method will reset the effective netclass calculation caches
    void SetNetclasses( const std::map<QString, std::shared_ptr<NETCLASS>>& netclasses );

    Gets all netclasses
    const std::map<QString, std::shared_ptr<NETCLASS>>& GetNetclasses() const;

    Gets all composite (multiple assignment / missing defaults) netclasses
    // Note the full connectivity or board net synchronisation must be run before calling
    // this, otherwise resolved netclasses may be missing
    const std::map<QString, std::shared_ptr<NETCLASS>>& GetCompositeNetclasses() const;

    Clears all netclasses
    /// Calling this method will reset the effective netclass calculation caches
    void ClearNetclasses();

    Gets all current net name to netclasses assignments
    const std::map<QString, std::set<QString>>& GetNetclassLabelAssignments() const;

    Clears all net name to netclasses assignments
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void ClearNetclassLabelAssignments();

    Clears a specific net name to netclass assignment
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void ClearNetclassLabelAssignment( const QString& netName );

    Sets a net name to netclasses assignment
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void SetNetclassLabelAssignment( const QString&           netName,
                                     const std::set<QString>& netclasses );

    Apppends to a net name to netclasses assignment
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void AppendNetclassLabelAssignment( const QString&           netName,
                                        const std::set<QString>& netclasses );

    Determines if a given net name has netclasses assigned
    bool HasNetclassLabelAssignment( const QString& netName ) const;

    Sets a netclass pattern assignment
    /// Calling this method will reset the effective netclass calculation caches
    void SetNetclassPatternAssignment( const QString& pattern, const QString& netclass );

    Sets all netclass pattern assignments
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void SetNetclassPatternAssignments(
            std::vector<std::pair<std::unique_ptr<EDA_COMBINED_MATCHER>, QString>>&&
                    netclassPatterns );

    Gets the netclass pattern assignments
    std::vector<std::pair<std::unique_ptr<EDA_COMBINED_MATCHER>, QString>>&
    GetNetclassPatternAssignments();

    Clears all netclass pattern assignments
    void ClearNetclassPatternAssignments();

    Clears effective netclass cache for the given net
    void ClearCacheForNet( const QString& netName );

    Clears the effective netclass cache for all nets
    void ClearAllCaches();

    Sets a net to color assignment
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void SetNetColorAssignment( const QString& netName, const KIGFX::COLOR4D& color );

    Gets all net name to color assignments
    const std::map<QString, KIGFX::COLOR4D>& GetNetColorAssignments() const;

    Clears all net name to color assignments
    /// Calling user is responsible for resetting the effective netclass calculation caches
    void ClearNetColorAssignments();

    Determines if an effective netclass for the given net name has been cached
    bool HasEffectiveNetClass( const QString& aNetName ) const;

    Returns an already cached effective netclass for the given net name
    /// @return The netclass, or default netclass if not found
    std::shared_ptr<NETCLASS> GetCachedEffectiveNetClass( const QString& aNetName ) const;

    Fetches the effective (may be aggregate) netclass for the given net name
    // If the effective netclass has not been computed, it will be created and cached.
    std::shared_ptr<NETCLASS> GetEffectiveNetClass( const QString& aNetName );

    Recomputes the internal values of all aggregate effective netclasses
    /// Called when a value of a user-defined netclass changes, but the whole netclass list is not
    /// being recomputed.
    void RecomputeEffectiveNetclasses();

    /**
     * Get a NETCLASS object from a given Netclass name string
     *
     * @param aNetClassName the Netclass name to resolve
     * @return shared pointer to the requested NETCLASS object, or the default NETCLASS
    */
    std::shared_ptr<NETCLASS> GetNetClassByName( const QString& aNetName ) const;

    /**
     * Parse a bus vector (e.g. A[7..0]) into name, begin, and end.
     *
     * Ensure that begin and end are positive and that end > begin.
     *
     * @param aBus is a bus vector label string
     * @param aName out is the bus name, e.g. "A"
     * @param aMemberList is a list of member strings, e.g. "A7", "A6", and so on
     * @return true if aBus was successfully parsed
     */
    static bool ParseBusVector( const QString& aBus, QString* aName,
                                std::vector<QString>* aMemberList );

    /**
     * Parse a bus group label into the name and a list of components.
     *
     * @param aGroup is the input label, e.g. "USB{DP DM}"
     * @param name is the output group name, e.g. "USB"
     * @param aMemberList is a list of member strings, e.g. "DP", "DM"
     * @return true if aGroup was successfully parsed
     */
    static bool ParseBusGroup( const QString& aGroup, QString* name,
                               std::vector<QString>* aMemberList );

private:
    bool migrateSchema0to1();
    bool migrateSchema1to2();
    bool migrateSchema2to3();
    bool migrateSchema3to4();

    /**
     * @brief Creates an effective aggregate netclass from the given constituent netclasses
     *
     * Takes the aggregate parameters from the constituent netclasses in priority order. If any
     * parameters are missing from the overall union, then they are filled from the default
     * netclass. Note that the netclasses vector will have the default netclass added if it is used
     * to provide missing defaults. The netclasses vector will be sorted by priority 1st and then
     * name alphabetically
     */
    void makeEffectiveNetclass( std::shared_ptr<NETCLASS>& effectiveNetclass,
                                std::vector<NETCLASS*>&    netclasses ) const;

    Adds any missing fields to the given netclass from the default netclass
    /// @returns true if any fields were added from the default netclass
    bool addMissingDefaults( NETCLASS* nc ) const;

    The default netclass
    std::shared_ptr<NETCLASS> m_defaultNetClass;

    Map of netclass names to netclass definitions
    std::map<QString, std::shared_ptr<NETCLASS>> m_netClasses;

    Map of net names to resolved netclasses
    std::map<QString, std::set<QString>> m_netClassLabelAssignments;

    List of net class pattern assignments
    std::vector<std::pair<std::unique_ptr<EDA_COMBINED_MATCHER>, QString>>
            m_netClassPatternAssignments;

    Map of netclass names to netclass definitions for
    // composite (multiple netclass assignment / missing defaults) netclasses
    std::map<QString, std::shared_ptr<NETCLASS>> m_compositeNetClasses;

    Map of netclass names to netclass definitions for implicit netclasses
    ///
    /// Implicit netclasses are those which are in a netclass label, but which do not have a
    /// netclass definition in the netclass setup panel. They contribute as a constituent
    /// netclass to enable DRC rules and name resolution, but do not contribute parameters
    // to the effective netclasses which contain them.
    std::map<QString, std::shared_ptr<NETCLASS>> m_impicitNetClasses;

    Cache of nets to pattern-matched netclasses
    std::map<QString, std::shared_ptr<NETCLASS>> m_effectiveNetclassCache;

    /**
     * A map of fully-qualified net names to colors used in the board context.
     * Since these color overrides are for the board, buses are not included here.
     * Only nets that the user has assigned custom colors to will be in this list.
     * Nets that no longer exist will be deleted during a netlist read in Pcbnew.
     */
    std::map<QString, KIGFX::COLOR4D> m_netColorAssignments;

    // TODO: Add diff pairs, bus information, etc.
};

#endif // KICAD_NET_SETTINGS_H
