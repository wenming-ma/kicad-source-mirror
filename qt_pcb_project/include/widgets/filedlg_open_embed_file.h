// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_FILEDLG_OPEN_EMBED_FILE_H
#define KICAD_FILEDLG_OPEN_EMBED_FILE_H

#include <QObject>
#include <QFileDialog>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>


class FILEDLG_OPEN_EMBED_FILE : public QObject
{
    Q_OBJECT

public:
    FILEDLG_OPEN_EMBED_FILE( bool aDefaultEmbed = true ) :
            m_embed( aDefaultEmbed )
    {};

    virtual void AddCustomControls( QVBoxLayout* layout )
    {
#ifdef Q_OS_MAC
        layout->addWidget( new QLabel( "\n\n" ) );  // Increase height of static box
#endif

        m_cb = new QCheckBox( tr( "Embed file" ) );
        m_cb->setChecked( m_embed );
        layout->addWidget( m_cb );
    }

    virtual void TransferDataFromCustomControls()
    {
        m_embed = m_cb->isChecked();
    }

    bool GetEmbed() const { return m_embed; }

private:
    bool m_embed;

    QCheckBox* m_cb = nullptr;

    FILEDLG_OPEN_EMBED_FILE( const FILEDLG_OPEN_EMBED_FILE& ) = delete;
    FILEDLG_OPEN_EMBED_FILE& operator=( const FILEDLG_OPEN_EMBED_FILE& ) = delete;
};

#endif //KICAD_FILEDLG_OPEN_EMBED_FILE_H
