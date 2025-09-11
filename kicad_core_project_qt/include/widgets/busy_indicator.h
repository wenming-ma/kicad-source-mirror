// QT_TRANSFORMATION_COMPLETED

#ifndef COMMON_WIDGETS_BUSY_INDICATOR__H
#define COMMON_WIDGETS_BUSY_INDICATOR__H

#include <functional>
#include <memory>
class BUSY_INDICATOR
{
public:
    using FACTORY = std::function<std::unique_ptr<BUSY_INDICATOR>()>;

    virtual ~BUSY_INDICATOR() = default;
};

#endif // COMMON_WIDGETS_BUSY_INDICATOR__H