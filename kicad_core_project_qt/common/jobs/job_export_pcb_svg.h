
#ifndef JOB_EXPORT_PCB_SVG_H
#define JOB_EXPORT_PCB_SVG_H

#include <kicommon.h>
#include <layer_ids.h>
#include <lseq.h>
#include <QString>
#include <jobs/job_export_pcb_plot.h>
#include "job.h"

class KICOMMON_API JOB_EXPORT_PCB_SVG : public JOB_EXPORT_PCB_PLOT
{
public:
    JOB_EXPORT_PCB_SVG();
    QString GetDefaultDescription() const override;
    QString GetSettingsDialogTitle() const override;

    bool         m_fitPageToBoard;
    unsigned int m_precision;

    enum class GEN_MODE
    {
        SINGLE,
        MULTI
    };

    GEN_MODE m_genMode;
};

#endif