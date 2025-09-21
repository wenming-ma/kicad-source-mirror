#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtGui/QColor>

class STD_BITMAP_BUTTON;

#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_IMPORT_GFX_SCH_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_IMPORT_GFX_SCH_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QLabel* m_staticTextFile;
		QLineEdit* m_textCtrlFileName;
		STD_BITMAP_BUTTON* m_browseButton;
		QRadioButton* m_rbInteractivePlacement;
		QRadioButton* m_rbAbsolutePlacement;
		QLabel* m_xLabel;
		QLineEdit* m_xCtrl;
		QLabel* m_xUnits;
		QLabel* m_yLabel;
		QLineEdit* m_yCtrl;
		QLabel* m_yUnits;
		QLabel* m_importScaleLabel;
		QLineEdit* m_importScaleCtrl;
		QLabel* m_lineWidthLabel;
		QLineEdit* m_lineWidthCtrl;
		QLabel* m_lineWidthUnits;
		QLabel* m_staticTextLineWidth1;
		QComboBox* m_choiceDxfUnits;
		QDialogButtonBox* m_sdbSizer;
		QPushButton* m_sdbSizerOK;
		QPushButton* m_sdbSizerCancel;

		// Virtual event handlers, override them in your derived class
		virtual void onBrowseFiles() {}
		virtual void onInteractivePlacement() {}
		virtual void originOptionOnUpdateUI() {}
		virtual void onAbsolutePlacement() {}


	public:

		DIALOG_IMPORT_GFX_SCH_BASE( QWidget* parent = nullptr, const QString& title = "Import Vector Graphics File" );

		~DIALOG_IMPORT_GFX_SCH_BASE();

};
