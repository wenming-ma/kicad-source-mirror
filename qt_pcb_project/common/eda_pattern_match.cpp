
#include <eda_pattern_match.h>
#include <limits>
#include <QRegularExpression>
#include <QStringList>
#include <algorithm>

// Helper to make the code cleaner when we want this operation
#define CLAMPED_VAL_INT_MAX( x )                                                      \
    std::min( x, static_cast<size_t>( std::numeric_limits<int>::max() ) )


bool EDA_PATTERN_MATCH_SUBSTR::SetPattern( const QString& aPattern )
{
    m_pattern = aPattern;
    return true;
}


QString const& EDA_PATTERN_MATCH_SUBSTR::GetPattern() const
{
    return m_pattern;
}


EDA_PATTERN_MATCH::FIND_RESULT EDA_PATTERN_MATCH_SUBSTR::Find( const QString& aCandidate ) const
{
    int loc = aCandidate.indexOf( m_pattern );

    if( loc == -1 )
        return {};
    else
        return { loc, static_cast<int>( m_pattern.size() ) };
}




bool EDA_PATTERN_MATCH_REGEX::SetPattern( const QString& aPattern )
{
    if( aPattern.startsWith( "^" ) && aPattern.endsWith( "$" ) )
    {
        m_pattern = aPattern;
    }
    else if( aPattern.startsWith( "/" ) )
    {
        // Requiring a '/' on the end means they get no feedback while they type
        m_pattern = aPattern.mid( 1 );

        if( m_pattern.endsWith( "/" ) )
            m_pattern = m_pattern.left( m_pattern.length() - 1 );
    }
    else
    {
        // For now regular expressions must be explicit
        return false;
    }

    m_regex.setPattern( m_pattern );
    return m_regex.isValid();
}


bool EDA_PATTERN_MATCH_REGEX_ANCHORED::SetPattern( const QString& aPattern )
{
    QString pattern( aPattern );

    if( !pattern.startsWith( "^" ) )
        pattern = "^" + pattern;

    if( !pattern.endsWith( "$" ) )
        pattern +=  "$";

    return EDA_PATTERN_MATCH_REGEX::SetPattern( pattern );
}


QString const& EDA_PATTERN_MATCH_REGEX::GetPattern() const
{
    return m_pattern;
}


EDA_PATTERN_MATCH::FIND_RESULT EDA_PATTERN_MATCH_REGEX::Find( const QString& aCandidate ) const
{
    if( m_regex.isValid() )
    {
        QRegularExpressionMatch match = m_regex.match( aCandidate );
        if( match.hasMatch() )
        {
            int start = match.capturedStart( 0 );
            int len = match.capturedLength( 0 );

            return { static_cast<int>( CLAMPED_VAL_INT_MAX( start ) ),
                     static_cast<int>( CLAMPED_VAL_INT_MAX( len ) ) };
        }
        else
        {
            return {};
        }
    }
    else
    {
        int loc = aCandidate.indexOf( m_pattern );

        if( loc == -1 )
            return {};
        else
            return { loc, static_cast<int>( m_pattern.size() ) };
    }
}


bool EDA_PATTERN_MATCH_WILDCARD::SetPattern( const QString& aPattern )
{
    m_wildcard_pattern = aPattern;

    // Compile the wildcard string to a regular expression
    QString regex;
    regex.reserve( 2 * aPattern.length() );   // no need to keep resizing, we know the size roughly

    const QString to_replace = ".*+?^${}()|[]/\\";

    for( auto it = aPattern.begin(); it < aPattern.end(); ++it )
    {
        QChar c = *it;

        if( c == '?' )
        {
            regex += ".";
        }
        else if( c == '*' )
        {
            regex += ".*";
        }
        else if( to_replace.indexOf( c ) != -1 )
        {
            regex += "\\";
            regex += c;
        }
        else
        {
            regex += c;
        }
    }

    return EDA_PATTERN_MATCH_REGEX::SetPattern( "/" + regex + "/" );
}


QString const& EDA_PATTERN_MATCH_WILDCARD::GetPattern() const
{
    return m_wildcard_pattern;
}


EDA_PATTERN_MATCH::FIND_RESULT EDA_PATTERN_MATCH_WILDCARD::Find( const QString& aCandidate ) const
{
    return EDA_PATTERN_MATCH_REGEX::Find( aCandidate );
}


