
#include <confirm.h>

#include <functional>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextStream>
#include <QDebug>


static const QString traceConfirm = QStringLiteral( "KICAD_CONFIRM" );


bool AskOverrideLock( QWidget* aParent, const QString& aMessage )
{
    QMessageBox msgBox( aParent );
    msgBox.setWindowTitle( _( "File Open Warning" ) );
    msgBox.setText( aMessage );
    msgBox.setDetailedText( _( "Interleaved saves may produce very unexpected results." ) );
    msgBox.setIcon( QMessageBox::Warning );
    
    QPushButton* cancelBtn = msgBox.addButton( _( "&Cancel" ), QMessageBox::YesRole );
    QPushButton* openBtn = msgBox.addButton( _( "&Open Anyway" ), QMessageBox::NoRole );
    
    msgBox.setDefaultButton( cancelBtn );
    msgBox.exec();
    
    return msgBox.clickedButton() == openBtn;
}


int UnsavedChangesDialog( QWidget* parent, const QString& aMessage, bool* aApplyToAll )
{
    static bool s_apply_to_all = false;

    QMessageBox msgBox( parent );
    msgBox.setWindowTitle( _( "Save Changes?" ) );
    msgBox.setText( aMessage );
    msgBox.setDetailedText( _( "If you don't save, all your changes will be permanently lost." ) );
    msgBox.setIcon( QMessageBox::Warning );
    
    QPushButton* saveBtn = msgBox.addButton( _( "&Save" ), QMessageBox::YesRole );
    QPushButton* discardBtn = msgBox.addButton( _( "&Discard Changes" ), QMessageBox::NoRole );
    QPushButton* cancelBtn = msgBox.addButton( _( "&Cancel" ), QMessageBox::RejectRole );
    
    msgBox.setDefaultButton( saveBtn );
    
    // Checkbox for "Apply to all" functionality not implemented
    // Qt doesn't have built-in checkbox support in QMessageBox
    
    int ret = msgBox.exec();
    
    if( aApplyToAll )
    {
        // Checkbox state handling not implemented
        *aApplyToAll = s_apply_to_all;
    }

    // Map Qt button roles to wx IDs
    if( msgBox.clickedButton() == saveBtn )
        return QMessageBox::Yes;
    else if( msgBox.clickedButton() == discardBtn )
        return QMessageBox::No;
    else
        return QMessageBox::Cancel;
}


int UnsavedChangesDialog( QWidget* parent, const QString& aMessage )
{
    return UnsavedChangesDialog( parent, aMessage, nullptr );
}


bool ConfirmRevertDialog( QWidget* parent, const QString& aMessage )
{
    QMessageBox msgBox( parent );
    msgBox.setText( aMessage );
    msgBox.setDetailedText( _( "Your current changes will be permanently lost." ) );
    msgBox.setIcon( QMessageBox::Warning );
    
    QPushButton* revertBtn = msgBox.addButton( _( "&Revert" ), QMessageBox::AcceptRole );
    QPushButton* cancelBtn = msgBox.addButton( _( "&Cancel" ), QMessageBox::RejectRole );
    
    msgBox.setDefaultButton( revertBtn );
    msgBox.exec();
    
    return msgBox.clickedButton() == revertBtn;
}


bool HandleUnsavedChanges( QWidget* aParent, const QString& aMessage,
                           const std::function<bool()>& aSaveFunction )
{
    switch( UnsavedChangesDialog( aParent, aMessage ) )
    {
    case QMessageBox::Yes:    return aSaveFunction();
    case QMessageBox::No:     return true;
    default:
    case QMessageBox::Cancel: return false;
    }
}


