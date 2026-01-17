
#ifndef DPI_SCALING_COMMON__H
#define DPI_SCALING_COMMON__H

#include <kicommon.h>
#include <dpi_scaling.h>

class COMMON_SETTINGS;
class QWidget;

class KICOMMON_API DPI_SCALING_COMMON : public DPI_SCALING
{
public:
    DPI_SCALING_COMMON( COMMON_SETTINGS* aConfig, const QWidget* aWindow );

    double GetScaleFactor() const override;

    double GetContentScaleFactor() const override;

    bool GetCanvasIsAutoScaled() const override;

    void SetDpiConfig( bool aAuto, double aValue ) override;

private:
    COMMON_SETTINGS* m_config;

    const QWidget* m_window;
};

#endif // DPI_SCALING__H