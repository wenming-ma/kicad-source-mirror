// QT_TRANSFORMATION_COMPLETED

#ifndef SELECTION_CONDITIONS_H_
#define SELECTION_CONDITIONS_H_

#include <functional>
#include <core/typeinfo.h>
#include <vector>

class SELECTION;

typedef std::function<bool (const SELECTION&)> SELECTION_CONDITION;

SELECTION_CONDITION operator||( const SELECTION_CONDITION& aConditionA,
                                const SELECTION_CONDITION& aConditionB );

SELECTION_CONDITION operator&&( const SELECTION_CONDITION& aConditionA,
                                const SELECTION_CONDITION& aConditionB );

SELECTION_CONDITION operator!( const SELECTION_CONDITION& aCondition );


typedef bool ( &SELECTION_BOOL )( const SELECTION& );

SELECTION_CONDITION operator||( const SELECTION_CONDITION& aConditionA,
                                SELECTION_BOOL aConditionB );

SELECTION_CONDITION operator||( SELECTION_BOOL aConditionA,
                                const SELECTION_CONDITION& aConditionB );

SELECTION_CONDITION operator&&( const SELECTION_CONDITION& aConditionA,
                                SELECTION_BOOL aConditionB );

SELECTION_CONDITION operator&&( SELECTION_BOOL aConditionA,
                                const SELECTION_CONDITION& aConditionB );

class SELECTION_CONDITIONS
{
public:
    static bool ShowAlways( const SELECTION& aSelection ) { return true; }

    static bool ShowNever( const SELECTION& aSelection ) { return false; }

    static bool NotEmpty( const SELECTION& aSelection );

    static bool Empty( const SELECTION& aSelection );

    static bool Idle( const SELECTION& aSelection );

    static bool IdleSelection( const SELECTION& aSelection );

    static SELECTION_CONDITION HasType( KICAD_T aType );

    static SELECTION_CONDITION HasTypes( std::vector<KICAD_T> aTypes );

    static SELECTION_CONDITION OnlyTypes( std::vector<KICAD_T> aTypes );

    static SELECTION_CONDITION Count( int aNumber );

    static SELECTION_CONDITION MoreThan( int aNumber );

    static SELECTION_CONDITION LessThan( int aNumber );

private:
    static bool hasTypeFunc( const SELECTION& aSelection, KICAD_T aType );

    static bool hasTypesFunc( const SELECTION& aSelection, std::vector<KICAD_T> aTypes );

    static bool onlyTypesFunc( const SELECTION& aSelection, std::vector<KICAD_T> aTypes );

    static bool countFunc( const SELECTION& aSelection, int aNumber );

    static bool moreThanFunc( const SELECTION& aSelection, int aNumber );

    static bool lessThanFunc( const SELECTION& aSelection, int aNumber );

    static bool orFunc( const SELECTION_CONDITION& aConditionA,
                        const SELECTION_CONDITION& aConditionB, const SELECTION& aSelection )
    {
        return aConditionA( aSelection ) || aConditionB( aSelection );
    }

    static bool andFunc( const SELECTION_CONDITION& aConditionA,
                         const SELECTION_CONDITION& aConditionB, const SELECTION& aSelection )
    {
        return aConditionA( aSelection ) && aConditionB( aSelection );
    }

    static bool notFunc( const SELECTION_CONDITION& aCondition, const SELECTION& aSelection )
    {
        return !aCondition( aSelection );
    }

    static bool orBoolFunc( const SELECTION_CONDITION& aConditionA,
                            SELECTION_BOOL& aConditionB, const SELECTION& aSelection )
    {
        return aConditionA( aSelection ) || aConditionB( aSelection );
    }

    static bool andBoolFunc( const SELECTION_CONDITION& aConditionA,
                             SELECTION_BOOL& aConditionB, const SELECTION& aSelection )
    {
        return aConditionA( aSelection ) && aConditionB( aSelection );
    }

    friend SELECTION_CONDITION operator||( const SELECTION_CONDITION& aConditionA,
                                           const SELECTION_CONDITION& aConditionB );

    friend SELECTION_CONDITION operator&&( const SELECTION_CONDITION& aConditionA,
                                           const SELECTION_CONDITION& aConditionB );

    friend SELECTION_CONDITION operator!( const SELECTION_CONDITION& aCondition );

    friend SELECTION_CONDITION operator||( const SELECTION_CONDITION& aConditionA,
                                           SELECTION_BOOL aConditionB );

    friend SELECTION_CONDITION operator&&( const SELECTION_CONDITION& aConditionA,
                                           SELECTION_BOOL aConditionB );
};

#endif /* SELECTION_CONDITIONS_H_ */
