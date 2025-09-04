#include <base_screen.h>
#include <eda_item.h>
#include <trace_helpers.h>

QString BASE_SCREEN::m_DrawingSheetFileName;


BASE_SCREEN::BASE_SCREEN( EDA_ITEM* aParent, KICAD_T aType ) :
    EDA_ITEM( aParent, aType )
{
    m_virtualPageNumber = 1;
    m_pageCount         = 1;
    m_Center            = true;
    m_flagModified      = false;
}


void BASE_SCREEN::InitDataPoints( const VECTOR2I& aPageSizeIU )
{
    if( m_Center )
    {
        m_DrawOrg.x = -aPageSizeIU.x / 2;
        m_DrawOrg.y = -aPageSizeIU.y / 2;
    }
    else
    {
        m_DrawOrg.x = 0;
        m_DrawOrg.y = 0;
    }

    m_LocalOrigin = { 0, 0 };
}


void BASE_SCREEN::SetPageCount( int aPageCount )
{
    if( aPageCount > 0 )
        m_pageCount = aPageCount;
}


const QString& BASE_SCREEN::GetPageNumber() const
{
    static QString pageNumber;

    if( m_pageNumber.isEmpty() )
        pageNumber = QString::number( m_virtualPageNumber );
    else
        pageNumber = m_pageNumber;

    return pageNumber;
}


#if defined(DEBUG)

void BASE_SCREEN::Show( int nestLevel, std::ostream& os ) const
{
    NestedSpace( nestLevel, os ) << '<' << GetClass().toLower().toLocal8Bit().data() << ">\n";
    NestedSpace( nestLevel, os ) << "</" << GetClass().toLower().toLocal8Bit().data() << ">\n";
}

#endif
