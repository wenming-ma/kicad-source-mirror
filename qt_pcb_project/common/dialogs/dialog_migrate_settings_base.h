#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QString>
#include <QtCore/QEvent>
class STD_BITMAP_BUTTON;

#include "dialog_shim.h"

#include "kicommon.h"

class KICOMMON_API DIALOG_MIGRATE_SETTINGS_BASE : public DIALOG_SHIM
{
	Q_OBJECT
	
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
		QHBoxLayout* m_standardButtons;
		QPushButton* m_standardButtonsOK;
		QPushButton* m_standardButtonsCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnPrevVerSelected() {}
		virtual void OnPathChanged() {}
		virtual void OnPathDefocused() {}
		virtual void OnChoosePath() {}
		virtual void OnDefaultSelected() {}


	public:

		DIALOG_MIGRATE_SETTINGS_BASE( QWidget* parent = nullptr, const QString& title = "Configure KiCad Settings Path" );

		~DIALOG_MIGRATE_SETTINGS_BASE();
		
		bool eventFilter( QObject* watched, QEvent* event ) override;

};

