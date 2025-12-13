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

#include "eco_items_provider.h"


ECO_ITEMS_PROVIDER::ECO_ITEMS_PROVIDER()
{
}


void ECO_ITEMS_PROVIDER::AddItem( std::shared_ptr<ECO_ITEM> aItem )
{
    if( aItem )
    {
        m_items[aItem->GetType()].push_back( aItem );
    }
}


void ECO_ITEMS_PROVIDER::Clear()
{
    m_items.clear();
}


int ECO_ITEMS_PROVIDER::GetCategoryCount() const
{
    return static_cast<int>( m_items.size() );
}


int ECO_ITEMS_PROVIDER::GetItemCount( ECO_ITEM::CHANGE_TYPE aType ) const
{
    auto it = m_items.find( aType );

    if( it != m_items.end() )
        return static_cast<int>( it->second.size() );

    return 0;
}


int ECO_ITEMS_PROVIDER::GetTotalItemCount() const
{
    int count = 0;

    for( const auto& pair : m_items )
        count += static_cast<int>( pair.second.size() );

    return count;
}


std::vector<ECO_ITEM::CHANGE_TYPE> ECO_ITEMS_PROVIDER::GetCategories() const
{
    std::vector<ECO_ITEM::CHANGE_TYPE> categories;

    for( const auto& pair : m_items )
    {
        if( !pair.second.empty() )
            categories.push_back( pair.first );
    }

    return categories;
}


std::vector<std::shared_ptr<ECO_ITEM>> ECO_ITEMS_PROVIDER::GetItems( ECO_ITEM::CHANGE_TYPE aType ) const
{
    auto it = m_items.find( aType );

    if( it != m_items.end() )
        return it->second;

    return std::vector<std::shared_ptr<ECO_ITEM>>();
}


std::shared_ptr<ECO_ITEM> ECO_ITEMS_PROVIDER::GetItem( ECO_ITEM::CHANGE_TYPE aType, int aIndex ) const
{
    auto it = m_items.find( aType );

    if( it != m_items.end() && aIndex >= 0 && aIndex < static_cast<int>( it->second.size() ) )
        return it->second[aIndex];

    return nullptr;
}


void ECO_ITEMS_PROVIDER::SetCategoryEnabled( ECO_ITEM::CHANGE_TYPE aType, bool aEnabled )
{
    auto it = m_items.find( aType );

    if( it != m_items.end() )
    {
        for( auto& item : it->second )
            item->SetEnabled( aEnabled );
    }
}


bool ECO_ITEMS_PROVIDER::IsCategoryFullyEnabled( ECO_ITEM::CHANGE_TYPE aType ) const
{
    auto it = m_items.find( aType );

    if( it == m_items.end() || it->second.empty() )
        return false;

    for( const auto& item : it->second )
    {
        if( !item->IsEnabled() )
            return false;
    }

    return true;
}


bool ECO_ITEMS_PROVIDER::IsCategoryPartiallyEnabled( ECO_ITEM::CHANGE_TYPE aType ) const
{
    auto it = m_items.find( aType );

    if( it == m_items.end() || it->second.empty() )
        return false;

    bool hasEnabled = false;
    bool hasDisabled = false;

    for( const auto& item : it->second )
    {
        if( item->IsEnabled() )
            hasEnabled = true;
        else
            hasDisabled = true;

        if( hasEnabled && hasDisabled )
            return true;
    }

    return false;
}


bool ECO_ITEMS_PROVIDER::IsCategoryFullyDisabled( ECO_ITEM::CHANGE_TYPE aType ) const
{
    auto it = m_items.find( aType );

    if( it == m_items.end() || it->second.empty() )
        return true;

    for( const auto& item : it->second )
    {
        if( item->IsEnabled() )
            return false;
    }

    return true;
}


void ECO_ITEMS_PROVIDER::EnableAll()
{
    for( auto& pair : m_items )
    {
        for( auto& item : pair.second )
            item->SetEnabled( true );
    }
}


void ECO_ITEMS_PROVIDER::DisableAll()
{
    for( auto& pair : m_items )
    {
        for( auto& item : pair.second )
            item->SetEnabled( false );
    }
}


std::vector<std::shared_ptr<ECO_ITEM>> ECO_ITEMS_PROVIDER::GetEnabledItems() const
{
    std::vector<std::shared_ptr<ECO_ITEM>> enabledItems;

    for( const auto& pair : m_items )
    {
        for( const auto& item : pair.second )
        {
            if( item->IsEnabled() )
                enabledItems.push_back( item );
        }
    }

    return enabledItems;
}


int ECO_ITEMS_PROVIDER::GetEnabledItemCount() const
{
    int count = 0;

    for( const auto& pair : m_items )
    {
        for( const auto& item : pair.second )
        {
            if( item->IsEnabled() )
                count++;
        }
    }

    return count;
}
