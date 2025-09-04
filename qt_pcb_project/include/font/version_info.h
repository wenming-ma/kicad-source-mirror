
#ifndef VERSION_INFO_H_
#define VERSION_INFO_H_

#include <kicommon.h>
#include <mutex>
#include <QString>

namespace KIFONT
{
class KICOMMON_API VERSION_INFO
{
public:
    static QString FontConfig();

    static QString FreeType();

    static QString HarfBuzz();

    static QString FontLibrary();

private:
    VERSION_INFO() {}
};

} //namespace KIFONT

#endif // VERSION_INFO_H_
