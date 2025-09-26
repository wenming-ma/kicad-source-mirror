
#include <pgm_base.h>
#include <clipboard.h>
#include <confirm.h>
#include <kidialog.h>
#include <kiway.h>
#include <widgets/qt_infobar.h>
#include <tools/symbol_editor_drawing_tools.h>
#include <symbol_edit_frame.h>
#include <symbol_library.h>
#include <template_fieldnames.h>
#include <wildcards_and_files_ext.h>
#include <symbol_lib_table.h>
#include <lib_symbol_library_manager.h>
#include <symbol_tree_pane.h>
#include <project/project_file.h>
#include <widgets/lib_tree.h>
#include <sch_io/kicad_legacy/sch_io_kicad_legacy.h>
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr.h>
#include <dialogs/dialog_lib_new_symbol.h>
#include <eda_list_dialog.h>
#include <QClipboard>
#include <QFileDialog>
#include <QDebug>
#include <QApplication>
#include <QGuiApplication>
#include <QMessageBox>
#include <QTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileInfo>
#include <QStringList>
#include <QScreen>
#include <project_sch.h>
#include <string_utils.h>
#include <i18n_utility.h>
#include "symbol_saveas_type.h"

#include <widgets/symbol_filedlg_save_as.h>
#include <io/kicad/kicad_io_utils.h>


void SYMBOL_EDIT_FRAME::UpdateTitle()
{
    QString title;

    if( GetCurSymbol() && IsSymbolFromSchematic() )
    {
        if( GetScreen() && GetScreen()->IsContentModified() )
            title = QStringLiteral( "*" );

        title += m_reference;
        title += QStringLiteral( " " ) + _( "[from schematic]" );
    }
    else if( GetCurSymbol() )
    {
        if( GetScreen() && GetScreen()->IsContentModified() )
            title = QStringLiteral( "*" );

        title += UnescapeString( GetCurSymbol()->GetLibId().Format() );

        if( m_libMgr && m_libMgr->LibraryExists( GetCurLib() ) && m_libMgr->IsLibraryReadOnly( GetCurLib() ) )
            title += QStringLiteral( " " ) + _( "[Read Only Library]" );
    }
    else
    {
        title = _( "[no symbol loaded]" );
    }

    title += QStringLiteral( " \u2014 " ) + _( "Symbol Editor" );
    setWindowTitle( title );
}


void SYMBOL_EDIT_FRAME::SelectActiveLibrary( const QString& aLibrary )
{
    QString selectedLib = aLibrary;

    if( selectedLib.isEmpty() )
        selectedLib = SelectLibraryFromList();

    if( !selectedLib.isEmpty() )
        SetCurLib( selectedLib );

    UpdateTitle();
}


bool SYMBOL_EDIT_FRAME::saveCurrentSymbol()
{
    if( GetCurSymbol() )
    {
        if( IsSymbolFromSchematic() )
        {
            SCH_EDIT_FRAME* schframe = (SCH_EDIT_FRAME*) Kiway().Player( FRAME_SCH, false );

            if( !schframe )      // happens when the schematic editor has been closed
            {
                DisplayErrorMessage( this, _( "No schematic currently open." ) );
                return false;
            }
            else
            {
                schframe->SaveSymbolToSchematic( *m_symbol, m_schematicSymbolUUID );
                GetScreen()->SetContentModified( false );
                return true;
            }
        }
        else
        {
            const QString& libName = GetCurSymbol()->GetLibId().GetLibNickname();

            if( m_libMgr->IsLibraryReadOnly( libName ) )
            {
                QString msg = QString::asprintf( _( "Symbol library '%s' is not writable." ).toUtf8().constData(),
                                                 libName.toUtf8().constData() );
                QString msg2 = _( "You must save to a different location." );

                if( OKOrCancelDialog( this, _( "Warning" ), msg, msg2 ) == QDialog::Accepted )
                    return saveLibrary( libName, true );
            }
            else
            {
                return saveLibrary( libName, false );
            }
        }
    }

    return false;
}


bool SYMBOL_EDIT_FRAME::LoadSymbol( const LIB_ID& aLibId, int aUnit, int aBodyStyle )
{
    LIB_ID libId = aLibId;

    // Some libraries can't be edited, so load the underlying chosen symbol
    if( SYMBOL_LIB_TABLE_ROW* lib = m_libMgr->GetLibrary( aLibId.GetLibNickname() ) )
    {
        if( lib->SchLibType() == SCH_IO_MGR::SCH_DATABASE
            || lib->SchLibType() == SCH_IO_MGR::SCH_CADSTAR_ARCHIVE
            || lib->SchLibType() == SCH_IO_MGR::SCH_HTTP )

        {
            try
            {
                LIB_SYMBOL* readOnlySym = PROJECT_SCH::SchSymbolLibTable( &Prj() )->LoadSymbol( aLibId );

                if( readOnlySym && readOnlySym->GetSourceLibId().IsValid() )
                    libId = readOnlySym->GetSourceLibId();
            }
            catch( const IO_ERROR& ioe )
            {
                QString msg;

                msg = QString::asprintf( _( "Error loading symbol %s from library '%s'." ).toUtf8().constData(),
                            aLibId.GetUniStringLibId().toUtf8().constData(), aLibId.GetUniStringLibItemName().toUtf8().constData() );
                DisplayErrorMessage( this, msg, ioe.What() );
                return false;
            }
        }
    }

    if( GetCurSymbol() && !IsSymbolFromSchematic()
            && GetCurSymbol()->GetLibId() == libId
            && GetUnit() == aUnit
            && GetBodyStyle() == aBodyStyle )
    {
        return true;
    }

    if( GetCurSymbol() && IsSymbolFromSchematic() && GetScreen()->IsContentModified() )
    {
        if( !HandleUnsavedChanges( this, _( "The current symbol has been modified.  Save changes?" ),
                                   [&]() -> bool
                                   {
                                       return saveCurrentSymbol();
                                   } ) )
        {
            return false;
        }
    }

    SelectActiveLibrary( libId.GetLibNickname() );

    if( LoadSymbolFromCurrentLib( libId.GetLibItemName(), aUnit, aBodyStyle ) )
    {
        m_treePane->GetLibTree()->SelectLibId( libId );
        m_treePane->GetLibTree()->ExpandLibId( libId );

        m_centerItemOnIdle = libId;
        if( !m_centerTimer )
        {
            m_centerTimer = new QTimer( this );
            connect( m_centerTimer, &QTimer::timeout, this, &SYMBOL_EDIT_FRAME::centerItemIdleHandler );
        }
        m_centerTimer->setSingleShot( true );
        m_centerTimer->start( 1 ); // Start immediately on next event loop iteration
        setSymWatcher( &libId );

        return true;
    }

    return false;
}


void SYMBOL_EDIT_FRAME::centerItemIdleHandler()
{
    m_treePane->GetLibTree()->CenterLibId( m_centerItemOnIdle );
    // No need to manually stop timer since it's set to single shot
}


bool SYMBOL_EDIT_FRAME::LoadSymbolFromCurrentLib( const QString& aAliasName, int aUnit,
                                                  int aBodyStyle )
{
    LIB_SYMBOL* alias = nullptr;

    try
    {
        alias = PROJECT_SCH::SchSymbolLibTable( &Prj() )->LoadSymbol( GetCurLib(), aAliasName );
    }
    catch( const IO_ERROR& ioe )
    {
        QString msg;

        msg = QString::asprintf( _( "Error loading symbol %s from library '%s'." ).toUtf8().constData(),
                    aAliasName.toUtf8().constData(),
                    GetCurLib().toUtf8().constData() );
        DisplayErrorMessage( this, msg, ioe.What() );
        return false;
    }

    if( !alias || !LoadOneLibrarySymbolAux( alias, GetCurLib(), aUnit, aBodyStyle ) )
        return false;

    // Enable synchronized pin edit mode for symbols with interchangeable units
    m_SyncPinEdit = GetCurSymbol()->IsMulti() && !GetCurSymbol()->UnitsLocked();

    ClearUndoRedoList();
    m_toolManager->RunAction( ACTIONS::zoomFitScreen );
    SetShowDeMorgan( GetCurSymbol()->Flatten()->HasAlternateBodyStyle() );

    if( aUnit > 0 )
        RebuildSymbolUnitsList();

    return true;
}


