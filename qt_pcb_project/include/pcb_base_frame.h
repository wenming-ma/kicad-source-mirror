// QT_TRANSFORMATION_COMPLETED

#ifndef  PCB_BASE_FRAME_H
#define  PCB_BASE_FRAME_H

#include <eda_units.h>
#include <eda_draw_frame.h>
#include <outline_mode.h>
#include <lib_id.h>
#include <pcb_display_options.h>
#include <pcb_draw_panel_gal.h>
#include <pcb_origin_transforms.h>
#include <pcb_screen.h>
#include <vector>

#include <QDateTime>
#include <QTimer>
#include <QWidget>
#include <QMainWindow>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QObject>
#include <QCloseEvent>

/* Forward declarations of classes. */
class APP_SETTINGS_BASE;
class BOARD;
class BOARD_CONNECTED_ITEM;
class COLOR_SETTINGS;
class EDA_ITEM;
class FOOTPRINT;
class PAD;
class EDA_3D_VIEWER_FRAME;
class GENERAL_COLLECTOR;
class GENERAL_COLLECTORS_GUIDE;
class BOARD_DESIGN_SETTINGS;
class LSET;
class ZONE_SETTINGS;
class PCB_PLOT_PARAMS;
class FP_LIB_TABLE;
class PCB_VIEWERS_SETTINGS_BASE;
class PCBNEW_SETTINGS;
class FOOTPRINT_EDITOR_SETTINGS;
struct MAGNETIC_SETTINGS;
class NL_PCBNEW_PLUGIN;
class PROGRESS_REPORTER;

#include <QFileSystemWatcher>

class QFileSystemWatcher;

class PCB_BASE_FRAME : public EDA_DRAW_FRAME
{
    Q_OBJECT
public:
    PCB_BASE_FRAME( KIWAY* aKiway, QWidget* aParent, FRAME_T aFrameType,
                    const QString& aTitle, const QPoint& aPos, const QSize& aSize,
                    long aStyle, const QString& aFrameName );

    ~PCB_BASE_FRAME();

    EDA_3D_VIEWER_FRAME* Get3DViewerFrame();

    virtual void Update3DView( bool aMarkDirty, bool aRefresh, const QString* aTitle = nullptr );

    FOOTPRINT* LoadFootprint( const LIB_ID& aFootprintId );

    BOX2I GetBoardBoundingBox( bool aBoardEdgesOnly = false ) const;

    const BOX2I GetDocumentExtents( bool aIncludeAllVisible = true ) const override;

    virtual void SetPageSettings( const PAGE_INFO& aPageSettings ) override;
    const PAGE_INFO& GetPageSettings() const override;
    const VECTOR2I GetPageSizeIU() const override;

    const VECTOR2I& GetGridOrigin() const override;
    void            SetGridOrigin( const VECTOR2I& aPoint ) override;

    const VECTOR2I& GetAuxOrigin() const;

    const VECTOR2I GetUserOrigin() const;

    ORIGIN_TRANSFORMS& GetOriginTransforms() override;

    QString MessageTextFromCoord( int aValue, ORIGIN_TRANSFORMS::COORD_TYPES_T aCoordType ) const
    {
        return MessageTextFromValue( m_originTransforms.ToDisplay( aValue, aCoordType ) );
    }

    const TITLE_BLOCK& GetTitleBlock() const override;
    void SetTitleBlock( const TITLE_BLOCK& aTitleBlock ) override;

    virtual BOARD_DESIGN_SETTINGS& GetDesignSettings() const;

    virtual COLOR_SETTINGS* GetColorSettings( bool aForceRefresh = false ) const override
    {
        Q_ASSERT_X( false, "PCB_BASE_FRAME", "Color settings requested for a PCB_BASE_FRAME that does not override!" );
        return nullptr;
    }

    void SetDrawBgColor( const COLOR4D& aColor ) override;

    const PCB_DISPLAY_OPTIONS& GetDisplayOptions() const { return m_displayOptions; }

    void SetDisplayOptions( const PCB_DISPLAY_OPTIONS& aOptions, bool aRefresh = true );

    virtual const PCB_PLOT_PARAMS& GetPlotSettings() const;
    virtual void SetPlotSettings( const PCB_PLOT_PARAMS& aSettings );

    virtual void ReloadFootprint( FOOTPRINT* aFootprint )
    {
        Q_ASSERT_X( false, "PCB_BASE_FRAME", "Attempted to reload a footprint for PCB_BASE_FRAME that does not override!" );
    }

    virtual void SetBoard( BOARD* aBoard, PROGRESS_REPORTER* aReporter = nullptr );

    BOARD* GetBoard() const
    {
        Q_ASSERT( m_pcb );
        return m_pcb;
    }

