
#ifndef __SPICE_SETTINGS_H__
#define __SPICE_SETTINGS_H__

#include <settings/nested_settings.h>
#include <QString>


/**
 * Storage for simulator specific settings.
 */
class SPICE_SETTINGS : public NESTED_SETTINGS
{
public:
    SPICE_SETTINGS( JSON_SETTINGS* aParent, const std::string& aPath );

    virtual ~SPICE_SETTINGS() {}

    virtual bool operator==( const SPICE_SETTINGS& aRhs ) const = 0;

    bool operator!=( const SPICE_SETTINGS& aRhs ) const { return !( *this == aRhs ); }

    QString GetWorkbookFilename() const { return m_workbookFilename; }
    void    SetWorkbookFilename( const QString& aFilename ) { m_workbookFilename = aFilename; }

    bool GetFixIncludePaths() const { return m_fixIncludePaths; }
    void SetFixIncludePaths( bool aFixIncludePaths ) { m_fixIncludePaths = aFixIncludePaths; }

private:
    QString m_workbookFilename;
    bool    m_fixIncludePaths;
};

/**
 * Ngspice simulator compatibility modes.
 *
 * @note The ngspice model modes are mutually exclusive.
 */
enum class NGSPICE_COMPATIBILITY_MODE
{
    USER_CONFIG,
    NGSPICE,
    PSPICE,
    LTSPICE,
    LT_PSPICE,
    HSPICE
};


/**
 * Container for Ngspice simulator settings.
 */
class NGSPICE_SETTINGS : public SPICE_SETTINGS
{
public:
    NGSPICE_SETTINGS( JSON_SETTINGS* aParent, const std::string& aPath );
    virtual ~NGSPICE_SETTINGS() {}

    bool operator==( const SPICE_SETTINGS& aRhs ) const override;

    NGSPICE_COMPATIBILITY_MODE GetCompatibilityMode() const { return m_compatibilityMode; }
    void SetCompatibilityMode( NGSPICE_COMPATIBILITY_MODE aMode ) { m_compatibilityMode = aMode; }

private:
    NGSPICE_COMPATIBILITY_MODE m_compatibilityMode;
};


#endif // __SPICE_SETTINGS_H__
