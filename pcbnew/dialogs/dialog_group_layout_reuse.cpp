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

#include <dialogs/dialog_group_layout_reuse.h>
#include <widgets/wx_grid.h>
#include <grid_tricks.h>
#include <pcb_edit_frame.h>
#include <pcb_group.h>
#include <footprint.h>
#include <zone.h>
#include <board.h>

#include <tools/multichannel_tool.h>


DIALOG_GROUP_LAYOUT_REUSE::DIALOG_GROUP_LAYOUT_REUSE( PCB_BASE_FRAME* aFrame,
                                                      MULTICHANNEL_TOOL* aParentTool ) :
        DIALOG_GROUP_LAYOUT_REUSE_BASE( aFrame ),
        m_parentTool( aParentTool ),
        m_frame( aFrame )
{
    m_board = aFrame->GetBoard();

    // Setup the group grid
    m_groupGrid->PushEventHandler( new GRID_TRICKS( static_cast<WX_GRID*>( m_groupGrid ) ) );
    m_groupGrid->EnableEditing( true );
    m_groupGrid->HideRowLabels();

    // Set column labels
    m_groupGrid->SetColLabelValue( 0, _( "Generate RA" ) );
    m_groupGrid->SetColLabelValue( 1, _( "Group Name" ) );
    m_groupGrid->SetColLabelValue( 2, _( "Components" ) );
    m_groupGrid->SetColLabelValue( 3, _( "Has Rule Area" ) );

    // Populate the grid
    populateGroupGrid();

    // Populate the reference group choice
    populateRefGroupChoice();

    // Set default option values
    RULE_AREAS_DATA* data = m_parentTool->GetData();
    m_cbCopyPlacement->SetValue( data->m_options.m_copyPlacement );
    m_cbCopyRouting->SetValue( data->m_options.m_copyRouting );
    m_cbConnectedOnly->SetValue( data->m_options.m_connectedRoutingOnly );
    m_cbCopyOtherItems->SetValue( data->m_options.m_copyOtherItems );
    m_cbIncludeLocked->SetValue( data->m_options.m_includeLockedItems );
    m_cbGroupItems->SetValue( data->m_options.m_groupItems );

    Layout();
    SetupStandardButtons();
    finishDialogSettings();
}


DIALOG_GROUP_LAYOUT_REUSE::~DIALOG_GROUP_LAYOUT_REUSE()
{
    m_groupGrid->PopEventHandler( true );
}


void DIALOG_GROUP_LAYOUT_REUSE::populateGroupGrid()
{
    m_groupEntries.clear();
    m_groupGrid->ClearGrid();

    if( m_groupGrid->GetNumberRows() > 0 )
        m_groupGrid->DeleteRows( 0, m_groupGrid->GetNumberRows() );

    // Collect all named groups with footprints
    for( PCB_GROUP* group : m_board->Groups() )
    {
        if( group->GetName().IsEmpty() )
            continue;

        GROUP_ENTRY entry;
        entry.m_groupName = group->GetName();
        entry.m_componentCount = 0;
        entry.m_hasRuleArea = false;
        entry.m_ruleArea = nullptr;

        // Count footprints in this group
        for( BOARD_ITEM* item : group->GetItems() )
        {
            if( item->Type() == PCB_FOOTPRINT_T )
                entry.m_componentCount++;
        }

        // Only include groups that have footprints
        if( entry.m_componentCount > 0 )
            m_groupEntries.push_back( entry );
    }

    // Check for existing rule areas
    RULE_AREAS_DATA* data = m_parentTool->GetData();
    for( GROUP_ENTRY& entry : m_groupEntries )
    {
        for( RULE_AREA& ra : data->m_areas )
        {
            if( ra.m_sourceType == PLACEMENT_SOURCE_T::GROUP_PLACEMENT &&
                ra.m_groupName == entry.m_groupName )
            {
                entry.m_hasRuleArea = ra.m_existsAlready;
                entry.m_ruleArea = &ra;
                break;
            }
        }
    }

    // Populate the grid
    if( m_groupEntries.size() > 0 )
        m_groupGrid->AppendRows( m_groupEntries.size() );

    for( size_t i = 0; i < m_groupEntries.size(); i++ )
    {
        GROUP_ENTRY& entry = m_groupEntries[i];

        // Generate checkbox
        m_groupGrid->SetCellRenderer( i, 0, new wxGridCellBoolRenderer );
        m_groupGrid->SetCellEditor( i, 0, new wxGridCellBoolEditor );
        m_groupGrid->SetCellValue( i, 0, wxT( "" ) );

        // Group name
        m_groupGrid->SetCellValue( i, 1, entry.m_groupName );
        m_groupGrid->SetReadOnly( i, 1 );

        // Component count
        m_groupGrid->SetCellValue( i, 2, wxString::Format( wxT( "%d" ), entry.m_componentCount ) );
        m_groupGrid->SetReadOnly( i, 2 );

        // Has rule area
        m_groupGrid->SetCellValue( i, 3, entry.m_hasRuleArea ? _( "Yes" ) : _( "No" ) );
        m_groupGrid->SetReadOnly( i, 3 );
    }

    m_groupGrid->AutoSizeColumns();
    m_groupGrid->Fit();
}


