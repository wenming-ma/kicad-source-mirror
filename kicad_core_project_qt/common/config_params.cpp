

#include <config_params.h>       // for PARAM_CFG_INT_WITH_SCALE, PARAM_CFG_...
#include <locale_io.h>
#include <math/util.h>             // for KiROUND
#include <QSettings>
#include <QtGlobal>

void qtConfigLoadParams( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList,
                         const QString& aGroup )
{
    Q_ASSERT( aCfg );

    for( PARAM_CFG* param : aList )
    {
        if( !param->m_Group.isEmpty() )
            aCfg->beginGroup( param->m_Group );
        else
            aCfg->beginGroup( aGroup );

        if( param->m_Setup )
            continue;

        param->ReadParam( aCfg );
    }
}


void qtConfigLoadSetups( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList )
{
    Q_ASSERT( aCfg );

    for( PARAM_CFG* param : aList )
    {
        if( !param->m_Setup )
            continue;

        param->ReadParam( aCfg );
    }
}


void qtConfigSaveParams( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList,
                         const QString& aGroup )
{
    Q_ASSERT( aCfg );

    for( PARAM_CFG* param : aList )
    {
        if( !param->m_Group.isEmpty() )
            aCfg->beginGroup( param->m_Group );
        else
            aCfg->beginGroup( aGroup );

        if( param->m_Setup )
            continue;

        if( param->m_Type == PARAM_COMMAND_ERASE )       // Erase all data
        {
            if( !param->m_Ident.isEmpty() )
                aCfg->remove( param->m_Ident );
        }
        else
        {
            param->SaveParam( aCfg );
        }
    }
}


void qtConfigSaveSetups( QSettings* aCfg, const std::vector<PARAM_CFG*>& aList )
{
    Q_ASSERT( aCfg );

    for( PARAM_CFG* param : aList )
    {
        if( !param->m_Setup )
            continue;

        if( param->m_Type == PARAM_COMMAND_ERASE )       // Erase all data
        {
            if( !param->m_Ident.isEmpty() )
                aCfg->remove( param->m_Ident );
        }
        else
        {
            param->SaveParam( aCfg );
        }
    }
}


void ConfigBaseWriteDouble( QSettings* aConfig, const QString& aKey, double aValue )
{
    // Use a single strategy, regardless of Qt version.
    // Want C locale float string.

    LOCALE_IO   toggle;
    QString     tnumber = QString::number( aValue, 'g', 16 );

    aConfig->setValue( aKey, tnumber );
}


PARAM_CFG::PARAM_CFG( const QString& ident, const paramcfg_id type,
                      const QChar* group, const QString& legacy )
{
    m_Ident = ident;
    m_Type  = type;
    m_Group = group ? QString(group) : QString();
    m_Setup = false;

    m_Ident_legacy = legacy;
}


PARAM_CFG_INT::PARAM_CFG_INT( const QString& ident, int* ptparam, int default_val,
                              int min, int max, const QChar* group, const QString& legacy ) :
        PARAM_CFG( ident, PARAM_INT, group, legacy )
{
    m_Pt_param = ptparam;
    m_Default  = default_val;
    m_Min = min;
    m_Max = max;
}


PARAM_CFG_INT::PARAM_CFG_INT( bool setup, const QString& ident, int* ptparam, int default_val,
                              int min, int max, const QChar* group, const QString& legacy ) :
        PARAM_CFG( ident, PARAM_INT, group, legacy )
{
    m_Pt_param = ptparam;
    m_Default  = default_val;
    m_Min   = min;
    m_Max   = max;
    m_Setup = setup;
}


void PARAM_CFG_INT::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    int itmp = m_Default;

    if( !aConfig->contains( m_Ident ) && !m_Ident_legacy.isEmpty() )
        itmp = aConfig->value( m_Ident_legacy, itmp ).toInt();
    else
        itmp = aConfig->value( m_Ident, itmp ).toInt();

    if( (itmp < m_Min) || (itmp > m_Max) )
        itmp = m_Default;

    *m_Pt_param = itmp;
}


void PARAM_CFG_INT::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    aConfig->setValue( m_Ident, *m_Pt_param );
}


PARAM_CFG_INT_WITH_SCALE::PARAM_CFG_INT_WITH_SCALE( const QString& ident, int* ptparam,
                                                    int default_val, int min, int max,
                                                    const QChar* group, double aBiu2cfgunit,
                                                    const QString& legacy_ident ) :
    PARAM_CFG_INT( ident, ptparam, default_val, min, max, group, legacy_ident )
{
    m_Type = PARAM_INT_WITH_SCALE;
    m_BIU_to_cfgunit = aBiu2cfgunit;
}


