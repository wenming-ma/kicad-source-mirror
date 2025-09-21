
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#pragma once

#include <widgets/filter_combobox.h>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QString>


class SYMBOL_FILTER_COMBOPOPUP;


class SYMBOL_FILTER_COMBOBOX : public FILTER_COMBOBOX
{
public:
    // Note: this list of arguments is here because it keeps us from having to customize
    // the constructor calls in Qt Designer.
    SYMBOL_FILTER_COMBOBOX( QWidget* parent, int id, const QPoint& pos = QPoint(),
                            const QSize& size = QSize(), long style = 0 );

    QString GetValue() const override;

protected:
    SYMBOL_FILTER_COMBOPOPUP* m_selectorPopup;
};
