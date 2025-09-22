
#ifndef NETLIST_EXPORTER_SPICE_H
#define NETLIST_EXPORTER_SPICE_H

#include "netlist_exporter_base.h"
#include <sim/sim_lib_mgr.h>
#include <sim/sim_library.h>
#include <sim/sim_model.h>
#include <sim/spice_generator.h>


class QWidget;


class NAME_GENERATOR
{
public:
    std::string Generate( const std::string& aProposedName );

private:
    std::unordered_set<std::string> m_names;
};


class NETLIST_EXPORTER_SPICE : public NETLIST_EXPORTER_BASE
{
public:
    enum OPTIONS
    {
        OPTION_ADJUST_INCLUDE_PATHS  = 0x0010,
        OPTION_ADJUST_PASSIVE_VALS   = 0x0020,
        OPTION_SAVE_ALL_VOLTAGES     = 0x0040,
        OPTION_SAVE_ALL_CURRENTS     = 0x0080,
        OPTION_SAVE_ALL_DISSIPATIONS = 0x0100,
        OPTION_CUR_SHEET_AS_ROOT     = 0x0200,
        OPTION_SIM_COMMAND           = 0x0400,
        OPTION_SAVE_ALL_EVENTS       = 0x0800,
        OPTION_DEFAULT_FLAGS =   OPTION_ADJUST_INCLUDE_PATHS
                               | OPTION_ADJUST_PASSIVE_VALS
                               | OPTION_SAVE_ALL_VOLTAGES
                               | OPTION_SAVE_ALL_CURRENTS
                               | OPTION_SAVE_ALL_DISSIPATIONS
                               | OPTION_SAVE_ALL_EVENTS
    };

    NETLIST_EXPORTER_SPICE( SCHEMATIC* aSchematic );

    /**
     * Write to specified output file.
     */
    bool WriteNetlist( const QString& aOutFileName, unsigned aNetlistOptions,
                       REPORTER& aReporter ) override;

    /**
     * Write the netlist in aFormatter.
     */
    bool DoWriteNetlist( const QString& aSimCommand, unsigned aSimOptions,
                         OUTPUTFORMATTER& aFormatter, REPORTER& aReporter );

    /**
     * Write the netlist head (title and so on).
     */
    virtual void WriteHead( OUTPUTFORMATTER& aFormatter, unsigned aNetlistOptions );

    /**
     * Write the tail (.end).
     */
    virtual void WriteTail( OUTPUTFORMATTER& aFormatter, unsigned aNetlistOptions );

    /**
     * Process the schematic and Spice libraries to create net mapping and a list of SPICE_ITEMs.
     * It is automatically called by WriteNetlist(), but might be used separately,
     * if only net mapping and the list of SPICE_ITEMs are required.
     * @return True if successful.
     */
    virtual bool ReadSchematicAndLibraries( unsigned aNetlistOptions, REPORTER& aReporter );

    /**
     * Remove formatting wrappers and replace illegal spice net name characters with underscores.
     */
    static void ConvertToSpiceMarkup( QString* aNetName );

    /**
     * Return the list of nets.
     */
    std::set<QString> GetNets() const { return m_nets; }

    /**
     * Return name of Spice device corresponding to a schematic symbol.
     *
     * @param aRefName is the component reference.
     * @return Spice device name or empty string if there is no such symbol in the netlist.
     * Normally the name is either a plain reference if the first character of reference
     * corresponds to the assigned device model type or a reference prefixed with a character
     * defining the device model type.
     */
    QString GetItemName( const QString& aRefName ) const;

    /**
     * Return the list of items representing schematic symbols in the Spice world.
     */
    const std::list<SPICE_ITEM>& GetItems() const { return m_items; }

    /**
     * Find and return the item corresponding to \a aRefName.
     */
    const SPICE_ITEM* FindItem( const QString& aRefName ) const;

    const std::vector<QString>& GetDirectives() { return m_directives; }

protected:
    void ReadDirectives( unsigned aNetlistOptions );
    virtual void WriteDirectives( const QString& aSimCommand, unsigned aSimOptions,
                                  OUTPUTFORMATTER& candidate ) const;

    virtual QString GenerateItemPinNetName( const QString& aNetName, int& aNcCounter ) const;

    /**
     * Return the paths of exported sheets (either all or the current one).
     */
    SCH_SHEET_LIST BuildSheetList( unsigned aNetlistOptions = 0 ) const;

private:
    void readRefName( SCH_SHEET_PATH& aSheet, SCH_SYMBOL& aSymbol, SPICE_ITEM& aItem,
                      std::set<std::string>& aRefNames );
    void readModel( SCH_SHEET_PATH& aSheet, SCH_SYMBOL& aSymbol, SPICE_ITEM& aItem,
                    REPORTER& aReporter );
    void readPinNumbers( SCH_SYMBOL& aSymbol, SPICE_ITEM& aItem,
                         const std::vector<PIN_INFO>& aPins );
    void readPinNetNames( SCH_SYMBOL& aSymbol, SPICE_ITEM& aItem,
                          const std::vector<PIN_INFO>& aPins, int& aNcCounter );
    void getNodePattern( SPICE_ITEM& aItem, std::vector<std::string>& aModifiers );
    void readNodePattern( SPICE_ITEM& aItem );

    void writeInclude( OUTPUTFORMATTER& aFormatter, unsigned aNetlistOptions,
                       const QString& aPath );

    void writeIncludes( OUTPUTFORMATTER& aFormatter, unsigned aNetlistOptions );
    void writeModels( OUTPUTFORMATTER& aFormatter );
    void writeItems( OUTPUTFORMATTER& aFormatter );

    SIM_LIB_MGR             m_libMgr;             ///< Holds libraries and models
    NAME_GENERATOR          m_modelNameGenerator; ///< Generates unique model names

    std::vector<QString>   m_directives;         ///< Spice directives found in the schematic sheet
    std::set<QString>      m_rawIncludes;        ///< include directives found in symbols
    std::set<QString>      m_nets;

    ///< Items representing schematic symbols in Spice world.
    std::list<SPICE_ITEM>   m_items;
};


#endif // NETLIST_EXPORTER_SPICE_H
