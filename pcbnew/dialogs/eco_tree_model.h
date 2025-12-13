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

#ifndef ECO_TREE_MODEL_H
#define ECO_TREE_MODEL_H

#include <wx/dataview.h>
#include <memory>
#include <vector>
#include <netlist_reader/eco_item.h>
#include <netlist_reader/eco_items_provider.h>

class EDA_DRAW_FRAME;

/**
 * Tree node representing either a category (change type) or an individual ECO item.
 */
class ECO_TREE_NODE
{
public:
    enum NODE_TYPE
    {
        CATEGORY,   ///< A category node (grouping by change type)
        ITEM        ///< An individual ECO item node
    };

    ECO_TREE_NODE( ECO_TREE_NODE* aParent, NODE_TYPE aType );
    ~ECO_TREE_NODE();

    NODE_TYPE                       m_nodeType;
    ECO_ITEM::CHANGE_TYPE           m_category;     ///< For CATEGORY nodes
    std::shared_ptr<ECO_ITEM>       m_ecoItem;      ///< For ITEM nodes

    ECO_TREE_NODE*                  m_parent;
    std::vector<ECO_TREE_NODE*>     m_children;
};


/**
 * wxDataViewModel implementation for the ECO tree view.
 * Supports two columns: checkbox and description.
 * Displays ECO items grouped by change type category.
 */
class ECO_TREE_MODEL : public wxDataViewModel, public wxEvtHandler
{
public:
    /// Column definitions
    enum COLUMNS
    {
        COL_ENABLED = 0,    ///< Checkbox column
        COL_DESCRIPTION,    ///< Text description column
        COL_COUNT           ///< Number of columns
    };

    /**
     * Convert a node pointer to a wxDataViewItem.
     */
    static wxDataViewItem ToItem( ECO_TREE_NODE const* aNode )
    {
        return wxDataViewItem( const_cast<void*>( static_cast<void const*>( aNode ) ) );
    }

    /**
     * Convert a wxDataViewItem to a node pointer.
     */
    static ECO_TREE_NODE* ToNode( wxDataViewItem aItem )
    {
        return static_cast<ECO_TREE_NODE*>( aItem.GetID() );
    }

    ECO_TREE_MODEL( EDA_DRAW_FRAME* aParentFrame, wxDataViewCtrl* aView );
    ~ECO_TREE_MODEL();

    /**
     * Update the model with data from a new ECO items provider.
     */
    void Update( std::shared_ptr<ECO_ITEMS_PROVIDER> aProvider );

    /**
     * Enable all items in all categories.
     */
    void EnableAll();

    /**
     * Disable all items in all categories.
     */
    void DisableAll();

    /**
     * Enable all currently selected items.
     */
    void EnableSelected();

    /**
     * Disable all currently selected items.
     */
    void DisableSelected();

    /**
     * Expand all category nodes.
     */
    void ExpandAll();

    /**
     * Collapse all category nodes.
     */
    void CollapseAll();

    /**
     * @return the associated ECO items provider.
     */
    std::shared_ptr<ECO_ITEMS_PROVIDER> GetProvider() const { return m_provider; }

    // wxDataViewModel interface implementation
    unsigned int GetColumnCount() const override { return COL_COUNT; }

    wxString GetColumnType( unsigned int aCol ) const override;

    void GetValue( wxVariant& aVariant, wxDataViewItem const& aItem,
                   unsigned int aCol ) const override;

    bool SetValue( wxVariant const& aVariant, wxDataViewItem const& aItem,
                   unsigned int aCol ) override;

    bool IsContainer( wxDataViewItem const& aItem ) const override;

    wxDataViewItem GetParent( wxDataViewItem const& aItem ) const override;

    unsigned int GetChildren( wxDataViewItem const& aItem,
                              wxDataViewItemArray& aChildren ) const override;

    bool HasContainerColumns( wxDataViewItem const& aItem ) const override { return true; }

    bool GetAttr( wxDataViewItem const& aItem, unsigned int aCol,
                  wxDataViewItemAttr& aAttr ) const override;

private:
    void rebuildModel( std::shared_ptr<ECO_ITEMS_PROVIDER> aProvider );
    void updateCategoryCheckState( ECO_TREE_NODE* aCategoryNode );
    void onSizeView( wxSizeEvent& aEvent );

    EDA_DRAW_FRAME*                     m_editFrame;
    wxDataViewCtrl*                     m_view;
    std::shared_ptr<ECO_ITEMS_PROVIDER> m_provider;

    std::vector<ECO_TREE_NODE*>         m_tree;  ///< Root category nodes
};

#endif // ECO_TREE_MODEL_H
