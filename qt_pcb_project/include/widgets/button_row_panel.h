// QT_TRANSFORMATION_COMPLETED

#ifndef SIMPLE_BUTTON_PANEL_H
#define SIMPLE_BUTTON_PANEL_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <functional>

// Forward defs for private-only classes
class QHBoxLayout;

class BUTTON_ROW_PANEL: public QWidget
{
    Q_OBJECT

public:

    using BTN_CALLBACK = std::function< void() >;

    struct BTN_DEF
    {
        int             m_id;
        QString         m_text;
        QString         m_tooltip;
        BTN_CALLBACK    m_callback;
    };

    using BTN_DEF_LIST = QVector<BTN_DEF>;

    BUTTON_ROW_PANEL( QWidget* aParent,
        const BTN_DEF_LIST& aLeftBtns,
        const BTN_DEF_LIST& aRightBtns );

private:

    void addButtons( bool aLeft, const BTN_DEF_LIST& aDefs );

    QHBoxLayout* m_layout;
};

#endif // SIMPLE_BUTTON_PANEL_H