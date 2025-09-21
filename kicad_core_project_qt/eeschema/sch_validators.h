

#ifndef _SCH_VALIDATORS_H_
#define _SCH_VALIDATORS_H_

#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <validators.h>

/*
 * A refinement of the NETNAME_VALIDATOR which also allows (and checks) bus definitions.
 */
class SCH_NETNAME_VALIDATOR : public NETNAME_VALIDATOR
{
public:
    SCH_NETNAME_VALIDATOR( QString* aVal = nullptr ) :
            NETNAME_VALIDATOR( aVal )
    { }

    SCH_NETNAME_VALIDATOR( bool aAllowSpaces ) :
            NETNAME_VALIDATOR( aAllowSpaces )
    { }

    SCH_NETNAME_VALIDATOR( const SCH_NETNAME_VALIDATOR& aValidator ) :
            NETNAME_VALIDATOR( aValidator )
    { }

    virtual QObject* Clone() const override { return new SCH_NETNAME_VALIDATOR( *this ); }

    QString IsValid( const QString& aVal ) const override;

private:
    static QRegularExpression m_busGroupRegex;
};

#endif // _SCH_VALIDATORS_H_
