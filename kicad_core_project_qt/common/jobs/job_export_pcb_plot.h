
#pragma once

#include <kicommon.h>
#include <plotprint_opts.h>
#include <layer_ids.h>
#include <lseq.h>
#include <QString>
#include "job.h"

class KICOMMON_API JOB_EXPORT_PCB_PLOT : public JOB
{
public:
    enum class PLOT_FORMAT
    {
        HPGL,
        GERBER,
        POST,
        DXF,
        PDF,
        SVG
    };

    JOB_EXPORT_PCB_PLOT( PLOT_FORMAT aFormat, const std::string& aType, bool aOutputIsDirectory );

public:
    std::optional<QString> m_argLayers;
    std::optional<QString> m_argCommonLayers;

    PLOT_FORMAT m_plotFormat;

    QString m_filename;
    QString m_colorTheme;
    QString m_drawingSheet;

    /**
     * Common Options
     */
    bool m_mirror;
    bool m_blackAndWhite;
    bool m_negative;

    bool m_sketchPadsOnFabLayers;
    bool m_hideDNPFPsOnFabLayers;
    bool m_sketchDNPFPsOnFabLayers;
    bool m_crossoutDNPFPsOnFabLayers;

    bool m_plotFootprintValues;
    bool m_plotRefDes;
    bool m_plotDrawingSheet;
    bool m_subtractSolderMaskFromSilk;
    bool m_plotPadNumbers;

    LSEQ m_plotLayerSequence;
    ///< Layers to include on all individual layer prints
    LSEQ m_plotOnAllLayersSequence;

    ///< Used by SVG & PDF
    DRILL_MARKS m_drillShapeOption;

    ///< Used by SVG/DXF/PDF/Gerbers
    bool m_useDrillOrigin;
};