void DIALOG_GROUP_LAYOUT_REUSE::populateRefGroupChoice()
{
    m_choiceRefGroup->Clear();
    m_choiceRefGroup->Append( wxT( "" ) );

    // Add groups that have rule areas
    for( const GROUP_ENTRY& entry : m_groupEntries )
    {
        if( entry.m_hasRuleArea )
            m_choiceRefGroup->Append( entry.m_groupName );
    }

    m_choiceRefGroup->SetSelection( 0 );
    populateAnchorFpChoice();
    updateCompatibilityList();
}


void DIALOG_GROUP_LAYOUT_REUSE::populateAnchorFpChoice()
{
    m_choiceAnchorFp->Clear();
    m_choiceAnchorFp->Append( wxT( "" ) );

    wxString refGroupName = m_choiceRefGroup->GetStringSelection();
    if( refGroupName.IsEmpty() )
    {
        m_choiceAnchorFp->SetSelection( 0 );
        return;
    }

    // Find the reference group and get its footprints
    for( PCB_GROUP* group : m_board->Groups() )
    {
        if( group->GetName() == refGroupName )
        {
            wxArrayString fpRefs;

            for( BOARD_ITEM* item : group->GetItems() )
            {
                if( item->Type() == PCB_FOOTPRINT_T )
                {
                    FOOTPRINT* fp = static_cast<FOOTPRINT*>( item );
                    fpRefs.Add( fp->GetReference() );
                }
            }

            fpRefs.Sort();

            for( const wxString& ref : fpRefs )
                m_choiceAnchorFp->Append( ref );

            break;
        }
    }

    m_choiceAnchorFp->SetSelection( 0 );
}


void DIALOG_GROUP_LAYOUT_REUSE::updateCompatibilityList()
{
    m_compatibilityList->Clear();

    wxString refGroupName = m_choiceRefGroup->GetStringSelection();
    if( refGroupName.IsEmpty() )
    {
        m_compatibilityList->Append( _( "Select a reference group to see compatibility." ) );
        return;
    }

    RULE_AREAS_DATA* data = m_parentTool->GetData();

    // Find reference area
    RULE_AREA* refArea = nullptr;
    for( RULE_AREA& ra : data->m_areas )
    {
        if( ra.m_sourceType == PLACEMENT_SOURCE_T::GROUP_PLACEMENT &&
            ra.m_groupName == refGroupName )
        {
            refArea = &ra;
            break;
        }
    }

    if( !refArea )
    {
        m_compatibilityList->Append( _( "Reference group has no rule area." ) );
        return;
    }

    // Show compatibility with other groups
    for( const auto& [targetArea, compatData] : data->m_compatMap )
    {
        if( targetArea->m_sourceType != PLACEMENT_SOURCE_T::GROUP_PLACEMENT )
            continue;

        wxString status;
        if( compatData.m_isOk )
            status = wxString::Format( _( "%s: OK" ), targetArea->m_groupName );
        else
            status = wxString::Format( _( "%s: %s" ), targetArea->m_groupName, compatData.m_errorMsg );

        m_compatibilityList->Append( status );
    }

    if( m_compatibilityList->GetCount() == 0 )
        m_compatibilityList->Append( _( "No target groups found." ) );
}


void DIALOG_GROUP_LAYOUT_REUSE::onClose( wxCloseEvent& event )
{
    event.Skip();
}


void DIALOG_GROUP_LAYOUT_REUSE::OnGenerateRuleAreas( wxCommandEvent& event )
{
    // Collect which groups should have rule areas generated
    RULE_AREAS_DATA* data = m_parentTool->GetData();

    int selectedCount = 0;
    for( size_t i = 0; i < m_groupEntries.size(); i++ )
    {
        wxString enabled = m_groupGrid->GetCellValue( i, 0 );
        bool shouldGenerate = ( enabled == wxT( "1" ) );

        // Find the corresponding rule area
        for( RULE_AREA& ra : data->m_areas )
        {
            if( ra.m_sourceType == PLACEMENT_SOURCE_T::GROUP_PLACEMENT &&
                ra.m_groupName == m_groupEntries[i].m_groupName )
            {
                ra.m_generateEnabled = shouldGenerate;
                if( shouldGenerate )
                    selectedCount++;
                break;
            }
        }
    }

    if( selectedCount == 0 )
    {
        wxMessageBox( _( "Please select at least one group to generate rule areas for." ),
                      _( "Generate Rule Areas" ), wxOK | wxICON_WARNING, this );
        return;
    }

    // Call the tool to generate rule areas
    // This will create the zones with the m_generateEnabled flags we just set
    m_parentTool->ExecuteRuleAreaGeneration();

    // Regenerate potential rule areas to update the state
    m_parentTool->GeneratePotentialRuleAreas();
    m_parentTool->FindExistingRuleAreas();

    // Refresh the dialog to show updated state
    populateGroupGrid();
    populateRefGroupChoice();

    wxMessageBox( wxString::Format( _( "Generated rule areas for %d groups." ), selectedCount ),
                  _( "Generate Rule Areas" ), wxOK | wxICON_INFORMATION, this );
}


