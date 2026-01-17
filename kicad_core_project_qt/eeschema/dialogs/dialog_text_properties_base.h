// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSizePolicy>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtCore/QEvent>
#include <widgets/qt_infobar.h>

class BITMAP_BUTTON;
class COLOR_SWATCH;
class FONT_CHOICE;
class QT_COMBOBOX;

#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_TEXT_PROPERTIES_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_TEXT_PROPERTIES_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QT_INFOBAR* m_infoBar;
		QGridLayout* m_textEntrySizer;
		QLabel* m_textLabel;
		QTextEdit* m_textCtrl;
		QLabel* m_syntaxHelp;
		QCheckBox* m_excludeFromSim;
		QLabel* m_fontLabel;
		FONT_CHOICE* m_fontCtrl;
		BITMAP_BUTTON* m_separator1;
		BITMAP_BUTTON* m_bold;
		BITMAP_BUTTON* m_italic;
		BITMAP_BUTTON* m_separator2;
		BITMAP_BUTTON* m_hAlignLeft;
		BITMAP_BUTTON* m_hAlignCenter;
		BITMAP_BUTTON* m_hAlignRight;
		BITMAP_BUTTON* m_separator3;
		BITMAP_BUTTON* m_vAlignTop;
		BITMAP_BUTTON* m_vAlignCenter;
		BITMAP_BUTTON* m_vAlignBottom;
		BITMAP_BUTTON* m_separator4;
		BITMAP_BUTTON* m_horizontal;
		BITMAP_BUTTON* m_vertical;
		BITMAP_BUTTON* m_separator5;
		QLabel* m_textSizeLabel;
		QLineEdit* m_textSizeCtrl;
		QLabel* m_textSizeUnits;
		QLabel* m_textColorLabel;
		QFrame* m_panelBorderColor1;
		COLOR_SWATCH* m_textColorSwatch;
		QCheckBox* m_borderCheckbox;
		QLabel* m_borderWidthLabel;
		QLineEdit* m_borderWidthCtrl;
		QLabel* m_borderWidthUnits;
		QLabel* m_borderColorLabel;
		QFrame* m_panelBorderColor;
		COLOR_SWATCH* m_borderColorSwatch;
		QLabel* m_borderStyleLabel;
		QComboBox* m_borderStyleCombo;
		QCheckBox* m_filledCtrl;
		QLabel* m_fillColorLabel;
		QFrame* m_panelFillColor;
		COLOR_SWATCH* m_fillColorSwatch;
		QCheckBox* m_hyperlinkCb;
		QT_COMBOBOX* m_hyperlinkCombo;
		QGridLayout* m_fgSymbolEditor;
		QCheckBox* m_privateCheckbox;
		QCheckBox* m_commonToAllUnits;
		QCheckBox* m_commonToAllBodyStyles;
		QHBoxLayout* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;
		QPushButton* m_sdbSizer1Cancel;

		// Virtual event handlers, override them in your derived class
		virtual void onMultiLineTCLostFocus( QFocusEvent* event ) { event->ignore(); }
		virtual void OnFormattingHelp() { }
		virtual void onBorderChecked() { }
		virtual void onFillChecked() { }
		virtual void onHyperlinkChecked() { }
		virtual void onHyperlinkCombo() { }
		virtual void onHyperlinkText() { }


	public:

		DIALOG_TEXT_PROPERTIES_BASE( QWidget* parent = nullptr, const QString& title = "Text Properties" );

		~DIALOG_TEXT_PROPERTIES_BASE();

};
