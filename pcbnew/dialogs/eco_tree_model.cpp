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

#include "eco_tree_model.h"
#include <wx/wupdlock.h>
#include <wx/settings.h>
#include <eda_draw_frame.h>

#define WX_DATAVIEW_WINDOW_PADDING 6


ECO_TREE_NODE::ECO_TREE_NODE( ECO_TREE_NODE* aParent, NODE_TYPE aType ) :
        m_nodeType( aType ),
        m_category( ECO_ITEM::CHANGE_TYPE::ADD_FOOTPRINT ),
        m_ecoItem( nullptr ),
        m_parent( aParent )
{
}


ECO_TREE_NODE::~ECO_TREE_NODE()
{
    for( ECO_TREE_NODE* child : m_children )
        delete child;
}


ECO_TREE_MODEL::ECO_TREE_MODEL( EDA_DRAW_FRAME* aParentFrame, wxDataViewCtrl* aView ) :
        m_editFrame( aParentFrame ),
        m_view( aView ),
        m_provider( nullptr )
{
    m_view->GetMainWindow()->Connect( wxEVT_SIZE,
                                      wxSizeEventHandler( ECO_TREE_MODEL::onSizeView ),
                                      nullptr, this );
}


ECO_TREE_MODEL::~ECO_TREE_MODEL()
{
    for( ECO_TREE_NODE* node : m_tree )
        delete node;
}


void ECO_TREE_MODEL::rebuildModel( std::shared_ptr<ECO_ITEMS_PROVIDER> aProvider )
{
    wxWindowUpdateLocker updateLock( m_view );

    // Clear selection to avoid issues
    if( m_view )
        m_view->UnselectAll();

    BeforeReset();

    m_provider = aProvider;

    // Delete old tree
    for( ECO_TREE_NODE* node : m_tree )
        delete node;

    m_tree.clear();

    if( !m_provider )
    {
        AfterReset();
        return;
    }

    // Build new tree: one category node per change type
    std::vector<ECO_ITEM::CHANGE_TYPE> categories = m_provider->GetCategories();

    for( ECO_ITEM::CHANGE_TYPE category : categories )
    {
        // Create category node
        ECO_TREE_NODE* categoryNode = new ECO_TREE_NODE( nullptr, ECO_TREE_NODE::CATEGORY );
        categoryNode->m_category = category;
        m_tree.push_back( categoryNode );

        // Add item nodes as children
        std::vector<std::shared_ptr<ECO_ITEM>> items = m_provider->GetItems( category );

        for( const auto& item : items )
        {
            ECO_TREE_NODE* itemNode = new ECO_TREE_NODE( categoryNode, ECO_TREE_NODE::ITEM );
            itemNode->m_ecoItem = item;
            categoryNode->m_children.push_back( itemNode );
        }
    }

    AfterReset();

#ifdef __WXGTK__
    // Linux requires reassociating model after reset
    m_view->AssociateModel( this );
#endif

    // Setup columns
    m_view->ClearColumns();

    // Checkbox column
    wxDataViewToggleRenderer* toggleRenderer = new wxDataViewToggleRenderer(
            wxT( "bool" ), wxDATAVIEW_CELL_ACTIVATABLE );
    wxDataViewColumn* checkCol = new wxDataViewColumn( wxEmptyString, toggleRenderer,
                                                       COL_ENABLED, 30,
                                                       wxALIGN_CENTER,
                                                       wxDATAVIEW_COL_RESIZABLE );
    m_view->AppendColumn( checkCol );

    // Description column
    int width = m_view->GetMainWindow()->GetRect().GetWidth() - 30 - WX_DATAVIEW_WINDOW_PADDING;
    m_view->AppendTextColumn( wxEmptyString, COL_DESCRIPTION, wxDATAVIEW_CELL_INERT, width );

    ExpandAll();
}


void ECO_TREE_MODEL::Update( std::shared_ptr<ECO_ITEMS_PROVIDER> aProvider )
{
    rebuildModel( aProvider );
}


void ECO_TREE_MODEL::EnableAll()
{
    if( m_provider )
    {
        m_provider->EnableAll();

        // Notify view of changes
        for( ECO_TREE_NODE* categoryNode : m_tree )
        {
            ItemChanged( ToItem( categoryNode ) );

            for( ECO_TREE_NODE* itemNode : categoryNode->m_children )
                ItemChanged( ToItem( itemNode ) );
        }
    }
}


