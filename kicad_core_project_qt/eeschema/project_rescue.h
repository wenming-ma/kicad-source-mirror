
#ifndef _LIB_CACHE_RESCUE_H_
#define _LIB_CACHE_RESCUE_H_

/* This code handles the case where an old schematic was made before
 * various changes were made, either to KiCad or to the libraries, and
 * the project needs to be recovered. The function of note is a member
 * of SCH_EDIT_FRAME, defined thus:
 *
 * bool SCH_EDIT_FRAME::RescueProject( bool aSilentIfNone );
 *
 * When this is called, a list of problematic symbols is compiled. If
 * this list is empty, then the function displays a notification and returns
 * (if aSilentIfNone is true, the notification is silenced).
 */

#include <QString>

#include <lib_symbol.h>
#include <sch_io/kicad_legacy/sch_io_kicad_legacy.h>
#include <class_draw_panel_gal.h>


class LIB_SYMBOL;
class SCH_SYMBOL;
class RESCUER;
class SCH_EDIT_FRAME;
class SCH_IO_KICAD_LEGACY;
class SCH_SHEET_PATH;
class SCHEMATIC;


enum RESCUE_TYPE
{
    RESCUE_CONFLICT,
    RESCUE_CASE,
};


class RESCUE_CANDIDATE
{
public:
    virtual ~RESCUE_CANDIDATE() {}

    /**
     * Get the name that was originally requested in the schematic
     */
    virtual QString GetRequestedName() const { return m_requested_name; }

    /**
     * Get the name we're proposing changing it to
     */
    virtual QString GetNewName() const { return m_new_name; }

    /**
     * Get the part that can be loaded from the project cache, if possible, or
     * else NULL.
     */
    virtual LIB_SYMBOL* GetCacheCandidate() const { return nullptr; }

    /**
     * Get the part the would be loaded from the libraries, if possible, or else
     * NULL.
     */
    virtual LIB_SYMBOL* GetLibCandidate() const { return m_lib_candidate; }

    int GetUnit() const { return m_unit; }

    int GetConvert() const { return m_convert; }

    /**
     * Get a description of the action proposed, for displaying in the UI.
     */
    virtual QString GetActionDescription() const = 0;

    /**
     * Perform the actual rescue action. If successful, this must log the rescue using
     * RESCUER::LogRescue to allow it to be reversed.
     * @return True on success.
     */
    virtual bool PerformAction( RESCUER* aRescuer ) = 0;

protected:
    QString    m_requested_name;
    QString    m_new_name;
    LIB_SYMBOL* m_lib_candidate;
    int         m_unit;
    int         m_convert;
};


class RESCUE_CASE_CANDIDATE : public RESCUE_CANDIDATE
{
public:
    /**
     * Grab all possible RESCUE_CASE_CANDIDATE objects into a vector.
     *
     * @param aRescuer is the working RESCUER instance.
     * @param aCandidates is the vector the will hold the candidates.
     */
    static void FindRescues( RESCUER& aRescuer, boost::ptr_vector<RESCUE_CANDIDATE>& aCandidates );

    /**
     * Create a RESCUE_CANDIDATE.
     *
     * @param aRequestedName us the name the schematic asks for.
     * @param aNewName is the name we want to change it to.
     * @param aLibCandidate is the part that will give us.
     * @param aUnit is the unit of the rescued symbol.
     * @param aConvert is the body style of the rescued symbol.
     */
    RESCUE_CASE_CANDIDATE( const QString& aRequestedName, const QString& aNewName,
                           LIB_SYMBOL* aLibCandidate, int aUnit = 0, int aConvert = 0 );

    RESCUE_CASE_CANDIDATE() { m_lib_candidate = nullptr; }

    virtual QString GetActionDescription() const override;

    virtual bool PerformAction( RESCUER* aRescuer ) override;
};


