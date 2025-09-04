// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_EDA_ANGLE_VARIANT_H
#define KICAD_EDA_ANGLE_VARIANT_H

#include <geometry/eda_angle.h>

#include <QVariant>

class EDA_ANGLE_VARIANT_DATA
{
public:
    EDA_ANGLE_VARIANT_DATA();

    EDA_ANGLE_VARIANT_DATA( double aAngleDegrees );

    EDA_ANGLE_VARIANT_DATA( const EDA_ANGLE& aAngle );

    bool Eq( EDA_ANGLE_VARIANT_DATA& aOther ) const;

    QString GetType() const { return QStringLiteral( "EDA_ANGLE" ); }

    bool Read( QString& aString );

    bool Write( QString& aString ) const;

    bool GetAsVariant( QVariant* aVariant ) const;

    static EDA_ANGLE_VARIANT_DATA* VariantDataFactory( const QVariant& aVariant );

    const EDA_ANGLE& Angle() { return m_angle; }

    void SetAngle( const EDA_ANGLE& aAngle ) { m_angle = aAngle;  }

protected:
    EDA_ANGLE m_angle;
};

#endif //KICAD_EDA_ANGLE_VARIANT_H
