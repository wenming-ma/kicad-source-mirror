// QT_TRANSFORMATION_COMPLETED

#ifndef EDA_ITEM_SEARCH_H
#define EDA_ITEM_SEARCH_H

#include <QString>
#include <QRegularExpression>

enum class EDA_SEARCH_MATCH_MODE
{
    PLAIN,
    WHOLEWORD,
    WILDCARD,
    REGEX,
    PERMISSIVE
};

struct EDA_SEARCH_DATA
{
    QString         findString;
    QString         replaceString;

    mutable QRegularExpression  regex;
    mutable QString regex_string;

    bool             searchAndReplace;
    bool             searchAllFields;

    bool                  matchCase;
    bool                  markersOnly;
    EDA_SEARCH_MATCH_MODE matchMode;

    EDA_SEARCH_DATA() :
            findString(),
            replaceString(),
            searchAndReplace( false ),
            searchAllFields( false ),
            matchCase( false ),
            markersOnly( false ),
            matchMode( EDA_SEARCH_MATCH_MODE::PLAIN )
    {
    }

    EDA_SEARCH_DATA( const EDA_SEARCH_DATA& other ) :
            findString( other.findString ),
            replaceString( other.replaceString ),
            regex_string( other.regex_string ),
            searchAndReplace( other.searchAndReplace ),
            matchCase( other.matchCase ),
            markersOnly( other.markersOnly ),
            matchMode( other.matchMode )
    {
        if( matchMode == EDA_SEARCH_MATCH_MODE::REGEX )
        {
            QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
            if( !matchCase )
                options |= QRegularExpression::CaseInsensitiveOption;
            regex.setPattern( findString );
            regex.setPatternOptions( options );
        }
    }

    virtual ~EDA_SEARCH_DATA() {}
};

struct SCH_SEARCH_DATA : public EDA_SEARCH_DATA
{
    bool searchAllPins;
    bool searchCurrentSheetOnly;
    bool searchSelectedOnly;
    bool searchNetNames;

    bool replaceReferences;

    SCH_SEARCH_DATA() :
            EDA_SEARCH_DATA(),
            searchAllPins( false ),
            searchCurrentSheetOnly( false ),
            searchSelectedOnly( false ),
            searchNetNames( false ),
            replaceReferences( false )
    {
    }
};

#endif
