///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
class WX_GRID;

#include "dialog_shim.h"
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/grid.h>
#include <wx/gdicmn.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_GROUP_LAYOUT_REUSE_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_GROUP_LAYOUT_REUSE_BASE : public DIALOG_SHIM
{
	private:

	protected:
		WX_GRID* m_groupGrid;
		wxButton* m_btnGenerateRuleAreas;
		wxStaticText* m_labelRefGroup;
		wxChoice* m_choiceRefGroup;
		wxStaticText* m_labelAnchorFp;
		wxChoice* m_choiceAnchorFp;
		wxCheckBox* m_cbCopyPlacement;
		wxCheckBox* m_cbCopyRouting;
		wxCheckBox* m_cbConnectedOnly;
		wxCheckBox* m_cbCopyOtherItems;
		wxCheckBox* m_cbIncludeLocked;
		wxCheckBox* m_cbGroupItems;
		wxListBox* m_compatibilityList	;
		wxStdDialogButtonSizer* m_sdbSizer;
		wxButton* m_sdbSizerOK;
		wxButton* m_sdbSizerCancel;

		// Virtual event handlers, override them in your derived class
		virtual void onClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnGenerateRuleAreas( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRefGroupChanged( wxCommandEvent& event ) { event.Skip(); }


	public:

		DIALOG_GROUP_LAYOUT_REUSE_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Group Layout Reuse"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );

		~DIALOG_GROUP_LAYOUT_REUSE_BASE();

};

