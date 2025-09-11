#pragma once

#ifndef STACKUP_PREDEFINED_PRMS_H
#define STACKUP_PREDEFINED_PRMS_H


#include <QString>
#include <QStringList>
#include <QColor>

#include <layer_ids.h>
#include <i18n_utility.h>       // For _HKI definition
#include <gal/color4d.h>

#include <board_stackup_manager/board_stackup.h>

// Keyword used in file to identify the dielectric layer type
#define KEY_CORE QStringLiteral("core")
#define KEY_PREPREG QStringLiteral("prepreg")

#define KEY_COPPER QStringLiteral("copper")

// key string used for not specified parameters
// Can be translated in dialogs, and is also a keyword outside dialogs
QString inline NotSpecifiedPrm()
{
    return _HKI( "Not specified" );
}

/**
 * @return true if the param value is specified:
 * not empty
 * not NotSpecifiedPrm() value or its translation
 */
bool IsPrmSpecified( const QString& aPrmValue );

#define DEFAULT_SOLDERMASK_OPACITY 0.83

// A reasonable Epsilon R value for solder mask dielectric
#define DEFAULT_EPSILON_R_SOLDERMASK 3.3

// A default Epsilon R value for silkscreen dielectric
#define DEFAULT_EPSILON_R_SILKSCREEN 1.0

// A minor struct to handle color in gerber job file and dialog
class FAB_LAYER_COLOR
{
public:
    FAB_LAYER_COLOR()
    {}

    FAB_LAYER_COLOR( const QString& aColorName, const QColor& aColor ) :
        m_colorName( aColorName ),
        m_color( aColor )
    {}

    const QString& GetName() const
    {
        return m_colorName;
    }

    KIGFX::COLOR4D GetColor( BOARD_STACKUP_ITEM_TYPE aItemType ) const
    {
        if( aItemType == BS_ITEM_TYPE_SOLDERMASK )
            return m_color.WithAlpha( DEFAULT_SOLDERMASK_OPACITY );
        else
            return m_color.WithAlpha( 1.0 );
    }

    /**
     * @return a color name acceptable in gerber job file
     * one of normalized color name, or the string R<integer>G<integer>B<integer>
     * integer is a decimal value from 0 to 255
     */
    const QString GetColorAsString() const;

private:
    QString         m_colorName;    // the name (in job file) of the color
                                    // User values are the HTML encoded "#rrggbbaa"
                                    // RGB hexa value.
    KIGFX::COLOR4D  m_color;
};


/**
 * @return a QStringList of standard copper finish names.
 * @param aTranslate = false for the initial names, true for translated names
 */
QStringList GetStandardCopperFinishes( bool aTranslate );

/**
 * @return a list of standard FAB_LAYER_COLOR items for silkscreen and solder mask.
 */
const std::vector<FAB_LAYER_COLOR>& GetStandardColors( BOARD_STACKUP_ITEM_TYPE aType );

/**
 * @return the index of the user defined color in ColorStandardList
 */
int GetColorUserDefinedListIdx( BOARD_STACKUP_ITEM_TYPE aType );

inline KIGFX::COLOR4D GetDefaultUserColor( BOARD_STACKUP_ITEM_TYPE aType )
{
    return GetStandardColors( aType )[GetColorUserDefinedListIdx( aType )].GetColor( aType );
}

inline KIGFX::COLOR4D GetStandardColor( BOARD_STACKUP_ITEM_TYPE aType, int aIdx )
{
    return GetStandardColors( aType )[ aIdx ].GetColor( aType );
}

inline const QString& GetStandardColorName( BOARD_STACKUP_ITEM_TYPE aType, int aIdx )
{
    return GetStandardColors( aType )[ aIdx ].GetName();
}

inline bool IsCustomColorIdx( BOARD_STACKUP_ITEM_TYPE aType, int aIdx )
{
    return aIdx == GetColorUserDefinedListIdx( aType );
}

/**
 * @return true if aName is a color name acceptable in gerber job files
 * @param aName is a color name like red, blue... (case insensitive)
 */
bool IsColorNameNormalized( const QString& aName );


#endif      // #ifndef STACKUP_PREDEFINED_PRMS_H
