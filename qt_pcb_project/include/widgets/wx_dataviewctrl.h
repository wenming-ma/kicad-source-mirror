// QT_TRANSFORMATION_COMPLETED
#ifndef WX_DATAVIEWCTRL_H_
#define WX_DATAVIEWCTRL_H_

#include <QTreeView>
#include <QModelIndex>
#include <QString>

class WX_DATAVIEWCTRL : public QTreeView
{
    Q_OBJECT

public:
    using QTreeView::QTreeView;

    QModelIndex GetPrevItem( const QModelIndex& aItem );

    QModelIndex GetNextItem( const QModelIndex& aItem );

    QModelIndex GetPrevSibling( const QModelIndex& aItem );

    QModelIndex GetNextSibling( const QModelIndex& aItem );

    void DoSetToolTipText( const QString &tip ) override {}

    void ExpandAll();
    void CollapseAll();
};

#endif // WX_DATAVIEWCTRL_H_