bool SYMBOL_EDIT_FRAME::LoadOneLibrarySymbolAux( LIB_SYMBOL* aEntry, const QString& aLibrary,
                                                 int aUnit, int aBodyStyle )
{
    bool rebuildMenuAndToolbar = false;

    if( !aEntry || aLibrary.isEmpty() )
        return false;

    if( aEntry->GetName().isEmpty() )
    {
        qWarning() << "Symbol in library" << aLibrary << "has empty name field.";
        return false;
    }

    m_toolManager->RunAction( ACTIONS::cancelInteractive );

    // Symbols from the schematic are edited in place and not managed by the library manager.
    if( IsSymbolFromSchematic() )
    {
        delete m_symbol;
        m_symbol = nullptr;

        SCH_SCREEN* screen = GetScreen();
        delete screen;
        SetScreen( m_dummyScreen );
        m_isSymbolFromSchematic = false;
        rebuildMenuAndToolbar = true;
    }

    LIB_SYMBOL* lib_symbol = m_libMgr->GetBufferedSymbol( aEntry->GetName(), aLibrary );
    Q_ASSERT( lib_symbol );

    m_unit = aUnit > 0 ? aUnit : 1;
    m_bodyStyle = aBodyStyle > 0 ? aBodyStyle : 1;

    // The buffered screen for the symbol
    SCH_SCREEN* symbol_screen = m_libMgr->GetScreen( lib_symbol->GetName(), aLibrary );

    SetScreen( symbol_screen );
    SetCurSymbol( new LIB_SYMBOL( *lib_symbol ), true );
    SetCurLib( aLibrary );

    if( rebuildMenuAndToolbar )
    {
        ReCreateMenuBar();
        ReCreateHToolbar();
        GetInfoBar()->Dismiss();
    }

    UpdateTitle();
    RebuildSymbolUnitsList();
    SetShowDeMorgan( GetCurSymbol()->HasAlternateBodyStyle() );

    ClearUndoRedoList();

    if( !IsSymbolFromSchematic() )
    {
        LIB_ID libId = GetCurSymbol()->GetLibId();
        setSymWatcher( &libId );
    }

    // Let tools add things to the view if necessary
    if( m_toolManager )
        m_toolManager->ResetTools( TOOL_BASE::MODEL_RELOAD );

    GetCanvas()->GetView()->UpdateAllItems( KIGFX::ALL );

    // Display the document information based on the entry selected just in
    // case the entry is an alias.
    UpdateMsgPanel();
    update(); // Qt equivalent of Refresh for widget repainting

    return true;
}


void SYMBOL_EDIT_FRAME::SaveAll()
{
    saveAllLibraries( false );
    m_treePane->GetLibTree()->RefreshLibTree();
}


void SYMBOL_EDIT_FRAME::CreateNewSymbol( const QString& aInheritFrom )
{
    m_toolManager->RunAction( ACTIONS::cancelInteractive );

    QStringList symbolNames;
    QString lib = getTargetLib();

    if( !m_libMgr->LibraryExists( lib ) )
    {
        lib = SelectLibraryFromList();

        if( !m_libMgr->LibraryExists( lib ) )
            return;
    }

    const auto validator = [&]( QString newName ) -> bool
            {
                if( newName.isEmpty() )
                {
                    QMessageBox::warning(this, QString(), _( "Symbol must have a name." ) );
                    return false;
                }

                if( !lib.isEmpty() && m_libMgr->SymbolExists( newName, lib ) )
                {
                    QString msg = QString::asprintf(
                            _( "Symbol '%s' already exists in library '%s'." ).toUtf8().constData(), newName.toUtf8().constData(), lib.toUtf8().constData() );

                    KIDIALOG errorDlg( this, msg, _( "Confirmation" ), QMessageBox::Ok | QMessageBox::Cancel );
                    errorDlg.setIcon( QMessageBox::Warning );
                    errorDlg.SetOKCancelLabels( _( "Overwrite" ), _( "Cancel" ) );

                    return errorDlg.exec() == QDialog::Accepted;
                }

                return true;
            };

    QStringList symbolNamesInLib;
    m_libMgr->GetSymbolNames( lib, symbolNamesInLib );

    DIALOG_LIB_NEW_SYMBOL dlg( this, symbolNamesInLib, aInheritFrom, validator );

    dlg.setMinimumSize( dlg.size() );

    if( dlg.exec() == QDialog::Rejected )
        return;

    QString name = dlg.GetName();

    LIB_SYMBOL new_symbol( name );  // do not create symbol on the heap, it will be buffered soon

    QString parentSymbolName = dlg.GetParentSymbolName();

    if( parentSymbolName.isEmpty() )
    {
        new_symbol.GetReferenceField().SetText( dlg.GetReference() );
        new_symbol.SetUnitCount( dlg.GetUnitCount() );

        // Initialize new_symbol.m_TextInside member:
        // if 0, pin text is outside the body (on the pin)
        // if > 0, pin text is inside the body
        if( dlg.GetPinNameInside() )
        {
            new_symbol.SetPinNameOffset( dlg.GetPinTextPosition() );

            if( new_symbol.GetPinNameOffset() == 0 )
                new_symbol.SetPinNameOffset( 1 );
        }
        else
        {
            new_symbol.SetPinNameOffset( 0 );
        }

        ( dlg.GetPowerSymbol() ) ? new_symbol.SetPower() : new_symbol.SetNormal();
        new_symbol.SetShowPinNumbers( dlg.GetShowPinNumber() );
        new_symbol.SetShowPinNames( dlg.GetShowPinName() );
        new_symbol.LockUnits( !dlg.GetUnitsInterchangeable() );
        new_symbol.SetExcludedFromBOM( !dlg.GetIncludeInBom() );
        new_symbol.SetExcludedFromBoard( !dlg.GetIncludeOnBoard() );

        if( dlg.GetUnitCount() < 2 )
            new_symbol.LockUnits( false );

        new_symbol.SetHasAlternateBodyStyle( dlg.GetAlternateBodyStyle() );
    }
    else
    {
        LIB_SYMBOL* parent = m_libMgr->GetAlias( parentSymbolName, lib );
        Q_ASSERT( parent );
        new_symbol.SetParent( parent );

        // Inherit the parent mandatory field attributes.
        for( int fieldId : MANDATORY_FIELDS )
        {
            SCH_FIELD* field = new_symbol.GetFieldById( fieldId );

            // the MANDATORY_FIELD_COUNT are exactly that in RAM.
            Q_ASSERT( field );

            SCH_FIELD* parentField = parent->GetFieldById( fieldId );

            Q_ASSERT( parentField );

            *field = *parentField;

            switch( fieldId )
            {
            case REFERENCE_FIELD:
                // parent's reference already copied
                break;

            case VALUE_FIELD:
                if( parent->IsPower() )
                    field->SetText( name );
                break;

            case FOOTPRINT_FIELD:
            case DATASHEET_FIELD:
                // - footprint might be the same as parent, but might not
                // - datasheet is most likely different
                // - probably best to play it safe and copy neither
                field->SetText( QString() );
                break;
            }

            field->SetParent( &new_symbol );
        }
    }

    m_libMgr->UpdateSymbol( &new_symbol, lib );
    SyncLibraries( false );
    LoadSymbol( name, lib, 1 );

    // must be called after loadSymbol, that calls SetShowDeMorgan, but
    // because the symbol is empty,it looks like it has no alternate body
    // and a derived symbol inherits its parent body.
    if( !new_symbol.GetParent().lock() )
        SetShowDeMorgan( dlg.GetAlternateBodyStyle() );
    else
        SetShowDeMorgan( new_symbol.HasAlternateBodyStyle() );
}


