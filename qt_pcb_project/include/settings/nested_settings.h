// QT_TRANSFORMATION_COMPLETED

#ifndef NESTED_SETTINGS_H_
#define NESTED_SETTINGS_H_

#include <QString>

#include <settings/json_settings.h>


class KICOMMON_API NESTED_SETTINGS : public JSON_SETTINGS
{
public:
    NESTED_SETTINGS( const std::string& aName, int aSchemaVersion, JSON_SETTINGS* aParent,
                     const std::string& aPath, bool aLoadFromFile = true );

    virtual ~NESTED_SETTINGS();

    bool LoadFromFile( const QString& aDirectory = "" ) override;

    bool SaveToFile( const QString& aDirectory = "", bool aForce = false ) override;

    void SetParent( JSON_SETTINGS* aParent, bool aLoadFromFile = true );

    JSON_SETTINGS* GetParent()
    {
        return m_parent;
    }

protected:

    JSON_SETTINGS* m_parent;

    std::string m_path;
};

#endif
