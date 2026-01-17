
#ifndef JOB_EXPORT_PCB_GERBER_H
#define JOB_EXPORT_PCB_GERBER_H

#include <kicommon.h>
#include <layer_ids.h>
#include <lseq.h>
#include <QString>
#include <jobs/job_export_pcb_plot.h>

class KICOMMON_API JOB_EXPORT_PCB_GERBER : public JOB_EXPORT_PCB_PLOT
{
public:
    JOB_EXPORT_PCB_GERBER( const std::string& aType );
    JOB_EXPORT_PCB_GERBER();
    QString GetDefaultDescription() const override;
    QString GetSettingsDialogTitle() const override;

    bool m_includeNetlistAttributes;
    bool m_useX2Format;
    bool m_disableApertureMacros;
    bool m_useProtelFileExtension;

    int m_precision;
};

#endif
