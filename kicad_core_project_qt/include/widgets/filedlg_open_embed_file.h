
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef KICAD_FILEDLG_OPEN_EMBED_FILE_H
#define KICAD_FILEDLG_OPEN_EMBED_FILE_H

#include <QFileDialog>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QWidget>


class FILEDLG_OPEN_EMBED_FILE : public QWidget
{
public:
    FILEDLG_OPEN_EMBED_FILE( bool aDefaultEmbed = true ) :
            m_embed( aDefaultEmbed )
    {};

    virtual void AddCustomControls( QFileDialog* dialog )
    {
        m_cb = new QCheckBox( "Embed file" );
        m_cb->setChecked( m_embed );

        QWidget* customWidget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout( customWidget );
        layout->addWidget( m_cb );

        dialog->setOption( QFileDialog::DontUseCustomDirectoryIcons, false );
    }

    virtual void TransferDataFromCustomControls()
    {
        m_embed = m_cb->isChecked();
    }

    bool GetEmbed() const { return m_embed; }

private:
    bool m_embed;

    QCheckBox* m_cb = nullptr;

    Q_DISABLE_COPY( FILEDLG_OPEN_EMBED_FILE )
};

#endif //KICAD_FILEDLG_OPEN_EMBED_FILE_H
