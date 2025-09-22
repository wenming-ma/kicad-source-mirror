// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
// Qt-based dialog header
// Transformed from wxWidgets to Qt framework

#pragma once

#include "dialog_shim.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>

class DIALOG_DRAW_LAYERS_SETTINGS_BASE : public DIALOG_SHIM
{
	private:
		QBoxLayout* m_namiSizer;

	protected:
		QLabel* m_stLayerNameTitle;
		QLabel* m_stLayerName;
		QLabel* m_stOffsetX;
		QLineEdit* m_tcOffsetX;
		QLabel* m_stUnitX;
		QLabel* m_stOffsetY;
		QLineEdit* m_tcOffsetY;
		QLabel* m_stUnitY;
		QLabel* m_stLayerRot;
		QLineEdit* m_tcRotation;
		QLabel* m_stUnitRot;
		QFrame* m_staticline1;
		QButtonGroup* m_rbScope;
		QFrame* m_staticline2;
		QDialogButtonBox* m_sdbSizerStdButtons;
		QPushButton* m_sdbSizerStdButtonsOK;
		QPushButton* m_sdbSizerStdButtonsCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnInitDlg();
		virtual void OnUpdateUI();


	public:

		DIALOG_DRAW_LAYERS_SETTINGS_BASE( QWidget* parent = nullptr, const QString& title = "Layers Settings" );

		~DIALOG_DRAW_LAYERS_SETTINGS_BASE();

};

