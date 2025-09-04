
#pragma once

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include "widgets/resettable_panel.h"

class PANEL_PLUGIN_SETTINGS_BASE : public RESETTABLE_PANEL
{
	Q_OBJECT

	private:

	protected:
		QLabel* m_staticText3;
		QCheckBox* m_cbEnableApi;
		QLabel* m_stApiStatus;
		QLabel* m_staticText2;
		QLineEdit* m_pickerPythonInterpreter;
		QPushButton* m_btnDetectAutomatically;

		virtual void OnEnableApiChecked();
		virtual void OnPythonInterpreterChanged();
		virtual void OnBtnDetectAutomaticallyClicked();


	public:
		QLabel* m_stPythonStatus;

		PANEL_PLUGIN_SETTINGS_BASE( QWidget* parent = nullptr );

		~PANEL_PLUGIN_SETTINGS_BASE();

};

