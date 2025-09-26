// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
///////////////////////////////////////////////////////////////////////////
// C++ code generated for Qt framework
// Transformed from wxWidgets to Qt
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_PRINT_GENERIC_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_PRINT_GENERIC_BASE : public DIALOG_SHIM
{
	private:

	protected:
		enum
		{
			ID_FRAME_SEL = 8200,
			ID_PRINT_OPTIONS,
		};

		QVBoxLayout* m_bUpperSizer;
		QGroupBox* m_sbOptionsSizer;
		QGridLayout* m_gbOptionsSizer;
		QLabel* m_outputModeLabel;
		QComboBox* m_outputMode;
		QCheckBox* m_titleBlock;
		QRadioButton* m_scale1;
		QRadioButton* m_scaleFit;
		QRadioButton* m_scaleCustom;
		QLineEdit* m_scaleCustomText;
		QLabel* m_infoText;
		QPushButton* m_buttonOption;
		QDialogButtonBox* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;
		QPushButton* m_sdbSizer1Apply;
		QPushButton* m_sdbSizer1Cancel;

		// Virtual event handlers, override them in your derived class
		virtual void onClose( QCloseEvent* event ) { event->accept(); }
		virtual void onSetCustomScale() { }
		virtual void onPageSetup() { }
		virtual void onPrintPreview() { }
		virtual void onCancelButtonClick() { }
		virtual void onPrintButtonClick() { }


	public:

		DIALOG_PRINT_GENERIC_BASE( QWidget* parent = nullptr, int id = -1, const QString& title = "Print", const QPoint& pos = QPoint(), const QSize& size = QSize( -1,-1 ), Qt::WindowFlags style = Qt::Dialog );

		~DIALOG_PRINT_GENERIC_BASE();

};

