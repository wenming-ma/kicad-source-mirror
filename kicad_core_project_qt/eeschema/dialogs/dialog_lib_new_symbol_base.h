// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
///////////////////////////////////////////////////////////////////////////
// C++ code generated for Qt framework
// Transformed from wxWidgets to Qt
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtGui/QColor>
class QT_INFOBAR;

#include "dialog_shim.h"
#include <widgets/symbol_filter_combobox.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_LIB_NEW_SYMBOL_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_LIB_NEW_SYMBOL_BASE : public DIALOG_SHIM
{
	Q_OBJECT

private:

		// Private event handlers
		void _qt_onPowerCheckBox();

private slots:
		void onPowerCheckBoxClicked();


	protected:
		QT_INFOBAR* m_infoBar;
		QLabel* m_staticTextName;
		QLineEdit* m_textName;
		QLabel* m_staticText5;
		SYMBOL_FILTER_COMBOBOX* m_comboInheritanceSelect;
		QLabel* m_staticTextDes;
		QLineEdit* m_textReference;
		QLabel* m_staticTextUnits;
		QSpinBox* m_spinPartCount;
		QCheckBox* m_checkUnitsInterchangeable;
		QCheckBox* m_checkHasAlternateBodyStyle;
		QCheckBox* m_checkIsPowerSymbol;
		QCheckBox* m_excludeFromBomCheckBox;
		QCheckBox* m_excludeFromBoardCheckBox;
		QLabel* m_staticPinTextPositionLabel;
		QLineEdit* m_textPinTextPosition;
		QLabel* m_staticPinTextPositionUnits;
		QCheckBox* m_checkShowPinNumber;
		QCheckBox* m_checkShowPinName;
		QCheckBox* m_checkShowPinNameInside;
		QDialogButtonBox* m_sdbSizer;
		QPushButton* m_sdbSizerOK;
		QPushButton* m_sdbSizerCancel;

		// Virtual event handlers, override them in your derived class
		virtual void onPowerCheckBox();


	public:

		DIALOG_LIB_NEW_SYMBOL_BASE( QWidget* parent = nullptr, const QString& title = "New Symbol" );

		~DIALOG_LIB_NEW_SYMBOL_BASE();

};
