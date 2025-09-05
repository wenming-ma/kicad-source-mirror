
#include <pgm_base.h>
#include <common.h>
#include <confirm.h>
#include <embedded_files.h>
#include <gestfich.h>
#include <settings/common_settings.h>
#include <i18n_utility.h>
#include <wildcards_and_files_ext.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMimeDatabase>
#include <QUrl>
#include <QDesktopServices>
#include <QDebug>
#include <QProcess>
#include <QObject>
// Mime type extensions (PDF files are not considered here)
static QMimeDatabase mimeDatabase;


bool GetAssociatedDocument( QWidget* aParent, const QString& aDocName, PROJECT* aProject,
                            SEARCH_STACK* aPaths, std::vector<EMBEDDED_FILES*> aFilesStack )
{
    QString      docname;
    QString      fullfilename;
    QString      msg;
    QString      command;
    bool         success = false;

    // Replace before resolving as we might have a URL in a variable
    docname = ResolveUriByEnvVars( aDocName, aProject );

    // Check if this is a URI and handle appropriately
    {
        QUrl uri( docname );

        if( uri.hasFragment() || uri.hasQuery() || !uri.scheme().isEmpty() )
        {
            QString scheme = uri.scheme().toLower();

            if( scheme != QString::fromStdString( FILEEXT::KiCadUriPrefix ) )
            {
                if( QDesktopServices::openUrl( QUrl( docname ) ) )
                    return true;
            }
            else
            {
                if( aFilesStack.empty() )
                {
                    qDebug() << "No EMBEDDED_FILES object provided for kicad_embed URI";
                    return false;
                }

                if( !docname.startsWith( QString::fromStdString( FILEEXT::KiCadUriPrefix + "://" ) ) )
                {
                    qDebug() << "Invalid kicad_embed URI" << docname;
                    return false;
                }

                docname = docname.mid( QString::fromStdString( FILEEXT::KiCadUriPrefix + "://" ).length() );

                QFileInfo temp_file = aFilesStack[0]->GetTemporaryFileName( docname );
                int       ii = 1;

                while( !temp_file.exists() && ii < (int) aFilesStack.size() )
                    temp_file = aFilesStack[ii++]->GetTemporaryFileName( docname );

                if( !temp_file.exists() )
                {
                    qDebug() << "Failed to get temp file" << docname << "for kicad_embed URI";
                    return false;
                }

                qDebug() << "Opening embedded file" << docname << "as" << temp_file.absoluteFilePath();
                docname = temp_file.absoluteFilePath();
            }
        }
    }

#ifdef __WINDOWS__
    docname.replace( UNIX_STRING_DIR_SEP, WIN_STRING_DIR_SEP );
#else
    docname.replace( WIN_STRING_DIR_SEP, UNIX_STRING_DIR_SEP );
#endif

    // Compute the full file name
    if( QDir::isAbsolutePath( docname ) || aPaths == nullptr )
        fullfilename = docname;
    // If the file exists, this is a trivial case: return the filename "as this".  the name can
    // be an absolute path, or a relative path like ./filename or ../<filename>.
    else if( QFileInfo::exists( docname ) )
        fullfilename = docname;
    else
        fullfilename = QString::fromStdString( aPaths->FindValidPath( docname.toStdString() ) );

    QString extension;

#ifdef __WINDOWS__
    extension = ".*";
#endif

    if( fullfilename.contains( '*' ) || fullfilename.contains( '?' ) )
    {
        QFileInfo fileInfo( fullfilename );
        fullfilename = QFileDialog::getOpenFileName( aParent, 
                                                   _("Documentation File"),
                                                   fileInfo.path(),
                                                   "All Files (*.*)" );

        if( fullfilename.isEmpty() )
            return false;
    }

    if( !QFileInfo::exists( fullfilename ) )
    {
        msg = QString( _("Documentation file '%1' not found.") ).arg( docname );
        DisplayErrorMessage( aParent, msg );
        return false;
    }

    QFileInfo currentFileName( fullfilename );

    // Use Qt to resolve any "." and ".." in the path
    fullfilename = currentFileName.absoluteFilePath();

    QString file_ext = currentFileName.suffix();

    if( file_ext.toLower() == "pdf" )
    {
        success = OpenPDF( fullfilename );
        return success;
    }

    // Try to launch appropriate application
    QMimeType mimeType = mimeDatabase.mimeTypeForFile( fullfilename );
    
    if( mimeType.isValid() )
    {
        success = QDesktopServices::openUrl( QUrl::fromLocalFile( fullfilename ) );
    }

    if( !success )
    {
        msg = QString( _("Unknown MIME type for documentation file '%1'") ).arg( fullfilename );
        DisplayErrorMessage( aParent, msg );
    }

    return success;
}
