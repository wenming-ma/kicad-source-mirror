
#ifndef _SYMBOL_LIBRARY_COMMON_H_
#define _SYMBOL_LIBRARY_COMMON_H_

#include <map>

#include <QStringList>
#include <QString>


class LIB_SYMBOL;
class SCH_BASE_FRAME;
class SYMBOL_LIB_TABLE;


enum class SCH_LIB_TYPE
{
    LT_EESCHEMA,
    LT_SYMBOL
};


/**
 * Symbol library map sorting helper.
 */
struct LibSymbolMapSort
{
    bool operator() ( const QString& aItem1, const QString& aItem2 ) const
    {
        return aItem1 < aItem2;
    }
};


///< Symbol library map sorted by the symbol name.
typedef std::map< QString, LIB_SYMBOL*, LibSymbolMapSort > LIB_SYMBOL_MAP;


/**
 * Helper object to filter a list of libraries.
 */
class SYMBOL_LIBRARY_FILTER
{
public:
    SYMBOL_LIBRARY_FILTER()
    {
        m_filterPowerSymbols = false;
        m_forceLoad = false;
    }

    /**
     * Add \a aLibName to the allowed libraries list.
     */
    void AddLib( const QString& aLibName )
    {
        m_allowedLibs.append( aLibName );
        m_forceLoad = false;
    }


    /**
     * Add \a aLibName to the allowed libraries list.
     */
    void LoadFrom( const QString& aLibName )
    {
        m_allowedLibs.clear();
        m_allowedLibs.append( aLibName );
        m_forceLoad = true;
    }

    /**
     * Clear the allowed libraries list (allows all libraries).
     */
    void ClearLibList()
    {
        m_allowedLibs.clear();
        m_forceLoad = false;
    }

    /**
     * Enable or disable the filtering of power symbols.
     */
    void FilterPowerSymbols( bool aFilterEnable )
    {
        m_filterPowerSymbols = aFilterEnable;
    }

    /**
     * @return true if the filtering of power symbols is on.
     */
    bool GetFilterPowerSymbols() const { return m_filterPowerSymbols; }


    /**
     * @return the list of the names of allowed libraries.
     */
    const QStringList& GetAllowedLibList() const { return m_allowedLibs; }

    /**
     * @return the name of the library to use to load a symbol or an a empty string if no
     *         library source available.
     */
    const QString& GetLibSource() const
    {
        static QString dummy;

        if( m_forceLoad && m_allowedLibs.count() > 0 )
            return m_allowedLibs[0];
        else
            return dummy;
    }

private:
    QStringList m_allowedLibs;        ///< List of filtered library names.
    bool          m_filterPowerSymbols; ///< Enable or disable power symbol filtering.
    bool          m_forceLoad;          ///< Force loading symbol from library m_allowedLibs[0].
};


#endif    // _SYMBOL_LIBRARY_COMMON_H_