void ECO_TREE_MODEL::DisableAll()
{
    if( m_provider )
    {
        m_provider->DisableAll();

        // Notify view of changes
        for( ECO_TREE_NODE* categoryNode : m_tree )
        {
            ItemChanged( ToItem( categoryNode ) );

            for( ECO_TREE_NODE* itemNode : categoryNode->m_children )
                ItemChanged( ToItem( itemNode ) );
        }
    }
}


void ECO_TREE_MODEL::EnableSelected()
{
    if( !m_view )
        return;

    wxDataViewItemArray selections;
    m_view->GetSelections( selections );

    for( const wxDataViewItem& item : selections )
    {
        ECO_TREE_NODE* node = ToNode( item );

        if( !node )
            continue;

        if( node->m_nodeType == ECO_TREE_NODE::CATEGORY )
        {
            // Enable all items in this category
            if( m_provider )
                m_provider->SetCategoryEnabled( node->m_category, true );

            ItemChanged( item );

            for( ECO_TREE_NODE* child : node->m_children )
                ItemChanged( ToItem( child ) );
        }
        else if( node->m_nodeType == ECO_TREE_NODE::ITEM && node->m_ecoItem )
        {
            node->m_ecoItem->SetEnabled( true );
            ItemChanged( item );

            // Update parent category state
            if( node->m_parent )
                ItemChanged( ToItem( node->m_parent ) );
        }
    }
}


void ECO_TREE_MODEL::DisableSelected()
{
    if( !m_view )
        return;

    wxDataViewItemArray selections;
    m_view->GetSelections( selections );

    for( const wxDataViewItem& item : selections )
    {
        ECO_TREE_NODE* node = ToNode( item );

        if( !node )
            continue;

        if( node->m_nodeType == ECO_TREE_NODE::CATEGORY )
        {
            // Disable all items in this category
            if( m_provider )
                m_provider->SetCategoryEnabled( node->m_category, false );

            ItemChanged( item );

            for( ECO_TREE_NODE* child : node->m_children )
                ItemChanged( ToItem( child ) );
        }
        else if( node->m_nodeType == ECO_TREE_NODE::ITEM && node->m_ecoItem )
        {
            node->m_ecoItem->SetEnabled( false );
            ItemChanged( item );

            // Update parent category state
            if( node->m_parent )
                ItemChanged( ToItem( node->m_parent ) );
        }
    }
}


void ECO_TREE_MODEL::ExpandAll()
{
    for( ECO_TREE_NODE* node : m_tree )
        m_view->Expand( ToItem( node ) );
}


void ECO_TREE_MODEL::CollapseAll()
{
    for( ECO_TREE_NODE* node : m_tree )
        m_view->Collapse( ToItem( node ) );
}


wxString ECO_TREE_MODEL::GetColumnType( unsigned int aCol ) const
{
    switch( aCol )
    {
    case COL_ENABLED:     return wxT( "bool" );
    case COL_DESCRIPTION: return wxT( "string" );
    default:              return wxT( "string" );
    }
}


void ECO_TREE_MODEL::GetValue( wxVariant& aVariant, wxDataViewItem const& aItem,
                                unsigned int aCol ) const
{
    ECO_TREE_NODE* node = ToNode( aItem );

    if( !node )
        return;

    switch( aCol )
    {
    case COL_ENABLED:
        if( node->m_nodeType == ECO_TREE_NODE::CATEGORY )
        {
            // For category, show checked if all items enabled, unchecked if all disabled
            // Note: tri-state would be ideal but wxDataViewToggleRenderer doesn't support it well
            if( m_provider )
                aVariant = m_provider->IsCategoryFullyEnabled( node->m_category );
            else
                aVariant = false;
        }
        else if( node->m_nodeType == ECO_TREE_NODE::ITEM && node->m_ecoItem )
        {
            aVariant = node->m_ecoItem->IsEnabled();
        }
        else
        {
            aVariant = false;
        }
        break;

    case COL_DESCRIPTION:
        if( node->m_nodeType == ECO_TREE_NODE::CATEGORY )
        {
            int count = m_provider ? m_provider->GetItemCount( node->m_category ) : 0;
            wxString categoryName = ECO_ITEM::GetChangeTypeName( node->m_category );
            aVariant = wxString::Format( wxT( "%s (%d)" ), categoryName, count );
        }
        else if( node->m_nodeType == ECO_TREE_NODE::ITEM && node->m_ecoItem )
        {
            aVariant = node->m_ecoItem->GetDescription();
        }
        else
        {
            aVariant = wxEmptyString;
        }
        break;

    default:
        aVariant = wxEmptyString;
        break;
    }
}


