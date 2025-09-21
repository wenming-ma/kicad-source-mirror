
#pragma once

#include <pcb_plot_params.h>
#include <QString>
#include <optional>

class BOARD;
class REPORTER;
class QFileInfo;
class JOB_EXPORT_PCB_PLOT;

class PCB_PLOTTER
{
public:
    PCB_PLOTTER( BOARD* aBoard, REPORTER* aReporter, PCB_PLOT_PARAMS& aParams );

    bool Plot( const QString& aOutputPath, const LSEQ& aLayersToPlot, const LSEQ& aCommonLayers,
               bool aUseGerberFileExtensions,
                bool aOutputPathIsSingle = false,
                std::optional<QString> aLayerName = std::nullopt,
                std::optional<QString> aSheetName = std::nullopt,
                std::optional<QString> aSheetPath = std::nullopt );

    /**
     * All copper layers that are disabled are actually selected
     * This is due to wonkyness in automatically selecting copper layers
     * for plotting when adding more than two layers to a board.
     * If plot options become accessible to the layers setup dialog
     * please move this functionality there!
     * This skips a copper layer if it is actually disabled on the board.
     */
    bool copperLayerShouldBeSkipped( PCB_LAYER_ID aLayerToPlot );

    /**
     * Complete a plot filename.
     *
     * It forces the output directory, adds a suffix to the name, and sets the specified extension.
     * The suffix is usually the layer name and replaces illegal file name character in the suffix
     * with an underscore character.
     *
     * @param aFilename is the file name to initialize that contains the base filename.
     * @param aOutputDir is the path.
     * @param aSuffix is the suffix to add to the base filename.
     * @param aExtension is the file extension.
     */
    static void BuildPlotFileName( QFileInfo* aFilename, const QString& aOutputDir, const QString& aSuffix,
                                   const QString& aExtension );

    /**
     * Translate a JOB to PCB_PLOT_PARAMS
     */
    static void PlotJobToPlotOpts( PCB_PLOT_PARAMS& aOpts, JOB_EXPORT_PCB_PLOT* aJob,
                                   REPORTER& aReporter );

protected:
    BOARD*          m_board;
    PCB_PLOT_PARAMS m_plotOpts;
    REPORTER*       m_reporter;

private:
    /**
     * Generates a final LSEQ for plotting by removing duplicates
     */
    LSEQ getPlotSequence( PCB_LAYER_ID aLayerToPlot, LSEQ aPlotWithAllLayersSeq );

};
