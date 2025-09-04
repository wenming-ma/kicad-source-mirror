
#ifndef CONFIG_PARAMS_H_
#define CONFIG_PARAMS_H_

#include <kicommon.h>
#include <set>
#include <limits>

#include <QSettings>
#include <QString>
#include <QStringList>
#include <boost/ptr_container/ptr_vector.hpp>


KICOMMON_API void ConfigBaseWriteDouble( QSettings* aConfig, const QString& aKey, double aValue );


/** Type of parameter in the configuration file */
enum paramcfg_id {
    PARAM_INT,
    PARAM_INT_WITH_SCALE,
    PARAM_DOUBLE,
    PARAM_BOOL,
    PARAM_LIBNAME_LIST,
    PARAM_WXSTRING,
    PARAM_WXSTRING_SET,
    PARAM_FILENAME,
    PARAM_COMMAND_ERASE,
    PARAM_LAYERS,
    PARAM_TRACKWIDTHS,
    PARAM_VIADIMENSIONS,
    PARAM_DIFFPAIRDIMENSIONS,
    PARAM_NETCLASSES,
    PARAM_SEVERITIES
};


class KICOMMON_API PARAM_CFG
{
public:
    PARAM_CFG( const QString& ident, const paramcfg_id type, const QChar* group = nullptr,
               const QString& legacy_ident = QString() );
    virtual ~PARAM_CFG() {}

    virtual void ReadParam( QSettings* aConfig ) const {};

    virtual void SaveParam( QSettings* aConfig ) const {};

    QString     m_Ident;
    paramcfg_id m_Type;
    QString     m_Group;
    bool        m_Setup;

    QString     m_Ident_legacy;
};


class KICOMMON_API PARAM_CFG_INT : public PARAM_CFG
{
public:
    PARAM_CFG_INT( const QString& ident, int* ptparam, int default_val = 0,
                   int min = std::numeric_limits<int>::min(),
                   int max = std::numeric_limits<int>::max(),
                   const QChar* group = nullptr,
                   const QString& legacy_ident = QString() );
    PARAM_CFG_INT( bool Insetup, const QString& ident, int* ptparam, int default_val = 0,
                   int min = std::numeric_limits<int>::min(),
                   int max = std::numeric_limits<int>::max(),
                   const QChar* group = nullptr,
                   const QString& legacy_ident = QString() );

    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;

    int* m_Pt_param;
    int  m_Min, m_Max;
    int  m_Default;
};

class KICOMMON_API PARAM_CFG_INT_WITH_SCALE : public PARAM_CFG_INT
{
public:
    PARAM_CFG_INT_WITH_SCALE( const QString& ident, int* ptparam, int default_val = 0,
                              int min = std::numeric_limits<int>::min(),
                              int max = std::numeric_limits<int>::max(),
                              const QChar* group = nullptr, double aBiu2cfgunit = 1.0,
                              const QString& legacy_ident = QString() );
    PARAM_CFG_INT_WITH_SCALE( bool insetup, const QString& ident, int* ptparam,
                              int default_val = 0,
                              int min = std::numeric_limits<int>::min(),
                              int max = std::numeric_limits<int>::max(),
                              const QChar* group = nullptr, double aBiu2cfgunit = 1.0,
                              const QString& legacy_ident = QString() );

    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;

public:
    double   m_BIU_to_cfgunit;
};


class KICOMMON_API PARAM_CFG_DOUBLE : public PARAM_CFG
{
public:
    PARAM_CFG_DOUBLE( const QString& ident, double* ptparam,
                      double default_val = 0.0, double min = 0.0, double max = 10000.0,
                      const QChar* group = nullptr );
    PARAM_CFG_DOUBLE( bool Insetup, const QString& ident, double* ptparam,
                      double default_val = 0.0, double min = 0.0, double max = 10000.0,
                      const QChar* group = nullptr );

    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;

    double* m_Pt_param;
    double  m_Default;
    double  m_Min, m_Max;
};


class KICOMMON_API PARAM_CFG_BOOL : public PARAM_CFG
{
public:
    PARAM_CFG_BOOL( const QString& ident, bool* ptparam,
                    int default_val = false, const QChar* group = nullptr,
                    const QString& legacy_ident = QString() );
    PARAM_CFG_BOOL( bool Insetup, const QString& ident, bool* ptparam,
                    int default_val = false, const QChar* group = nullptr,
                    const QString& legacy_ident = QString() );

    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;

    bool* m_Pt_param;
    int   m_Default;
};


class KICOMMON_API PARAM_CFG_QSTRING : public PARAM_CFG
{
public:
    PARAM_CFG_QSTRING( const QString& ident, QString* ptparam, const QChar* group = nullptr );

    PARAM_CFG_QSTRING( bool            Insetup,
                       const QString&  ident,
                       QString*        ptparam,
                       const QString&  default_val = QString(),
                       const QChar* group = nullptr );

    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;

    QString* m_Pt_param;
    QString  m_default;
};


class KICOMMON_API PARAM_CFG_QSTRING_SET : public PARAM_CFG
{
public:
    PARAM_CFG_QSTRING_SET( const QString& ident, std::set<QString>* ptparam,
                           const QChar* group = nullptr );

    PARAM_CFG_QSTRING_SET( bool               Insetup,
                           const QString&     ident,
                           std::set<QString>* ptparam,
                           const QChar* group = nullptr );

    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;

    std::set<QString>* m_Pt_param;
};


class KICOMMON_API PARAM_CFG_FILENAME : public PARAM_CFG
{
public:
    PARAM_CFG_FILENAME( const QString& ident, QString* ptparam,
            const QChar* group = nullptr );
    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;

    QString* m_Pt_param;
};


class KICOMMON_API PARAM_CFG_LIBNAME_LIST : public PARAM_CFG
{
public:
    QStringList* m_Pt_param;

public:
    PARAM_CFG_LIBNAME_LIST( const QChar* ident, QStringList* ptparam,
                            const QChar* group = nullptr );

    virtual void ReadParam( QSettings* aConfig ) const override;
    virtual void SaveParam( QSettings* aConfig ) const override;
};


KICOMMON_API void QSettingsSaveSetups( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList );

KICOMMON_API void QSettingsSaveParams( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList,
                                       const QString& aGroup );

KICOMMON_API void QSettingsLoadSetups( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList );

KICOMMON_API void QSettingsLoadParams( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList,
                                       const QString& aGroup );


#endif  // CONFIG_PARAMS_H_