bool ECO_TREE_MODEL::SetValue( wxVariant const& aVariant, wxDataViewItem const& aItem,
                                unsigned int aCol )
{
    ECO_TREE_NODE* node = ToNode( aItem );

    if( !node || aCol != COL_ENABLED )
        return false;

    bool newValue = aVariant.GetBool();

    if( node->m_nodeType == ECO_TREE_NODE::CATEGORY )
    {
        // Enable/disable all children
        if( m_provider )
            m_provider->SetCategoryEnabled( node->m_category, newValue );

        // Notify view that children changed
        for( ECO_TREE_NODE* child : node->m_children )
            ItemChanged( ToItem( child ) );
    }
    else if( node->m_nodeType == ECO_TREE_NODE::ITEM && node->m_ecoItem )
    {
        node->m_ecoItem->SetEnabled( newValue );

        // Update parent category checkbox state
        if( node->m_parent )
            ItemChanged( ToItem( node->m_parent ) );
    }

    return true;
}


bool ECO_TREE_MODEL::IsContainer( wxDataViewItem const& aItem ) const
{
    ECO_TREE_NODE* node = ToNode( aItem );

    if( node == nullptr )  // Root
        return true;

    return node->m_nodeType == ECO_TREE_NODE::CATEGORY;
}


wxDataViewItem ECO_TREE_MODEL::GetParent( wxDataViewItem const& aItem ) const
{
    ECO_TREE_NODE* node = ToNode( aItem );

    if( node )
        return ToItem( node->m_parent );

    return wxDataViewItem( nullptr );
}


unsigned int ECO_TREE_MODEL::GetChildren( wxDataViewItem const& aItem,
                                           wxDataViewItemArray& aChildren ) const
{
    ECO_TREE_NODE* node = ToNode( aItem );

    // If null, return root level nodes (categories)
    const std::vector<ECO_TREE_NODE*>& children = node ? node->m_children : m_tree;

    for( ECO_TREE_NODE* child : children )
        aChildren.push_back( ToItem( child ) );

    return children.size();
}


bool ECO_TREE_MODEL::GetAttr( wxDataViewItem const& aItem, unsigned int aCol,
                               wxDataViewItemAttr& aAttr ) const
{
    ECO_TREE_NODE* node = ToNode( aItem );

    if( !node )
        return false;

    // Make category headers bold
    if( node->m_nodeType == ECO_TREE_NODE::CATEGORY )
    {
        aAttr.SetBold( true );
        return true;
    }

    // Gray out disabled items
    if( node->m_nodeType == ECO_TREE_NODE::ITEM && node->m_ecoItem && !node->m_ecoItem->IsEnabled() )
    {
        wxColour textColour = wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT );
        aAttr.SetColour( textColour );
        aAttr.SetItalic( true );
        return true;
    }

    return false;
}


void ECO_TREE_MODEL::updateCategoryCheckState( ECO_TREE_NODE* aCategoryNode )
{
    if( aCategoryNode && aCategoryNode->m_nodeType == ECO_TREE_NODE::CATEGORY )
    {
        ItemChanged( ToItem( aCategoryNode ) );
    }
}


void ECO_TREE_MODEL::onSizeView( wxSizeEvent& aEvent )
{
    int width = m_view->GetMainWindow()->GetRect().GetWidth() - WX_DATAVIEW_WINDOW_PADDING;

    if( m_view->GetColumnCount() > 1 )
    {
        int checkWidth = m_view->GetColumn( COL_ENABLED )->GetWidth();
        m_view->GetColumn( COL_DESCRIPTION )->SetWidth( width - checkWidth );
    }

    aEvent.Skip();
}
