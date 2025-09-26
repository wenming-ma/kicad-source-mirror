// Pin Properties Dialog Base Class
// Transformed from wxWidgets to Qt framework

#pragma once

#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QGroupBox>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QString>
class PinShapeComboBox;
class PinTypeComboBox;
class STD_BITMAP_BUTTON;
class QTableWidget;
class QLabel;

#include "dialog_shim.h"

// Pin Properties Dialog Base Class
class DIALOG_PIN_PROPERTIES_BASE : public DIALOG_SHIM
{
	Q_OBJECT

private:

protected:
		QLabel* m_infoBar;
		QLabel* m_pinNameLabel;
		QLineEdit* m_textPinName;
		QLabel* m_pinNumberLabel;
		QLineEdit* m_textPinNumber;
		QLabel* m_posXLabel;
		QLineEdit* m_posXCtrl;
		QLabel* m_posYLabel;
		QLineEdit* m_posYCtrl;
		QLabel* m_pinLengthLabel;
		QLineEdit* m_pinLengthCtrl;
		QLabel* m_nameSizeLabel;
		QLineEdit* m_nameSizeCtrl;
		QLabel* m_numberSizeLabel;
		QLineEdit* m_numberSizeCtrl;
		QLabel* m_staticTextEType;
		PinTypeComboBox* m_choiceElectricalType;
		QLabel* m_staticTextGstyle;
		PinShapeComboBox* m_choiceStyle;
		QLabel* m_staticTextOrient;
		QComboBox* m_choiceOrientation;
		QLabel* m_posXUnits;
		QLabel* m_posYUnits;
		QLabel* m_pinLengthUnits;
		QLabel* m_nameSizeUnits;
		QLabel* m_numberSizeUnits;
		QCheckBox* m_checkApplyToAllParts;
		QCheckBox* m_checkApplyToAllBodyStyles;
		QCheckBox* m_checkShow;
		QLabel* m_staticText16;
		QWidget* m_panelShowPin;
		QGroupBox* m_alternatesTurndown;
		QTableWidget* m_alternatesGrid;
		STD_BITMAP_BUTTON* m_addAlternate;
		STD_BITMAP_BUTTON* m_deleteAlternate;
		QDialogButtonBox* m_sdbSizerButtons;
		QPushButton* m_sdbSizerButtonsOK;
		QPushButton* m_sdbSizerButtonsCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnSize( QResizeEvent* event ) { event->accept(); }
		virtual void OnUpdateUI( QEvent* event ) { event->accept(); }
		virtual void OnPropertiesChange() { }
		virtual void OnPaintShowPanel( QPaintEvent* event ) { event->accept(); }
		virtual void OnCollapsiblePaneChange() { }
		virtual void OnAddAlternate() { }
		virtual void OnDeleteAlternate() { }


public:

		DIALOG_PIN_PROPERTIES_BASE( QWidget* parent = nullptr, const QString& title = "Pin Properties" );

		~DIALOG_PIN_PROPERTIES_BASE();

};
