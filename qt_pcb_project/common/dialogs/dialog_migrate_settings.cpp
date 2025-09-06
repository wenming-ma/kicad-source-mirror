/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 Jon Evans <jon@craftyjon.com>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <bitmaps.h>
#include <dialogs/dialog_migrate_settings.h>
#include <settings/settings_manager.h>
#include <widgets/std_bitmap_button.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPushButton>


DIALOG_MIGRATE_SETTINGS::DIALOG_MIGRATE_SETTINGS( SETTINGS_MANAGER* aManager ) :
        DIALOG_MIGRATE_SETTINGS_BASE( nullptr, tr("Configure KiCad Settings Path") ), 
        m_manager( aManager )
{
    // TODO: Set minimum size if needed
    // setMinimumSize( QSize(480, -1) );
    
    m_standardButtonsCancel->setText( tr("Quit KiCad") );
    
    // TODO: Set bitmap for button when bitmap system is ready
    // m_btnCustomPath->SetBitmap( KiBitmapBundle( BITMAPS::small_folder ) );
    
    // Disabled for now. See https://gitlab.com/kicad/code/kicad/-/issues/9826
    m_cbCopyLibraryTables->hide();
    
    // TODO: Setup standard buttons when functionality is ready
    // SetupStandardButtons();
    
    layout()->setSizeConstraint( QLayout::SetFixedSize );
    adjustSize();
    
    // Center the dialog
    setWindowFlags( windowFlags() | Qt::Dialog );
    setAttribute( Qt::WA_DeleteOnClose, false );
}


DIALOG_MIGRATE_SETTINGS::~DIALOG_MIGRATE_SETTINGS()
{
}


bool DIALOG_MIGRATE_SETTINGS::TransferDataToWindow()
{
    QString str = QString( tr("Welcome to KiCad %1!") ).arg( SETTINGS_MANAGER::GetSettingsVersion() );
    m_lblWelcome->setText( str );
    
    std::vector<QString> paths;
    
    m_btnUseDefaults->setChecked( true );
    
    if( !m_manager->GetPreviousVersionPaths( &paths ) )
    {
        m_btnPrevVer->setText( tr("Import settings from a previous version (none found)") );
    }
    else
    {
        m_cbPath->clear();
        
        for( const auto& path : paths )
            m_cbPath->addItem( path );
        
        m_cbPath->setCurrentIndex( 0 );
    }
    
    // SetValue does not fire the event, so manually call the handler
    OnDefaultSelected();
    
    adjustSize();
    
    return true;
}


bool DIALOG_MIGRATE_SETTINGS::TransferDataFromWindow()
{
    if( m_btnPrevVer->isChecked() )
    {
        m_manager->SetMigrateLibraryTables( false );
        
        // Round-trip through a QFileInfo object to remove any trailing separators
        QFileInfo fileInfo( m_cbPath->currentText() );
        m_manager->SetMigrationSource( fileInfo.absolutePath() );
    }
    else
    {
        m_manager->SetMigrateLibraryTables( false );
        m_manager->SetMigrationSource( QString() );
    }
    
    return true;
}


void DIALOG_MIGRATE_SETTINGS::OnPrevVerSelected()
{
    m_standardButtonsOK->setEnabled( true );
    m_cbPath->setEnabled( true );
    m_btnCustomPath->setEnabled( true );
    m_cbCopyLibraryTables->setEnabled( true );
    validatePath();
}


void DIALOG_MIGRATE_SETTINGS::OnPathChanged()
{
    validatePath();
}


void DIALOG_MIGRATE_SETTINGS::OnPathDefocused()
{
    validatePath();
}


void DIALOG_MIGRATE_SETTINGS::OnChoosePath()
{
    QString dir = QFileDialog::getExistingDirectory( this, 
                                                      tr("Select Settings Path"),
                                                      m_cbPath->currentText(),
                                                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    
    if( !dir.isEmpty() )
    {
        m_cbPath->setCurrentText( dir );
        validatePath();
    }
}


void DIALOG_MIGRATE_SETTINGS::OnDefaultSelected()
{
    m_standardButtonsOK->setEnabled( true );
    m_cbPath->setEnabled( false );
    m_btnCustomPath->setEnabled( false );
    m_cbCopyLibraryTables->setEnabled( false );
    showPathError( false );
}


bool DIALOG_MIGRATE_SETTINGS::validatePath()
{
    QString path = m_cbPath->currentText();
    bool valid = m_manager->IsSettingsPathValid( path );
    
    showPathError( !valid );
    m_standardButtonsOK->setEnabled( valid && !path.isEmpty() );
    
    return valid;
}


void DIALOG_MIGRATE_SETTINGS::showPathError( bool aShow )
{
    m_lblPathError->setVisible( aShow );
    layout()->update();
    adjustSize();
}