void SYMBOL_EDIT_FRAME::Save()
{
    QString libName;

    if( IsLibraryTreeShown() )
        libName = GetTreeLIBID().GetUniStringLibNickname();

    if( libName.isEmpty() )
    {
        saveCurrentSymbol();
    }
    else if( m_libMgr->IsLibraryReadOnly( libName ) )
    {
        QString msg = QString::asprintf( _( "Symbol library '%s' is not writable." ).toUtf8().constData(),
                                         libName.toUtf8().constData() );
        QString msg2 = _( "You must save to a different location." );

        if( OKOrCancelDialog( this, _( "Warning" ), msg, msg2 ) == QDialog::Accepted )
            saveLibrary( libName, true );
    }
    else
    {
        saveLibrary( libName, false );
    }

    if( IsLibraryTreeShown() )
        m_treePane->GetLibTree()->RefreshLibTree();

    UpdateTitle();
}


void SYMBOL_EDIT_FRAME::SaveLibraryAs()
{
    const QString& libName = GetTargetLibId().GetLibNickname();

    if( !libName.isEmpty() )
    {
        saveLibrary( libName, true );
        m_treePane->GetLibTree()->RefreshLibTree();
    }
}


void SYMBOL_EDIT_FRAME::SaveSymbolCopyAs( bool aOpenCopy )
{
    saveSymbolCopyAs( aOpenCopy );

    m_treePane->GetLibTree()->RefreshLibTree();
}


/**
 * Get a list of all the symbols in the parental chain of a symbol,
 * with the "leaf" symbol at the start and the "rootiest" symbol at the end.
 *
 * If the symbol is not an alias, the list will contain only the symbol itself.
 */
static std::vector<LIB_SYMBOL_SPTR> GetParentChain( const LIB_SYMBOL& aSymbol )
{
    std::vector<LIB_SYMBOL_SPTR> chain( { aSymbol.SharedPtr() } );

    while( chain.back()->IsAlias() )
    {
        LIB_SYMBOL_SPTR parent = chain.back()->GetParent().lock();
        chain.push_back( parent );
    }

    return chain;
}


/**
 * Check if a planned overwrite would put a symbol into it's own inheritance chain.
 * This causes infinite loops and other unpleasantness and makes not sense - inheritance
 * must be acyclic.
 *
 * Returns a pair of bools:
 *   - first: true if the symbol would be saved into it's own ancestry
 *   - second: true if the symbol would be saved into it's own descendents
 */
static std::pair<bool, bool> CheckSavingIntoOwnInheritance( LIB_SYMBOL_LIBRARY_MANAGER& aLibMgr,
                                                            LIB_SYMBOL&                 aSymbol,
                                                            const QString& aNewSymbolName,
                                                            const QString& aNewLibraryName )
{
    const QString& oldLibraryName = aSymbol.GetLibId().GetLibNickname();

    // Cannot be intersecting if in different libs
    if( aNewLibraryName != oldLibraryName )
        return { false, false };

    // Or if the target symbol doesn't exist
    if( !aLibMgr.SymbolExists( aNewSymbolName, aNewLibraryName ) )
        return { false, false };

    bool inAncestry = false;
    bool inDescendents = false;

    {
        const std::vector<LIB_SYMBOL_SPTR> parentChainFromUs = GetParentChain( aSymbol );

        // Ignore the leaf symbol (0) - that must match
        for( size_t i = 1; i < parentChainFromUs.size(); ++i )
        {
            // Attempting to overwrite a symbol in the parental chain
            if( parentChainFromUs[i]->GetName() == aNewSymbolName )
            {
                inAncestry = true;
                break;
            }
        }
    }

    {
        LIB_SYMBOL* targetSymbol = aLibMgr.GetAlias( aNewSymbolName, aNewLibraryName );
        const std::vector<LIB_SYMBOL_SPTR> parentChainFromTarget = GetParentChain( *targetSymbol );
        const QString                     oldSymbolName = aSymbol.GetName();

        // Ignore the leaf symbol - it'll match if we're saving the symbol
        // to the same name, and that would be OK
        for( size_t i = 1; i < parentChainFromTarget.size(); ++i )
        {
            if( parentChainFromTarget[i]->GetName() == oldSymbolName )
            {
                inDescendents = true;
                break;
            }
        }
    }

    return { inAncestry, inDescendents };
}


/**
 * Get a list of all the symbols in the parental chain of a symbol that have conflicts
 * when transposed to a different library.
 *
 * This doesn't check for dangerous conflicts like saving into a symbol's own inheritance,
 * this is just about which symbols will get overwritten if saved with these names.
 */
static std::vector<QString> CheckForParentalChainConflicts( LIB_SYMBOL_LIBRARY_MANAGER& aLibMgr,
                                                             LIB_SYMBOL&                 aSymbol,
                                                             const QString& newSymbolName,
                                                             const QString& newLibraryName )
{
    std::vector<QString> conflicts;
    const QString&       oldLibraryName = aSymbol.GetLibId().GetLibNickname();

    if( newLibraryName == oldLibraryName )
    {
        // Saving into the same library - the only conflict could be the symbol itself
        if( aLibMgr.SymbolExists( newSymbolName, newLibraryName ) )
        {
            conflicts.push_back( newSymbolName );
        }
    }
    else
    {
        // In a different library, check the whole chain
        const std::vector<LIB_SYMBOL_SPTR> parentChain = GetParentChain( aSymbol );

        for( size_t i = 0; i < parentChain.size(); ++i )
        {
            if( i == 0 )
            {
                // This is the leaf symbol which the user actually named
                if( aLibMgr.SymbolExists( newSymbolName, newLibraryName ) )
                {
                    conflicts.push_back( newSymbolName );
                }
            }
            else
            {
                LIB_SYMBOL_SPTR chainSymbol = parentChain[i];
                if( aLibMgr.SymbolExists( chainSymbol->GetName(), newLibraryName ) )
                {
                    conflicts.push_back( chainSymbol->GetName() );
                }
            }
        }
    }

    return conflicts;
}


/**
 * This is a class that handles state involved in saving a symbol copy as a new symbol.
 *
 * This isn't as simple as it sounds, because you also have to copy all the parent symbols
 * (which is a chain affair).
 */
class SYMBOL_SAVE_AS_HANDLER
{
public:
    enum class CONFLICT_STRATEGY
    {
        // Just overwrite any existing symbols in the target library
        OVERWRITE,
        // Add a suffix until we find a name that doesn't conflict
        RENAME,
        // Could have a mode that asks for every one, be then we'll need a fancier
        // SAVE_AS_DIALOG subdialog with Overwrite/Rename/Prompt/Cancel
        // PROMPT
    };

    SYMBOL_SAVE_AS_HANDLER( LIB_SYMBOL_LIBRARY_MANAGER& aLibMgr, CONFLICT_STRATEGY aStrategy,
                            bool aValueFollowsName ) :
            m_libMgr( aLibMgr ),
            m_strategy( aStrategy ),
            m_valueFollowsName( aValueFollowsName )
    {
    }

