
#ifndef JOB_EXPORT_PCB_DXF_H
#define JOB_EXPORT_PCB_DXF_H

#include <kicommon.h>
#include <layer_ids.h>
#include <lseq.h>
#include <QString>
#include <jobs/job_export_pcb_plot.h>
#include "job.h"

class KICOMMON_API JOB_EXPORT_PCB_DXF : public JOB_EXPORT_PCB_PLOT
{
public:
    JOB_EXPORT_PCB_DXF();
    QString GetDefaultDescription() const override;
    QString GetSettingsDialogTitle() const override;

    enum class DXF_UNITS
    {
        INCH,       // Do not use IN: it conflicts with a Windows header
        MM
    };

    enum class GEN_MODE
    {
        SINGLE,
        MULTI
    };

public:
    bool      m_plotGraphicItemsUsingContours;
    bool      m_polygonMode;
    DXF_UNITS m_dxfUnits;

    GEN_MODE  m_genMode;
};

#endif