PARAM_CFG_INT_WITH_SCALE::PARAM_CFG_INT_WITH_SCALE( bool setup, const QString& ident, int* ptparam,
                                                    int default_val, int min, int max,
                                                    const QChar* group, double aBiu2cfgunit,
                                                    const QString& legacy_ident ) :
    PARAM_CFG_INT( setup, ident, ptparam, default_val, min, max, group, legacy_ident )
{
    m_Type = PARAM_INT_WITH_SCALE;
    m_BIU_to_cfgunit = aBiu2cfgunit;
}


void PARAM_CFG_INT_WITH_SCALE::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    double dtmp = (double) m_Default * m_BIU_to_cfgunit;
    if( !aConfig->contains( m_Ident ) && !m_Ident_legacy.isEmpty() )
        dtmp = aConfig->value( m_Ident_legacy, dtmp ).toDouble();
    else
        dtmp = aConfig->value( m_Ident, dtmp ).toDouble();

    int itmp = KiROUND( dtmp / m_BIU_to_cfgunit );

    if( (itmp < m_Min) || (itmp > m_Max) )
        itmp = m_Default;

    *m_Pt_param = itmp;
}


void PARAM_CFG_INT_WITH_SCALE::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    // We cannot use aConfig->Write for a double, because
    // this function uses a format with very few digits in mantissa,
    // and truncate issues are frequent.
    // We use our function.
    ConfigBaseWriteDouble( aConfig, m_Ident, *m_Pt_param * m_BIU_to_cfgunit );
}


PARAM_CFG_DOUBLE::PARAM_CFG_DOUBLE( const QString& ident, double* ptparam,
                                    double default_val, double min, double max,
                                    const QChar* group ) :
        PARAM_CFG( ident, PARAM_DOUBLE, group )
{
    m_Pt_param = ptparam;
    m_Default  = default_val;
    m_Min = min;
    m_Max = max;
}


PARAM_CFG_DOUBLE::PARAM_CFG_DOUBLE( bool          Insetup,
                                    const QString& ident,
                                    double*       ptparam,
                                    double        default_val,
                                    double        min,
                                    double        max,
                                    const QChar* group ) :
        PARAM_CFG( ident, PARAM_DOUBLE, group )
{
    m_Pt_param = ptparam;
    m_Default  = default_val;
    m_Min   = min;
    m_Max   = max;
    m_Setup = Insetup;
}


void PARAM_CFG_DOUBLE::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    double dtmp = m_Default;
    dtmp = aConfig->value( m_Ident, dtmp ).toDouble();

    if( (dtmp < m_Min) || (dtmp > m_Max) )
        dtmp = m_Default;

    *m_Pt_param = dtmp;
}


void PARAM_CFG_DOUBLE::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    // We cannot use aConfig->Write for a double, because
    // this function uses a format with very few digits in mantissa,
    // and truncate issues are frequent.
    // We use our function.
    ConfigBaseWriteDouble( aConfig, m_Ident, *m_Pt_param );
}


PARAM_CFG_BOOL::PARAM_CFG_BOOL( const QString& ident, bool* ptparam, int default_val,
                                const QChar* group, const QString& legacy ) :
        PARAM_CFG( ident, PARAM_BOOL, group, legacy )
{
    m_Pt_param = ptparam;
    m_Default  = default_val ? true : false;
}


PARAM_CFG_BOOL::PARAM_CFG_BOOL( bool Insetup, const QString& ident, bool* ptparam,
                                int default_val, const QChar* group, const QString& legacy ) :
        PARAM_CFG( ident, PARAM_BOOL, group, legacy )
{
    m_Pt_param = ptparam;
    m_Default  = default_val ? true : false;
    m_Setup    = Insetup;
}


void PARAM_CFG_BOOL::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    int itmp = (int) m_Default;

    if( !aConfig->contains( m_Ident ) && !m_Ident_legacy.isEmpty() )
        itmp = aConfig->value( m_Ident_legacy, itmp ).toInt();
    else
        itmp = aConfig->value( m_Ident, itmp ).toInt();

    *m_Pt_param = itmp ? true : false;
}


void PARAM_CFG_BOOL::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    aConfig->setValue( m_Ident, *m_Pt_param );
}


PARAM_CFG_QSTRING::PARAM_CFG_QSTRING( const QString& ident, QString* ptparam,
                                        const QChar* group ) :
        PARAM_CFG( ident, PARAM_WXSTRING, group )
{
    m_Pt_param = ptparam;
}


