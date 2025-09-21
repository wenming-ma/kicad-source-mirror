
#ifndef JOB_EXPORT_PCB_GERBERS_H
#define JOB_EXPORT_PCB_GERBERS_H

#include <kicommon.h>
#include "job_export_pcb_gerber.h"
#include <layer_ids.h>
#include <lset.h>
#include <QString>
#include "job.h"

class KICOMMON_API JOB_EXPORT_PCB_GERBERS : public JOB_EXPORT_PCB_GERBER
{
public:
    JOB_EXPORT_PCB_GERBERS();
    QString GetDefaultDescription() const override;
    QString GetSettingsDialogTitle() const override;

    bool m_useBoardPlotParams;

    bool m_createJobsFile;
};

#endif