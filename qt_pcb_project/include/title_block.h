#ifndef TITLE_BLOCK_H
#define TITLE_BLOCK_H

#include <QString>
#include <QStringList>
#include <ki_exception.h>

class OUTPUTFORMATTER;
class PROJECT;

class KICOMMON_API TITLE_BLOCK
{
    // Texts are stored in QStringList.
    // TEXTS_IDX gives the index of known texts in this array
    enum TEXTS_IDX
    {
        TITLE_IDX = 0,
        DATE_IDX,
        REVISION_IDX,
        COMPANY_IDX,
        COMMENT1_IDX    // idx of the first comment: one can have more than 1 comment
    };

public:
    TITLE_BLOCK() {};
    virtual ~TITLE_BLOCK() {};      // a virtual dtor seems needed to build
                                    // python lib without warning

    void SetTitle( const QString& aTitle )
    {
        setTbText( TITLE_IDX, aTitle );
    }

    const QString& GetTitle() const
    {
        return getTbText( TITLE_IDX );
    }

    void SetDate( const QString& aDate )
    {
        setTbText( DATE_IDX, aDate );
    }

    const QString& GetDate() const
    {
        return getTbText( DATE_IDX );
    }

    void SetRevision( const QString& aRevision )
    {
        setTbText( REVISION_IDX, aRevision );
    }

    const QString& GetRevision() const
    {
        return getTbText( REVISION_IDX );
    }

    void SetCompany( const QString& aCompany )
    {
        setTbText( COMPANY_IDX, aCompany );
    }

    const QString& GetCompany() const
    {
        return getTbText( COMPANY_IDX );
    }

    void SetComment( int aIdx, const QString& aComment )
    {
        aIdx += COMMENT1_IDX;
        return setTbText( aIdx, aComment );
    }

    const QString& GetComment( int aIdx ) const
    {
        aIdx += COMMENT1_IDX;
        return getTbText( aIdx );
    }

    void Clear()
    {
        m_tbTexts.clear();
    }

    static void GetContextualTextVars( QStringList* aVars );
    bool TextVarResolver( QString* aToken, const PROJECT* aProject, int aFlags = 0 ) const;

    virtual void Format( OUTPUTFORMATTER* aFormatter ) const;

    static QString GetCurrentDate();

private:
    QStringList m_tbTexts;

    void setTbText( int aIdx, const QString& aText )
    {
        if( m_tbTexts.size() <= aIdx )
        {
            m_tbTexts.resize( aIdx + 1 );
        }

        m_tbTexts[aIdx] = aText;
    }

    const QString& getTbText( int aIdx ) const
    {
        static const QString m_emptytext;

        if( m_tbTexts.size() > aIdx )
            return m_tbTexts[aIdx];
        else
            return m_emptytext;
    }
};

#endif // TITLE_BLOCK_H
