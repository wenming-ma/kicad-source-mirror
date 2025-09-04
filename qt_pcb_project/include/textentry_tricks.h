
#ifndef TEXTENTRY_TRICKS_H
#define TEXTENTRY_TRICKS_H


#include <QLineEdit>
#include <QKeyEvent>

// Add cut/copy/paste to a QLineEdit instance.
// While these are normally handled without our intervention, this is not always the case.
struct TEXTENTRY_TRICKS
{
    static void OnCharHook( QLineEdit* aTextEntry, QKeyEvent& aEvent );

protected:
    static bool isCtrl( int aChar, const QKeyEvent& e );
    static bool isShiftCtrl( int aChar, const QKeyEvent& e );
};

#endif  // TEXTENTRY_TRICKS_H
