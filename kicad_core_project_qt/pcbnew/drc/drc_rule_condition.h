
#ifndef DRC_RULE_CONDITION_H
#define DRC_RULE_CONDITION_H

#include <core/typeinfo.h>
#include <layer_ids.h>
#include <QString>

class BOARD_ITEM;
class PCBEXPR_UCODE;
class REPORTER;


class DRC_RULE_CONDITION
{
public:
    DRC_RULE_CONDITION( const QString& aExpression = "" );
    ~DRC_RULE_CONDITION();

    bool EvaluateFor( const BOARD_ITEM* aItemA, const BOARD_ITEM* aItemB, int aConstraint,
                      PCB_LAYER_ID aLayer, REPORTER* aReporter = nullptr );

    bool Compile( REPORTER* aReporter, int aSourceLine = 0, int aSourceOffset = 0 );

    void SetExpression( const QString& aExpression ) { m_expression = aExpression; }
    QString GetExpression() const { return m_expression; }

private:
    QString                        m_expression;
    std::unique_ptr<PCBEXPR_UCODE> m_ucode;
};


#endif // DRC_RULE_CONDITION_H
