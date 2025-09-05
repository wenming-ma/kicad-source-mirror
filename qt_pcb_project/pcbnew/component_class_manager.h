
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCBNEW_COMPONENT_CLASS_MANAGER_H
#define PCBNEW_COMPONENT_CLASS_MANAGER_H

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <QString>

#include <board_item.h>

/*
 * A lightweight representation of a component class. The membership within
 * m_consituentClasses allows determination of the type of class this is:
 *
 * m_constituentClasses.size() == 0: This is a null class (no assigment).
 *     m_name is empty.
 * m_constituentClasses.size() == 1: This is an atomic class. The constituent class
 *     pointer refers to itself. m_name contains the name of the atomic class
 * m_constituentClasses.size() > 1: This is a composite class. The constituent class
 *     pointers refer to all atomic members. m_name contains a comma-delimited list of
 *     all atomic member class names.
 */
class COMPONENT_CLASS
{
public:
    COMPONENT_CLASS( const QString& name ) : m_name( name ) {}

    /// Fetches the display name of this component class
    QString GetName() const;

    /// Fetches the full name of this component class
    const QString& GetFullName() const { return m_name; }

    /// Adds a constituent component class to an effective component class
    void AddConstituentClass( COMPONENT_CLASS* componentClass );

    /// Determines if this (effective) component class contains a specific sub-class
    bool ContainsClassName( const QString& className ) const;

    /// Determines if this (effective) component class is empty (i.e. no classes defined)
    bool IsEmpty() const;

    /// Fetches a vector of the constituent classes for this (effective) class
    const std::vector<COMPONENT_CLASS*>& GetConstituentClasses() const
    {
        return m_constituentClasses;
    }

private:
    /// The full name of the component class
    QString m_name;

    /// The COMPONENT_CLASS objects contributing to this complete component class
    std::vector<COMPONENT_CLASS*> m_constituentClasses;
};

/*
 * A class to manage Component Classes in a board context
 *
 * This manager owns generated COMPONENT_CLASS objects, and guarantees that pointers to managed
 * objects are valid for the duration of the board lifetime. Note that, in order to maintain this
 * guarantee, there are two methods that must be called when updating the board from the netlist
 * (InitNetlistUpdate and FinishNetlistUpdate).
 */
class COMPONENT_CLASS_MANAGER
{
public:
    COMPONENT_CLASS_MANAGER();

    /// @brief Gets the full effective class name for the given set of constituent classes
    static QString
    GetFullClassNameForConstituents( const std::unordered_set<QString>& classNames );

    /// @brief Gets the full effective class name for the given set of constituent classes
    /// @param classNames a sorted vector of consituent class names
    static QString GetFullClassNameForConstituents( const std::vector<QString>& classNames );

    /// @brief Gets an effective component class for the given constituent class names
    /// @param classes The names of the constituent component classes
    /// @return Effective COMPONENT_CLASS object
    COMPONENT_CLASS* GetEffectiveComponentClass( const std::unordered_set<QString>& classNames );

    /// Returns the unassigned component class
    const COMPONENT_CLASS* GetNoneComponentClass() const { return m_noneClass.get(); }

    /// Prepare the manager for a board update
    /// Must be called prior to updating the PCB from the netlist
    void InitNetlistUpdate();

    /// Cleans up the manager after a board update
    /// Must be called after updating the PCB from the netlist
    void FinishNetlistUpdate();

    /// Resets the contents of the manager
    // All pointers to COMPONENT_CLASS objects will being invalid
    void Reset();

    /// @brief Fetches a read-only map of the fundamental component classes
    const std::unordered_map<QString, std::unique_ptr<COMPONENT_CLASS>>& GetClasses() const
    {
        return m_classes;
    }

protected:
    /// All individual component classes
    std::unordered_map<QString, std::unique_ptr<COMPONENT_CLASS>> m_classes;

    /// Generated effective component classes
    std::unordered_map<QString, std::unique_ptr<COMPONENT_CLASS>> m_effectiveClasses;

    /// Cache of all individual component classes (for netlist updating)
    std::unordered_map<QString, std::unique_ptr<COMPONENT_CLASS>> m_classesCache;

    /// Cache of all generated effective component classes (for netlist updating)
    std::unordered_map<QString, std::unique_ptr<COMPONENT_CLASS>> m_effectiveClassesCache;

    /// The class to represent an unassigned component class
    std::unique_ptr<COMPONENT_CLASS> m_noneClass;
};

#endif