    virtual BOARD_ITEM_CONTAINER* GetModel() const = 0;

    EDA_ITEM* GetItem( const KIID& aId ) const override;

    void FocusOnItem( BOARD_ITEM* aItem, PCB_LAYER_ID aLayer = UNDEFINED_LAYER );
    void FocusOnItems( std::vector<BOARD_ITEM*> aItems, PCB_LAYER_ID aLayer = UNDEFINED_LAYER );

    void HideSolderMask();
    void ShowSolderMask();

    // General
    virtual void ShowChangedLanguage() override;
    virtual void UpdateStatusBar() override;

    PCB_SCREEN* GetScreen() const override { return (PCB_SCREEN*) EDA_DRAW_FRAME::GetScreen(); }

    EDA_3D_VIEWER_FRAME* CreateAndShow3D_Frame();

    GENERAL_COLLECTORS_GUIDE GetCollectorsGuide();

    QString SelectLibrary( const QString& aNicknameExisting );

    void OnModify() override;

    FOOTPRINT* CreateNewFootprint( QString aFootprintName, const QString& aLibName );

    void PlaceFootprint( FOOTPRINT* aFootprint, bool aRecreateRatsnest = true );

    void ShowPadPropertiesDialog( PAD* aPad );

    FOOTPRINT* SelectFootprintFromLibrary( LIB_ID aPreselect = LIB_ID() );

    virtual void AddFootprintToBoard( FOOTPRINT* aFootprint );

    void Compile_Ratsnest( bool aDisplayStatus );

    virtual void SaveCopyInUndoList( EDA_ITEM* aItemToCopy, UNDO_REDO aTypeCommand ) {};

    virtual void SaveCopyInUndoList( const PICKED_ITEMS_LIST& aItemsList,
                                     UNDO_REDO aTypeCommand ) {};

    virtual void AppendCopyToUndoList( const PICKED_ITEMS_LIST& aItemsList,
                                       UNDO_REDO aTypeCommand ) {};


    PCB_LAYER_ID SelectOneLayer( PCB_LAYER_ID aDefaultLayer, LSET aNotAllowedLayersMask = LSET(),
                                 QPoint aDlgPosition = QPoint() );

    virtual void SwitchLayer( PCB_LAYER_ID aLayer );

    virtual void SetActiveLayer( PCB_LAYER_ID aLayer ) { GetScreen()->m_Active_Layer = aLayer; }
    virtual PCB_LAYER_ID GetActiveLayer() const { return GetScreen()->m_Active_Layer; }

    SEVERITY GetSeverity( int aErrorCode ) const override;

    virtual void OnDisplayOptionsChanged() {}

    void LoadSettings( APP_SETTINGS_BASE* aCfg ) override;
    void SaveSettings( APP_SETTINGS_BASE* aCfg ) override;

    PCBNEW_SETTINGS* GetPcbNewSettings() const;

    FOOTPRINT_EDITOR_SETTINGS* GetFootprintEditorSettings() const;

    virtual PCB_VIEWERS_SETTINGS_BASE* GetViewerSettingsBase() const;

    virtual MAGNETIC_SETTINGS* GetMagneticItemsSettings();

    void CommonSettingsChanged( int aFlags ) override;

    PCB_DRAW_PANEL_GAL* GetCanvas() const override;

    virtual void ActivateGalCanvas() override;

    void AddBoardChangeListener( QObject* aListener );

    void RemoveBoardChangeListener( QObject* aListener );


protected:
    bool canCloseWindow( QCloseEvent& aCloseEvent ) override;

    void handleActivateEvent( QEvent& aEvent ) override;

    void handleIconizeEvent( QEvent& aEvent ) override;

    virtual void doReCreateMenuBar() override;

    FOOTPRINT* loadFootprint( const LIB_ID& aFootprintId );

    virtual void unitsChangeRefresh() override;

    void rebuildConnectivity();

    void setFPWatcher( FOOTPRINT* aFootprint );

protected:
    BOARD*                  m_pcb;
    PCB_DISPLAY_OPTIONS     m_displayOptions;
    PCB_ORIGIN_TRANSFORMS   m_originTransforms;

private:
    std::unique_ptr<NL_PCBNEW_PLUGIN>    m_spaceMouse;

    std::unique_ptr<QFileSystemWatcher> m_watcher;
    QString                              m_watcherFileName;
    QDateTime                            m_watcherLastModified;
    QTimer                               m_watcherDebounceTimer;
    bool                                 m_inFpChangeTimerEvent;

    std::vector<QObject*> m_boardChangeListeners;

signals:
    void boardChanged();

public slots:
    void OnFPChange();
    void OnFpChangeDebounceTimer();
};

#endif  // PCB_BASE_FRAME_H
