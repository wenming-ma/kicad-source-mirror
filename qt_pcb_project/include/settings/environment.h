
// QT_TRANSFORMATION_COMPLETED
#ifndef KICAD_ENVIRONMENT_H
#define KICAD_ENVIRONMENT_H

#include <QHash>
#include <QString>


class ENV_VAR_ITEM
{
public:
    ENV_VAR_ITEM( const QString& aValue = QString(), bool aIsDefinedExternally = false ) :
            m_value( aValue ),
            m_isBuiltin( true ),
            m_isDefinedExternally( aIsDefinedExternally ),
            m_isDefinedInSettings( false )
    {
    }

    ENV_VAR_ITEM( const QString& aKey, const QString& aValue,
                  const QString& aDefaultValue = QString() ) :
            m_key( aKey ),
            m_value( aValue ),
            m_defaultValue( aDefaultValue ),
            m_isBuiltin( true ),
            m_isDefinedExternally( false ),
            m_isDefinedInSettings( false )
    {
    }

    ~ENV_VAR_ITEM() {}

    bool GetDefinedExternally() const { return m_isDefinedExternally; }
    void SetDefinedExternally( bool aIsDefinedExternally = true )
    {
        m_isDefinedExternally = aIsDefinedExternally;
    }

    bool GetDefinedInSettings() const { return m_isDefinedInSettings; }
    void SetDefinedInSettings( bool aDefined = true ) { m_isDefinedInSettings = aDefined; }

    QString GetKey() const { return m_key; }

    const QString& GetValue() const { return m_value; }
    void SetValue( const QString& aValue ) { m_value = aValue; }

    QString GetDefault() const { return m_defaultValue; }

    QString GetSettingsValue() const { return m_settingsValue; }
    void SetSettingsValue( const QString& aValue ) { m_settingsValue = aValue; }

    bool GetBuiltin() const { return m_isBuiltin; }

    bool IsDefault() const
    {
        return m_isBuiltin && m_value == m_defaultValue;
    }

private:
    QString m_key;
    QString m_value;
    QString m_defaultValue;
    QString m_settingsValue;

    bool m_isBuiltin;
    bool m_isDefinedExternally;
    bool m_isDefinedInSettings;
};

typedef QHash<QString, ENV_VAR_ITEM>                    ENV_VAR_MAP;
typedef QHash<QString, ENV_VAR_ITEM>::iterator          ENV_VAR_MAP_ITER;
typedef QHash<QString, ENV_VAR_ITEM>::const_iterator    ENV_VAR_MAP_CITER;

#endif // KICAD_ENVIRONMENT_H