void DIALOG_GROUP_LAYOUT_REUSE::OnRefGroupChanged( wxCommandEvent& event )
{
    populateAnchorFpChoice();

    // Update compatibility check
    wxString refGroupName = m_choiceRefGroup->GetStringSelection();
    if( !refGroupName.IsEmpty() )
    {
        // Find the reference zone and run compatibility check
        RULE_AREAS_DATA* data = m_parentTool->GetData();

        for( RULE_AREA& ra : data->m_areas )
        {
            if( ra.m_sourceType == PLACEMENT_SOURCE_T::GROUP_PLACEMENT &&
                ra.m_groupName == refGroupName &&
                ra.m_zone )
            {
                m_parentTool->CheckRACompatibility( ra.m_zone );
                break;
            }
        }
    }

    updateCompatibilityList();
}


bool DIALOG_GROUP_LAYOUT_REUSE::TransferDataFromWindow()
{
    RULE_AREAS_DATA* data = m_parentTool->GetData();

    // Update options
    data->m_options.m_copyPlacement = m_cbCopyPlacement->GetValue();
    data->m_options.m_copyRouting = m_cbCopyRouting->GetValue();
    data->m_options.m_connectedRoutingOnly = m_cbConnectedOnly->GetValue();
    data->m_options.m_copyOtherItems = m_cbCopyOtherItems->GetValue();
    data->m_options.m_includeLockedItems = m_cbIncludeLocked->GetValue();
    data->m_options.m_groupItems = m_cbGroupItems->GetValue();

    // Set the reference rule area based on selected reference group
    wxString refGroupName = m_choiceRefGroup->GetStringSelection();
    data->m_refRA = nullptr;

    if( !refGroupName.IsEmpty() )
    {
        for( RULE_AREA& ra : data->m_areas )
        {
            if( ra.m_sourceType == PLACEMENT_SOURCE_T::GROUP_PLACEMENT &&
                ra.m_groupName == refGroupName )
            {
                data->m_refRA = &ra;
                break;
            }
        }
    }

    // Validate that a reference group is selected for layout copy
    if( !data->m_refRA )
    {
        // Check if any copy option is enabled
        if( data->m_options.m_copyPlacement || data->m_options.m_copyRouting ||
            data->m_options.m_copyOtherItems )
        {
            wxMessageBox( _( "Please select a reference group with a rule area to copy layout from." ),
                          _( "Group Layout Reuse" ), wxOK | wxICON_WARNING, this );
            return false;
        }
    }

    // Set anchor footprint
    wxString anchorFpRef = m_choiceAnchorFp->GetStringSelection();
    if( anchorFpRef.IsEmpty() )
    {
        data->m_options.m_anchorFp = nullptr;
    }
    else
    {
        for( FOOTPRINT* fp : m_board->Footprints() )
        {
            if( fp->GetReference() == anchorFpRef )
            {
                data->m_options.m_anchorFp = fp;
                break;
            }
        }
    }

    // Update which groups should have rule areas generated
    for( size_t i = 0; i < m_groupEntries.size(); i++ )
    {
        wxString enabled = m_groupGrid->GetCellValue( i, 0 );
        bool shouldGenerate = ( enabled == wxT( "1" ) );

        for( RULE_AREA& ra : data->m_areas )
        {
            if( ra.m_sourceType == PLACEMENT_SOURCE_T::GROUP_PLACEMENT &&
                ra.m_groupName == m_groupEntries[i].m_groupName )
            {
                ra.m_generateEnabled = shouldGenerate;
                break;
            }
        }
    }

    // Update which targets should be copied to
    for( auto& [targetArea, compatData] : data->m_compatMap )
    {
        // Enable copy for all compatible targets
        compatData.m_doCopy = compatData.m_isOk;
    }

    return true;
}


bool DIALOG_GROUP_LAYOUT_REUSE::TransferDataToWindow()
{
    return true;
}