bool EDA_PATTERN_MATCH_WILDCARD_ANCHORED::SetPattern( const QString& aPattern )
{
    m_wildcard_pattern = aPattern;

    // Compile the wildcard string to a regular expression
    QString regex;
    regex.reserve( 2 * aPattern.length() );   // no need to keep resizing, we know the size roughly

    const QString to_replace = ".*+?^${}()|[]/\\";

    regex +=  "^";

    for( auto it = aPattern.begin(); it < aPattern.end(); ++it )
    {
        QChar c = *it;

        if( c == '?' )
        {
            regex += ".";
        }
        else if( c == '*' )
        {
            regex += ".*";
        }
        else if( to_replace.indexOf( c ) != -1 )
        {
            regex += "\\";
            regex += c;
        }
        else
        {
            regex += c;
        }
    }

    regex += "$";

    return EDA_PATTERN_MATCH_REGEX::SetPattern( regex );
}


bool EDA_PATTERN_MATCH_RELATIONAL::SetPattern( const QString& aPattern )
{
    QRegularExpression regex_search( R"(^(\w+)(<|<=|=|>=|>)([-+]?[\d.]*)(\w*)$)" );

    QRegularExpressionMatch match = regex_search.match( aPattern );
    bool matches = match.hasMatch();

    if( !matches || match.lastCapturedIndex() < 4 )
        return false;

    m_pattern = aPattern;
    QString key = match.captured( 1 );
    QString rel = match.captured( 2 );
    QString val = match.captured( 3 );
    QString unit = match.captured( 4 );

    m_key = key.toLower();

    if( rel == "<" )
        m_relation = LT;
    else if( rel == "<=" )
        m_relation = LE;
    else if( rel == "=" )
        m_relation = EQ;
    else if( rel == ">=" )
        m_relation = GE;
    else if( rel == ">" )
        m_relation = GT;
    else
        return false;

    if( val == "" )
    {
        // Matching on empty values keeps the match list from going empty when the user
        // types the relational operator character, which helps prevent confusion.
        m_relation = ANY;
    }
    else
    {
        bool ok;
        m_value = val.toDouble( &ok );
        if( !ok )
            return false;
    }

    auto unit_it = m_units.find( unit.toLower() );

    if( unit_it != m_units.end() )
        m_value *= unit_it->second;
    else
        return false;

    m_pattern = aPattern;

    return true;
}


QString const& EDA_PATTERN_MATCH_RELATIONAL::GetPattern() const
{
    return m_pattern;
}


EDA_PATTERN_MATCH::FIND_RESULT EDA_PATTERN_MATCH_RELATIONAL::Find( const QString& aCandidate ) const
{
    QStringList tokens = aCandidate.split( QRegularExpression( "\\s+" ), Qt::SkipEmptyParts );
    size_t lastpos = 0;

    for( const QString& token : tokens )
    {
        int found_delta = FindOne( token );

        if( found_delta != EDA_PATTERN_NOT_FOUND )
        {
            size_t found = (size_t) found_delta + lastpos;
            return { static_cast<int>( CLAMPED_VAL_INT_MAX( found ) ), 0 };
        }

        lastpos += token.length() + 1; // +1 for whitespace
    }

    return {};
}


int EDA_PATTERN_MATCH_RELATIONAL::FindOne( const QString& aCandidate ) const
{
    QRegularExpression regex_description( R"((\w+)[=:]([-+]?[\d.]+)(\w*))" );

    QRegularExpressionMatch match = regex_description.match( aCandidate );
    bool matches = match.hasMatch();

    if( !matches )
        return EDA_PATTERN_NOT_FOUND;

    int start = match.capturedStart( 0 );
    QString key = match.captured( 1 );
    QString val = match.captured( 2 );
    QString unit = match.captured( 3 );

    int istart = static_cast<int>( CLAMPED_VAL_INT_MAX( start ) );

    if( key.toLower() != m_key )
        return EDA_PATTERN_NOT_FOUND;

    bool ok;
    double val_parsed = val.toDouble( &ok );

    if( !ok )
        return EDA_PATTERN_NOT_FOUND;

    auto unit_it = m_units.find( unit.toLower() );

    if( unit_it != m_units.end() )
        val_parsed *= unit_it->second;

    switch( m_relation )
    {
    case LT:  return val_parsed <  m_value ? istart : EDA_PATTERN_NOT_FOUND;
    case LE:  return val_parsed <= m_value ? istart : EDA_PATTERN_NOT_FOUND;
    case EQ:  return val_parsed == m_value ? istart : EDA_PATTERN_NOT_FOUND;
    case GE:  return val_parsed >= m_value ? istart : EDA_PATTERN_NOT_FOUND;
    case GT:  return val_parsed >  m_value ? istart : EDA_PATTERN_NOT_FOUND;
    case ANY: return istart;
    default:  return EDA_PATTERN_NOT_FOUND;
    }
}


