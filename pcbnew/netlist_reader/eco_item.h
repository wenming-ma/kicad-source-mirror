/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2024 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef ECO_ITEM_H
#define ECO_ITEM_H

#include <wx/string.h>

class COMPONENT;
class FOOTPRINT;
class PAD;

/**
 * Represents a single Engineering Change Order (ECO) item.
 * Each ECO_ITEM describes one atomic change to be made to the PCB
 * when updating from schematic.
 */
class ECO_ITEM
{
public:
    /**
     * Type of change represented by this ECO item.
     */
    enum class CHANGE_TYPE
    {
        ADD_FOOTPRINT,       ///< Add new footprint to board
        DELETE_FOOTPRINT,    ///< Remove unused footprint from board
        REPLACE_FOOTPRINT,   ///< Change footprint type/library
        UPDATE_REFERENCE,    ///< Update reference designator
        UPDATE_VALUE,        ///< Update value field
        UPDATE_PATH,         ///< Update symbol association path
        UPDATE_PROPERTIES,   ///< Update component properties
        UPDATE_ATTRIBUTES,   ///< Update attributes (exclude from BOM, etc.)
        CONNECT_PAD,         ///< Connect pad to a net
        DISCONNECT_PAD,      ///< Disconnect pad from net
        RECONNECT_PAD,       ///< Change pad net connection
        ADD_NET,             ///< Add new net to board
        RECONNECT_ZONE,      ///< Reconnect copper zone to different net
    };

    /**
     * Construct an ECO item.
     * @param aType The type of change
     * @param aReference The component reference (e.g., "R1", "U2")
     */
    ECO_ITEM( CHANGE_TYPE aType, const wxString& aReference );

    ~ECO_ITEM() = default;

    // Type and identification
    CHANGE_TYPE GetType() const { return m_type; }
    const wxString& GetReference() const { return m_reference; }

    // Description for display
    void SetDescription( const wxString& aDesc ) { m_description = aDesc; }
    const wxString& GetDescription() const { return m_description; }

    // Enable/disable state
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled( bool aEnabled ) { m_enabled = aEnabled; }

    // Associated netlist component
    void SetComponent( COMPONENT* aComponent ) { m_component = aComponent; }
    COMPONENT* GetComponent() const { return m_component; }

    // Associated board footprint
    void SetFootprint( FOOTPRINT* aFootprint ) { m_footprint = aFootprint; }
    FOOTPRINT* GetFootprint() const { return m_footprint; }

    // For pad operations
    void SetPad( PAD* aPad ) { m_pad = aPad; }
    PAD* GetPad() const { return m_pad; }

    // Old and new values for display (e.g., "10k" -> "4.7k")
    void SetOldValue( const wxString& aValue ) { m_oldValue = aValue; }
    const wxString& GetOldValue() const { return m_oldValue; }

    void SetNewValue( const wxString& aValue ) { m_newValue = aValue; }
    const wxString& GetNewValue() const { return m_newValue; }

    // For pad/net operations
    void SetPadNumber( const wxString& aPadNum ) { m_padNumber = aPadNum; }
    const wxString& GetPadNumber() const { return m_padNumber; }

    void SetNetName( const wxString& aNetName ) { m_netName = aNetName; }
    const wxString& GetNetName() const { return m_netName; }

    void SetOldNetName( const wxString& aNetName ) { m_oldNetName = aNetName; }
    const wxString& GetOldNetName() const { return m_oldNetName; }

    /**
     * Get a user-friendly name for a change type.
     */
    static wxString GetChangeTypeName( CHANGE_TYPE aType );

private:
    CHANGE_TYPE m_type;           ///< Type of change
    wxString    m_reference;      ///< Component reference (e.g., "R1", "U2")
    wxString    m_description;    ///< Human-readable description
    bool        m_enabled;        ///< Whether this change should be applied

    COMPONENT*  m_component;      ///< Associated netlist component (may be null)
    FOOTPRINT*  m_footprint;      ///< Associated board footprint (may be null)
    PAD*        m_pad;            ///< Associated pad (for pad operations)

    wxString    m_oldValue;       ///< Current/old value (for display)
    wxString    m_newValue;       ///< New value (for display)
    wxString    m_padNumber;      ///< Pad number (for pad operations)
    wxString    m_netName;        ///< Net name (for net operations)
    wxString    m_oldNetName;     ///< Old net name (for reconnect operations)
};

#endif // ECO_ITEM_H
