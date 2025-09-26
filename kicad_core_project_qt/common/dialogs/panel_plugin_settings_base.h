#pragma once

#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtCore/QStringList>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QWidget>
#include <QtCore/QString>
#include <QtCore/QEvent>
#include "widgets/resettable_panel.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PANEL_PLUGIN_SETTINGS_BASE
///////////////////////////////////////////////////////////////////////////////
class PANEL_PLUGIN_SETTINGS_BASE : public RESETTABLE_PANEL
{
	private:

	protected:
		QLabel* m_staticText3;
		QCheckBox* m_cbEnableApi;
		QLabel* m_stApiStatus;
		QLabel* m_staticText2;
		QPushButton* m_pickerPythonInterpreter;
		QPushButton* m_btnDetectAutomatically;

		// Virtual event handlers, override them in your derived class
		virtual void OnEnableApiChecked() {}
		virtual void OnPythonInterpreterChanged() {}
		virtual void OnBtnDetectAutomaticallyClicked() {}


	public:
		QLabel* m_stPythonStatus;

		PANEL_PLUGIN_SETTINGS_BASE( QWidget* parent = nullptr );

		~PANEL_PLUGIN_SETTINGS_BASE();

};

