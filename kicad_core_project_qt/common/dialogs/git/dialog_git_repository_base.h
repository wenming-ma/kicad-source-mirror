///////////////////////////////////////////////////////////////////////////
// C++ code generated for Qt framework
// Transformed from wxWidgets implementation
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtCore/QStringList>
#include <QtWidgets/QFileDialog>
#include <QtCore/QString>
#include <QtCore/QEvent>
#include <QtGui/QCloseEvent>
#include <QtGui/QFocusEvent>
#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_GIT_REPOSITORY_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_GIT_REPOSITORY_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QVBoxLayout* bSizerMain;
		QLabel* m_staticText1;
		QFrame* m_staticline1;
		QLabel* m_staticText3;
		QLineEdit* m_txtName;
		QLabel* m_staticText4;
		QLineEdit* m_txtURL;
		QLabel* m_staticText9;
		QComboBox* m_ConnType;
		QWidget* m_panelAuth;
		QLabel* m_staticText2;
		QFrame* m_staticline2;
		QCheckBox* m_cbCustom;
		QWidget* m_fpSSHKey;
		QLabel* m_staticText11;
		QLineEdit* m_txtUsername;
		QLabel* m_labelPass1;
		QLineEdit* m_txtPassword;
		QHBoxLayout* m_sdbSizer;
		QPushButton* m_sdbSizerOK;
		QPushButton* m_sdbSizerCancel;
		QPushButton* m_sdbSizerHelp;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( QCloseEvent* event ) { event->accept(); }
		virtual void OnUpdateUI() { }
		virtual void OnLocationExit( QFocusEvent* event ) { }
		virtual void OnSelectConnType() { }
		virtual void onCbCustom() { }
		virtual void OnFileUpdated() { }
		virtual void OnTestClick() { }
		virtual void OnOKClick() { }


	public:

		DIALOG_GIT_REPOSITORY_BASE( QWidget* parent = nullptr, const QString& title = "Git Repository" );

		~DIALOG_GIT_REPOSITORY_BASE();

};

