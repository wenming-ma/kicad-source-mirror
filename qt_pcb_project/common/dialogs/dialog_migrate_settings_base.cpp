///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// Qt port of dialog_migrate_settings_base
///////////////////////////////////////////////////////////////////////////

#include "widgets/std_bitmap_button.h"
#include "dialog_migrate_settings_base.h"

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QDialogButtonBox>

///////////////////////////////////////////////////////////////////////////

DIALOG_MIGRATE_SETTINGS_BASE::DIALOG_MIGRATE_SETTINGS_BASE( QWidget* parent, const QString& title ) : 
    DIALOG_SHIM( parent )
{
    setWindowTitle( title );
    setMinimumSize( QSize(480, -1) );
    
    m_sizer = new QVBoxLayout( this );
    
    QVBoxLayout* bSizer6 = new QVBoxLayout();
    
    m_lblWelcome = new QLabel( tr("Welcome to KiCad %s!"), this );
    QFont font = m_lblWelcome->font();
    font.setBold(true);
    m_lblWelcome->setFont(font);
    bSizer6->addWidget( m_lblWelcome );
    
    bSizer6->addSpacing( 10 );
    
    m_staticText2 = new QLabel( tr("How would you like to configure KiCad?"), this );
    bSizer6->addWidget( m_staticText2 );
    
    m_btnPrevVer = new QRadioButton( tr("Import settings from a previous version at:"), this );
    bSizer6->addWidget( m_btnPrevVer );
    
    QHBoxLayout* bSizer5 = new QHBoxLayout();
    bSizer5->addSpacing( 20 );
    
    m_cbPath = new QComboBox( this );
    m_cbPath->setEditable( true );
    bSizer5->addWidget( m_cbPath, 1 );
    
    m_btnCustomPath = new STD_BITMAP_BUTTON( this, -1, QPixmap() );
    m_btnCustomPath->setToolTip( tr("Choose a different path") );
    bSizer5->addWidget( m_btnCustomPath );
    
    bSizer6->addLayout( bSizer5 );
    
    m_lblPathError = new QLabel( tr("The selected path does not contain valid KiCad settings!"), this );
    QPalette palette = m_lblPathError->palette();
    palette.setColor( QPalette::WindowText, QColor(255, 43, 0) );
    m_lblPathError->setPalette( palette );
    m_lblPathError->hide();
    bSizer6->addWidget( m_lblPathError );
    
    m_cbCopyLibraryTables = new QCheckBox( tr("Import library configuration from previous version"), this );
    m_cbCopyLibraryTables->setChecked( true );
    m_cbCopyLibraryTables->setToolTip( tr("When checked, the symbol and footprint library tables from the previous version will be imported into this version") );
    bSizer6->addWidget( m_cbCopyLibraryTables );
    
    m_btnUseDefaults = new QRadioButton( tr("Start with default settings"), this );
    bSizer6->addWidget( m_btnUseDefaults );
    
    m_sizer->addLayout( bSizer6 );
    
    // Standard buttons
    m_standardButtons = new QHBoxLayout();
    m_standardButtonsOK = new QPushButton( tr("OK"), this );
    m_standardButtonsCancel = new QPushButton( tr("Cancel"), this );
    m_standardButtons->addStretch();
    m_standardButtons->addWidget( m_standardButtonsOK );
    m_standardButtons->addWidget( m_standardButtonsCancel );
    m_sizer->addLayout( m_standardButtons );
    
    setLayout( m_sizer );
    
    // Connect Events
    connect( m_btnPrevVer, &QRadioButton::clicked, this, &DIALOG_MIGRATE_SETTINGS_BASE::OnPrevVerSelected );
    connect( m_cbPath, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DIALOG_MIGRATE_SETTINGS_BASE::OnPathChanged );
    connect( m_cbPath, &QComboBox::editTextChanged, this, &DIALOG_MIGRATE_SETTINGS_BASE::OnPathChanged );
    connect( m_btnCustomPath, &STD_BITMAP_BUTTON::clicked, this, &DIALOG_MIGRATE_SETTINGS_BASE::OnChoosePath );
    connect( m_btnUseDefaults, &QRadioButton::clicked, this, &DIALOG_MIGRATE_SETTINGS_BASE::OnDefaultSelected );
    
    // For focus out event, we need to install event filter
    m_cbPath->installEventFilter( this );
}

DIALOG_MIGRATE_SETTINGS_BASE::~DIALOG_MIGRATE_SETTINGS_BASE()
{
}

bool DIALOG_MIGRATE_SETTINGS_BASE::eventFilter( QObject* watched, QEvent* event )
{
    if( watched == m_cbPath && event->type() == QEvent::FocusOut )
    {
        OnPathDefocused();
        return false;
    }
    return DIALOG_SHIM::eventFilter( watched, event );
}