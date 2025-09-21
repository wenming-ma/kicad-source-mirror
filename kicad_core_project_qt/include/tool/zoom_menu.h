// Qt transformation completed - wxWidgets to Qt framework conversion
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef ZOOM_MENU_H
#define ZOOM_MENU_H

#include <tool/action_menu.h>

class EDA_DRAW_FRAME;

class ZOOM_MENU : public ACTION_MENU
{
public:
    ZOOM_MENU( EDA_DRAW_FRAME* aParent );

    void UpdateTitle() override;

private:
    ACTION_MENU* create() const override
    {
        return new ZOOM_MENU( m_parent );
    }

    OPT_TOOL_EVENT eventHandler( QAction* aAction ) override;
    void update() override;

    EDA_DRAW_FRAME* m_parent;
};

#endif /* ZOOM_MENU_H */
