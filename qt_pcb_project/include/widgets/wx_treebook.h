// QT_TRANSFORMATION_COMPLETED
#ifndef WX_TREEBOOK_H
#define WX_TREEBOOK_H

#include <functional>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QString>

class WX_TREEBOOK : public QWidget
{
    Q_OBJECT

public:
    WX_TREEBOOK( QWidget *parent = nullptr, const QString& name = QString() );

    bool AddLazyPage( std::function<QWidget*( QWidget* aParent )> aLazyCtor,
                      const QString& text, bool bSelect = false, int imageId = -1 );

    bool AddLazySubPage( std::function<QWidget*( QWidget* aParent )> aLazyCtor,
                         const QString& text, bool bSelect = false, int imageId = -1 );

    QWidget* ResolvePage( size_t aPage );

private slots:
    void onTreeItemChanged();

private:
    QTreeWidget* m_tree;
    QStackedWidget* m_pages;
    QHBoxLayout* m_layout;
};


#endif // WX_TREEBOOK_H