PARAM_CFG_QSTRING::PARAM_CFG_QSTRING( bool Insetup, const QString& ident, QString* ptparam,
                                        const QString& default_val, const QChar* group ) :
        PARAM_CFG( ident, PARAM_WXSTRING, group )
{
    m_Pt_param = ptparam;
    m_Setup    = Insetup;
    m_default  = default_val;
}


void PARAM_CFG_QSTRING::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    *m_Pt_param = aConfig->value( m_Ident, m_default ).toString();
}


void PARAM_CFG_QSTRING::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    aConfig->setValue( m_Ident, *m_Pt_param );
}


PARAM_CFG_QSTRING_SET::PARAM_CFG_QSTRING_SET( const QString& ident, std::set<QString>* ptparam,
                                                const QChar* group ) :
        PARAM_CFG( ident, PARAM_WXSTRING_SET, group )
{
    m_Pt_param = ptparam;
}


PARAM_CFG_QSTRING_SET::PARAM_CFG_QSTRING_SET( bool Insetup, const QString& ident,
                                                std::set<QString>* ptparam, const QChar* group ) :
        PARAM_CFG( ident, PARAM_WXSTRING_SET, group )
{
    m_Pt_param = ptparam;
    m_Setup    = Insetup;
}


void PARAM_CFG_QSTRING_SET::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    for( int i = 1; true; ++i )
    {
        QString key, data;

        key = m_Ident;
        key += QString::number( i );
        data = aConfig->value( key, "" ).toString();

        if( data.isEmpty() )
            break;

        m_Pt_param->insert( data );
    }
}


void PARAM_CFG_QSTRING_SET::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    int i = 1;

    for( const QString& str : *m_Pt_param )
    {
        QString key;

        key = m_Ident;
        key += QString::number( i++ );

        aConfig->setValue( key, str );
    }
}


PARAM_CFG_FILENAME::PARAM_CFG_FILENAME( const QString& ident,
                                        QString*     ptparam,
                                        const QChar* group ) :
        PARAM_CFG( ident, PARAM_FILENAME, group )
{
    m_Pt_param = ptparam;
}


void PARAM_CFG_FILENAME::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    QString prm = aConfig->value( m_Ident ).toString();
    // file names are stored using Unix notation
    // under Window we must use \ instead of /
    // mainly if there is a server name in path (something like \\server\kicad)
#ifdef __WINDOWS__
    prm.replace( "/", "\\" );
#endif
    *m_Pt_param = prm;
}


void PARAM_CFG_FILENAME::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    QString prm = *m_Pt_param;

    // filenames are stored using Unix notation
    prm.replace( "\\", "/" );
    aConfig->setValue( m_Ident, prm );
}


PARAM_CFG_LIBNAME_LIST::PARAM_CFG_LIBNAME_LIST( const QChar*  ident,
                                                std::vector<std::string>* ptparam,
                                                const QChar*  group ) :
        PARAM_CFG( QString(ident), PARAM_LIBNAME_LIST, group )
{
    m_Pt_param = ptparam;
}


void PARAM_CFG_LIBNAME_LIST::ReadParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    int            indexlib = 1; // We start indexlib to 1 because first
                                 // lib name is LibName1
    QString        libname, id_lib;
    std::vector<std::string>* libname_list = m_Pt_param;

    while( 1 )
    {
        id_lib = m_Ident;
        id_lib += QString::number( indexlib );
        indexlib++;
        libname = aConfig->value( id_lib, "" ).toString();

        if( libname.isEmpty() )
            break;

        // file names are stored using Unix notation
        // under Window we must use \ instead of /
        // mainly if there is a server name in path (something like \\server\kicad)
#ifdef __WINDOWS__
        libname.replace( "/", "\\" );
#endif
        libname_list->push_back( libname.toStdString() );
    }
}


void PARAM_CFG_LIBNAME_LIST::SaveParam( QSettings* aConfig ) const
{
    if( !m_Pt_param || !aConfig )
        return;

    std::vector<std::string>* libname_list = m_Pt_param;

    QString       configkey;
    QString       libname;

    for( int indexlib = 0;  indexlib < static_cast<int>(libname_list->size());  indexlib++ )
    {
        configkey = m_Ident;

        // We use indexlib+1 because first lib name is LibName1
        configkey += QString::number( indexlib + 1 );
        libname = QString::fromStdString( libname_list->at( indexlib ) );

        // filenames are stored using Unix notation
        libname.replace( "\\", "/" );
        aConfig->setValue( configkey, libname );
    }
}
