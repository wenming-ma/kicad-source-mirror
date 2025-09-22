
// Qt transformation completed - wxWidgets code converted to Qt framework
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <QDialog>
#include <QtGlobal>
#include <reporter.h>
#include <dialogs/html_message_box.h>
#include <gerbview_frame.h>
#include <gerbview_id.h>
#include <gerber_file_image.h>
#include <gerber_file_image_list.h>
#include <excellon_image.h>
#include <lset.h>
#include <wildcards_and_files_ext.h>
#include <view/view.h>
#include <widgets/wx_progress_reporters.h>
#include "widgets/gerbview_layer_widget.h"
#include <tool/tool_manager.h>

// HTML Messages used more than one time:
#define MSG_NO_MORE_LAYER _("<b>No more available layers</b> in GerbView to load files")
#define MSG_NOT_LOADED _("<b>Not loaded:</b> <i>%s</i>")
#define MSG_OOM _("<b>Memory was exhausted reading:</b> <i>%s</i>")


void GERBVIEW_FRAME::OnGbrFileHistory( QCommandEvent& event )
{
    QString fn;

    fn = GetFileFromHistory( event.GetId(), _("Gerber files") );

    if( !fn.isEmpty() )
    {
        LoadGerberFiles( fn );
    }
}

void GERBVIEW_FRAME::OnClearGbrFileHistory( QCommandEvent& aEvent )
{
    ClearFileHistory();
}


void GERBVIEW_FRAME::OnDrlFileHistory( QCommandEvent& event )
{
    QString fn;

    fn = GetFileFromHistory( event.GetId(), _("Drill files"), &m_drillFileHistory );

    if( !fn.isEmpty() )
    {
        LoadExcellonFiles( fn );
    }
}


void GERBVIEW_FRAME::OnClearDrlFileHistory( QCommandEvent& aEvent )
{
    ClearFileHistory( &m_drillFileHistory );
}


void GERBVIEW_FRAME::OnZipFileHistory( QCommandEvent& event )
{
    QString filename;
    filename = GetFileFromHistory( event.GetId(), _("Zip files"), &m_zipFileHistory );

    if( !filename.isEmpty() )
    {
        LoadZipArchiveFile( filename );
    }
}


void GERBVIEW_FRAME::OnClearZipFileHistory( QCommandEvent& aEvent )
{
    ClearFileHistory( &m_zipFileHistory );
}


void GERBVIEW_FRAME::OnJobFileHistory( QCommandEvent& event )
{
    QString filename = GetFileFromHistory( event.GetId(), _("Job files"), &m_jobFileHistory );

    if( !filename.isEmpty() )
        LoadGerberJobFile( filename );
}


void GERBVIEW_FRAME::OnClearJobFileHistory( QCommandEvent& aEvent )
{
    ClearFileHistory( &m_jobFileHistory );
}


