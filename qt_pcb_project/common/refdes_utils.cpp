
#include <refdes_utils.h>

#include <string_utils.h>

#include <algorithm>
#include <cctype>
#include <QRegularExpression>


namespace UTIL
{

QString GetRefDesPrefix( const QString& aRefDes )
{
    // find the first non-digit, non-question-mark character from the back
    auto res = std::find_if( aRefDes.rbegin(), aRefDes.rend(),
            []( QChar aChr )
            {
                return aChr != '?' && !std::isdigit( aChr.unicode() );
            } );

    return QString( aRefDes.begin(), res.base() );
}


QString GetRefDesUnannotated( const QString& aSource )
{
   return UTIL::GetRefDesPrefix( aSource ) + "?";
}


int GetRefDesNumber( const QString& aRefDes )
{
    int retval = -1; // negative to indicate not found
    int firstnum = aRefDes.indexOf( QRegularExpression("[0-9]") );

    if( firstnum != -1 )
    {
        QString candidateValue = aRefDes.mid( firstnum );
        bool    ok;
        long    result = candidateValue.toLong( &ok );

        if( !ok )
            retval = -1;
        else
            retval = static_cast<int>( result );
    }

    return retval;
}

} // namespace UTIL