    bool DoSave( LIB_SYMBOL& symbol, const QString& aNewSymName, const QString& aNewLibName )
    {
        std::vector<LIB_SYMBOL_SPTR> parentChain;
        // If we're saving into the same library, we don't need to check the parental chain
        // because we can just keep the same parent symbol
        if( aNewLibName == QString(symbol.GetLibId().GetLibNickname()) )
            parentChain.push_back( symbol.SharedPtr() );
        else
            parentChain = GetParentChain( symbol );

        std::vector<QString> newNames;

        // Iterate backwards (i.e. from the root down)
        for( int i = (int) parentChain.size() - 1; i >= 0; --i )
        {
            LIB_SYMBOL_SPTR& oldSymbol = parentChain[i];

            LIB_SYMBOL new_symbol( *oldSymbol );

            QString newName;
            if( i == 0 )
            {
                // This is the leaf symbol which the user actually named
                newName = aNewSymName;
            }
            else
            {
                // Somewhere in the inheritance chain, use the conflict resolution strategy
                newName = oldSymbol->GetName();
            }

            newName = resolveConflict( newName, aNewLibName );
            new_symbol.SetName( newName );

            if( m_valueFollowsName )
                new_symbol.GetValueField().SetText( newName );

            if( i == (int) parentChain.size() - 1 )
            {
                // This is the root symbol
                // Nothing extra to do, it's just a simple symbol with no parents
            }
            else
            {
                // Get the buffered new copy in the new library (with the name we gave it)
                LIB_SYMBOL* newParent = m_libMgr.GetAlias( newNames.back(), aNewLibName );

                // We should have stored this already, why didn't we get it back?
                Q_ASSERT( newParent );
                new_symbol.SetParent( newParent );
            }

            newNames.push_back( newName );
            m_libMgr.UpdateSymbol( &new_symbol, aNewLibName );
        }

        return true;
    }

private:
    QString resolveConflict( const QString& proposed, const QString& aNewLibName ) const
    {
        switch( m_strategy )
        {
        case CONFLICT_STRATEGY::OVERWRITE:
        {
            // In an overwrite strategy, we don't care about conflicts
            return proposed;
        }
        case CONFLICT_STRATEGY::RENAME:
        {
            // In a rename strategy, we need to find a name that doesn't conflict
            int suffix = 1;

            while( true )
            {
                    QString newName = QString::asprintf( "%s_%d", proposed.toUtf8().constData(), suffix );

                if( !m_libMgr.SymbolExists( newName, aNewLibName ) )
                    return newName;

                ++suffix;
            }
            break;
        }
            // No default
        }

        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid conflict strategy" );
        return "";
    }

    LIB_SYMBOL_LIBRARY_MANAGER& m_libMgr;
    CONFLICT_STRATEGY           m_strategy;
    bool                        m_valueFollowsName;
};


enum SAVE_AS_IDS
{
    ID_MAKE_NEW_LIBRARY = QDialog::DialogCode::Accepted + 1000 + 1,
    ID_OVERWRITE_CONFLICTS,
    ID_RENAME_CONFLICTS,
};


class SAVE_AS_DIALOG : public EDA_LIST_DIALOG
{
public:
    using SymLibNameValidator =
            std::function<int( const QString& libName, const QString& symbolName )>;

    SAVE_AS_DIALOG( SYMBOL_EDIT_FRAME* aParent, const QString& aSymbolName,
                    const QString& aLibraryPreselect, SymLibNameValidator aValidator,
                    SYMBOL_SAVE_AS_HANDLER::CONFLICT_STRATEGY& aConflictStrategy ) :
            EDA_LIST_DIALOG( aParent, _( "Save Symbol As" ), false ),
            m_validator( std::move( aValidator ) ), m_conflictStrategy( aConflictStrategy )
    {
        COMMON_SETTINGS*           cfg = Pgm().GetCommonSettings();
        PROJECT_FILE&              project = aParent->Prj().GetProjectFile();
        SYMBOL_LIB_TABLE*          tbl = PROJECT_SCH::SchSymbolLibTable( &Prj() );
        std::vector<QString>      libNicknames = tbl->GetLogicalLibs();
        QStringList              headers;
        std::vector<QStringList> itemsToDisplay;

        headers.append( _( "Nickname" ) );
        headers.append( _( "Description" ) );

        for( const QString& nickname : libNicknames )
        {
            if( alg::contains( project.m_PinnedSymbolLibs, nickname )
                || alg::contains( cfg->m_Session.pinned_symbol_libs, nickname ) )
            {
                QStringList item;
                item.append( LIB_TREE_MODEL_ADAPTER::GetPinningSymbol() + nickname );
                item.append( tbl->GetDescription( nickname ) );
                itemsToDisplay.push_back( item );
            }
        }

        for( const QString& nickname : libNicknames )
        {
            if( !alg::contains( project.m_PinnedSymbolLibs, nickname )
                    && !alg::contains( cfg->m_Session.pinned_symbol_libs, nickname ) )
            {
                QStringList item;
                item.append( nickname );
                item.append( tbl->GetDescription( nickname ) );
                itemsToDisplay.push_back( item );
            }
        }

        initDialog( headers, itemsToDisplay, aLibraryPreselect );

        SetListLabel( _( "Save in library:" ) );
        SetOKLabel( _( "Save" ) );

        QHBoxLayout* bNameSizer = new QHBoxLayout(); // Qt layouts don't need orientation for HBox

        QLabel* label = new QLabel( _( "Name:" ), this );
        bNameSizer->addWidget( label, 0, Qt::AlignVCenter );

        m_symbolNameCtrl = new QLineEdit( UnescapeString( aSymbolName ), this );
        bNameSizer->addWidget( m_symbolNameCtrl, 1, Qt::AlignVCenter );

        QPushButton* newLibraryButton = new QPushButton( _( "New Library..." ), this );
        m_ButtonsSizer->insertSpacing( 0, 80 );
        m_ButtonsSizer->insertWidget( 0, newLibraryButton, 0, Qt::AlignVCenter );

        if( QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout()) )
            mainLayout->insertLayout( 0, bNameSizer );

        connect( newLibraryButton, &QPushButton::clicked,
                [this]()
                {
                    done( ID_MAKE_NEW_LIBRARY );
                } );

        // Set tab order to put nameTextCtrl first
        setTabOrder( m_symbolNameCtrl, nullptr );

        SetInitialFocus( m_symbolNameCtrl );

        SetupStandardButtons();

        layout()->update();
        adjustSize();

        // Center the dialog using QGuiApplication::primaryScreen()
        QScreen* primaryScreen = QGuiApplication::primaryScreen();
        if( primaryScreen )
            move( primaryScreen->geometry().center() - rect().center() );
    }

    QString GetSymbolName()
    {
        QString symbolName = m_symbolNameCtrl->text();
        symbolName = symbolName.trimmed(); // Qt trimmed() removes both leading and trailing whitespace
        symbolName.replace( " ", "_" );
        return EscapeString( symbolName, CTX_LIBID );
    }

protected:
    bool TransferDataFromWindow() override
    {
        int ret = m_validator( GetTextSelection(), GetSymbolName() );

        if( ret == QDialog::Rejected )
            return false;

        if( ret == ID_OVERWRITE_CONFLICTS )
            m_conflictStrategy = SYMBOL_SAVE_AS_HANDLER::CONFLICT_STRATEGY::OVERWRITE;
        else if( ret == ID_RENAME_CONFLICTS )
            m_conflictStrategy = SYMBOL_SAVE_AS_HANDLER::CONFLICT_STRATEGY::RENAME;

        return true;
    }

private:
    QLineEdit*                                m_symbolNameCtrl;
    SymLibNameValidator                        m_validator;
    SYMBOL_SAVE_AS_HANDLER::CONFLICT_STRATEGY& m_conflictStrategy;
};


