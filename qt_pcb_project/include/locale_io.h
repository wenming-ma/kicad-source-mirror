
#ifndef LOCALE_IO_H
#define LOCALE_IO_H

#include <kicommon.h>
#include <atomic>
#include <string>

// set USE_QTLOCALE 0 to use setlocale, 1 to use QLocale:
#if defined( _WIN32 )
#define USE_QTLOCALE 1
#else
#define USE_QTLOCALE 0
#endif

class QLocale;

class KICOMMON_API LOCALE_IO
{
public:
    LOCALE_IO();
    ~LOCALE_IO();

private:
#if USE_QTLOCALE
    QLocale* m_qtLocale;
#else
    // The locale in use before switching to the "C" locale
    // (the locale can be set by user, and is not always the system locale)
    std::string m_user_locale;
#endif
};

#endif
