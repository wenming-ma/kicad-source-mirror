
// QT_TRANSFORMATION_COMPLETED

#ifndef COMMON_WIDGETS_QT_BUSY_INDICATOR__H
#define COMMON_WIDGETS_QT_BUSY_INDICATOR__H

#include <widgets/busy_indicator.h>
#include <memory>

class QApplication;

class QT_BUSY_INDICATOR : public BUSY_INDICATOR
{
public:
    QT_BUSY_INDICATOR();

private:
    std::unique_ptr<QApplication> m_cursor;
};

#endif // COMMON_WIDGETS_QT_BUSY_INDICATOR__H
