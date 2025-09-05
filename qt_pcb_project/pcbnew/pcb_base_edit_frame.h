
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef BASE_EDIT_FRAME_H
#define BASE_EDIT_FRAME_H

#include <pcb_base_frame.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QEvent>

class APPEARANCE_CONTROLS;
class LAYER_PAIR_SETTINGS;
class BOARD_ITEM_CONTAINER;
class PANEL_SELECTION_FILTER;
class PCB_TEXTBOX;
class PCB_TABLE;
class PCB_TEXT;
class PCB_SHAPE;

/**
 * Common, abstract interface for edit frames.
 */
class PCB_BASE_EDIT_FRAME : public PCB_BASE_FRAME
{
public:
    PCB_BASE_EDIT_FRAME( KIWAY* aKiway, QWidget* aParent, FRAME_T aFrameType,
                         const QString& aTitle, const QPoint& aPos, const QSize& aSize,
                         long aStyle, const QString& aFrameName );

    virtual ~PCB_BASE_EDIT_FRAME();

    bool TryBefore( QEvent& aEvent ) override;

    void doCloseWindow() override;

    QString CreateNewLibrary( const QString& aLibName = QString(),
                               const QString& aProposedName = QString() );

    QString CreateNewProjectLibrary( const QString& aLibName = QString(),
                                      const QString& aProposedName = QString() );

    bool AddLibrary( const QString& aLibName = QString(), FP_LIB_TABLE* aTable = nullptr );

    virtual void OnEditItemRequest( BOARD_ITEM* aItem ) {};

    void SaveCopyInUndoList( EDA_ITEM* aItemToCopy, UNDO_REDO aTypeCommand ) override;

    void SaveCopyInUndoList( const PICKED_ITEMS_LIST& aItemsList, UNDO_REDO aCommandType ) override;

    void AppendCopyToUndoList( const PICKED_ITEMS_LIST& aItemsList,
                               UNDO_REDO aCommandType ) override;

    void RestoreCopyFromRedoList( QEvent& aEvent );

    void RestoreCopyFromUndoList( QEvent& aEvent );

    void RollbackFromUndo();

    void PutDataInPreviousState( PICKED_ITEMS_LIST* aList );

    bool UndoRedoBlocked() const
    {
        return m_undoRedoBlocked;
    }

    void UndoRedoBlock( bool aBlock = true )
    {
        m_undoRedoBlocked = aBlock;
    }

    void SetGridVisibility( bool aVisible ) override;

    void SetObjectVisible( GAL_LAYER_ID aLayer, bool aVisible = true );

    virtual EDA_ANGLE GetRotationAngle() const;

    //void SetRotationAngle( EDA_ANGLE aRotationAngle );

    void ShowReferenceImagePropertiesDialog( BOARD_ITEM* aBitmap );
    void ShowTextPropertiesDialog( PCB_TEXT* aText );
    int ShowTextBoxPropertiesDialog( PCB_TEXTBOX* aTextBox );
    void ShowGraphicItemPropertiesDialog( PCB_SHAPE* aShape );

    void ActivateGalCanvas() override;

    virtual void SetBoard( BOARD* aBoard, PROGRESS_REPORTER* aReporter = nullptr ) override;

    COLOR_SETTINGS* GetColorSettings( bool aForceRefresh = false ) const override;


    void ClearUndoORRedoList( UNDO_REDO_LIST whichList, int aItemCount = -1 ) override;

    void ClearListAndDeleteItems( PICKED_ITEMS_LIST* aList );

    QString GetDesignRulesPath();

    APPEARANCE_CONTROLS* GetAppearancePanel() { return m_appearancePanel; }

    LAYER_PAIR_SETTINGS* GetLayerPairSettings() { return m_layerPairSettings.get(); }

    void ToggleProperties() override;

    void GetContextualTextVars( BOARD_ITEM* aSourceItem, const QString& aCrossRef,
                                QStringList* aTokens );

protected:
    FP_LIB_TABLE* selectLibTable( bool aOptional = false );

    QString createNewLibrary( const QString& aLibName, const QString& aProposedName,
                               FP_LIB_TABLE* aTable );

    void handleActivateEvent( QEvent& aEvent ) override;

    void saveCopyInUndoList( PICKED_ITEMS_LIST* commandToUndo, const PICKED_ITEMS_LIST& aItemsList,
                             UNDO_REDO aCommandType );

    void unitsChangeRefresh() override;

    virtual void onDarkModeToggle();

protected:
    bool                    m_undoRedoBlocked;

    PANEL_SELECTION_FILTER*              m_selectionFilterPanel;
    APPEARANCE_CONTROLS*                 m_appearancePanel;
    std::unique_ptr<LAYER_PAIR_SETTINGS> m_layerPairSettings;

    QTabWidget*          m_tabbedPanel;        // Panel with Layers and Object Inspector tabs

    bool                    m_darkMode;
};

#endif
