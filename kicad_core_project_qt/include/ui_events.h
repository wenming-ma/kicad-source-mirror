#ifndef UI_EVENTS_H
#define UI_EVENTS_H

#include <kicommon.h>
#include <QEvent>

// Custom event types for EDA application
const QEvent::Type EDA_EVT_UNITS_CHANGED = static_cast<QEvent::Type>(QEvent::User + 1);
const QEvent::Type EDA_LANG_CHANGED = static_cast<QEvent::Type>(QEvent::User + 2);

#endif