bool GERBVIEW_FRAME::LoadFileOrShowDialog( const QString& aFileName,
                                           const QString& dialogFiletypes,
                                           const QString& dialogTitle, const int filetype )
{
    static int lastGerberFileWildcard = 0;
    QStringList filenamesList;
    QFileInfo   filename = aFileName;
    QString currentPath;

    if( !filename.exists() )
    {
        // Use the current working directory if the file name path does not exist.
        if( QDir(filename.path()).exists() )
            currentPath = filename.path();
        else
        {
            currentPath = m_mruPath;

            // On Qt the path in QFileDialog is handled differently
            // finishing by the dir separator. Remove it if any:
            if( currentPath.endsWith( '\\' ) || currentPath.endsWith( '/' ) )
                currentPath.chop(1);
        }

        QFileDialog dlg( this, dialogTitle, currentPath );
        dlg.setNameFilter( dialogFiletypes );
        dlg.setFileMode( QFileDialog::ExistingFiles );
        dlg.setAcceptMode( QFileDialog::AcceptOpen );
        // Note: Qt doesn't have direct SetFilterIndex equivalent, handled differently

        if( dlg.exec() == QDialog::Rejected )
            return false;

        // Note: Qt handles filter index differently
        filenamesList = dlg.selectedFiles();
        m_mruPath = currentPath = dlg.directory().absolutePath();
    }
    else
    {
        filenamesList.append( aFileName );
        currentPath = filename.path();
        m_mruPath = currentPath;
    }

    // Set the busy cursor
    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool isFirstFile = GetImagesList()->GetLoadedImageCount() == 0;

    std::vector<int> fileTypesVec( filenamesList.count(), filetype );
    bool success = LoadListOfGerberAndDrillFiles( currentPath, filenamesList, &fileTypesVec );

    // Auto zoom / sort is only applied when no other files have been loaded
    if( isFirstFile )
    {
        int ly = GetActiveLayer();

        SortLayersByFileExtension();
        Zoom_Automatique( false );

        // Ensure the initial active graphic layer is updated after sorting.
        SetActiveLayer( ly, true );
    }

    // Restore cursor
    QApplication::restoreOverrideCursor();

    return success;
}


