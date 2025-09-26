
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#include <board.h>
#include <dialog_plot_base.h>
#include <pcb_plot_params.h>
#include <widgets/unit_binder.h>

// the plot dialog window name, used by Qt
#define DLG_WINDOW_NAME QStringLiteral( "plot_dialog-window" )

class QListWidget;
class QPushButton;

class JOB_EXPORT_PCB_PLOT;

/**
 * A dialog to set the plot options and create plot files in various formats.
 */
class DIALOG_PLOT : public DIALOG_PLOT_BASE
{
public:
    DIALOG_PLOT( PCB_EDIT_FRAME* aEditFrame );
    DIALOG_PLOT( PCB_EDIT_FRAME* aEditFrame, QWidget* aParent,
                 JOB_EXPORT_PCB_PLOT* aJob = nullptr );

    virtual ~DIALOG_PLOT();

private:

    // Event called functions
    void Plot( QEvent& event );
    void onOutputDirectoryBrowseClicked( QEvent& event );
    void OnRightClickLayers( QMouseEvent& event );
    void OnRightClickAllLayers( QMouseEvent& event );
    void SetPlotFormat( QEvent& event );
    void OnChangeDXFPlotMode( QEvent& event );
    void OnSetScaleOpt( QEvent& event );
    void CreateDrillFile( QEvent& event );
    void OnGerberX2Checked( QEvent& event );
    void onRunDRC( QEvent& event );
    void onOpenOutputDirectory( QEvent& event );
    void onBoardSetup( QEvent& aEvent );

    void onPlotAllListMoveUp( QEvent& aEvent );
    void onPlotAllListMoveDown( QEvent& aEvent );

    void onDNPCheckbox( QEvent& event );
    void onSketchPads( QEvent& event );

    // other functions
    void init_Dialog();      // main initialization
    void reInitDialog();     // initialization after calling drill dialog
    void applyPlotSettings();
    PLOT_FORMAT getPlotFormat();

    void setPlotModeChoiceSelection( OUTLINE_MODE aPlotMode )
    {
        m_plotModeOpt->setCurrentIndex( aPlotMode == SKETCH ? 1 : 0 );
    }

    void arrangeAllLayersList( const LSEQ& aSeq );
    void loadPlotParamsFromJob();
    void transferPlotParamsToJob();

private:
    PCB_EDIT_FRAME*     m_editFrame;
    LSEQ                m_layerList;                // List to hold CheckListBox layer numbers
    double              m_XScaleAdjust;             // X scale factor adjust to compensate
                                                    // plotter X scaling error
    double              m_YScaleAdjust;             // X scale factor adjust to compensate
                                                    // plotter Y scaling error
    int                 m_PSWidthAdjust;            // Global width correction for exact line width
                                                    // in postscript output.
                                                    // this is a correction factor for tracks width
                                                    // when plotted
    int                 m_widthAdjustMinValue;      // Global track width limits
    int                 m_widthAdjustMaxValue;      // tracks width will be "clipped" whenever the
                                                    // m_PSWidthAdjust to these limits.
    UNIT_BINDER         m_defaultPenSize;
    UNIT_BINDER         m_trackWidthCorrection;

    QString            m_DRCWarningTemplate;

    PCB_PLOT_PARAMS     m_plotOpts;

    QListWidget*    m_plotAllLayersList;

    STD_BITMAP_BUTTON*  m_bpMoveUp;
    STD_BITMAP_BUTTON*  m_bpMoveDown;

    JOB_EXPORT_PCB_PLOT* m_job;

    /// The plot on all layers ordering the last time the dialog was opened.
    static LSEQ         s_lastAllLayersOrder;
};
