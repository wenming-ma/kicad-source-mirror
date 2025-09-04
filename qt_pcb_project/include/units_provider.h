#ifndef  UNITS_PROVIDER_H
#define  UNITS_PROVIDER_H

#include <eda_units.h>
#include <origin_transforms.h>
#include <core/minoptmax.h>
#include <optional>
#include <utility>
#include <QString>


class UNITS_PROVIDER
{
public:
    UNITS_PROVIDER( const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits ) :
            m_iuScale( aIuScale ),
            m_userUnits( aUnits )
    {}

    virtual ~UNITS_PROVIDER()
    {}

    EDA_UNITS GetUserUnits() const { return m_userUnits; }
    void SetUserUnits( EDA_UNITS aUnits ) { m_userUnits = aUnits; }

    virtual void GetUnitPair( EDA_UNITS& aPrimaryUnit, EDA_UNITS& aSecondaryUnits )
    {
        aPrimaryUnit    = GetUserUnits();
        aSecondaryUnits = EDA_UNIT_UTILS::IsImperialUnit( aPrimaryUnit ) ? EDA_UNITS::MM
                                                                         : EDA_UNITS::MILS;
    }

    const EDA_IU_SCALE& GetIuScale() const { return m_iuScale; }
    // No SetIuScale(); scale is invariant

    virtual ORIGIN_TRANSFORMS& GetOriginTransforms()
    {
        static ORIGIN_TRANSFORMS identityTransform;

        return identityTransform;
    }

    QString StringFromValue( double aValue, bool aAddUnitLabel = false,
                             EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE ) const
    {
        return EDA_UNIT_UTILS::UI::StringFromValue( GetIuScale(), GetUserUnits(), aValue,
                                                    aAddUnitLabel, aType );
    }

    QString StringFromOptionalValue( std::optional<int> aValue, bool aAddUnitLabel = false,
                                     EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE ) const
    {
        if( !aValue )
            return NullUiString;
        else
            return EDA_UNIT_UTILS::UI::StringFromValue( GetIuScale(), GetUserUnits(),
                                                        aValue.value(), aAddUnitLabel, aType );
    }

    QString StringFromValue( const EDA_ANGLE& aValue, bool aAddUnitLabel = false ) const
    {
        return EDA_UNIT_UTILS::UI::StringFromValue( unityScale, EDA_UNITS::DEGREES,
                                                    aValue.AsDegrees(), aAddUnitLabel,
                                                    EDA_DATA_TYPE::DISTANCE );
    }

    // A lower-precision version of StringFromValue().
    // Should ONLY be used for status text and messages. Not suitable for dialogs, files, etc.
    // where the loss of precision matters.
    QString MessageTextFromValue( double aValue, bool aAddUnitLabel = true,
                                  EDA_DATA_TYPE aType = EDA_DATA_TYPE::DISTANCE ) const
    {
        return EDA_UNIT_UTILS::UI::MessageTextFromValue( GetIuScale(), GetUserUnits(), aValue,
                                                         aAddUnitLabel, aType );
    }

    QString MessageTextFromValue( const EDA_ANGLE& aValue, bool aAddUnitLabel = true ) const
    {
        return EDA_UNIT_UTILS::UI::MessageTextFromValue( unityScale, EDA_UNITS::DEGREES,
                                                         aValue.AsDegrees(), aAddUnitLabel,
                                                         EDA_DATA_TYPE::DISTANCE );
    }

    QString MessageTextFromMinOptMax( const MINOPTMAX<int>& aValue ) const
    {
        return EDA_UNIT_UTILS::UI::MessageTextFromMinOptMax( GetIuScale(), GetUserUnits(), aValue );
    };

    // Converts aTextValue in aUnits to internal units used by the frame.
    // Warning: This utilizes the current locale and will break if decimal formats differ
    int ValueFromString( const QString& aTextValue,
                         EDA_DATA_TYPE   aType = EDA_DATA_TYPE::DISTANCE ) const
    {
        double value = EDA_UNIT_UTILS::UI::DoubleValueFromString( GetIuScale(), GetUserUnits(),
                                                                  aTextValue, aType );

        return KiROUND<double, int>( value );
    }

    // Converts aTextValue in aUnits to internal units used by the frame. Allows the return
    // of an empty optional if the string represents a null value (currently empty string)
    // Warning: This utilizes the current locale and will break if decimal formats differ
    std::optional<int>
    OptionalValueFromString( const QString& aTextValue,
                             EDA_DATA_TYPE   aType = EDA_DATA_TYPE::DISTANCE ) const
    {
        // Handle null (empty) values
        if( aTextValue == NullUiString )
            return {};

        double value = EDA_UNIT_UTILS::UI::DoubleValueFromString( GetIuScale(), GetUserUnits(),
                                                                  aTextValue, aType );

        return KiROUND<double, int>( value );
    }

    EDA_ANGLE AngleValueFromString( const QString& aTextValue ) const
    {
        double angle = EDA_UNIT_UTILS::UI::DoubleValueFromString( GetIuScale(), EDA_UNITS::DEGREES,
                                                                  aTextValue );

        return EDA_ANGLE( angle, DEGREES_T );
    }

    // The string that is used in the UI to represent a null value
    static inline const QString NullUiString = "";

private:
    const EDA_IU_SCALE& m_iuScale;
    EDA_UNITS           m_userUnits;
};

#endif  // UNITS_PROVIDER_H
