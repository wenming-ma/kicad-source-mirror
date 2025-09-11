
#ifndef DRC_RULE_PARSER_H
#define DRC_RULE_PARSER_H

#include <core/typeinfo.h>
#include <netclass.h>
#include <layer_ids.h>
#include <drc/drc_rule.h>
#include <drc_rules_lexer.h>
#include <QString>


class BOARD_ITEM;


#define DRC_RULE_FILE_VERSION      20200610


class DRC_RULES_PARSER : public DRC_RULES_LEXER
{
public:
    DRC_RULES_PARSER( const QString& aSource, const QString& aSourceDescr );
    DRC_RULES_PARSER( FILE* aFile, const QString& aFilename );

    void Parse( std::vector<std::shared_ptr<DRC_RULE>>& aRules, REPORTER* aReporter );

private:
    std::shared_ptr<DRC_RULE> parseDRC_RULE();

    void parseConstraint( DRC_RULE* aRule );
    void parseValueWithUnits( const QString& aExpr, int& aResult, bool aUnitless = false );
    LSET parseLayer( QString* aSource );
    SEVERITY parseSeverity();
    void parseUnknown();

    void reportError( const QString& aMessage );
    void reportDeprecation( const QString& oldToken, const QString newToken );

private:
    int       m_requiredVersion;
    bool      m_tooRecent;
    REPORTER* m_reporter;
};

#endif      // DRC_RULE_PARSER_H