int OKOrCancelDialog( QWidget* aParent, const QString& aWarning, const QString& aMessage,
                      const QString& aDetailedMessage, const QString& aOKLabel,
                      const QString& aCancelLabel, bool* aApplyToAll )
{
    QMessageBox msgBox( aParent );
    msgBox.setWindowTitle( aWarning );
    msgBox.setText( aMessage );
    msgBox.setIcon( QMessageBox::Warning );

    if( !aDetailedMessage.isEmpty() )
        msgBox.setDetailedText( aDetailedMessage );

    QPushButton* okBtn = msgBox.addButton( aOKLabel.isEmpty() ? _( "&OK" ) : aOKLabel, QMessageBox::AcceptRole );
    QPushButton* cancelBtn = msgBox.addButton( aCancelLabel.isEmpty() ? _( "&Cancel" ) : aCancelLabel, QMessageBox::RejectRole );
    
    msgBox.setDefaultButton( okBtn );
    
    // Checkbox for "Apply to all" functionality not implemented
    // Qt doesn't have built-in checkbox support in QMessageBox
    
    int ret = msgBox.exec();

    if( aApplyToAll )
        *aApplyToAll = true; // Checkbox state handling not implemented

    return msgBox.clickedButton() == okBtn ? QMessageBox::Ok : QMessageBox::Cancel;
}


// DisplayError should be deprecated, use DisplayErrorMessage instead
void DisplayError( QWidget* aParent, const QString& aText, int aDisplayTime )
{
    QApplication* app = qApp;
    if( !app )
    {
        qDebug() << aText;
        return;
    }

    if( app->type() == QApplication::Tty )
    {
        QTextStream( stderr ) << aText << Qt::endl;
        return;
    }

    QMessageBox::Icon icon = aDisplayTime > 0 ? QMessageBox::Information : QMessageBox::Critical;
    QMessageBox msgBox( aParent );
    msgBox.setWindowTitle( _( "Warning" ) );
    msgBox.setText( aText );
    msgBox.setIcon( icon );
    msgBox.setStandardButtons( QMessageBox::Ok );
    msgBox.exec();
}


void DisplayErrorMessage( QWidget* aParent, const QString& aText, const QString& aExtraInfo )
{
    QApplication* app = qApp;
    if( !app )
    {
        qDebug() << aText << aExtraInfo;
        return;
    }

    if( app->type() == QApplication::Tty )
    {
        QTextStream( stderr ) << aText << Qt::endl;
        return;
    }

    QMessageBox msgBox( aParent );
    msgBox.setWindowTitle( _( "Error" ) );
    msgBox.setText( aText );
    msgBox.setIcon( QMessageBox::Critical );
    msgBox.setStandardButtons( QMessageBox::Ok );

    if( !aExtraInfo.isEmpty() )
        msgBox.setDetailedText( aExtraInfo );

    msgBox.exec();
}


void DisplayInfoMessage( QWidget* aParent, const QString& aMessage, const QString& aExtraInfo )
{
    QApplication* app = qApp;
    if( !app || !app->activeWindow() )
    {
        qDebug() << traceConfirm << aMessage << aExtraInfo;
        return;
    }

    if( app->type() == QApplication::Tty )
    {
        QTextStream( stdout ) << aMessage << " " << aExtraInfo << Qt::endl;
        return;
    }

    QMessageBox msgBox( aParent );
    msgBox.setWindowTitle( _( "Information" ) );
    msgBox.setText( aMessage );
    msgBox.setIcon( QMessageBox::Information );
    msgBox.setStandardButtons( QMessageBox::Ok );

    if( !aExtraInfo.isEmpty() )
        msgBox.setDetailedText( aExtraInfo );

    msgBox.exec();
}


bool IsOK( QWidget* aParent, const QString& aMessage )
{
    QMessageBox msgBox( aParent );
    msgBox.setWindowTitle( _( "Confirmation" ) );
    msgBox.setText( aMessage );
    msgBox.setIcon( QMessageBox::Question );
    
    QPushButton* yesBtn = msgBox.addButton( _( "&Yes" ), QMessageBox::AcceptRole );
    QPushButton* noBtn = msgBox.addButton( _( "&No" ), QMessageBox::RejectRole );
    
    msgBox.setDefaultButton( yesBtn );
    msgBox.exec();
    
    return msgBox.clickedButton() == yesBtn;
}


int SelectSingleOption( QWidget* aParent, const QString& aTitle,
                        const QString& aMessage, const QStringList& aOptions )
{
    bool ok;
    QString item = QInputDialog::getItem( aParent, aTitle, aMessage, aOptions, 0, false, &ok );
    
    if( !ok )
        return -1;
        
    return aOptions.indexOf( item );
}