void SYMBOL_EDIT_FRAME::saveSymbolCopyAs( bool aOpenCopy )
{
    LIB_SYMBOL* symbol = getTargetSymbol();

    if( !symbol )
        return;

    LIB_ID   old_lib_id = symbol->GetLibId();
    QString symbolName = old_lib_id.GetLibItemName();
    QString libraryName = old_lib_id.GetLibNickname();
    bool     valueFollowsName = symbol->GetValueField().GetText() == symbolName;
    QString msg;
    bool     done = false;

    // This is the function that will be called when the user clicks OK in the dialog and checks
    // if the proposed name has problems, and asks for clarification.
    const auto dialogValidatorFunc =
            [&]( const QString& newLib, const QString& newName ) -> int
            {
                if( newLib.isEmpty() )
                {
                    QMessageBox::warning(this, QString(), _( "A library must be specified." ) );
                    return QDialog::Rejected;
                }

                if( newName.isEmpty() )
                {
                    QMessageBox::warning(this, QString(), _( "Symbol must have a name." ) );
                    return QDialog::Rejected;
                }

                if( m_libMgr->IsLibraryReadOnly( newLib ) )
                {
                    msg = QString::asprintf( _( "Library '%s' is read-only. Choose a "
                                               "different library to save the symbol '%s' to." ).toUtf8().constData(),
                                            newLib.toUtf8().constData(),
                                            UnescapeString( newName ).toUtf8().constData() );
                    QMessageBox::warning(this, QString(), msg );
                    return QDialog::Rejected;
                }

                /**
                 * If we save over a symbol that is in the inheritance chain of the symbol we're
                 * saving, we'll end up with a circular inheritance chain, which is bad.
                 */
                const auto& [inAncestry, inDescendents] =
                        CheckSavingIntoOwnInheritance( *m_libMgr, *symbol, newName, newLib );

                if( inAncestry )
                {
                    msg = QString::asprintf( _( "Symbol '%s' cannot replace another symbol '%s' "
                                               "that it descends from" ).toUtf8().constData(),
                                            symbolName.toUtf8().constData(),
                                            UnescapeString( newName ).toUtf8().constData() );
                    QMessageBox::warning(this, QString(), msg );
                    return QDialog::Rejected;
                }

                if( inDescendents )
                {
                    msg = QString::asprintf( _( "Symbol '%s' cannot replace another symbol '%s' "
                                               "that is a descendent of it." ).toUtf8().constData(),
                                            symbolName.toUtf8().constData(),
                                            UnescapeString( newName ).toUtf8().constData() );
                    QMessageBox::warning(this, QString(), msg );
                    return QDialog::Rejected;
                }

                const std::vector<QString> conflicts =
                        CheckForParentalChainConflicts( *m_libMgr, *symbol, newName, newLib );

                if( conflicts.size() == 1 && conflicts.front() == newName )
                {
                    // The simplest case is when the symbol itself has a conflict
                    msg = QString::asprintf( _( "Symbol '%s' already exists in library '%s'. "
                                               "Do you want to overwrite it?" ).toUtf8().constData(),
                                            UnescapeString( newName ).toUtf8().constData(),
                                            newLib.toUtf8().constData() );

                    KIDIALOG errorDlg( this, msg, _( "Confirmation" ),
                                       QMessageBox::Ok | QMessageBox::Cancel );
                    errorDlg.setIcon( QMessageBox::Warning );
                    errorDlg.SetOKCancelLabels( _( "Overwrite" ), _( "Cancel" ) );

                    return errorDlg.exec() == QDialog::Accepted ? ID_OVERWRITE_CONFLICTS
                                                           : (int) QDialog::Rejected;
                }
                else if( !conflicts.empty() )
                {
                    // If there are conflicts in the parental chain, we need to ask the user
                    // if they want to overwrite all of them.
                    // A more complex UI might allow the user to re-parent the symbol to an
                    // existing symbol in the target lib, or rename all the parents somehow.
                    msg = QString::asprintf( _( "The following symbols in the inheritance chain of "
                                               "'%s' already exist in library '%s':\n" ).toUtf8().constData(),
                                            UnescapeString( symbolName ).toUtf8().constData(),
                                            newLib.toUtf8().constData() );

                    for( const QString& conflict : conflicts )
                        msg += QString::asprintf( "  %s\n", conflict.toUtf8().constData() );

                    msg += _( "\nDo you want to overwrite all of them, or rename the new symbols?" );

                    KIDIALOG errorDlg( this, msg, _( "Confirmation" ),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
                    errorDlg.setIcon( QMessageBox::Warning );
                    // Note: SetYesNoCancelLabels may not be available in Qt version
                    // Using default button labels for now

                    switch( errorDlg.exec() )
                    {
                    case QMessageBox::Yes: return ID_OVERWRITE_CONFLICTS;
                    case QMessageBox::No:  return ID_RENAME_CONFLICTS;
                    default:       return QDialog::Rejected;
                    }
                }

                return QDialog::Accepted;
            };

    auto strategy = SYMBOL_SAVE_AS_HANDLER::CONFLICT_STRATEGY::OVERWRITE;

    // Keep asking the user for a new name until they give a valid one or cancel the operation
    while( !done )
    {
        // TODO: Fix SAVE_AS_DIALOG abstract class issue
        // SAVE_AS_DIALOG dlg( this, symbolName, libraryName, dialogValidatorFunc, strategy );
        // For now, create a simple placeholder dialog
        QDialog dlg( this );

        int ret = dlg.exec();

        switch( ret )
        {
        case QDialog::Rejected:
            return;

        case QDialog::Accepted: // No conflicts
        case ID_OVERWRITE_CONFLICTS:
        case ID_RENAME_CONFLICTS:
            // TODO: Implement proper dialog methods once SAVE_AS_DIALOG is fixed
            // symbolName = dlg.GetSymbolName();
            // libraryName = dlg.GetTextSelection();
            symbolName = QString("placeholder_symbol");
            libraryName = QString("placeholder_lib");

            if( ret == ID_RENAME_CONFLICTS )
                strategy = SYMBOL_SAVE_AS_HANDLER::CONFLICT_STRATEGY::RENAME;

            done = true;
            break;

        case ID_MAKE_NEW_LIBRARY:
        {
            QFileInfo newLibrary( AddLibraryFile( true ) );
            libraryName = newLibrary.baseName();
            break;
        }

        default:
            break;
        }
    }

    SYMBOL_SAVE_AS_HANDLER saver( *m_libMgr, strategy, valueFollowsName );

    saver.DoSave( *symbol, symbolName, libraryName );

    SyncLibraries( false );

    if( aOpenCopy )
        LoadSymbol( symbolName, libraryName, 1 );
}


void SYMBOL_EDIT_FRAME::ExportSymbol()
{
    QString msg;
    LIB_SYMBOL* symbol = getTargetSymbol();

    if( !symbol )
    {
        ShowInfoBarError( _( "There is no symbol selected to save." ) );
        return;
    }

    QString baseName = symbol->GetName().toLower();
    QString fileName = baseName + "." + QString::fromStdString(FILEEXT::KiCadSymbolLibFileExtension);

    QFileDialog dlg( this );
    dlg.setWindowTitle( _( "Export Symbol" ) );
    dlg.setDirectory( m_mruPath );
    dlg.selectFile( fileName );
    dlg.setNameFilter( FILEEXT::KiCadSymbolLibFileWildcard() );
    dlg.setAcceptMode( QFileDialog::AcceptSave );

    if( dlg.exec() == QDialog::Rejected )
        return;

    auto strategy = SYMBOL_SAVE_AS_HANDLER::CONFLICT_STRATEGY::OVERWRITE;

    QString selectedPath = dlg.selectedFiles().isEmpty() ? QString() : dlg.selectedFiles().first();
    QFileInfo fn( selectedPath );
    QString absolutePath = fn.absoluteFilePath();

    QString                    libraryName;
    std::unique_ptr<LIB_SYMBOL> flattenedSymbol = symbol->Flatten();

    for( const QString& candidate : m_libMgr->GetLibraryNames() )
    {
        if( m_libMgr->GetLibrary( candidate )->GetFullURI( true ) == absolutePath )
            libraryName = candidate;
    }

    if( !libraryName.isEmpty() )
    {
        SYMBOL_SAVE_AS_HANDLER saver( *m_libMgr, strategy, false );

        if( m_libMgr->IsLibraryReadOnly( libraryName ) )
        {
            msg = QString::asprintf( _( "Library '%s' is read-only." ).toUtf8().constData(), libraryName.toUtf8().constData() );
            DisplayError( this, msg );
            return;
        }

        if( m_libMgr->SymbolExists( symbol->GetName(), libraryName ) )
        {
            msg = QString::asprintf( _( "Symbol '%s' already exists in library '%s'." ).toUtf8().constData(),
                                    symbol->GetName().toUtf8().constData(), libraryName.toUtf8().constData() );

            KIDIALOG errorDlg( this, msg, _( "Confirmation" ), QMessageBox::Ok | QMessageBox::Cancel );
            errorDlg.setIcon( QMessageBox::Warning );
            errorDlg.SetOKCancelLabels( _( "Overwrite" ), _( "Cancel" ) );
            errorDlg.DoNotShowCheckbox( __FILE__, __LINE__ );

            if( errorDlg.exec() == QDialog::Rejected )
                return;
        }

        saver.DoSave( *flattenedSymbol, symbol->GetName(), libraryName );

        SyncLibraries( false );
        return;
    }

    LIB_SYMBOL* old_symbol = nullptr;
    SCH_IO_MGR::SCH_FILE_T pluginType = SCH_IO_MGR::GuessPluginTypeFromLibPath( absolutePath );

    if( pluginType == SCH_IO_MGR::SCH_FILE_UNKNOWN )
        pluginType = SCH_IO_MGR::SCH_KICAD;

    IO_RELEASER<SCH_IO> pi( SCH_IO_MGR::FindPlugin( pluginType ) );

    if( fn.exists() )
    {
        try
        {
            old_symbol = pi->LoadSymbol( absolutePath, symbol->GetName() );
        }
        catch( const IO_ERROR& ioe )
        {
            msg = QString::asprintf( _( "Error occurred attempting to load symbol library file '%s'." ).toUtf8().constData(),
                        absolutePath.toUtf8().constData() );
            DisplayErrorMessage( this, msg, ioe.What() );
            return;
        }

        if( old_symbol )
        {
            msg = QString::asprintf( _( "Symbol %s already exists in library '%s'." ).toUtf8().constData(),
                        UnescapeString( symbol->GetName() ).toUtf8().constData(),
                        fn.fileName().toUtf8().constData() );

            KIDIALOG errorDlg( this, msg, _( "Confirmation" ), QMessageBox::Ok | QMessageBox::Cancel );
            errorDlg.setIcon( QMessageBox::Warning );
            errorDlg.SetOKCancelLabels( _( "Overwrite" ), _( "Cancel" ) );
            errorDlg.DoNotShowCheckbox( __FILE__, __LINE__ );

            if( errorDlg.exec() == QDialog::Rejected )
                return;
        }
    }

    if( !fn.dir().isReadable() )
    {
        msg = QString::asprintf( _( "Insufficient permissions to save library '%s'." ).toUtf8().constData(), absolutePath.toUtf8().constData() );
        DisplayError( this, msg );
        return;
    }

    try
    {
        if( !fn.exists() )
            pi->CreateLibrary( absolutePath );

        // The flattened symbol is most likely what the user would want.  As some point in
        // the future as more of the symbol library inheritance is implemented, this may have
        // to be changes to save symbols of inherited symbols.
        pi->SaveSymbol( absolutePath, flattenedSymbol.release() );
    }
    catch( const IO_ERROR& ioe )
    {
        msg = QString::asprintf( _( "Failed to create symbol library file '%s'." ).toUtf8().constData(), absolutePath.toUtf8().constData() );
        DisplayErrorMessage( this, msg, ioe.What() );
        msg = QString::asprintf( _( "Error creating symbol library '%s'." ).toUtf8().constData(), fn.fileName().toUtf8().constData() );
        // SetStatusText( msg ); // TODO: Implement status text functionality
        return;
    }

    m_mruPath = fn.dir().path();

    msg = QString::asprintf( _( "Symbol %s saved to library '%s'." ).toUtf8().constData(),
                UnescapeString( symbol->GetName() ).toUtf8().constData(),
                absolutePath.toUtf8().constData() );
    // SetStatusText( msg ); // TODO: Implement status text functionality
}


void SYMBOL_EDIT_FRAME::UpdateAfterSymbolProperties( QString* aOldName )
{
    Q_ASSERT( m_symbol );

    QString lib = GetCurLib();

    if( !lib.isEmpty() && aOldName && *aOldName != m_symbol->GetName() )
    {
        // Test the current library for name conflicts
        if( m_libMgr->SymbolExists( m_symbol->GetName(), lib ) )
        {
            QString msg = QString::asprintf( _( "Symbol name '%s' already in use." ).toUtf8().constData(),
                                             UnescapeString( m_symbol->GetName() ).toUtf8().constData() );

            DisplayErrorMessage( this, msg );
            m_symbol->SetName( *aOldName );
        }
        else
        {
            m_libMgr->UpdateSymbolAfterRename( m_symbol, *aOldName, lib );
        }

        // Reselect the renamed symbol
        m_treePane->GetLibTree()->SelectLibId( LIB_ID( lib, m_symbol->GetName() ) );
    }

    RebuildSymbolUnitsList();
    SetShowDeMorgan( GetCurSymbol()->Flatten()->HasAlternateBodyStyle() );
    UpdateTitle();

    // N.B. The view needs to be rebuilt first as the Symbol Properties change may invalidate
    // the view pointers by rebuilting the field table
    RebuildView();
    UpdateMsgPanel();

    OnModify();
}


void SYMBOL_EDIT_FRAME::DeleteSymbolFromLibrary()
{
    std::vector<LIB_ID> toDelete = GetSelectedLibIds();

    if( toDelete.empty() )
        toDelete.emplace_back( GetTargetLibId() );

    for( LIB_ID& libId : toDelete )
    {
        if( m_libMgr->IsSymbolModified( libId.GetLibItemName(), libId.GetLibNickname() )
            && !IsOK( this, QString::asprintf( _( "The symbol '%s' has been modified.\n"
                                                 "Do you want to remove it from the library?" ).toUtf8().constData(),
                                              libId.GetUniStringLibItemName().toUtf8().constData() ) ) )
        {
            continue;
        }

        if( m_libMgr->HasDerivedSymbols( libId.GetLibItemName(), libId.GetLibNickname() ) )
        {
            QString msg;

            msg = QString::asprintf(
                    _( "The symbol %s is used to derive other symbols.\n"
                       "Deleting this symbol will delete all of the symbols derived from it.\n\n"
                       "Do you wish to delete this symbol and all of its derivatives?" ).toUtf8().constData(),
                    libId.GetLibItemName().c_str() );

            QMessageBox dlg( this );
            dlg.setWindowTitle( _( "Warning" ) );
            dlg.setText( msg );
            dlg.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
            dlg.setDefaultButton( QMessageBox::No );
            dlg.button( QMessageBox::Yes )->setText( _( "Delete Symbol" ) );
            dlg.button( QMessageBox::No )->setText( _( "Keep Symbol" ) );

            if( dlg.exec() == QMessageBox::No )
                continue;
        }

        if( IsCurrentSymbol( libId ) )
            emptyScreen();

        m_libMgr->RemoveSymbol( libId.GetLibItemName(), libId.GetLibNickname() );
    }

    m_treePane->GetLibTree()->RefreshLibTree();
}


void SYMBOL_EDIT_FRAME::CopySymbolToClipboard()
{
    std::vector<LIB_ID> symbols;

    if( GetTreeLIBIDs( symbols ) == 0 )
        return;

    STRING_FORMATTER formatter;

    for( LIB_ID& libId : symbols )
    {
        LIB_SYMBOL* symbol = m_libMgr->GetBufferedSymbol( libId.GetLibItemName(),
                                                          libId.GetLibNickname() );

        if( !symbol )
            continue;

        std::unique_ptr<LIB_SYMBOL> tmp = symbol->Flatten();
        SCH_IO_KICAD_SEXPR::FormatLibSymbol( tmp.get(), formatter );
    }

    std::string prettyData = formatter.GetString();
    KICAD_FORMAT::Prettify( prettyData, true );

    // Set clipboard data using Qt API
    auto clipboard = QApplication::clipboard();
    clipboard->setText( QString::fromStdString( prettyData ) );
}


void SYMBOL_EDIT_FRAME::DuplicateSymbol( bool aFromClipboard )
{
    LIB_ID libId = GetTargetLibId();
    QString lib = libId.GetLibNickname();

    if( !m_libMgr->LibraryExists( lib ) )
        return;

    std::vector<LIB_SYMBOL*> newSymbols;

    if( aFromClipboard )
    {
        std::string clipboardData = GetClipboardUTF8();

        try
        {
            newSymbols = SCH_IO_KICAD_SEXPR::ParseLibSymbols( clipboardData, "Clipboard" );
        }
        catch( IO_ERROR& e )
        {
            qDebug() << "Can not paste:" << e.Problem();
        }
    }
    else if( LIB_SYMBOL* srcSymbol = m_libMgr->GetBufferedSymbol( libId.GetLibItemName(), lib ) )
    {
        newSymbols.emplace_back( new LIB_SYMBOL( *srcSymbol ) );

        // Derive from same parent.
        if( srcSymbol->IsAlias() )
        {
            if( std::shared_ptr<LIB_SYMBOL> srcParent = srcSymbol->GetParent().lock() )
                newSymbols.back()->SetParent( srcParent.get() );
        }
    }

    if( newSymbols.empty() )
        return;

    for( LIB_SYMBOL* symbol : newSymbols )
    {
        ensureUniqueName( symbol, lib );
        m_libMgr->UpdateSymbol( symbol, lib );

        LoadOneLibrarySymbolAux( symbol, lib, GetUnit(), GetBodyStyle() );
    }

    SyncLibraries( false );
    m_treePane->GetLibTree()->SelectLibId( LIB_ID( lib, newSymbols[0]->GetName() ) );

    for( LIB_SYMBOL* symbol : newSymbols )
        delete symbol;
}


void SYMBOL_EDIT_FRAME::ensureUniqueName( LIB_SYMBOL* aSymbol, const QString& aLibrary )
{
    if( aSymbol )
    {
        int      i = 1;
        QString newName = aSymbol->GetName();

        // Append a number to the name until the name is unique in the library.
        while( m_libMgr->SymbolExists( newName, aLibrary ) )
            newName = QString::asprintf( "%s_%d", aSymbol->GetName().toUtf8().constData(), i++ );

        aSymbol->SetName( newName );
    }
}


void SYMBOL_EDIT_FRAME::Revert( bool aConfirm )
{
    LIB_ID libId = GetTargetLibId();
    const QString& libName = libId.GetLibNickname();

    // Empty if this is the library itself that is selected.
    const QString& symbolName = libId.GetLibItemName();

    QString msg = QString::asprintf( _( "Revert '%s' to last version saved?" ).toUtf8().constData(),
                                     (symbolName.isEmpty() ? libName : symbolName).toUtf8().constData() );

    if( aConfirm && !ConfirmRevertDialog( this, msg ) )
        return;

    bool reload_currentSymbol = false;
    QString curr_symbolName = symbolName;

    if( GetCurSymbol() )
    {
        // the library itself is reverted: the current symbol will be reloaded only if it is
        // owned by this library
        if( symbolName.isEmpty() )
        {
            LIB_ID curr_libId = GetCurSymbol()->GetLibId();
            reload_currentSymbol = (libName == QString(curr_libId.GetLibNickname()));

            if( reload_currentSymbol )
                curr_symbolName = QString(curr_libId.GetUniStringLibItemName());
        }
        else
        {
            reload_currentSymbol = IsCurrentSymbol( libId );
        }
    }

    int unit = m_unit;

    if( reload_currentSymbol )
        emptyScreen();

    if( symbolName.isEmpty() )
    {
        m_libMgr->RevertLibrary( libName );
    }
    else
    {
        libId = m_libMgr->RevertSymbol( libId.GetLibItemName(), libId.GetLibNickname() );

        m_treePane->GetLibTree()->SelectLibId( libId );
        m_libMgr->ClearSymbolModified( libId.GetLibItemName(), libId.GetLibNickname() );
    }

    if( reload_currentSymbol && m_libMgr->SymbolExists( curr_symbolName, libName ) )
        LoadSymbol( curr_symbolName, libName, unit );

    m_treePane->GetLibTree()->RefreshLibTree();
}


void SYMBOL_EDIT_FRAME::RevertAll()
{
    Q_ASSERT( m_libMgr, "Library manager object not created." );

    Revert( false );
    m_libMgr->RevertAll();
}


void SYMBOL_EDIT_FRAME::LoadSymbol( const QString& aAlias, const QString& aLibrary, int aUnit )
{
    if( GetCurSymbol() && IsSymbolFromSchematic() && GetScreen()->IsContentModified() )
    {
        if( !HandleUnsavedChanges( this, _( "The current symbol has been modified.  Save changes?" ),
                                   [&]() -> bool
                                   {
                                       return saveCurrentSymbol();
                                   } ) )
        {
            return;
        }
    }

    LIB_SYMBOL* symbol = m_libMgr->GetBufferedSymbol( aAlias, aLibrary );

    if( !symbol )
    {
        DisplayError( this, QString::asprintf( _( "Symbol %s not found in library '%s'." ).toUtf8().constData(),
                                              aAlias.toUtf8().constData(),
                                              aLibrary.toUtf8().constData() ) );
        m_treePane->GetLibTree()->RefreshLibTree();
        return;
    }

    // Optimize default edit options for this symbol
    // Usually if units are locked, graphic items are specific to each unit
    // and if units are interchangeable, graphic items are common to units
    SYMBOL_EDITOR_DRAWING_TOOLS* tools = GetToolManager()->GetTool<SYMBOL_EDITOR_DRAWING_TOOLS>();
    tools->SetDrawSpecificUnit( symbol->UnitsLocked() );

    LoadOneLibrarySymbolAux( symbol, aLibrary, aUnit, 0 );
}


bool SYMBOL_EDIT_FRAME::saveLibrary( const QString& aLibrary, bool aNewFile )
{
    QFileInfo fn;
    QString   msg;
    SYMBOL_SAVEAS_TYPE     type = SYMBOL_SAVEAS_TYPE::NORMAL_SAVE_AS;
    SCH_IO_MGR::SCH_FILE_T fileType = SCH_IO_MGR::SCH_FILE_T::SCH_KICAD;
    PROJECT&   prj = Prj();

    m_toolManager->RunAction( ACTIONS::cancelInteractive );

    if( !aNewFile && ( aLibrary.isEmpty() || !PROJECT_SCH::SchSymbolLibTable( &prj )->HasLibrary( aLibrary ) ) )
    {
        ShowInfoBarError( _( "No library specified." ) );
        return false;
    }

    if( aNewFile )
    {
        SEARCH_STACK* search = PROJECT_SCH::SchSearchS( &prj );

        // Get a new name for the library
        QString default_path = prj.GetRString( PROJECT::SCH_LIB_PATH );

        if( default_path.isEmpty() )
            default_path = QString::fromStdString(search->LastVisitedPath());

        QString defaultFileName = aLibrary + "." + QString::fromStdString(FILEEXT::KiCadSymbolLibFileExtension);

        QString wildcards = FILEEXT::KiCadSymbolLibFileWildcard();

        QFileDialog dlg( this );
        dlg.setWindowTitle( QString::asprintf( _( "Save Library '%s' As..." ).toUtf8().constData(), aLibrary.toUtf8().constData() ) );
        dlg.setDirectory( default_path );
        dlg.selectFile( defaultFileName );
        dlg.setNameFilter( wildcards );
        dlg.setAcceptMode( QFileDialog::AcceptSave );
        dlg.setOption( QFileDialog::DontConfirmOverwrite, true );

        SYMBOL_FILEDLG_SAVE_AS saveAsHook( type );
        // TODO: Implement custom hook equivalent for Qt
        // dlg.SetCustomizeHook( saveAsHook );

        if( dlg.exec() == QDialog::Rejected )
            return false;

        QString selectedPath = dlg.selectedFiles().isEmpty() ? QString() : dlg.selectedFiles().first();
        fn = QFileInfo( selectedPath );

        prj.SetRString( PROJECT::SCH_LIB_PATH, fn.dir().path() );

        if( fn.suffix().isEmpty() )
        {
            QString newPath = fn.absoluteFilePath() + "." + QString::fromStdString(FILEEXT::KiCadSymbolLibFileExtension);
            fn = QFileInfo( newPath );
        }

        type = saveAsHook.GetOption();
    }
    else
    {
        QString libraryPath = PROJECT_SCH::SchSymbolLibTable( &prj )->GetFullURI( aLibrary );
        fn = QFileInfo( libraryPath );
        fileType = SCH_IO_MGR::GuessPluginTypeFromLibPath( fn.absoluteFilePath() );

        if( fileType == SCH_IO_MGR::SCH_FILE_UNKNOWN )
            fileType = SCH_IO_MGR::SCH_KICAD;
    }

    // Verify the user has write privileges before attempting to save the library file.
    if( !aNewFile && m_libMgr->IsLibraryReadOnly( aLibrary ) )
        return false;

    ClearMsgPanel();

    // Copy .kicad_symb file to .bak.
    if( !backupFile( fn, "bak" ) )
        return false;

    if( !m_libMgr->SaveLibrary( aLibrary, fn.absoluteFilePath(), fileType ) )
    {
        msg = QString::asprintf( _( "Failed to save changes to symbol library file '%s'." ).toUtf8().constData(),
                    fn.absoluteFilePath().toUtf8().constData() );
        DisplayErrorMessage( this, _( "Error Saving Library" ), msg );
        return false;
    }

    if( !aNewFile )
    {
        m_libMgr->ClearLibraryModified( aLibrary );

        // Update the library modification time so that we don't reload based on the watcher
        if( aLibrary == getTargetLib() )
            SetSymModificationTime( fn.lastModified() );
    }
    else
    {
        bool resyncLibTree = false;
        QString originalLibNickname = getTargetLib();
        QString forceRefresh;

        switch( type )
        {
        case SYMBOL_SAVEAS_TYPE::REPLACE_TABLE_ENTRY:
            resyncLibTree = replaceLibTableEntry( originalLibNickname, fn.absoluteFilePath() );
            forceRefresh = originalLibNickname;
            break;

        case SYMBOL_SAVEAS_TYPE::ADD_GLOBAL_TABLE_ENTRY:
            resyncLibTree = addLibTableEntry( fn.absoluteFilePath() );
            break;

        case SYMBOL_SAVEAS_TYPE::ADD_PROJECT_TABLE_ENTRY:
            resyncLibTree = addLibTableEntry( fn.absoluteFilePath(), PROJECT_LIB_TABLE );
            break;

        default:
            break;
        }

        if( resyncLibTree )
        {
            FreezeLibraryTree();
            SyncLibraries( true, false, forceRefresh );
            ThawLibraryTree();
        }
    }

    ClearMsgPanel();
    msg = QString::asprintf( _( "Symbol library file '%s' saved." ).toUtf8().constData(), fn.absoluteFilePath().toUtf8().constData() );
    RebuildSymbolUnitsList();

    return true;
}


bool SYMBOL_EDIT_FRAME::saveAllLibraries( bool aRequireConfirmation )
{
    QString msg, msg2;
    bool     doSave = true;
    int      dirtyCount = 0;
    bool     applyToAll = false;
    bool     retv = true;

    for( const QString& libNickname : m_libMgr->GetLibraryNames() )
    {
        if( m_libMgr->IsLibraryModified( libNickname ) )
            dirtyCount++;
    }

    for( const QString& libNickname : m_libMgr->GetLibraryNames() )
    {
        if( m_libMgr->IsLibraryModified( libNickname ) )
        {
            if( aRequireConfirmation && !applyToAll )
            {
                msg = QString::asprintf( _( "Save changes to '%s' before closing?" ).toUtf8().constData(), libNickname.toUtf8().constData() );

                switch( UnsavedChangesDialog( this, msg, dirtyCount > 1 ? &applyToAll : nullptr ) )
                {
                case QMessageBox::Yes: doSave = true;  break;
                case QMessageBox::No:  doSave = false; break;
                default:
                case QDialog::Rejected: return false;
                }
            }

            if( doSave )
            {
                // If saving under existing name fails then do a Save As..., and if that
                // fails then cancel close action.
                if( m_libMgr->IsLibraryReadOnly( libNickname ) )
                {
                    msg = QString::asprintf( _( "Symbol library '%s' is not writable." ).toUtf8().constData(), libNickname.toUtf8().constData() );
                    msg2 = _( "You must save to a different location." );

                    if( dirtyCount == 1 )
                    {
                        if( OKOrCancelDialog( this, _( "Warning" ), msg, msg2 ) != QDialog::Accepted )
                        {
                            retv = false;
                            continue;
                        }
                    }
                    else
                    {
                        m_infoBar->Dismiss();
                        m_infoBar->ShowMessageFor( msg + QStringLiteral( "  " ) + msg2,
                                                   2000, QMessageBox::Warning );

                        while( m_infoBar->isVisible() )
                            QApplication::processEvents();

                        retv = false;
                        continue;
                    }
                }
                else if( saveLibrary( libNickname, false ) )
                {
                    continue;
                }

                if( !saveLibrary( libNickname, true ) )
                    retv = false;
            }
        }
    }

    UpdateTitle();
    return retv;
}


void SYMBOL_EDIT_FRAME::UpdateSymbolMsgPanelInfo()
{
    EDA_DRAW_FRAME::ClearMsgPanel();

    if( !m_symbol )
        return;

    QString msg = m_symbol->GetName();

    AppendMsgPanel( _( "Name" ), UnescapeString( msg ), 8 );

    if( m_symbol->IsAlias() )
    {
        LIB_SYMBOL_SPTR parent = m_symbol->GetParent().lock();

        msg = parent ? parent->GetName() : _( "Undefined!" );
        AppendMsgPanel( _( "Parent" ), UnescapeString( msg ), 8 );
    }

    static QString UnitLetterStr = QStringLiteral( "?ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
    msg = QString(UnitLetterStr[m_unit]);

    AppendMsgPanel( _( "Unit" ), msg, 8 );

    if( m_bodyStyle == BODY_STYLE::DEMORGAN )
        msg = _( "Alternate" );
    else if( m_bodyStyle == BODY_STYLE::BASE )
        msg = _( "Standard" );
    else
        msg = QStringLiteral( "?" );

    AppendMsgPanel( _( "Body" ), msg, 8 );

    if( m_symbol->IsPower() )
        msg = _( "Power Symbol" );
    else
        msg = _( "Symbol" );

    AppendMsgPanel( _( "Type" ), msg, 8 );
    AppendMsgPanel( _( "Description" ), m_symbol->GetDescription(), 8 );
    AppendMsgPanel( _( "Keywords" ), m_symbol->GetKeyWords() );
    AppendMsgPanel( _( "Datasheet" ), m_symbol->GetDatasheetField().GetText() );
}
