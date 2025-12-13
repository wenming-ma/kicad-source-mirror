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

#ifndef ECO_ITEMS_PROVIDER_H
#define ECO_ITEMS_PROVIDER_H

#include <vector>
#include <map>
#include <memory>
#include "eco_item.h"

/**
 * Manages a collection of ECO items organized by change type.
 * Provides hierarchical access for tree view display and supports
 * bulk enable/disable operations.
 */
class ECO_ITEMS_PROVIDER
{
public:
    ECO_ITEMS_PROVIDER();
    ~ECO_ITEMS_PROVIDER() = default;

    /**
     * Add an ECO item to the collection.
     * Items are automatically grouped by their change type.
     */
    void AddItem( std::shared_ptr<ECO_ITEM> aItem );

    /**
     * Remove all items from the collection.
     */
    void Clear();

    /**
     * @return the number of different change type categories that have items.
     */
    int GetCategoryCount() const;

    /**
     * @return the number of items in a specific category.
     */
    int GetItemCount( ECO_ITEM::CHANGE_TYPE aType ) const;

    /**
     * @return the total number of ECO items across all categories.
     */
    int GetTotalItemCount() const;

    /**
     * @return a list of all change types that have at least one item.
     */
    std::vector<ECO_ITEM::CHANGE_TYPE> GetCategories() const;

    /**
     * @return all items of a specific change type.
     */
    std::vector<std::shared_ptr<ECO_ITEM>> GetItems( ECO_ITEM::CHANGE_TYPE aType ) const;

    /**
     * Get a specific item from a category.
     * @param aType The change type category
     * @param aIndex Index within the category
     * @return The ECO item, or nullptr if index is out of range
     */
    std::shared_ptr<ECO_ITEM> GetItem( ECO_ITEM::CHANGE_TYPE aType, int aIndex ) const;

    /**
     * Enable or disable all items in a category.
     */
    void SetCategoryEnabled( ECO_ITEM::CHANGE_TYPE aType, bool aEnabled );

    /**
     * Check if all items in a category are enabled.
     */
    bool IsCategoryFullyEnabled( ECO_ITEM::CHANGE_TYPE aType ) const;

    /**
     * Check if at least one (but not all) items in a category are enabled.
     * Used for tri-state checkbox display.
     */
    bool IsCategoryPartiallyEnabled( ECO_ITEM::CHANGE_TYPE aType ) const;

    /**
     * Check if no items in a category are enabled.
     */
    bool IsCategoryFullyDisabled( ECO_ITEM::CHANGE_TYPE aType ) const;

    /**
     * Enable all items in all categories.
     */
    void EnableAll();

    /**
     * Disable all items in all categories.
     */
    void DisableAll();

    /**
     * @return all items that are currently enabled.
     */
    std::vector<std::shared_ptr<ECO_ITEM>> GetEnabledItems() const;

    /**
     * @return the count of enabled items.
     */
    int GetEnabledItemCount() const;

private:
    /// Items organized by change type
    std::map<ECO_ITEM::CHANGE_TYPE, std::vector<std::shared_ptr<ECO_ITEM>>> m_items;
};

#endif // ECO_ITEMS_PROVIDER_H
