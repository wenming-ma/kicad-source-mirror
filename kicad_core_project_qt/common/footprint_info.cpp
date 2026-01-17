
/*
 * Functions to read footprint libraries and fill m_footprints by available footprints names
 * and their documentation (comments and keywords)
 */

#include <footprint_info.h>
#include <fp_lib_table.h>
#include <dialogs/html_message_box.h>
#include <string_utils.h>
#include <kiface_ids.h>
#include <kiway.h>
#include <lib_id.h>
#include <thread>
#include <utility>
#include <QString>
#include <QStringList>
#include <kiface_base.h>
#include <locale_io.h>

FOOTPRINT_INFO* FOOTPRINT_LIST::GetFootprintInfo( const QString& aLibNickname,
                                                  const QString& aFootprintName )
{
    if( aFootprintName.isEmpty() )
        return nullptr;

    for( std::unique_ptr<FOOTPRINT_INFO>& fp : m_list )
    {
        if( aLibNickname == fp->GetLibNickname() && aFootprintName == fp->GetFootprintName() )
            return fp.get();
    }

    return nullptr;
}


FOOTPRINT_INFO* FOOTPRINT_LIST::GetFootprintInfo( const QString& aFootprintName )
{
    if( aFootprintName.isEmpty() )
        return nullptr;

    LIB_ID fpid;

    Q_ASSERT_X( fpid.Parse( aFootprintName ) < 0, "GetFootprintInfo",
               QString::asprintf( "'%s' is not a valid LIB_ID.", qPrintable(aFootprintName) ).toStdString().c_str() );
    if( fpid.Parse( aFootprintName ) >= 0 )
        return nullptr;

    return GetFootprintInfo( fpid.GetLibNickname(), fpid.GetLibItemName() );
}


std::vector<SEARCH_TERM> FOOTPRINT_INFO::GetSearchTerms()
{
    std::vector<SEARCH_TERM> terms;

    terms.emplace_back( SEARCH_TERM( GetLibNickname(), 4 ) );
    terms.emplace_back( SEARCH_TERM( GetName(), 8 ) );
    terms.emplace_back( SEARCH_TERM( GetLIB_ID().Format(), 16 ) );

    QStringList keywordTokens = GetKeywords().split( " ", Qt::SkipEmptyParts );

    for( const QString& token : keywordTokens )
        terms.emplace_back( SEARCH_TERM( token, 4 ) );

    // Also include keywords as one long string, just in case
    terms.emplace_back( SEARCH_TERM( GetKeywords(), 1 ) );
    terms.emplace_back( SEARCH_TERM( GetDesc(), 1 ) );

    return terms;
}


bool FOOTPRINT_INFO::InLibrary( const QString& aLibrary ) const
{
    return aLibrary == m_nickname;
}


bool operator<( const FOOTPRINT_INFO& lhs, const FOOTPRINT_INFO& rhs )
{
    int retv = StrNumCmp( lhs.m_nickname, rhs.m_nickname, false );

    if( retv != 0 )
        return retv < 0;

    // Technically footprint names are not case sensitive because the file name is used
    // as the footprint name.  On windows this would be problematic because windows does
    // not support case sensitive file names by default.  This should not cause any issues
    // and allow for a future change to use the name defined in the footprint file.
    return StrNumCmp( lhs.m_fpname, rhs.m_fpname, false ) < 0;
}


void FOOTPRINT_LIST::DisplayErrors( QWidget* aWindow )
{
    // @todo: go to a more HTML !<table>! ? centric output, possibly with recommendations
    // for remedy of errors.  Add numeric error codes to PARSE_ERROR, and switch on them for
    // remedies, etc.  Full access is provided to everything in every exception!

    HTML_MESSAGE_BOX dlg( aWindow, _( "Load Error" ) );

    dlg.MessageSet( _( "Errors were encountered loading footprints:" ) );

    QString msg;

    while( std::unique_ptr<IO_ERROR> error = PopError() )
    {
        QString tmp = EscapeHTML( error->Problem() );

        // Preserve new lines in error messages so queued errors don't run together.
        tmp.replace( "\n", "<BR>" );
        msg += "<p>" + tmp + "</p>";
    }

    dlg.AddHTML_Text( msg );

    dlg.ShowModal();
}


static FOOTPRINT_LIST* get_instance_from_id( KIWAY& aKiway, int aId )
{
    void* ptr = nullptr;

    try
    {
        ptr = Kiface().IfaceOrAddress( aId );

        if( !ptr )
        {
            KIFACE* kiface = aKiway.KiFACE( KIWAY::FACE_PCB );
            ptr = kiface->IfaceOrAddress( aId );
        }

        return static_cast<FOOTPRINT_LIST*>( ptr );
    }
    catch( ... )
    {
        return nullptr;
    }
}


FOOTPRINT_LIST* FOOTPRINT_LIST::GetInstance( KIWAY& aKiway )
{
    FOOTPRINT_LIST* footprintInfo = get_instance_from_id( aKiway, KIFACE_FOOTPRINT_LIST );

    if( !footprintInfo )
        return nullptr;

    if( !footprintInfo->GetCount() )
        footprintInfo->ReadCacheFromFile( aKiway.Prj().GetProjectPath() + "fp-info-cache" );

    return footprintInfo;
}
