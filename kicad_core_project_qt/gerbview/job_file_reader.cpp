

#include <json_common.h>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QCoreApplication>

#include <wildcards_and_files_ext.h>
#include <gerbview.h>
#include <richio.h>
#include <locale_io.h>
#include <string_utils.h>
#include <gerber_file_image.h>
#include <gerber_file_image_list.h>
#include <gerbview_frame.h>
#include <reporter.h>
#include <gbr_metadata.h>
#include <dialogs/html_message_box.h>
#include <view/view.h>


using json = nlohmann::json;

/**
 * this class read and parse a Gerber job file to extract useful info
 * for GerbView
 *
 * In a gerber job file, old (deprecated) format, data lines start by
 * %TF.     (usual Gerber X2 info)
 * %TJ.B.   (board info)
 * %TJ.D.   (design info)
 * %TJ.L.   (layers info)
 * some others are not yet handled by Kicad
 * M02*     is the last line

 * In a gerber job file, JSON format, first lines are
 *   {
 *    "Header":
 * and the block ( a JSON array) containing the filename of files to load is
 *    "FilesAttributes":
 *    [
 *      {
 *        "Path":  "interf_u-Composant.gbr",
 *        "FileFunction":  "Copper,L1,Top",
 *        "FilePolarity":  "Positive"
 *      },
 *      {
 *        "Path":  "interf_u-In1.Cu.gbr",
 *        "FileFunction":  "Copper,L2,Inr",
 *        "FilePolarity":  "Positive"
 *      },
 *    ],
 */

class GERBER_JOBFILE_READER
{
public:
    GERBER_JOBFILE_READER( const QString& aFileName, REPORTER* aReporter )
    {
        m_filename = QFileInfo(aFileName);
        m_reporter = aReporter;
    }

    ~GERBER_JOBFILE_READER() {}

    bool ReadGerberJobFile();       /// read a .gbrjob file
    QStringList& GetGerberFiles() { return m_GerberFiles; }

private:
    REPORTER* m_reporter;
    QFileInfo m_filename;
    QStringList m_GerberFiles;    // List of gerber files in job

    // Convert a JSON string, that uses escaped sequence of 4 hexadecimal digits
    // to encode unicode chars when not ASCII7 codes
    // json11 converts this sequence to UTF8 string
    QString formatStringFromJSON( const std::string& name );
};


bool GERBER_JOBFILE_READER::ReadGerberJobFile()
{
    // Read the gerber file */
   FILE* jobFile = fopen( m_filename.absoluteFilePath().toStdString().c_str(), "rt" );

    if( jobFile == nullptr )
        return false;

    LOCALE_IO toggleIo;

    FILE_LINE_READER jobfileReader( jobFile, m_filename.absoluteFilePath() );  // Will close jobFile

    QString data;

    // detect the file format: old (deprecated) gerber format of official JSON format
    bool json_format = false;

    char* line = jobfileReader.ReadLine();

    if( !line )     // end of file
        return false;

    data = line;

    if( data.contains( "{" ) )
        json_format = true;

    if( json_format )
    {
        while( ( line = jobfileReader.ReadLine() ) != nullptr )
            data += '\n' + QString(line);

        try
        {
            json js = json::parse( TO_UTF8( data ) );

            for( json& entry : js["FilesAttributes"] )
            {
                std::string name = entry["Path"].get<std::string>();
                m_GerberFiles.append( formatStringFromJSON( name ) );
            }
        }
        catch( ... )
        {
            return false;
        }
    }
    else
    {
        if( m_reporter )
            m_reporter->ReportTail( _( "This job file uses an outdated format. Please recreate it." ),
                                    RPT_SEVERITY_WARNING );

        return false;
    }

    return true;
}


QString GERBER_JOBFILE_READER::formatStringFromJSON( const std::string& name )
{
    // Convert a JSON string, that uses a escaped sequence of 4 hexadecimal digits
    // to encode unicode chars
    // Our json11 library returns in this case a UTF8 sequence. Just convert it to
    // a QString.
    QString wstr = From_UTF8( name.c_str() );
    return wstr;
}



bool GERBVIEW_FRAME::LoadGerberJobFile( const QString& aFullFileName )
{
    QFileInfo filename = QFileInfo(aFullFileName);
    QString currentPath;
    bool success = true;

    if( !filename.exists() )
    {
        // Use the current working directory if the file name path does not exist.
        if( QDir(filename.path()).exists() )
            currentPath = filename.path();
        else
            currentPath = m_mruPath;

        QFileDialog dlg( this, _("Open Gerber Job File"),
                          currentPath,
                          FILEEXT::GerberJobFileWildcard() );
        dlg.setFileMode(QFileDialog::ExistingFile);
        dlg.setAcceptMode(QFileDialog::AcceptOpen);

        if( dlg.exec() == QDialog::Rejected )
            return false;

        filename = QFileInfo(dlg.selectedFiles().first());
        currentPath = filename.path();
        m_mruPath = currentPath;
    }
    else
    {
        currentPath = filename.path();
        m_mruPath = currentPath;
    }

    WX_STRING_REPORTER reporter;

    if( filename.exists() )
    {
        GERBER_JOBFILE_READER gbjReader( filename.absoluteFilePath(), &reporter );

        if( gbjReader.ReadGerberJobFile() )
        {
            // Update the list of recent drill files.
            UpdateFileHistory( filename.absoluteFilePath(), &m_jobFileHistory );

            Clear_DrawLayers( false );
            ClearMsgPanel();

            QStringList& gbrfiles = gbjReader.GetGerberFiles();

            // 0 = Gerber file type
            std::vector<int> fileTypesVec( gbrfiles.count(), 0 );
            success = LoadListOfGerberAndDrillFiles( currentPath, gbrfiles, &fileTypesVec );

            Zoom_Automatique( false );
        }
    }

    SortLayersByX2Attributes();

    if( reporter.HasMessage() )
    {
        QCoreApplication::processEvents();  // Allows slice of time to redraw the screen
                        // to refresh widgets, before displaying messages
        HTML_MESSAGE_BOX mbox( this, _( "Messages" ) );
        mbox.ListSet( reporter.GetMessages() );
        mbox.ShowModal();
    }

    return success;
}
