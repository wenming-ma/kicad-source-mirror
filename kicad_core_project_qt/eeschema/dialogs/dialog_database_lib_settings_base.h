// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
#include <QtCore/QEvent>
#include <QtCore/QSize>
#include <QtCore/QPoint>

class QT_INFOBAR;

#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_DATABASE_LIB_SETTINGS_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_DATABASE_LIB_SETTINGS_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QT_INFOBAR* m_infoBar;
		QRadioButton* m_rbDSN;
		QLineEdit* m_txtDSN;
		QLabel* m_staticText2;
		QLineEdit* m_txtUser;
		QLabel* m_staticText3;
		QLineEdit* m_txtPassword;
		QRadioButton* m_rbConnectionString;
		QLineEdit* m_txtConnectionString;
		QPushButton* m_btnTest;
		QPushButton* m_btnReloadConfig;
		QLabel* m_staticText5;
		QSpinBox* m_spinCacheSize;
		QLabel* m_staticText6;
		QSpinBox* m_spinCacheTimeout;
		QDialogButtonBox* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;
		QPushButton* m_sdbSizer1Cancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( QCloseEvent* event ) { event->ignore(); }
		virtual void OnDSNSelected() {}
		virtual void OnConnectionStringSelected() {}
		virtual void OnBtnTest() {}
		virtual void OnBtnReloadConfig() {}
		virtual void OnCloseClick() {}
		virtual void OnApplyClick() {}


	public:
		QLabel* m_stConnectionTestStatus;
		QLabel* m_stLibrariesStatus;

		DIALOG_DATABASE_LIB_SETTINGS_BASE( QWidget* parent = nullptr, int id = -1, const QString& title = "Database Library Settings", const QPoint& pos = QPoint(), const QSize& size = QSize( 500,600 ) );

		~DIALOG_DATABASE_LIB_SETTINGS_BASE();

};

