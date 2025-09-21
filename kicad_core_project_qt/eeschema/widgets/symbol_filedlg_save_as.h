
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef SYMBOL_FILEDLG_SAVE_AS_
#define SYMBOL_FILEDLG_SAVE_AS_

#include <symbol_editor/symbol_saveas_type.h>
#include <QFileDialog>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QString>

class SYMBOL_FILEDLG_SAVE_AS : public QObject
{
public:
    SYMBOL_FILEDLG_SAVE_AS( SYMBOL_SAVEAS_TYPE aOption ) : m_option( aOption ){};

    virtual void AddCustomControls( QGroupBox* customizer )
    {
        QString padding;
#ifdef __APPLE__
        padding = "     ";
#endif

        // Radio buttons are only grouped if they are consecutive.  If we want padding, we need to add it
        // to the radio button labels
        m_simpleSaveAs         = new QRadioButton( "Do not update library tables" + padding );
        m_replaceTableEntry    = new QRadioButton( "Update existing library table entry" + padding );
        m_addGlobalTableEntry  = new QRadioButton( "Add new global library table entry" + padding );
        m_addProjectTableEntry = new QRadioButton( "Add new project library table entry" + padding );

        QVBoxLayout* layout = new QVBoxLayout( customizer );
        layout->addWidget( m_simpleSaveAs );
        layout->addWidget( m_replaceTableEntry );
        layout->addWidget( m_addGlobalTableEntry );
        layout->addWidget( m_addProjectTableEntry );

        if( m_option == SYMBOL_SAVEAS_TYPE::NORMAL_SAVE_AS )
            m_simpleSaveAs->setChecked( true );

        if( m_option == SYMBOL_SAVEAS_TYPE::REPLACE_TABLE_ENTRY )
            m_replaceTableEntry->setChecked( true );

        if( m_option == SYMBOL_SAVEAS_TYPE::ADD_GLOBAL_TABLE_ENTRY )
            m_addGlobalTableEntry->setChecked( true );

        if( m_option == SYMBOL_SAVEAS_TYPE::ADD_PROJECT_TABLE_ENTRY )
            m_addProjectTableEntry->setChecked( true );
    }

    virtual void TransferDataFromCustomControls()
    {
        if( m_replaceTableEntry->isChecked() )
            m_option = SYMBOL_SAVEAS_TYPE::REPLACE_TABLE_ENTRY;
        else if( m_addGlobalTableEntry->isChecked() )
            m_option = SYMBOL_SAVEAS_TYPE::ADD_GLOBAL_TABLE_ENTRY;
        else if( m_addProjectTableEntry->isChecked() )
            m_option = SYMBOL_SAVEAS_TYPE::ADD_PROJECT_TABLE_ENTRY;
        else
            m_option = SYMBOL_SAVEAS_TYPE::NORMAL_SAVE_AS;
    }

    SYMBOL_SAVEAS_TYPE GetOption() const { return m_option; }

private:
    SYMBOL_SAVEAS_TYPE m_option = SYMBOL_SAVEAS_TYPE::NORMAL_SAVE_AS;

    QRadioButton* m_simpleSaveAs         = nullptr;
    QRadioButton* m_replaceTableEntry    = nullptr;
    QRadioButton* m_addGlobalTableEntry  = nullptr;
    QRadioButton* m_addProjectTableEntry = nullptr;

    Q_DISABLE_COPY( SYMBOL_FILEDLG_SAVE_AS );
};

#endif
