#include "widgets/std_bitmap_button.h"

#include "dialog_import_gfx_sch_base.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QSizePolicy>
#include <QtCore/QStringList>

///////////////////////////////////////////////////////////////////////////

DIALOG_IMPORT_GFX_SCH_BASE::DIALOG_IMPORT_GFX_SCH_BASE( QWidget* parent, const QString& title ) : DIALOG_SHIM( parent )
{
	setWindowTitle( title );

	QVBoxLayout* bSizerMain = new QVBoxLayout( this );

	QHBoxLayout* bSizerFile = new QHBoxLayout();

	m_staticTextFile = new QLabel( "File:", this );
	m_staticTextFile->setToolTip( "Only vectors will be imported.  Bitmaps and fonts will be ignored." );

	bSizerFile->addWidget( m_staticTextFile, 0, Qt::AlignVCenter );
	bSizerFile->addSpacing( 5 );

	m_textCtrlFileName = new QLineEdit( this );
	m_textCtrlFileName->setToolTip( "Only vectors will be imported.  Bitmaps and fonts will be ignored." );
	m_textCtrlFileName->setMinimumSize( QSize( 300, -1 ) );

	bSizerFile->addWidget( m_textCtrlFileName, 1, Qt::AlignVCenter );
	bSizerFile->addSpacing( 5 );

	m_browseButton = new STD_BITMAP_BUTTON( this, -1, QPixmap(), QPoint(), QSize(), 0 );
	bSizerFile->addWidget( m_browseButton, 0 );


	bSizerMain->addLayout( bSizerFile );
	bSizerMain->addSpacing( 10 );

	QGroupBox* placementGroup = new QGroupBox( "Placement", this );
	QVBoxLayout* sbSizer2 = new QVBoxLayout( placementGroup );

	QVBoxLayout* bSizerOptions = new QVBoxLayout();

	m_rbInteractivePlacement = new QRadioButton( "Interactive placement", placementGroup );
	m_rbInteractivePlacement->setChecked( true );
	bSizerOptions->addWidget( m_rbInteractivePlacement );
	bSizerOptions->addSpacing( 5 );

	QHBoxLayout* bSizerUserPos = new QHBoxLayout();

	m_rbAbsolutePlacement = new QRadioButton( "At", placementGroup );
	bSizerUserPos->addWidget( m_rbAbsolutePlacement, 0, Qt::AlignVCenter );

	QHBoxLayout* bSizerPosSettings = new QHBoxLayout();

	m_xLabel = new QLabel( "X:", placementGroup );
	bSizerPosSettings->addWidget( m_xLabel, 0, Qt::AlignVCenter );
	bSizerPosSettings->addSpacing( 5 );

	m_xCtrl = new QLineEdit( placementGroup );
	m_xCtrl->setMaxLength( 10 );
	m_xCtrl->setToolTip( "DXF origin on PCB Grid, X Coordinate" );

	bSizerPosSettings->addWidget( m_xCtrl, 1, Qt::AlignVCenter );
	bSizerPosSettings->addSpacing( 5 );

	m_xUnits = new QLabel( "mm", placementGroup );
	bSizerPosSettings->addWidget( m_xUnits, 0, Qt::AlignVCenter );
	bSizerPosSettings->addSpacing( 5 );

	m_yLabel = new QLabel( "Y:", placementGroup );
	bSizerPosSettings->addWidget( m_yLabel, 0, Qt::AlignVCenter );
	bSizerPosSettings->addSpacing( 5 );

	m_yCtrl = new QLineEdit( placementGroup );
	m_yCtrl->setMaxLength( 10 );
	m_yCtrl->setToolTip( "DXF origin on PCB Grid, Y Coordinate" );

	bSizerPosSettings->addWidget( m_yCtrl, 0, Qt::AlignVCenter );
	bSizerPosSettings->addSpacing( 5 );

	m_yUnits = new QLabel( "mm", placementGroup );
	bSizerPosSettings->addWidget( m_yUnits, 0, Qt::AlignVCenter );


	bSizerUserPos->addLayout( bSizerPosSettings, 1 );

	bSizerOptions->addLayout( bSizerUserPos );

	sbSizer2->addLayout( bSizerOptions );

	bSizerMain->addWidget( placementGroup );
	bSizerMain->addSpacing( 10 );

	QGroupBox* importGroup = new QGroupBox( "Import Parameters", this );
	QVBoxLayout* sbSizer1 = new QVBoxLayout( importGroup );

	QHBoxLayout* bSizer7 = new QHBoxLayout();

	QGridLayout* fgSizerImportSettings = new QGridLayout();
	fgSizerImportSettings->setColumnStretch( 1, 1 );
	fgSizerImportSettings->setHorizontalSpacing( 5 );
	fgSizerImportSettings->setVerticalSpacing( 5 );

	m_importScaleLabel = new QLabel( "Import scale:", importGroup );
	fgSizerImportSettings->addWidget( m_importScaleLabel, 0, 0, Qt::AlignVCenter );

	m_importScaleCtrl = new QLineEdit( importGroup );
	fgSizerImportSettings->addWidget( m_importScaleCtrl, 0, 1, Qt::AlignVCenter );

	bSizer7->addLayout( fgSizerImportSettings, 1 );

	sbSizer1->addLayout( bSizer7 );

	bSizerMain->addWidget( importGroup );
	bSizerMain->addSpacing( 10 );

	QGroupBox* dxfGroup = new QGroupBox( "DXF Parameters", this );
	QVBoxLayout* sbSizer3 = new QVBoxLayout( dxfGroup );

	QHBoxLayout* bSizer81 = new QHBoxLayout();

	QGridLayout* fgDxfImportSettings = new QGridLayout();
	fgDxfImportSettings->setColumnStretch( 1, 1 );
	fgDxfImportSettings->setHorizontalSpacing( 5 );
	fgDxfImportSettings->setVerticalSpacing( 5 );

	m_lineWidthLabel = new QLabel( "Default line width:", dxfGroup );
	fgDxfImportSettings->addWidget( m_lineWidthLabel, 0, 0, Qt::AlignVCenter );

	m_lineWidthCtrl = new QLineEdit( dxfGroup );
	fgDxfImportSettings->addWidget( m_lineWidthCtrl, 0, 1, Qt::AlignVCenter );

	m_lineWidthUnits = new QLabel( "mm", dxfGroup );
	fgDxfImportSettings->addWidget( m_lineWidthUnits, 0, 2, Qt::AlignVCenter );

	m_staticTextLineWidth1 = new QLabel( "Default units:", dxfGroup );
	fgDxfImportSettings->addWidget( m_staticTextLineWidth1, 1, 0, Qt::AlignVCenter );

	m_choiceDxfUnits = new QComboBox( dxfGroup );
	m_choiceDxfUnits->setCurrentIndex( 0 );
	fgDxfImportSettings->addWidget( m_choiceDxfUnits, 1, 1 );

	bSizer81->addLayout( fgDxfImportSettings, 1 );

	sbSizer3->addLayout( bSizer81 );

	bSizerMain->addWidget( dxfGroup, 1 );
	bSizerMain->addSpacing( 10 );

	m_sdbSizer = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
	m_sdbSizerOK = m_sdbSizer->button( QDialogButtonBox::Ok );
	m_sdbSizerCancel = m_sdbSizer->button( QDialogButtonBox::Cancel );

	bSizerMain->addWidget( m_sdbSizer );

	setLayout( bSizerMain );
	adjustSize();

	// Connect Events
	connect( m_browseButton, &STD_BITMAP_BUTTON::clicked, this, &DIALOG_IMPORT_GFX_SCH_BASE::onBrowseFiles );
	connect( m_rbInteractivePlacement, &QRadioButton::clicked, this, &DIALOG_IMPORT_GFX_SCH_BASE::onInteractivePlacement );
	connect( m_rbAbsolutePlacement, &QRadioButton::clicked, this, &DIALOG_IMPORT_GFX_SCH_BASE::onAbsolutePlacement );
	connect( m_sdbSizer, &QDialogButtonBox::accepted, this, &QDialog::accept );
	connect( m_sdbSizer, &QDialogButtonBox::rejected, this, &QDialog::reject );
}

DIALOG_IMPORT_GFX_SCH_BASE::~DIALOG_IMPORT_GFX_SCH_BASE()
{
}
