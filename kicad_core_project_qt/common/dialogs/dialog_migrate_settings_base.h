// Qt dialog class for KiCad settings migration

#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QFont>
#include <QtGui/QColor>

class STD_BITMAP_BUTTON;

#include "dialog_shim.h"

#include "kicommon.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_MIGRATE_SETTINGS_BASE
///////////////////////////////////////////////////////////////////////////////
class KICOMMON_API DIALOG_MIGRATE_SETTINGS_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QVBoxLayout* m_sizer;
		QLabel* m_lblWelcome;
		QLabel* m_staticText2;
		QRadioButton* m_btnPrevVer;
		QComboBox* m_cbPath;
		STD_BITMAP_BUTTON* m_btnCustomPath;
		QLabel* m_lblPathError;
		QCheckBox* m_cbCopyLibraryTables;
		QRadioButton* m_btnUseDefaults;
		QDialogButtonBox* m_standardButtons;
		QPushButton* m_standardButtonsOK;
		QPushButton* m_standardButtonsCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnPrevVerSelected() {}
		virtual void OnPathChanged() {}
		virtual void OnPathDefocused() {}
		virtual void OnChoosePath() {}
		virtual void OnDefaultSelected() {}


	public:

		DIALOG_MIGRATE_SETTINGS_BASE( QWidget* parent = nullptr, const QString& title = "Configure KiCad Settings Path", const QPoint& pos = QPoint(), const QSize& size = QSize() );

		~DIALOG_MIGRATE_SETTINGS_BASE();

};

