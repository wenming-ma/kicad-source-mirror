
#pragma once

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QSlider>
#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPixmap>
#include "dialog_shim.h"

class DIALOG_COLOR_PICKER_BASE : public DIALOG_SHIM
{
	Q_OBJECT

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
		QHBoxLayout* m_SizerDefinedColors;
		QGridLayout* m_fgridColor;
		QHBoxLayout* m_SizerTransparency;
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

		virtual void onRGBMouseClick();
		virtual void onRGBMouseDrag();
		virtual void OnChangeEditRed();
		virtual void OnChangeEditGreen();
		virtual void OnChangeEditBlue();
		virtual void onHSVMouseClick();
		virtual void onHSVMouseDrag();
		virtual void onSize();
		virtual void OnChangeEditHue();
		virtual void OnChangeEditSat();
		virtual void OnChangeBrightness();
		virtual void OnChangeAlpha();
		virtual void OnColorValueText();
		virtual void OnResetButton();


	public:

		DIALOG_COLOR_PICKER_BASE( QWidget* parent = nullptr, const QString& title = "Color Picker" );

		~DIALOG_COLOR_PICKER_BASE();

};

