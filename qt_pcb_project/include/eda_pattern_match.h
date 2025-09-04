

#ifndef EDA_PATTERN_MATCH_H
#define EDA_PATTERN_MATCH_H

#include <kicommon.h>
#include <QVector>
#include <QHash>
#include <memory>
#include <QString>
#include <QRegularExpression>

static const int EDA_PATTERN_NOT_FOUND = -1;

/**
 * A structure for storing weighted search terms.
 *
 * @note An exact match is scored at 8 * Score while a match at the start of the text is scored
 * at 2 * Score.
 */
struct KICOMMON_API SEARCH_TERM
{
    SEARCH_TERM( const QString& aText, int aScore ) :
            Text( aText ),
            Score( aScore ),
            Normalized( false )
    {}

    QString Text;
    int     Score;
    bool    Normalized;
};


/**
 * Interface for a pattern matcher for which there are several implementations.
 */
class KICOMMON_API EDA_PATTERN_MATCH
{
public:
    struct FIND_RESULT
    {
        int start  = EDA_PATTERN_NOT_FOUND;
        int length = 0;

        bool valid() const
        {
            return start != EDA_PATTERN_NOT_FOUND;
        }

        explicit operator bool() const
        {
            return valid();
        }
    };

    virtual ~EDA_PATTERN_MATCH() {}

    /**
     * Set the pattern against which candidates will be matched.
     *
     * @return false if the pattern not be processed.
     */
    virtual bool SetPattern( const QString& aPattern ) = 0;

    /**
     * Return the pattern passed to SetPattern().
     */
    virtual QString const& GetPattern() const = 0;

    /**
     * Return the location and possibly length of a match if a given candidate
     * string matches the set pattern.
     *
     * Otherwise, return an invalid #FIND_RESULT.
     */
    virtual FIND_RESULT Find( const QString& aCandidate ) const = 0;
};


/**
 * Match simple substring.
 */
class KICOMMON_API EDA_PATTERN_MATCH_SUBSTR : public EDA_PATTERN_MATCH
{
public:

    virtual bool SetPattern( const QString& aPattern ) override;
    virtual QString const& GetPattern() const override;
    virtual FIND_RESULT     Find( const QString& aCandidate ) const override;

protected:
    QString m_pattern;
};


/**
 * Match regular expression.
 */
class KICOMMON_API EDA_PATTERN_MATCH_REGEX : public EDA_PATTERN_MATCH
{
public:

    virtual bool SetPattern( const QString& aPattern ) override;
    virtual QString const& GetPattern() const override;
    virtual FIND_RESULT     Find( const QString& aCandidate ) const override;

protected:
    QString m_pattern;
    QRegularExpression m_regex;
};


class KICOMMON_API EDA_PATTERN_MATCH_REGEX_ANCHORED : public EDA_PATTERN_MATCH_REGEX
{
public:
    virtual bool SetPattern( const QString& aPattern ) override;
};


class KICOMMON_API EDA_PATTERN_MATCH_WILDCARD : public EDA_PATTERN_MATCH_REGEX
{
public:

    virtual bool SetPattern( const QString& aPattern ) override;
    virtual QString const& GetPattern() const override;
    virtual FIND_RESULT     Find( const QString& aCandidate ) const override;

protected:
    QString m_wildcard_pattern;
};


class KICOMMON_API EDA_PATTERN_MATCH_WILDCARD_ANCHORED : public EDA_PATTERN_MATCH_WILDCARD
{
public:
    virtual bool SetPattern( const QString& aPattern ) override;
};


/**
 * Relational match.
 *
 * Matches tokens of the format:
 *
 *      key:value       or      key=value
 *
 * with search patterns of the format:
 *
 *      key<value, key<=value, key=value, key>=value, key>value
 *
 * by parsing the value numerically and comparing.
 */
class KICOMMON_API EDA_PATTERN_MATCH_RELATIONAL : public EDA_PATTERN_MATCH
{
public:
    virtual bool SetPattern( const QString& aPattern ) override;
    virtual QString const& GetPattern() const override;
    virtual FIND_RESULT     Find( const QString& aCandidate ) const override;
    int FindOne( const QString& aCandidate ) const;

protected:

    enum RELATION { LT, LE, EQ, GE, GT, ANY };

    QString m_pattern;
    QString m_key;
    RELATION m_relation;
    double   m_value;

    static const QHash<QString, double> m_units;
};


enum COMBINED_MATCHER_CONTEXT
{
    CTX_LIBITEM,
    CTX_NET,
    CTX_NETCLASS,
    CTX_SIGNAL,
    CTX_SEARCH
};


class KICOMMON_API EDA_COMBINED_MATCHER
{
public:
    EDA_COMBINED_MATCHER( const QString& aPattern, COMBINED_MATCHER_CONTEXT aContext );

    /**
     * Deleted copy or else we have to implement copy constructors for all EDA_PATTERN_MATCH classes
     * due to this class' m_matchers member being copied.
     */
    EDA_COMBINED_MATCHER( EDA_COMBINED_MATCHER const& ) = delete;

    /**
     * Deleted copy or else we have to implement copy constructors for all EDA_PATTERN_MATCH classes
     * due to this class' m_matchers member being copied
     */
    EDA_COMBINED_MATCHER& operator=( EDA_COMBINED_MATCHER const& ) = delete;

    /**
     * Look in all existing matchers, return the earliest match of any of the existing.
     *
     * @param aTerm                 term to look for.
     * @param aMatchersTriggered    out: number of matcher that found the term.
     * @param aPostion              out: where the term was found, or #EDA_PATTERN_NOT_FOUND.
     *
     * @return true if any matchers found the term
     */
    bool Find( const QString& aTerm, int& aMatchersTriggered, int& aPosition );

    bool Find( const QString& aTerm );

    bool StartsWith( const QString& aTerm );

    const QString& GetPattern() const;

    int ScoreTerms( QVector<SEARCH_TERM>& aWeightedTerms );

private:
    void AddMatcher( const QString& aPattern, std::unique_ptr<EDA_PATTERN_MATCH> aMatcher );

    QVector<std::unique_ptr<EDA_PATTERN_MATCH>> m_matchers;
    QString m_pattern;
};

#endif  // EDA_PATTERN_MATCH_H
