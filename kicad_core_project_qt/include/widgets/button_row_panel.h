
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef SIMPLE_BUTTON_PANEL_H
#define SIMPLE_BUTTON_PANEL_H

#include <QWidget>
#include <QString>

#include <vector>
#include <functional>


// Forward defs for private-only classes
class QHBoxLayout;


/**
 * A panel that contains buttons, arranged on the left and/or right sides.
 */
class BUTTON_ROW_PANEL: public QWidget
{
public:

    /**
     * Callback function definition. A callback of this type can be registered
     * to handle the button click event.
     */
    using BTN_CALLBACK = std::function< void( void ) >;

    /**
     * The information needed to instantiate a button on a BUTTON_ROW_PANEL.
     */
    struct BTN_DEF
    {
        /**
         * The button ID. Can be -1, but should be unique if you
         * want to work out which button this was from an event handler.
         */
        int             m_id;

        /**
         * The button display text.
         */
        QString         m_text;

        /**
         * Button tooltip text - empty string for no tooltip
         */
        QString         m_tooltip;

        /**
         * The callback fired when the button is clicked. Can be nullptr,
         * but then the button is useless.
         */
        BTN_CALLBACK    m_callback;
    };

    /**
     * A list of BTN_DEFs, used to group buttons into the left/right groups.
     */
    using BTN_DEF_LIST = std::vector<BTN_DEF>;

    /**
     * Construct a SIMPLE_BUTTON_PANEL with a set of buttons on each side.
     *
     * @param aLeftBtns: buttons on the left side, from left to right
     * @param aRightBtns: buttons on the right side, from left to right
     */
    BUTTON_ROW_PANEL( QWidget* aParent,
        const BTN_DEF_LIST& aLeftBtns,
        const BTN_DEF_LIST& aRightBtns );

private:

    /**
     * Add a set of buttons to one side of the panel.
     *
     * @param aSizer the sizer to add them to
     * @param aLeft  place on the left (false for right)
     * @param aDefs  list of button defs, from left to right
     */
    void addButtons( bool aLeft, const BTN_DEF_LIST& aDefs );

    QHBoxLayout* m_sizer;
};

#endif // SIMPLE_BUTTON_PANEL_H