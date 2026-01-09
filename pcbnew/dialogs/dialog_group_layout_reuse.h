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

#ifndef DIALOG_GROUP_LAYOUT_REUSE_H
#define DIALOG_GROUP_LAYOUT_REUSE_H

#include <vector>
#include <board.h>

#include <dialogs/dialog_group_layout_reuse_base.h>

class PCB_BASE_FRAME;
class MULTICHANNEL_TOOL;
struct RULE_AREA;

/**
 * Dialog for Group-based layout reuse in KiCad 7.0.
 *
 * This dialog allows users to:
 * - View available groups on the board
 * - Generate placement rule areas for groups
 * - Select a reference group and copy its layout to target groups
 * - Configure copy options (placement, routing, other items, etc.)
 */
class DIALOG_GROUP_LAYOUT_REUSE : public DIALOG_GROUP_LAYOUT_REUSE_BASE
{
public:
    DIALOG_GROUP_LAYOUT_REUSE( PCB_BASE_FRAME* aFrame, MULTICHANNEL_TOOL* aParentTool );
    ~DIALOG_GROUP_LAYOUT_REUSE();

    bool TransferDataFromWindow() override;
    bool TransferDataToWindow() override;

protected:
    // Event handlers
    void onClose( wxCloseEvent& event ) override;
    void OnGenerateRuleAreas( wxCommandEvent& event ) override;
    void OnRefGroupChanged( wxCommandEvent& event ) override;

private:
    /**
     * Populate the group grid with available groups from the board.
     */
    void populateGroupGrid();

    /**
     * Populate the reference group choice control.
     */
    void populateRefGroupChoice();

    /**
     * Populate the anchor footprint choice based on selected reference group.
     */
    void populateAnchorFpChoice();

    /**
     * Update the compatibility list based on the selected reference group.
     */
    void updateCompatibilityList();

    /**
     * Entry for the group grid.
     */
    struct GROUP_ENTRY
    {
        wxString    m_groupName;
        int         m_componentCount;
        bool        m_hasRuleArea;
        RULE_AREA*  m_ruleArea;
    };

    std::vector<GROUP_ENTRY>    m_groupEntries;
    MULTICHANNEL_TOOL*          m_parentTool;
    BOARD*                      m_board;
    PCB_BASE_FRAME*             m_frame;
};

#endif // DIALOG_GROUP_LAYOUT_REUSE_H