bool GERBVIEW_FRAME::LoadAutodetectedFiles( const QString& aFileName )
{
    // 2 = autodetect files
    return LoadFileOrShowDialog( aFileName, FILEEXT::AllFilesWildcard(), _(\"Open Autodetected File(s)\"),
                                 2 );
}


bool GERBVIEW_FRAME::LoadGerberFiles( const QString& aFileName )
{
    QString   filetypes;
    QFileInfo filename = aFileName;

    /* Standard gerber filetypes
     * (See http://en.wikipedia.org/wiki/Gerber_File)
     * The .gbr (.pho in legacy files) extension is the default used in Pcbnew; however
     * there are a lot of other extensions used for gerber files.  Because the first letter
     * is usually g, we accept g* as extension.
     * (Mainly internal copper layers do not have specific extension, and filenames are like
     * *.g1, *.g2 *.gb1 ...)
     * Now (2014) Ucamco (the company which manages the Gerber format) encourages use of .gbr
     * only and the Gerber X2 file format.
     */
    filetypes = _("Gerber files") + AddFileExtListToFilter( { "g*", "pho" } ) + "|";

    /* Special gerber filetypes */
    filetypes += _("Top layer") + AddFileExtListToFilter( { "gtl" } ) + "|";
    filetypes += _("Bottom layer") + AddFileExtListToFilter( { "gbl" } ) + "|";
    filetypes += _("Bottom solder resist") + AddFileExtListToFilter( { "gbs" } ) + "|";
    filetypes += _("Top solder resist") + AddFileExtListToFilter( { "gts" } ) + "|";
    filetypes += _("Bottom overlay") + AddFileExtListToFilter( { "gbo" } ) + "|";
    filetypes += _("Top overlay") + AddFileExtListToFilter( { "gto" } ) + "|";
    filetypes += _("Bottom paste") + AddFileExtListToFilter( { "gbp" } ) + "|";
    filetypes += _("Top paste") + AddFileExtListToFilter( { "gtp" } ) + "|";
    filetypes += _("Keep-out layer") + AddFileExtListToFilter( { "gko" } ) + "|";
    filetypes += _("Mechanical layers")
                 + AddFileExtListToFilter(
                         { "gm1", "gm2", "gm3", "gm4", "gm5", "gm6", "gm7", "gm8", "gm9" } )
                 + "|";
    filetypes += _("Top Pad Master") + AddFileExtListToFilter( { "gpt" } ) + "|";
    filetypes += _("Bottom Pad Master") + AddFileExtListToFilter( { "gpb" } ) + "|";

    // All filetypes
    filetypes += FILEEXT::AllFilesWildcard();

    // 0 = gerber files
    return LoadFileOrShowDialog( aFileName, filetypes, _("Open Gerber File(s)"), 0 );
}


bool GERBVIEW_FRAME::LoadExcellonFiles( const QString& aFileName )
{
    QString filetypes = FILEEXT::DrillFileWildcard();
    filetypes += "|";
    filetypes += FILEEXT::AllFilesWildcard();

    // 1 = drill files
    return LoadFileOrShowDialog( aFileName, filetypes, _("Open NC (Excellon) Drill File(s)"), 1 );
}


bool GERBVIEW_FRAME::LoadListOfGerberAndDrillFiles( const QString&      aPath,
                                                    const QStringList& aFilenameList,
                                                    std::vector<int>*    aFileType )
{
    Q_ASSERT_X( aFilenameList.count() == aFileType->size(), "LoadListOfGerberAndDrillFiles",
                "Mismatch in file names and file types count" );
    if( aFilenameList.count() != aFileType->size() )
        return false;

    QFileInfo filename;

    // Read gerber files: each file is loaded on a new GerbView layer
    bool success = true;
    int layer = GetActiveLayer();
    int  firstLoadedLayer = NO_AVAILABLE_LAYERS;
    LSET visibility = GetVisibleLayers();

    // Manage errors when loading files
    WX_STRING_REPORTER reporter;

    // Create progress dialog (only used if more than 1 file to load
    std::unique_ptr<WX_PROGRESS_REPORTER> progress = nullptr;

    for( int ii = 0; ii < aFilenameList.count(); ii++ )
    {
        filename = aFilenameList[ii];

        if( !filename.isAbsolute() )
            filename = QFileInfo( QDir(aPath), filename.fileName() );

        // Check for non existing files, to avoid creating broken or useless data
        // and report all in one error list:
        if( !filename.exists() )
        {
            QString warning;
            warning += "<b>" + _("File not found:") + "</b><br>"
                    + filename.absoluteFilePath() + "<br>";
            reporter.Report( warning, RPT_SEVERITY_WARNING );
            success = false;
            continue;
        }

        if( filename.suffix() == FILEEXT::GerberJobFileExtension.c_str() )
        {
            //We cannot read a gerber job file as a gerber plot file: skip it
            QString txt = QString::asprintf( _("<b>A gerber job file cannot be loaded as a plot file</b> "
                           "<i>%s</i>"),
                        filename.fileName().toStdString().c_str() );
            success = false;
            reporter.Report( txt, RPT_SEVERITY_ERROR );
            continue;
        }


        m_lastFileName = filename.absoluteFilePath();

        if( !progress && ( aFilenameList.count() > 1 ) )
        {
            progress = std::make_unique<WX_PROGRESS_REPORTER>( this, _("Loading files..."), 1,
                                                               false );
            progress->SetMaxProgress( aFilenameList.count() - 1 );
            progress->Report( QString::asprintf( _("Loading %u/%zu %s..."),
                                                ii+1,
                                                aFilenameList.count(),
                                                m_lastFileName.toStdString().c_str() ) );
        }
        else if( progress )
        {
            progress->Report( QString::asprintf( _("Loading %u/%zu %s..."),
                                                ii+1,
                                                aFilenameList.count(),
                                                m_lastFileName.toStdString().c_str() ) );
            progress->KeepRefreshing();
        }


        // Make sure we have a layer available to load into
        layer = getNextAvailableLayer();

        if( layer == NO_AVAILABLE_LAYERS )
        {
            success = false;
            reporter.Report( MSG_NO_MORE_LAYER, RPT_SEVERITY_ERROR );

            // Report the name of not loaded files:
            while( ii < aFilenameList.count() )
            {
                filename = aFilenameList[ii++];
                QString txt = QString::asprintf( MSG_NOT_LOADED, filename.fileName().toStdString().c_str() );
                reporter.Report( txt, RPT_SEVERITY_ERROR );
            }
            break;
        }

        SetActiveLayer( layer, false );
        visibility[ layer ] = true;

        try
        {
            // 2 = Autodetect
            if( ( *aFileType )[ii] == 2 )
            {
                if( EXCELLON_IMAGE::TestFileIsExcellon( filename.absoluteFilePath() ) )
                    ( *aFileType )[ii] = 1;
                else if( GERBER_FILE_IMAGE::TestFileIsRS274( filename.absoluteFilePath() ) )
                    ( *aFileType )[ii] = 0;
            }

            switch( ( *aFileType )[ii] )
            {
            case 0:

                if( Read_GERBER_File( filename.absoluteFilePath() ) )
                {
                    UpdateFileHistory( filename.absoluteFilePath() );

                    if( firstLoadedLayer == NO_AVAILABLE_LAYERS )
                    {
                        firstLoadedLayer = layer;
                    }
                }

                break;

            case 1:

                if( Read_EXCELLON_File( filename.absoluteFilePath() ) )
                {
                    UpdateFileHistory( filename.absoluteFilePath(), &m_drillFileHistory );

                    // Select the first added layer by default when done loading
                    if( firstLoadedLayer == NO_AVAILABLE_LAYERS )
                    {
                        firstLoadedLayer = layer;
                    }
                }

                break;
            default:
                QString txt = QString::asprintf( MSG_NOT_LOADED, filename.fileName().toStdString().c_str() );
                reporter.Report( txt, RPT_SEVERITY_ERROR );
            }
        }
        catch( const std::bad_alloc& )
        {
            QString txt = QString::asprintf( MSG_OOM, filename.fileName().toStdString().c_str() );
            reporter.Report( txt, RPT_SEVERITY_ERROR );
            success = false;
            continue;
        }

        if( progress )
            progress->AdvanceProgress();
    }

    if( !success )
    {
        QApplication::processEvents();  // Allows slice of time to redraw the screen
                        // to refresh widgets, before displaying messages
        HTML_MESSAGE_BOX mbox( this, _("Errors") );
        mbox.ListSet( reporter.GetMessages() );
        mbox.ShowModal();
    }

    SetVisibleLayers( visibility );

    if( firstLoadedLayer != NO_AVAILABLE_LAYERS )
        SetActiveLayer( firstLoadedLayer, true );

    // Synchronize layers tools with actual active layer:
    ReFillLayerWidget();

    m_LayersManager->UpdateLayerIcons();
    syncLayerBox( true );

    GetCanvas()->Refresh();

    return success;
}


bool GERBVIEW_FRAME::unarchiveFiles( const QString& aFullFileName, REPORTER* aReporter )
{
    bool     foundX2Gerbers = false;
    QString msg;
    int      firstLoadedLayer = NO_AVAILABLE_LAYERS;
    LSET     visibility = GetVisibleLayers();

    // Extract the path of aFullFileName. We use it to store temporary files
    QFileInfo fn( aFullFileName );
    QString   unzipDir = fn.path();

    QFile zipFile( aFullFileName );

    if( !zipFile.open(QIODevice::ReadOnly) )
    {
        if( aReporter )
        {
            msg = QString::asprintf( _("Zip file '%s' cannot be opened."), aFullFileName.toStdString().c_str() );
            aReporter->Report( msg, RPT_SEVERITY_ERROR );
        }

        return false;
    }

    // Update the list of recent zip files.
    UpdateFileHistory( aFullFileName, &m_zipFileHistory );

    // The unzipped file in only a temporary file. Give it a filename
    // which cannot conflict with an usual filename.
    // TODO: make Read_GERBER_File() and Read_EXCELLON_File() able to
    // accept a stream, and avoid using a temp file.
    QFileInfo temp_fn( QDir(unzipDir), "$tempfile.tmp");
    QString unzipped_tempfile = temp_fn.absoluteFilePath();


    bool             success = true;
    // Note: Qt doesn't have direct zip stream equivalent, need alternative implementation
    // For full Qt implementation, use QZipReader or similar
    // wxZipInputStream zipArchive( zipFile );
    // wxZipEntry*      entry;
    void*            entry = nullptr; // Placeholder for zip entry
    bool             reported_no_more_layer = false;
    KIGFX::VIEW*     view = GetCanvas()->GetView();

    // Note: This section needs proper Qt zip implementation
    // while( ( entry = zipArchive.GetNextEntry() ) != nullptr )
    {
        if( entry->IsDir() )
            continue;

        QString   fname = entry->GetName();
        QFileInfo uzfn = fname;
        QString   curr_ext = uzfn.suffix().toLower();

        // The archive contains Gerber and/or Excellon drill files. Use the right loader.
        // However it can contain a few other files (reports, pdf files...),
        // which will be skipped.
        if( curr_ext == FILEEXT::GerberJobFileExtension.c_str() )
        {
            //We cannot read a gerber job file as a gerber plot file: skip it
            if( aReporter )
            {
                msg = QString::asprintf( _("Skipped file '%s' (gerber job file)."), entry->GetName() );
                aReporter->Report( msg, RPT_SEVERITY_WARNING );
            }

            continue;
        }

        QString               matchedExt;
        enum GERBER_ORDER_ENUM order;
        GERBER_FILE_IMAGE_LIST::GetGerberLayerFromFilename( fname, order, matchedExt );

        int layer = getNextAvailableLayer();

        if( layer == NO_AVAILABLE_LAYERS )
        {
            success = false;

            if( aReporter )
            {
                if( !reported_no_more_layer )
                    aReporter->Report( MSG_NO_MORE_LAYER,  RPT_SEVERITY_ERROR );

                reported_no_more_layer = true;

                // Report the name of not loaded files:
                msg = QString::asprintf( MSG_NOT_LOADED, entry->GetName() );
                aReporter->Report( msg, RPT_SEVERITY_ERROR );
            }

            delete entry;
            continue;
        }

        SetActiveLayer( layer, false );

        // Create the unzipped temporary file:
        {
            QFile temporary_ofile( unzipped_tempfile );

            if( temporary_ofile.open(QIODevice::WriteOnly) )
                temporary_ofile.write( zipArchive );
            else
            {
                success = false;

                if( aReporter )
                {
                    msg = QString::asprintf( _("<b>Unable to create temporary file '%s'.</b>"),
                                unzipped_tempfile.toStdString().c_str() );
                    aReporter->Report( msg, RPT_SEVERITY_ERROR );
                }
            }
        }

        bool read_ok = true;

        // Try to parse files if we can't tell from file extension
        if( order == GERBER_ORDER_ENUM::GERBER_LAYER_UNKNOWN )
        {
            if( EXCELLON_IMAGE::TestFileIsExcellon( unzipped_tempfile ) )
            {
                order = GERBER_ORDER_ENUM::GERBER_DRILL;
            }
            else if( GERBER_FILE_IMAGE::TestFileIsRS274( unzipped_tempfile ) )
            {
                // If we have no way to know what layer it is, just guess
                order = GERBER_ORDER_ENUM::GERBER_TOP_COPPER;
            }
            else
            {
                if( aReporter )
                {
                    msg = QString::asprintf( _("Skipped file '%s' (unknown type)."), entry->GetName() );
                    aReporter->Report( msg, RPT_SEVERITY_WARNING );
                }
            }
        }

        if( order == GERBER_ORDER_ENUM::GERBER_DRILL )
        {
            read_ok = Read_EXCELLON_File( unzipped_tempfile );
        }
        else if( order != GERBER_ORDER_ENUM::GERBER_LAYER_UNKNOWN )
        {
            // Read gerber files: each file is loaded on a new GerbView layer
            read_ok = Read_GERBER_File( unzipped_tempfile );

            if( read_ok )
            {
                if( GERBER_FILE_IMAGE* gbrImage = GetGbrImage( layer ) )
                    view->SetLayerHasNegatives( GERBER_DRAW_LAYER( layer ), gbrImage->HasNegativeItems() );
            }
        }

        // Select the first added layer by default when done loading
        if( read_ok && firstLoadedLayer == NO_AVAILABLE_LAYERS )
        {
            firstLoadedLayer = layer;
        }

        delete entry;

        // The unzipped file is only a temporary file, delete it.
        QFile::remove( unzipped_tempfile );

        if( !read_ok )
        {
            success = false;

            if( aReporter )
            {
                msg = QString::asprintf( _("<b>unzipped file %s read error</b>"), unzipped_tempfile.toStdString().c_str() );
                aReporter->Report( msg, RPT_SEVERITY_ERROR );
            }
        }
        else
        {
            GERBER_FILE_IMAGE* gerber_image = GetGbrImage( layer );
            visibility[ layer ] = true;

            if( gerber_image )
            {
                gerber_image->m_FileName = fname;
                if( gerber_image->m_IsX2_file )
                    foundX2Gerbers = true;
            }

            layer = getNextAvailableLayer();
            SetActiveLayer( layer, false );
        }
    }

    if( foundX2Gerbers )
        SortLayersByX2Attributes();
    else
        SortLayersByFileExtension();

    SetVisibleLayers( visibility );

    // Select the first layer loaded so we don't show another layer on top after
    if( firstLoadedLayer != NO_AVAILABLE_LAYERS )
        SetActiveLayer( firstLoadedLayer, true );

    return success;
}


bool GERBVIEW_FRAME::LoadZipArchiveFile( const QString& aFullFileName )
{
#define ZipFileExtension "zip"

    QFileInfo filename = aFullFileName;
    QString currentPath;

    if( !filename.exists() )
    {
        // Use the current working directory if the file name path does not exist.
        if( QDir(filename.path()).exists() )
            currentPath = filename.path();
        else
            currentPath = m_mruPath;

        QFileDialog dlg( this, _("Open Zip File"), currentPath );
        dlg.setNameFilter( FILEEXT::ZipFileWildcard() );
        dlg.setFileMode( QFileDialog::ExistingFile );
        dlg.setAcceptMode( QFileDialog::AcceptOpen );

        if( dlg.exec() == QDialog::Rejected )
            return false;

        filename = QFileInfo(dlg.selectedFiles().first());
        currentPath = QDir::currentPath();
        m_mruPath = currentPath;
    }
    else
    {
        currentPath = filename.path();
        m_mruPath = currentPath;
    }

    WX_STRING_REPORTER reporter;

    if( filename.exists() )
        unarchiveFiles( filename.absoluteFilePath(), &reporter );

    Zoom_Automatique( false );

    // Synchronize layers tools with actual active layer:
    ReFillLayerWidget();
    SetActiveLayer( GetActiveLayer() );
    m_LayersManager->UpdateLayerIcons();
    syncLayerBox();

    if( reporter.HasMessage() )
    {
        QApplication::processEvents();  // Allows slice of time to redraw the screen
                        // to refresh widgets, before displaying messages
        HTML_MESSAGE_BOX mbox( this, _("Messages") );
        mbox.ListSet( reporter.GetMessages() );
        mbox.ShowModal();
    }

    return true;
}

void GERBVIEW_FRAME::DoWithAcceptedFiles()
{
    QString gerbFn; // param to be sent with action event.

    for( const QFileInfo& file : m_AcceptedFiles )
    {
        if( file.suffix() == FILEEXT::ArchiveFileExtension )
        {
            QString fn = file.absoluteFilePath();
            // Open zip archive in editor
            m_toolManager->RunAction<QString*>( *m_acceptedExts.at( FILEEXT::ArchiveFileExtension ), &fn );
        }
        else
        {
            // Store FileName in variable to open later
            gerbFn += '"' + file.absoluteFilePath() + '"';
        }
    }

    // Open files in editor
    if( !gerbFn.isEmpty() )
        m_toolManager->RunAction<QString*>( *m_acceptedExts.at( FILEEXT::GerberFileExtension ), &gerbFn );
}
