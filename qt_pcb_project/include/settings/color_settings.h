// QT_TRANSFORMATION_COMPLETED

#ifndef _COLOR_SETTINGS_H
#define _COLOR_SETTINGS_H

#include <unordered_map>
#include <QString>

#include <gal/color4d.h>
#include <settings/json_settings.h>
#include <settings/parameters.h>

class QSettings;

using KIGFX::COLOR4D;
class KICOMMON_API COLOR_SETTINGS : public JSON_SETTINGS
{
public:
    explicit COLOR_SETTINGS( const QString& aFilename = QStringLiteral( "user" ),
                             bool aAbsolutePath = false );

    virtual ~COLOR_SETTINGS() {}

    COLOR_SETTINGS( const COLOR_SETTINGS& aOther );

    COLOR_SETTINGS& operator=( const COLOR_SETTINGS &aOther );

    bool MigrateFromLegacy( QSettings* aCfg ) override;

    COLOR4D GetColor( int aLayer ) const;

    COLOR4D GetDefaultColor( int aLayer );

    void SetColor( int aLayer, const COLOR4D& aColor );

    const QString& GetName() const { return m_displayName; }
    void SetName( const QString& aName ) { m_displayName = aName; }

    bool GetOverrideSchItemColors() const { return m_overrideSchItemColors; }
    void SetOverrideSchItemColors( bool aFlag ) { m_overrideSchItemColors = aFlag; }

    static std::vector<COLOR_SETTINGS*> CreateBuiltinColorSettings();

    static const QString COLOR_BUILTIN_DEFAULT;
    static const QString COLOR_BUILTIN_CLASSIC;

private:
    bool migrateSchema0to1();

    void initFromOther( const COLOR_SETTINGS& aOther );

private:
    QString m_displayName;
    bool    m_overrideSchItemColors;

    std::unordered_map<int, COLOR4D> m_colors;
    std::unordered_map<int, COLOR4D> m_defaultColors;
};

class COLOR_MAP_PARAM : public PARAM_BASE
{
public:
    COLOR_MAP_PARAM( const std::string& aJsonPath, int aMapKey, COLOR4D aDefault,
                     std::unordered_map<int, COLOR4D>* aMap, bool aReadOnly = false ) :
            PARAM_BASE( aJsonPath, aReadOnly ), m_key( aMapKey ), m_default( aDefault ),
            m_map( aMap )
    {}

    void Load( const JSON_SETTINGS& aSettings, bool aResetIfMissing = true ) const override
    {
        if( m_readOnly )
            return;

        if( std::optional<COLOR4D> optval = aSettings.Get<COLOR4D>( m_path ) )
            ( *m_map )[ m_key ] = *optval;
        else if( aResetIfMissing )
            ( *m_map )[ m_key ] = m_default;
    }

    void Store( JSON_SETTINGS* aSettings ) const override
    {
        aSettings->Set<COLOR4D>( m_path, ( *m_map )[ m_key ] );
    }

    int GetKey() const
    {
        return m_key;
    }

    COLOR4D GetDefault() const
    {
        return m_default;
    }

    void SetDefault() override
    {
        ( *m_map )[ m_key ] = m_default;
    }

    bool MatchesFile( const JSON_SETTINGS& aSettings ) const override
    {
        if( std::optional<COLOR4D> optval = aSettings.Get<COLOR4D>( m_path ) )
            return m_map->count( m_key ) && ( *optval == m_map->at( m_key ) );

        // If the JSON doesn't exist, the map shouldn't exist either
        return !m_map->count( m_key );
    }

private:
    int m_key;

    COLOR4D m_default;

    std::unordered_map<int, COLOR4D>* m_map;
};

#endif
