// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#ifndef WIDGETS_TAB_TRAVERSAL__H
#define WIDGETS_TAB_TRAVERSAL__H

/**
 * @file
 * Functions for manipulating tab traversal in forms and dialogs.
 */

#include <QWidget>

#include <vector>

namespace KIUI
{

/**
 * Set a list of controls to have a defined sequential tab order.
 *
 * Each control in the list will come after the previous one. The first control will
 * keep its current position. The end result will be that the given control
 * will be sequential when tabbed though.
 *
 * This can be slightly clearer than manually calling MoveAfterInTabOrder
 * on each control in turn.
 *
 * @param aControlsInTabOrder list of controls (QWidgets) in desired tab order
 */
void SetControlsTabOrder( const std::vector<QWidget*>& aControlsInTabOrder );

} // namespace KIUI

#endif // WIDGETS_TAB_TRAVERSAL__H
