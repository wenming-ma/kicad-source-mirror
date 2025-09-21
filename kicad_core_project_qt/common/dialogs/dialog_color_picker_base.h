#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtCore/QString>
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>
#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_COLOR_PICKER_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_COLOR_PICKER_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QTabWidget* m_notebook;
		QWidget* m_panelFreeColors;
		QLabel* m_RgbBitmap;
		QLabel* m_staticTextR;
		QLabel* m_staticTextG;
		QLabel* m_staticTextB;
		QSpinBox* m_spinCtrlRed;
		QSpinBox* m_spinCtrlGreen;
		QSpinBox* m_spinCtrlBlue;
		QLabel* m_HsvBitmap;
		QLabel* m_staticTextHue;
		QLabel* m_staticTextSat;
		QSpinBox* m_spinCtrlHue;
		QSpinBox* m_spinCtrlSaturation;
		QLabel* m_staticTextBright;
		QSlider* m_sliderBrightness;
		QWidget* m_panelDefinedColors;
		QBoxLayout* m_SizerDefinedColors;
		QGridLayout* m_fgridColor;
		QBoxLayout* m_SizerTransparency;
		QLabel* m_opacityLabel;
		QSlider* m_sliderTransparency;
		QLabel* m_staticTextOldColor;
		QLabel* m_OldColorRect;
		QLabel* m_NewColorRect;
		QLineEdit* m_colorValue;
		QPushButton* m_resetToDefault;
		QDialogButtonBox* m_sdbSizer;
		QPushButton* m_sdbSizerOK;
		QPushButton* m_sdbSizerCancel;

		// Virtual event handlers, override them in your derived class
		virtual void onRGBMouseClick( QMouseEvent* event ) {}
		virtual void onRGBMouseDrag( QMouseEvent* event ) {}
		virtual void OnChangeEditRed( int value ) {}
		virtual void OnChangeEditGreen( int value ) {}
		virtual void OnChangeEditBlue( int value ) {}
		virtual void onHSVMouseClick( QMouseEvent* event ) {}
		virtual void onHSVMouseDrag( QMouseEvent* event ) {}
		virtual void onSize( QResizeEvent* event ) {}
		virtual void OnChangeEditHue( int value ) {}
		virtual void OnChangeEditSat( int value ) {}
		virtual void OnChangeBrightness( int value ) {}
		virtual void OnChangeAlpha( int value ) {}
		virtual void OnColorValueText( const QString& text ) {}
		virtual void OnResetButton() {}


	public:

		DIALOG_COLOR_PICKER_BASE( QWidget* parent = nullptr, const QString& title = "Color Picker" );

		~DIALOG_COLOR_PICKER_BASE();

};

