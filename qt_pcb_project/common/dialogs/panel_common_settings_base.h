
#pragma once

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include "widgets/resettable_panel.h"

class STD_BITMAP_BUTTON;

class PANEL_COMMON_SETTINGS_BASE : public RESETTABLE_PANEL
{
	Q_OBJECT

	private:

	protected:
		QLabel* m_staticText20;
		QFrame* m_staticline3;
		QComboBox* m_antialiasing;
		QLabel* m_antialiasingFallbackLabel;
		QComboBox* m_antialiasingFallback;
		QLabel* m_staticText21;
		QFrame* m_staticline2;
		QLineEdit* m_textEditorPath;
		STD_BITMAP_BUTTON* m_textEditorBtn;
		QHBoxLayout* bSizerFileManager;
		QLabel* m_staticTextFileManager;
		QLineEdit* m_textCtrlFileManager;
		QRadioButton* m_defaultPDFViewer;
		QRadioButton* m_otherPDFViewer;
		QLineEdit* m_PDFViewerPath;
		STD_BITMAP_BUTTON* m_pdfViewerBtn;
		QLabel* m_staticText22;
		QFrame* m_staticline1;
		QCheckBox* m_checkBoxIconsInMenus;
		QCheckBox* m_showScrollbars;
		QCheckBox* m_focusFollowSchPcb;
		QCheckBox* m_hotkeyFeedback;
		QCheckBox* m_gridStriping;
		QLabel* m_stIconTheme;
		QRadioButton* m_rbIconThemeLight;
		QRadioButton* m_rbIconThemeDark;
		QRadioButton* m_rbIconThemeAuto;
		QLabel* m_stToolbarIconSize;
		QRadioButton* m_rbIconSizeSmall;
		QRadioButton* m_rbIconSizeNormal;
		QRadioButton* m_rbIconSizeLarge;
		QGridLayout* m_gbUserInterface;
		QLabel* m_staticTextCanvasScale;
		QDoubleSpinBox* m_canvasScaleCtrl;
		QCheckBox* m_canvasScaleAuto;
		QCheckBox* m_scaleFonts;
		QLabel* m_fontScalingHelp;
		QLabel* m_highContrastLabel;
		QLineEdit* m_highContrastCtrl;
		QLabel* m_highContrastUnits;
		QLabel* m_staticText23;
		QFrame* m_staticline6;
		QCheckBox* m_warpMouseOnMove;
		QCheckBox* m_NonImmediateActions;
		QLabel* m_staticText24;
		QFrame* m_staticline5;
		QCheckBox* m_cbRememberOpenFiles;
		QLabel* m_staticTextautosave;
		QSpinBox* m_SaveTime;
		QLabel* m_staticTextFileHistorySize;
		QSpinBox* m_fileHistorySize;
		QLabel* m_staticTextClear3DCache;
		QSpinBox* m_Clear3DCacheFilesOlder;
		QLabel* m_staticTextDays;
		QLabel* m_staticText25;
		QFrame* m_staticline4;
		QCheckBox* m_cbBackupEnabled;
		QCheckBox* m_cbBackupAutosave;
		QLabel* m_staticText9;
		QSpinBox* m_backupLimitTotalFiles;
		QLabel* m_staticText10;
		QSpinBox* m_backupLimitDailyFiles;
		QLabel* m_staticText11;
		QSpinBox* m_backupMinInterval;
		QLabel* m_staticText15;
		QLabel* m_staticText16;
		QSpinBox* m_backupLimitTotalSize;
		QLabel* m_staticText17;

		virtual void OnTextEditorClick();
		virtual void OnRadioButtonPdfViewer();
		virtual void OnPDFViewerClick();
		virtual void OnCanvasScaleAuto();


	public:

		PANEL_COMMON_SETTINGS_BASE( QWidget* parent = nullptr );

		~PANEL_COMMON_SETTINGS_BASE();

};

