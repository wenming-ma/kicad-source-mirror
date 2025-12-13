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

#include "eco_item.h"
#include <wx/intl.h>


ECO_ITEM::ECO_ITEM( CHANGE_TYPE aType, const wxString& aReference ) :
        m_type( aType ),
        m_reference( aReference ),
        m_enabled( true ),
        m_component( nullptr ),
        m_footprint( nullptr ),
        m_pad( nullptr )
{
}


wxString ECO_ITEM::GetChangeTypeName( CHANGE_TYPE aType )
{
    switch( aType )
    {
    case CHANGE_TYPE::ADD_FOOTPRINT:
        return _( "Add Footprints" );
    case CHANGE_TYPE::DELETE_FOOTPRINT:
        return _( "Delete Footprints" );
    case CHANGE_TYPE::REPLACE_FOOTPRINT:
        return _( "Replace Footprints" );
    case CHANGE_TYPE::UPDATE_REFERENCE:
        return _( "Update References" );
    case CHANGE_TYPE::UPDATE_VALUE:
        return _( "Update Values" );
    case CHANGE_TYPE::UPDATE_PATH:
        return _( "Update Symbol Links" );
    case CHANGE_TYPE::UPDATE_PROPERTIES:
        return _( "Update Properties" );
    case CHANGE_TYPE::UPDATE_ATTRIBUTES:
        return _( "Update Attributes" );
    case CHANGE_TYPE::CONNECT_PAD:
        return _( "Connect Pads" );
    case CHANGE_TYPE::DISCONNECT_PAD:
        return _( "Disconnect Pads" );
    case CHANGE_TYPE::RECONNECT_PAD:
        return _( "Reconnect Pads" );
    case CHANGE_TYPE::ADD_NET:
        return _( "Add Nets" );
    case CHANGE_TYPE::RECONNECT_ZONE:
        return _( "Reconnect Zones" );
    default:
        return _( "Unknown" );
    }
}
