// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#pragma once

#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QColor>

class STD_BITMAP_BUTTON;

#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////

#define ID_ALLOW_PRINT_PAD_ON_SILKSCREEN 6000
#define ID_MIROR_OPT 6001

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_PLOT_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_PLOT_BASE : public DIALOG_SHIM
{
	Q_OBJECT

	private:

	protected:
		QVBoxLayout* m_MainSizer;
		QLabel* m_staticTextPlotFmt;
		QComboBox* m_plotFormatOpt;
		QLabel* m_staticTextDir;
		QLineEdit* m_outputDirectoryName;
		STD_BITMAP_BUTTON* m_browseButton;
		STD_BITMAP_BUTTON* m_openDirButton;
		QVBoxLayout* bmiddleSizer;
		QGroupBox* m_LayersSizer;
		QListWidget* m_layerCheckListBox;
		QVBoxLayout* m_PlotOptionsSizer;
		QCheckBox* m_plotSheetRef;
		QCheckBox* m_subtractMaskFromSilk;
		QCheckBox* m_plotDNP;
		QRadioButton* m_hideDNP;
		QRadioButton* m_crossoutDNP;
		QCheckBox* m_sketchPadsOnFabLayers;
		QCheckBox* m_plotPadNumbers;
		QCheckBox* m_zoneFillCheck;
		QLabel* drillMarksLabel;
		QComboBox* m_drillShapeOpt;
		QLabel* scalingLabel;
		QComboBox* m_scaleOpt;
		QLabel* plotModeLabel;
		QComboBox* m_plotModeOpt;
		QCheckBox* m_useAuxOriginCheckBox;
		QCheckBox* m_plotMirrorOpt;
		QCheckBox* m_plotPSNegativeOpt;
		QHBoxLayout* m_SizerSolderMaskAlert;
		QLabel* m_bitmapAlert;
		QLabel* m_staticTextAlert;
		QLabel* m_staticTextAlert1;
		QPushButton* m_boardSetup;
		QGroupBox* m_GerberOptionsSizer;
		QCheckBox* m_useGerberExtensions;
		QCheckBox* m_generateGerberJobFile;
		QLabel* coordFormatLabel;
		QComboBox* m_coordFormatCtrl;
		QCheckBox* m_useGerberX2Format;
		QCheckBox* m_useGerberNetAttributes;
		QCheckBox* m_disableApertMacros;
		QGroupBox* m_HPGLOptionsSizer;
		QLabel* m_hpglPenLabel;
		QLineEdit* m_hpglPenCtrl;
		QLabel* m_hpglPenUnits;
		QGroupBox* m_PSOptionsSizer;
		QLabel* m_fineAdjustXLabel;
		QLineEdit* m_fineAdjustXCtrl;
		QLabel* m_fineAdjustYLabel;
		QLineEdit* m_fineAdjustYCtrl;
		QLabel* m_widthAdjustLabel;
		QLineEdit* m_widthAdjustCtrl;
		QLabel* m_widthAdjustUnits;
		QCheckBox* m_forcePSA4OutputOpt;
		QGroupBox* m_SizerDXF_options;
		QCheckBox* m_DXF_plotModeOpt;
		QLabel* DXF_exportUnitsLabel;
		QComboBox* m_DXF_plotUnits;
		QCheckBox* m_DXF_plotTextStrokeFontOpt;
		QGroupBox* m_svgOptionsSizer;
		QLabel* svgPrecisionLabel;
		QSpinBox* m_svgPrecsision;
		QLabel* m_staticText18;
		QComboBox* m_SVGColorChoice;
		QCheckBox* m_SVG_fitPageToBoard;
		QGroupBox* m_PDFOptionsSizer;
		QLabel* m_staticText19;
		QComboBox* m_PDFColorChoice;
		QCheckBox* m_frontFPPropertyPopups;
		QCheckBox* m_backFPPropertyPopups;
		QCheckBox* m_pdfMetadata;
		QCheckBox* m_pdfSingle;
		QTextEdit* m_messagesPanel;
		QHBoxLayout* m_sizerButtons;
		QPushButton* m_buttonDRC;
		QLabel* m_DRCExclusionsWarning;
		QDialogButtonBox* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;
		QPushButton* m_sdbSizer1Apply;
		QPushButton* m_sdbSizer1Cancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnInitDialog() {}
		virtual void SetPlotFormat() {}
		virtual void onOutputDirectoryBrowseClicked() {}
		virtual void onOpenOutputDirectory() {}
		virtual void onDNPCheckbox() {}
		virtual void onSketchPads() {}
		virtual void OnSetScaleOpt() {}
		virtual void onBoardSetup() {}
		virtual void OnGerberX2Checked() {}
		virtual void OnChangeDXFPlotMode() {}
		virtual void onRunDRC() {}
		virtual void CreateDrillFile() {}
		virtual void Plot() {}


	public:

		DIALOG_PLOT_BASE( QWidget* parent, const QString& title = "Plot" );

		~DIALOG_PLOT_BASE();

};

