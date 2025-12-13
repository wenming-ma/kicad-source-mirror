///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "widgets/wx_html_report_panel.h"

#include "dialog_update_pcb_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_UPDATE_PCB_BASE::DIALOG_UPDATE_PCB_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bUpperSizer;
	bUpperSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Options") ), wxVERTICAL );

	m_cbRelinkFootprints = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Re-link footprints to schematic symbols based on their reference designators"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbRelinkFootprints->SetToolTip( _("Normally footprints are linked to their symbols via their Unique IDs.  Select this option only if you want to reset the footprint linkages based on their reference designators.") );

	sbSizer1->Add( m_cbRelinkFootprints, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_cbDeleteExtraFootprints = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Delete footprints with no symbols"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbDeleteExtraFootprints->SetToolTip( _("Remove from the board unlocked footprints which are not linked to a schematic symbol.") );

	sbSizer1->Add( m_cbDeleteExtraFootprints, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );

	m_cbUpdateFootprints = new wxCheckBox( sbSizer1->GetStaticBox(), wxID_ANY, _("Replace footprints with those specified in the schematic"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cbUpdateFootprints->SetValue(true);
	m_cbUpdateFootprints->SetToolTip( _("Normally footprints on the board should be changed to match footprint assignment changes made in the schematic. Uncheck this only if you don't want to change existing footprints on the board.") );

	sbSizer1->Add( m_cbUpdateFootprints, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );


	bUpperSizer->Add( sbSizer1, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );


	bMainSizer->Add( bUpperSizer, 0, wxALL|wxEXPAND, 5 );

	// ECO tree view toolbar
	wxBoxSizer* bToolbarSizer;
	bToolbarSizer = new wxBoxSizer( wxHORIZONTAL );

	m_btnEnableAll = new wxButton( this, wxID_ANY, _("Enable All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnEnableAll->SetToolTip( _("Enable all changes") );
	bToolbarSizer->Add( m_btnEnableAll, 0, wxALL, 5 );

	m_btnDisableAll = new wxButton( this, wxID_ANY, _("Disable All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnDisableAll->SetToolTip( _("Disable all changes") );
	bToolbarSizer->Add( m_btnDisableAll, 0, wxALL, 5 );

	bToolbarSizer->Add( 20, 0, 0, wxEXPAND, 5 );

	m_btnExpandAll = new wxButton( this, wxID_ANY, _("Expand All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnExpandAll->SetToolTip( _("Expand all categories") );
	bToolbarSizer->Add( m_btnExpandAll, 0, wxALL, 5 );

	m_btnCollapseAll = new wxButton( this, wxID_ANY, _("Collapse All"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnCollapseAll->SetToolTip( _("Collapse all categories") );
	bToolbarSizer->Add( m_btnCollapseAll, 0, wxALL, 5 );

	bMainSizer->Add( bToolbarSizer, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );

	// Splitter window for changes tree and messages panel
	wxBoxSizer* bLowerSizer;
	bLowerSizer = new wxBoxSizer( wxVERTICAL );
	bLowerSizer->SetMinSize( wxSize( 660,400 ) );

	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxSP_LIVE_UPDATE );
	m_splitter->SetSashGravity( 0.5 );
	m_splitter->SetMinimumPaneSize( 100 );

	// Top panel: Changes tree view
	m_panelChanges = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bChangesSizer;
	bChangesSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbChanges;
	sbChanges = new wxStaticBoxSizer( new wxStaticBox( m_panelChanges, wxID_ANY, _("Changes To Apply") ), wxVERTICAL );

	m_changesView = new wxDataViewCtrl( sbChanges->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE );
	sbChanges->Add( m_changesView, 1, wxEXPAND | wxALL, 5 );

	bChangesSizer->Add( sbChanges, 1, wxEXPAND | wxALL, 0 );

	m_panelChanges->SetSizer( bChangesSizer );
	m_panelChanges->Layout();
	bChangesSizer->Fit( m_panelChanges );

	// Bottom panel: Messages
	m_panelMessages = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bMessagesSizer;
	bMessagesSizer = new wxBoxSizer( wxVERTICAL );

	m_messagePanel = new WX_HTML_REPORT_PANEL( m_panelMessages, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
	bMessagesSizer->Add( m_messagePanel, 1, wxEXPAND | wxALL, 0 );

	m_panelMessages->SetSizer( bMessagesSizer );
	m_panelMessages->Layout();
	bMessagesSizer->Fit( m_panelMessages );

	m_splitter->SplitHorizontally( m_panelChanges, m_panelMessages, 200 );
	bLowerSizer->Add( m_splitter, 1, wxEXPAND | wxALL, 5 );


	bMainSizer->Add( bLowerSizer, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT, 5 );

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();

	bMainSizer->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );


	this->SetSizer( bMainSizer );
	this->Layout();
	bMainSizer->Fit( this );

	// Connect Events
	m_cbRelinkFootprints->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnOptionChanged ), NULL, this );
	m_cbDeleteExtraFootprints->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnOptionChanged ), NULL, this );
	m_cbUpdateFootprints->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnOptionChanged ), NULL, this );
	m_btnEnableAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnEnableAllClick ), NULL, this );
	m_btnDisableAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnDisableAllClick ), NULL, this );
	m_btnExpandAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnExpandAllClick ), NULL, this );
	m_btnCollapseAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnCollapseAllClick ), NULL, this );
	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnUpdateClick ), NULL, this );
}

DIALOG_UPDATE_PCB_BASE::~DIALOG_UPDATE_PCB_BASE()
{
	// Disconnect Events
	m_cbRelinkFootprints->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnOptionChanged ), NULL, this );
	m_cbDeleteExtraFootprints->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnOptionChanged ), NULL, this );
	m_cbUpdateFootprints->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnOptionChanged ), NULL, this );
	m_btnEnableAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnEnableAllClick ), NULL, this );
	m_btnDisableAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnDisableAllClick ), NULL, this );
	m_btnExpandAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnExpandAllClick ), NULL, this );
	m_btnCollapseAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnCollapseAllClick ), NULL, this );
	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_UPDATE_PCB_BASE::OnUpdateClick ), NULL, this );

}