class RESCUE_CACHE_CANDIDATE: public RESCUE_CANDIDATE
{
    LIB_SYMBOL* m_cache_candidate;

public:
    /**
     * Grab all possible #RESCUE_CACHE_CANDIDATE objects into a vector.
     *
     * @param aRescuer is the working RESCUER instance.
     * @param aCandidates is the vector the will hold the candidates.
     */
    static void FindRescues( RESCUER& aRescuer, boost::ptr_vector<RESCUE_CANDIDATE>& aCandidates );

    /**
     * Create a RESCUE_CACHE_CANDIDATE.
     *
     * @param aRequestedName is the name the schematic asks for.
     * @param aNewName is the name we want to change it to.
     * @param aCacheCandidate is the part from the cache.
     * @param aLibCandidate is the part that would be loaded from the library.
     * @param aUnit is the unit of the rescued symbol.
     * @param aConvert is the body style of the rescued symbol.
     */
    RESCUE_CACHE_CANDIDATE( const QString& aRequestedName, const QString& aNewName,
                            LIB_SYMBOL* aCacheCandidate, LIB_SYMBOL* aLibCandidate,
                            int aUnit = 0, int aConvert = 0 );

    RESCUE_CACHE_CANDIDATE();

    virtual LIB_SYMBOL* GetCacheCandidate() const override { return m_cache_candidate; }

    virtual QString GetActionDescription() const override;

    virtual bool PerformAction( RESCUER* aRescuer ) override;
};


class RESCUE_SYMBOL_LIB_TABLE_CANDIDATE : public RESCUE_CANDIDATE
{
public:
    /**
     * Grab all possible RESCUE_SYMBOL_LIB_TABLE_CANDIDATE objects into a vector.
     *
     * @param aRescuer is the working #RESCUER instance.
     * @param aCandidates is the vector the will hold the candidates.
     */
    static void FindRescues( RESCUER& aRescuer, boost::ptr_vector<RESCUE_CANDIDATE>& aCandidates );

    /**
     * Create RESCUE_CANDIDATE.
     *
     * @param aRequestedName is the name the schematic asks for.
     * @param aNewName is the name we want to change it to.
     * @param aCacheCandidate is the part from the cache.
     * @param aLibCandidate is the part that would be loaded from the library.
     * @param aUnit is the unit of the rescued symbol.
     * @param aConvert is the body style of the rescued symbol.
     */
    RESCUE_SYMBOL_LIB_TABLE_CANDIDATE( const LIB_ID& aRequestedId, const LIB_ID& aNewId,
                                       LIB_SYMBOL* aCacheCandidate, LIB_SYMBOL* aLibCandidate,
                                       int aUnit = 0, int aConvert = 0 );

    RESCUE_SYMBOL_LIB_TABLE_CANDIDATE();

    virtual LIB_SYMBOL* GetCacheCandidate() const override { return m_cache_candidate; }

    virtual QString GetActionDescription() const override;

    virtual bool PerformAction( RESCUER* aRescuer ) override;

private:
    LIB_ID m_requested_id;
    LIB_ID m_new_id;
    LIB_SYMBOL* m_cache_candidate;
};


class RESCUE_LOG
{
public:
    SCH_SYMBOL*  symbol;
    QString     old_name;
    QString     new_name;
};


class RESCUER
{
public:
    RESCUER( PROJECT& aProject, SCHEMATIC* aSchematic, SCH_SHEET_PATH* aCurrentSheet,
             EDA_DRAW_PANEL_GAL::GAL_TYPE aGalBackeEndType );

    virtual ~RESCUER()
    {
    }

    /**
     * Write the rescue library.
     *
     * Called after successful PerformAction()s. If this fails, undo the actions.
     *
     * @return True on success.
     */
    virtual bool WriteRescueLibrary( QWidget *aParent ) = 0;

    virtual void OpenRescueLibrary() = 0;

    /**
     * Populate the RESCUER with all possible candidates.
     */
    virtual void FindCandidates() = 0;

    virtual void AddSymbol( LIB_SYMBOL* aNewSymbol ) = 0;

