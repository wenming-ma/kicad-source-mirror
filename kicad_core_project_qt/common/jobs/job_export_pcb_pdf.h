#ifndef JOB_EXPORT_PCB_PDF_H
#define JOB_EXPORT_PCB_PDF_H

#include <kicommon.h>
#include <kicommon.h>
#include <layer_ids.h>
#include <lseq.h>
#include <QString>
#include <jobs/job_export_pcb_plot.h>


class KICOMMON_API JOB_EXPORT_PCB_PDF : public JOB_EXPORT_PCB_PLOT
{
public:
    JOB_EXPORT_PCB_PDF();
    QString GetDefaultDescription() const override;
    QString GetSettingsDialogTitle() const override;

    bool m_pdfFrontFPPropertyPopups;
    bool m_pdfBackFPPropertyPopups;
    bool m_pdfMetadata;

    bool m_pdfSingle;

    ///< This is a hack to deal with cli having the wrong behavior
    ///< We will deprecate out the wrong behavior, at which point this enum
    ///< can be replaced with a bool
    enum class GEN_MODE
    {
        ///< DEPRECATED MODE
        ALL_LAYERS_ONE_FILE,
        ///< "Single Document" mode
        ONE_PAGE_PER_LAYER_ONE_FILE,
        ///< The most traditional output mode KiCad has had
        ALL_LAYERS_SEPARATE_FILE
    };

    ///< uused by the cli, will be removed when the other behavior is deprecated
    GEN_MODE m_pdfGenMode;
};

#endif
