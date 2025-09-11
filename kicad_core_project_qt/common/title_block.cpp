
#include <richio.h>
#include <common.h>
#include <title_block.h>
#include <core/kicad_algo.h>
#include <QDateTime>


void TITLE_BLOCK::Format( OUTPUTFORMATTER* aFormatter ) const
{
    // Don't write the title block information if there is nothing to write.
    bool isempty = true;
    for( unsigned idx = 0; idx < m_tbTexts.size(); idx++ )
    {
        if( ! m_tbTexts[idx].isEmpty() )
        {
            isempty = false;
            break;
        }
    }

    if( !isempty  )
    {
        aFormatter->Print( "(title_block" );

        if( !GetTitle().isEmpty() )
            aFormatter->Print( "(title %s)", aFormatter->Quotew( GetTitle() ).c_str() );

        if( !GetDate().isEmpty() )
            aFormatter->Print( "(date %s)", aFormatter->Quotew( GetDate() ).c_str() );

        if( !GetRevision().isEmpty() )
            aFormatter->Print( "(rev %s)", aFormatter->Quotew( GetRevision() ).c_str() );

        if( !GetCompany().isEmpty() )
            aFormatter->Print( "(company %s)", aFormatter->Quotew( GetCompany() ).c_str() );

        for( int ii = 0; ii < 9; ii++ )
        {
            if( !GetComment(ii).isEmpty() )
            {
                aFormatter->Print( "(comment %d %s)",
                                   ii+1,
                                   aFormatter->Quotew( GetComment(ii) ).c_str() );
            }
        }

        aFormatter->Print( ")" );
    }
}


void TITLE_BLOCK::GetContextualTextVars( QStringList* aVars )
{
    if( !alg::contains( *aVars, QString( "ISSUE_DATE" ) ) )
    {
        aVars->push_back( QString( "ISSUE_DATE" ) );
        aVars->push_back( QString( "CURRENT_DATE" ) );
        aVars->push_back( QString( "REVISION" ) );
        aVars->push_back( QString( "TITLE" ) );
        aVars->push_back( QString( "COMPANY" ) );
        aVars->push_back( QString( "COMMENT1" ) );
        aVars->push_back( QString( "COMMENT2" ) );
        aVars->push_back( QString( "COMMENT3" ) );
        aVars->push_back( QString( "COMMENT4" ) );
        aVars->push_back( QString( "COMMENT5" ) );
        aVars->push_back( QString( "COMMENT6" ) );
        aVars->push_back( QString( "COMMENT7" ) );
        aVars->push_back( QString( "COMMENT8" ) );
        aVars->push_back( QString( "COMMENT9" ) );
    }
}


QString TITLE_BLOCK::GetCurrentDate()
{
    return QDateTime::currentDateTime().toString( Qt::ISODate );
};


bool TITLE_BLOCK::TextVarResolver( QString* aToken, const PROJECT* aProject, int aFlags ) const
{
    bool tokenUpdated = false;
    QString originalToken = *aToken;

    if( aToken->compare( QString( "ISSUE_DATE" ), Qt::CaseInsensitive ) == 0 )
    {
        *aToken = GetDate();
        tokenUpdated = true;
    }
    else if( aToken->compare( QString( "CURRENT_DATE" ), Qt::CaseInsensitive ) == 0 )
    {
        *aToken = GetCurrentDate();
        tokenUpdated = true;
    }
    else if( aToken->compare( QString( "REVISION" ), Qt::CaseInsensitive ) == 0 )
    {
        *aToken = GetRevision();
        tokenUpdated = true;
    }
    else if( aToken->compare( QString( "TITLE" ), Qt::CaseInsensitive ) == 0 )
    {
        *aToken = GetTitle();
        tokenUpdated = true;
    }
    else if( aToken->compare( QString( "COMPANY" ), Qt::CaseInsensitive ) == 0 )
    {
        *aToken = GetCompany();
        tokenUpdated = true;
    }
    else if( aToken->left( aToken->length() - 1 ).compare( QString( "COMMENT" ), Qt::CaseInsensitive ) == 0 )
    {
        QChar c = aToken->at( aToken->length() - 1 );

        switch( c.toLatin1() )
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            *aToken = GetComment( c.toLatin1() - '1' );
            tokenUpdated = true;
        }
    }

    if( tokenUpdated )
    {
        if( aToken->compare( QString( "CURRENT_DATE" ), Qt::CaseInsensitive ) == 0 )
            *aToken = GetCurrentDate();
        else if( aProject )
            *aToken = ExpandTextVars( *aToken, aProject, aFlags );

        // This is the default fallback, so don't claim we resolved it
        if( *aToken == QString( "${" ) + originalToken + QString( "}" ) )
            return false;

       return true;
    }

    return false;
}


