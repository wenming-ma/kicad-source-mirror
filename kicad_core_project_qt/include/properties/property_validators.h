// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_PROPERTY_VALIDATORS_H
#define KICAD_PROPERTY_VALIDATORS_H

#include <QString>
#include <QVariant>
#include <properties/property_validator.h>
#include <units_provider.h>



template<typename T>
class VALIDATION_ERROR_TOO_LARGE : public VALIDATION_ERROR
{
public:
    T Actual;
    T Maximum;
    EDA_DATA_TYPE DataType;

    VALIDATION_ERROR_TOO_LARGE( T aActual, T aMaximum,
                                EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE ) :
            Actual( aActual ),
            Maximum( aMaximum ),
            DataType( aType )
    {}

    QString Format( UNITS_PROVIDER* aUnits ) const override
    {
        bool addUnit = DataType != EDA_DATA_TYPE::UNITLESS;
        return QString( "Value must be less than or equal to %1" )
                .arg( aUnits->StringFromValue( Maximum, addUnit ) );
    }
};


template<typename T>
class VALIDATION_ERROR_TOO_SMALL : public VALIDATION_ERROR
{
public:
    T Actual;
    T Minimum;
    EDA_DATA_TYPE DataType;

    VALIDATION_ERROR_TOO_SMALL( T aActual, T aMinimum,
                                EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE ) :
            Actual( aActual ),
            Minimum( aMinimum ),
            DataType( aType )
    {}

    QString Format( UNITS_PROVIDER* aUnits ) const override
    {
        bool addUnit = DataType != EDA_DATA_TYPE::UNITLESS;
        return QString( "Value must be greater than or equal to %1" )
                .arg( aUnits->StringFromValue( Minimum, addUnit ) );
    }
};


class VALIDATION_ERROR_MSG : public VALIDATION_ERROR
{
public:
    QString Message;

    VALIDATION_ERROR_MSG( const QString& aMessage ) : Message( aMessage ) {}

    QString Format( UNITS_PROVIDER* aUnits ) const override
    {
        return Message;
    }
};


class PROPERTY_VALIDATORS
{
public:

    template<int Min, int Max>
    static VALIDATOR_RESULT RangeIntValidator( const QVariant&& aValue, EDA_ITEM* aItem )
    {
        Q_ASSERT_X( aValue.type() == QVariant::Int || aValue.canConvert<std::optional<int>>(),
                    "RangeIntValidator", "Expecting int-containing value" );

        int val = 0;

        if( aValue.type() == QVariant::Int )
        {
            val = aValue.toInt();
        }
        else if( aValue.canConvert<std::optional<int>>() )
        {
            auto optVal = aValue.value<std::optional<int>>();
            if( optVal.has_value() )
                val = optVal.value();
            else
                return std::nullopt;     // no value for a std::optional is always valid
        }

        if( val > Max )
            return std::make_unique<VALIDATION_ERROR_TOO_LARGE<int>>( val, Max );
        else if( val < Min )
            return std::make_unique<VALIDATION_ERROR_TOO_SMALL<int>>( val, Min );

        return std::nullopt;
    }

    static VALIDATOR_RESULT PositiveIntValidator( const QVariant&& aValue, EDA_ITEM* aItem )
    {
        Q_ASSERT_X( aValue.type() == QVariant::Int || aValue.canConvert<std::optional<int>>(),
                    "PositiveIntValidator", "Expecting int-containing value" );

        int val = 0;

        if( aValue.type() == QVariant::Int )
        {
            val = aValue.toInt();
        }
        else if( aValue.canConvert<std::optional<int>>() )
        {
            auto optVal = aValue.value<std::optional<int>>();
            if( optVal.has_value() )
                val = optVal.value();
            else
                return std::nullopt;     // no value for a std::optional is always valid
        }

        if( val < 0 )
            return std::make_unique<VALIDATION_ERROR_TOO_SMALL<int>>( val, 0 );

        return std::nullopt;
    }

    static VALIDATOR_RESULT PositiveRatioValidator( const QVariant&& aValue, EDA_ITEM* aItem )
    {
        Q_ASSERT_X( aValue.type() == QVariant::Double, "PositiveRatioValidator", 
                    "Expecting double-containing value" );

        double val = aValue.toDouble();

        if( val > 1.0 )
        {
            return std::make_unique<VALIDATION_ERROR_TOO_LARGE<double>>( val, 1.0,
                                                                         EDA_DATA_TYPE::UNITLESS );
        }
        else if( val < 0.0 )
        {
            return std::make_unique<VALIDATION_ERROR_TOO_SMALL<double>>( val, 0.0,
                                                                         EDA_DATA_TYPE::UNITLESS );
        }

        return std::nullopt;
    }
};

#endif //KICAD_PROPERTY_VALIDATORS_H
