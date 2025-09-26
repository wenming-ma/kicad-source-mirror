// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#pragma once

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDialog>
#include <QtCore/QString>
#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////

#define ID_LAYERS_MAP_DIALOG_BASE 1000
#define ID_M_STATICLINESEP 1001
#define ID_M_STATICTEXTCOPPERLAYERCOUNT 1002
#define ID_M_COMBOCOPPERLAYERSCOUNT 1003
#define ID_STORE_CHOICE 1004
#define ID_GET_PREVIOUS_CHOICE 1005
#define ID_RESET_CHOICE 1006

///////////////////////////////////////////////////////////////////////////////
/// Class LAYERS_MAP_DIALOG_BASE
///////////////////////////////////////////////////////////////////////////////
class LAYERS_MAP_DIALOG_BASE : public DIALOG_SHIM
{
	Q_OBJECT
	private:

		// Private event handlers
		void _qt_OnBrdLayersCountSelection();
		void _qt_OnStoreSetup();
		void _qt_OnGetSetup();
		void _qt_OnResetClick();


	protected:
		QLabel* m_staticTextLayerSel;
		QVBoxLayout* m_bSizerLayerList;
		QGridLayout* m_flexLeftColumnBoxSizer;
		QFrame* m_staticlineSep;
		QGridLayout* m_flexRightColumnBoxSizer;
		QLabel* m_staticTextCopperlayerCount;
		QComboBox* m_comboCopperLayersCount;
		QPushButton* m_buttonStore;
		QPushButton* m_buttonRetrieve;
		QPushButton* m_buttonReset;
		QDialogButtonBox* m_sdbSizerButtons;
		QPushButton* m_sdbSizerButtonsOK;
		QPushButton* m_sdbSizerButtonsCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnBrdLayersCountSelection() {}
		virtual void OnStoreSetup() {}
		virtual void OnGetSetup() {}
		virtual void OnResetClick() {}


	public:

		LAYERS_MAP_DIALOG_BASE( QWidget* parent = nullptr, int id = ID_LAYERS_MAP_DIALOG_BASE, const QString& title = "Layer Selection" );

		~LAYERS_MAP_DIALOG_BASE();

};

