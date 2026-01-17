
#ifndef  I18N_UTILITY_H
#define  I18N_UTILITY_H

#include <QCoreApplication>

// Standard translation macro for user-visible strings
// Uses Qt's translation system to support internationalization
#ifndef _
#define _( s ) QCoreApplication::translate( "", (s) )
#endif

// A define to allow translation of strings which must be stored in English (for instance
// because they are used both as keywords and as messages in dialogs
// We do not want to use the tr() usual function from Qt, which calls translation functions,
// because the English string is used in key file configuration
// The translated string is used only when displaying the help window.
// Therefore translation tools have to use the "_" and the "_HKI" prefix to extract
// strings to translate
#define _HKI( x ) QStringLiteral( x )

#endif // I18N_UTILITY_H