    /**
     * Display a dialog to allow the user to select rescues.
     *
     * @param aAskShowAgain - whether the "Never Show Again" button should be visible
     */
    virtual void InvokeDialog( QWidget* aParent, bool aAskShowAgain ) = 0;

    /**
     * Filter out duplicately named rescue candidates.
     */
    void RemoveDuplicates();

    /**
     * Return the number of rescue candidates found.
     */
    size_t GetCandidateCount() { return m_all_candidates.size(); }

    /**
     * Get the number of rescue candidates chosen by the user.
     */
    size_t GetChosenCandidateCount() { return m_chosen_candidates.size(); }

    /**
     * Get the list of symbols that need rescued.
     */
    std::vector<SCH_SYMBOL*>* GetSymbols() { return &m_symbols; }

    /**
     * Return the #SCH_PROJECT object for access to the symbol libraries.
     */
    PROJECT* GetPrj() { return m_prj; }

    SCHEMATIC* Schematic() { return m_schematic; }

    /**
     * Used by individual #RESCUE_CANDIDATE objects to log a rescue for undoing.
     */
    void LogRescue( SCH_SYMBOL *aSymbol, const QString& aOldName, const QString& aNewName );

    /**
     * Perform all chosen rescue actions, logging them to be undone if necessary.
     *
     * @return True on success
     */
    bool DoRescues();

    /**
     * Reverse the effects of all rescues on the project.
     */
    void UndoRescues();

    static bool RescueProject( QWidget* aParent, RESCUER& aRescuer, bool aRunningOnDemand );

protected:
    friend class DIALOG_RESCUE_EACH;

    std::vector<SCH_SYMBOL*> m_symbols;
    PROJECT* m_prj;
    SCHEMATIC* m_schematic;
    EDA_DRAW_PANEL_GAL::GAL_TYPE m_galBackEndType;
    SCH_SHEET_PATH* m_currentSheet;

    boost::ptr_vector<RESCUE_CANDIDATE> m_all_candidates;
    std::vector<RESCUE_CANDIDATE*> m_chosen_candidates;

    std::vector<RESCUE_LOG> m_rescue_log;
};


class LEGACY_RESCUER : public RESCUER
{
public:
    LEGACY_RESCUER( PROJECT& aProject, SCHEMATIC* aSchematic, SCH_SHEET_PATH* aCurrentSheet,
                    EDA_DRAW_PANEL_GAL::GAL_TYPE aGalBackEndType ) :
        RESCUER( aProject, aSchematic, aCurrentSheet, aGalBackEndType )
    {
    }

    virtual ~LEGACY_RESCUER()
    {
    }

    virtual void FindCandidates() override;

    virtual void InvokeDialog( QWidget* aParent, bool aAskShowAgain ) override;

    virtual void OpenRescueLibrary() override;

    virtual bool WriteRescueLibrary( QWidget *aParent ) override;

    virtual void AddSymbol( LIB_SYMBOL* aNewSymbol ) override;

private:
    std::unique_ptr<SYMBOL_LIB> m_rescue_lib;
};


class SYMBOL_LIB_TABLE_RESCUER : public RESCUER
{
public:
    SYMBOL_LIB_TABLE_RESCUER( PROJECT& aProject, SCHEMATIC* aSchematic,
                              SCH_SHEET_PATH* aCurrentSheet,
                              EDA_DRAW_PANEL_GAL::GAL_TYPE aGalBackeEndType );

    virtual ~SYMBOL_LIB_TABLE_RESCUER()
    {
    }

    virtual void FindCandidates() override;

    virtual void InvokeDialog( QWidget* aParent, bool aAskShowAgain ) override;

    virtual void OpenRescueLibrary() override;

    virtual bool WriteRescueLibrary( QWidget* aParent ) override;

    virtual void AddSymbol( LIB_SYMBOL* aNewSymbol ) override;

private:
    std::vector<std::unique_ptr<LIB_SYMBOL>> m_rescueLibSymbols;

    std::unique_ptr<std::map<std::string, UTF8>> m_properties;   ///< Library plugin properties.
};

#endif // _LIB_CACHE_RESCUE_H_
