// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_COLOR4D_VARIANT_H
#define KICAD_COLOR4D_VARIANT_H

#include <gal/color4d.h>
#include <QVariant>
#include <QString>

class COLOR4D_VARIANT_DATA
{
public:
    COLOR4D_VARIANT_DATA();

    COLOR4D_VARIANT_DATA( const QString& aColorStr );

    COLOR4D_VARIANT_DATA( const KIGFX::COLOR4D& aColor );

    bool Eq( const COLOR4D_VARIANT_DATA& aOther ) const;

    QString GetType() const { return QString( "COLOR4D" ); }

    bool Read( QString& aString );

    bool Write( QString& aString ) const;

    static QVariant toQVariant( const KIGFX::COLOR4D& aColor );
    
    static KIGFX::COLOR4D fromQVariant( const QVariant& aVariant );

    const KIGFX::COLOR4D& Color() { return m_color; }

    void SetColor( const KIGFX::COLOR4D& aColor ) { m_color = aColor; }

protected:
    KIGFX::COLOR4D m_color;
};

#endif //KICAD_COLOR4D_VARIANT_H
