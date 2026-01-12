///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "widgets/wx_grid.h"

#include "dialog_group_layout_reuse_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_GROUP_LAYOUT_REUSE_BASE::DIALOG_GROUP_LAYOUT_REUSE_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 550,500 ), wxDefaultSize );

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizerGroups;
	sbSizerGroups = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Source Groups") ), wxVERTICAL );

	m_groupGrid = new WX_GRID( sbSizerGroups->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_groupGrid->CreateGrid( 0, 5 );
	m_groupGrid->EnableEditing( true );
	m_groupGrid->EnableGridLines( true );
	m_groupGrid->EnableDragGridSize( false );
	m_groupGrid->SetMargins( 0, 0 );

	// Columns
	m_groupGrid->SetColSize( 0, 50 );
	m_groupGrid->SetColSize( 1, 150 );
	m_groupGrid->SetColSize( 2, 80 );
	m_groupGrid->SetColSize( 3, 100 );
	m_groupGrid->SetColSize( 4, 80 );
	m_groupGrid->EnableDragColMove( false );
	m_groupGrid->EnableDragColSize( true );
	m_groupGrid->SetColLabelValue( 0, _("Select") );
	m_groupGrid->SetColLabelValue( 1, _("Group Name") );
	m_groupGrid->SetColLabelValue( 2, _("Footprints") );
	m_groupGrid->SetColLabelValue( 3, _("Has Rule Area") );
	m_groupGrid->SetColLabelValue( 4, _("Status") );
	m_groupGrid->SetColLabelSize( 22 );
	m_groupGrid->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_groupGrid->EnableDragRowSize( false );
	m_groupGrid->SetRowLabelSize( 0 );
	m_groupGrid->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_groupGrid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_CENTER );
	m_groupGrid->SetMinSize( wxSize( -1,150 ) );

	sbSizerGroups->Add( m_groupGrid, 1, wxALL|wxEXPAND, 5 );

	m_btnGenerateRuleAreas = new wxButton( sbSizerGroups->GetStaticBox(), wxID_ANY, _("Generate Rule Areas for Selected Groups"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerGroups->Add( m_btnGenerateRuleAreas, 0, wxALL, 5 );


	bSizerMain->Add( sbSizerGroups, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 10 );

	wxStaticBoxSizer* sbSizerReference;
	sbSizerReference = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Reference") ), wxVERTICAL );

	wxFlexGridSizer* fgSizerRef;
	fgSizerRef = new wxFlexGridSizer( 0, 2, 5, 10 );
	fgSizerRef->AddGrowableCol( 1 );
	fgSizerRef->SetFlexibleDirection( wxBOTH );
	fgSizerRef->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_labelRefGroup = new wxStaticText( sbSizerReference->GetStaticBox(), wxID_ANY, _("Reference Group:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelRefGroup->Wrap( -1 );
	fgSizerRef->Add( m_labelRefGroup, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

	wxArrayString m_choiceRefGroupChoices;
	m_choiceRefGroup = new wxChoice( sbSizerReference->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceRefGroupChoices, 0 );
	m_choiceRefGroup->SetSelection( 0 );
	fgSizerRef->Add( m_choiceRefGroup, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT, 5 );

	m_labelAnchorFp = new wxStaticText( sbSizerReference->GetStaticBox(), wxID_ANY, _("Anchor Footprint (Optional):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_labelAnchorFp->Wrap( -1 );
	fgSizerRef->Add( m_labelAnchorFp, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

	wxArrayString m_choiceAnchorFpChoices;
	m_choiceAnchorFp = new wxChoice( sbSizerReference->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceAnchorFpChoices, 0 );
	m_choiceAnchorFp->SetSelection( 0 );
	fgSizerRef->Add( m_choiceAnchorFp, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT, 5 );


	sbSizerReference->Add( fgSizerRef, 0, wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizerReference, 0, wxEXPAND|wxLEFT|wxRIGHT, 10 );

	wxStaticBoxSizer* sbSizerCopyOptions;
	sbSizerCopyOptions = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Copy Options") ), wxVERTICAL );

	m_cbCopyPlacement = new wxCheckBox( sbSizerCopyOptions->GetStaticBox(), wxID_ANY, _("Copy footprint placement"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbCopyPlacement->SetValue(true);
	sbSizerCopyOptions->Add( m_cbCopyPlacement, 0, wxLEFT|wxRIGHT|wxTOP, 5 );

	m_cbCopyRouting = new wxCheckBox( sbSizerCopyOptions->GetStaticBox(), wxID_ANY, _("Copy routing"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbCopyRouting->SetValue(true);
	sbSizerCopyOptions->Add( m_cbCopyRouting, 0, wxLEFT|wxRIGHT|wxTOP, 5 );

	wxBoxSizer* bSizerIndent;
	bSizerIndent = new wxBoxSizer( wxHORIZONTAL );


	bSizerIndent->Add( 20, 0, 1, wxEXPAND, 5 );

	m_cbConnectedOnly = new wxCheckBox( sbSizerCopyOptions->GetStaticBox(), wxID_ANY, _("Only connected routing within area"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbConnectedOnly->SetValue(true);
	bSizerIndent->Add( m_cbConnectedOnly, 0, wxALL, 5 );


	sbSizerCopyOptions->Add( bSizerIndent, 1, wxLEFT|wxRIGHT, 5 );

	m_cbCopyOtherItems = new wxCheckBox( sbSizerCopyOptions->GetStaticBox(), wxID_ANY, _("Copy other items (text, shapes, zones)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbCopyOtherItems->SetValue(true);
	sbSizerCopyOptions->Add( m_cbCopyOtherItems, 0, wxLEFT|wxRIGHT|wxTOP, 5 );

	m_cbIncludeLocked = new wxCheckBox( sbSizerCopyOptions->GetStaticBox(), wxID_ANY, _("Include locked items"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerCopyOptions->Add( m_cbIncludeLocked, 0, wxLEFT|wxRIGHT|wxTOP, 5 );

	m_cbGroupItems = new wxCheckBox( sbSizerCopyOptions->GetStaticBox(), wxID_ANY, _("Group copied items with target rule areas"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerCopyOptions->Add( m_cbGroupItems, 0, wxLEFT|wxRIGHT|wxTOP, 5 );


	bSizerMain->Add( sbSizerCopyOptions, 0, wxEXPAND, 10 );

	wxStaticBoxSizer* sbSizerCompatibility;
	sbSizerCompatibility = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Compatibility Check") ), wxVERTICAL );

	m_compatibilityList	 = new wxListBox( sbSizerCompatibility->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_SINGLE );
	m_compatibilityList	->SetMinSize( wxSize( -1,80 ) );

	sbSizerCompatibility->Add( m_compatibilityList	, 1, wxALL|wxEXPAND, 5 );


	bSizerMain->Add( sbSizerCompatibility, 0, wxEXPAND, 10 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();

	bSizerMain->Add( m_sdbSizer, 0, wxBOTTOM|wxEXPAND|wxTOP, 5 );


	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DIALOG_GROUP_LAYOUT_REUSE_BASE::onClose ) );
	m_btnGenerateRuleAreas->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_GROUP_LAYOUT_REUSE_BASE::OnGenerateRuleAreas ), NULL, this );
	m_choiceRefGroup->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DIALOG_GROUP_LAYOUT_REUSE_BASE::OnRefGroupChanged ), NULL, this );
}

DIALOG_GROUP_LAYOUT_REUSE_BASE::~DIALOG_GROUP_LAYOUT_REUSE_BASE()
{
}