const QHash<QString, double> EDA_PATTERN_MATCH_RELATIONAL::m_units = {
    { "p",  1e-12 },
    { "n",  1e-9 },
    { "u",  1e-6 },
    { "m",  1e-3 },
    { "",   1. },
    { "k",  1e3 },
    { "meg", 1e6 },
    { "g",  1e9 },
    { "t",  1e12 },
    { "ki", 1024. },
    { "mi", 1048576. },
    { "gi", 1073741824. },
    { "ti", 1099511627776. } };


EDA_COMBINED_MATCHER::EDA_COMBINED_MATCHER( const QString& aPattern,
                                            COMBINED_MATCHER_CONTEXT aContext ) :
        m_pattern( aPattern )
{
    switch( aContext )
    {
    case CTX_LIBITEM:
        // Whatever syntax users prefer, it shall be matched.
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_REGEX>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_WILDCARD>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_RELATIONAL>() );

        // If any of the above matchers couldn't be created because the pattern
        // syntax does not match, the substring will try its best.
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_SUBSTR>() );
        break;

    case CTX_NET:
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_REGEX>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_WILDCARD>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_SUBSTR>() );
        break;

    case CTX_NETCLASS:
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_REGEX_ANCHORED>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_WILDCARD_ANCHORED>() );
        break;

    case CTX_SIGNAL:
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_REGEX>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_WILDCARD>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_SUBSTR>() );
        break;

    case CTX_SEARCH:
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_REGEX>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_WILDCARD>() );
        AddMatcher( aPattern, std::make_unique<EDA_PATTERN_MATCH_SUBSTR>() );
        break;
    }
}


bool EDA_COMBINED_MATCHER::Find( const QString& aTerm, int& aMatchersTriggered, int& aPosition )
{
    aPosition = EDA_PATTERN_NOT_FOUND;
    aMatchersTriggered = 0;

    for( const std::unique_ptr<EDA_PATTERN_MATCH>& matcher : m_matchers )
    {
        EDA_PATTERN_MATCH::FIND_RESULT local_find = matcher->Find( aTerm );

        if( local_find )
        {
            aMatchersTriggered += 1;

            if( local_find.start < aPosition || aPosition == EDA_PATTERN_NOT_FOUND )
                aPosition = local_find.start;
        }
    }

    return aPosition != EDA_PATTERN_NOT_FOUND;
}


bool EDA_COMBINED_MATCHER::Find( const QString& aTerm )
{
    for( const std::unique_ptr<EDA_PATTERN_MATCH>& matcher : m_matchers )
    {
        if( matcher->Find( aTerm ).start >= 0 )
            return true;
    }

    return false;
}


bool EDA_COMBINED_MATCHER::StartsWith( const QString& aTerm )
{
    for( const std::unique_ptr<EDA_PATTERN_MATCH>& matcher : m_matchers )
    {
        if( matcher->Find( aTerm ).start == 0 )
            return true;
    }

    return false;
}


int EDA_COMBINED_MATCHER::ScoreTerms( QVector<SEARCH_TERM>& aWeightedTerms )
{
    int score = 0;

    for( SEARCH_TERM& term : aWeightedTerms )
    {
        if( !term.Normalized )
        {
            term.Text = term.Text.toLower().trimmed();

            // Don't cause KiCad to hang if someone accidentally pastes the PCB or schematic
            // into the search box.
            if( term.Text.length() > 1000 )
                term.Text = term.Text.left( 1000 );

            term.Normalized = true;
        }

        int found_pos = EDA_PATTERN_NOT_FOUND;
        int matchers_fired = 0;

        if( GetPattern() == term.Text )
        {
            score += 8 * term.Score;
        }
        else if( Find( term.Text, matchers_fired, found_pos ) )
        {
            if( found_pos == 0 )
                score += 2 * term.Score;
            else
                score += term.Score;
        }
    }

    return score;
}


QString const& EDA_COMBINED_MATCHER::GetPattern() const
{
    return m_pattern;
}


void EDA_COMBINED_MATCHER::AddMatcher( const QString &aPattern,
                                       std::unique_ptr<EDA_PATTERN_MATCH> aMatcher )
{
    if ( aMatcher->SetPattern( aPattern ) )
        m_matchers.push_back( std::move( aMatcher ) );
}
