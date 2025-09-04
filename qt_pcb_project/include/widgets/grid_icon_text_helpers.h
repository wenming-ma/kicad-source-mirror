// QT_TRANSFORMATION_COMPLETED

#ifndef GRID_ICON_TEXT_HELPERS_H
#define GRID_ICON_TEXT_HELPERS_H

#include <QPixmap>
#include <QIcon>
#include <QComboBox>
#include <QTableWidget>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QVector>
#include <QStringList>
#include <QPainter>
#include <QRect>
#include <QSize>
#include <vector>

class QTableWidget;
enum class BITMAPS : unsigned int;


//---- Grid helpers: custom QStyledItemDelegate that renders icon and a label ------------

class GRID_CELL_ICON_TEXT_RENDERER : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_ICON_TEXT_RENDERER( const std::vector<BITMAPS>& icons, const QStringList& names );

    GRID_CELL_ICON_TEXT_RENDERER( const QIcon& aIcon,
                                  QSize aPreferredIconSize = QSize() );

    void paint( QPainter* painter, const QStyleOptionViewItem& option,
                const QModelIndex& index ) const override;
    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const override;

private:
    std::vector<BITMAPS> m_icons;
    QStringList          m_names;

    // For single-icon mode
    QIcon m_icon;
    QSize m_iconSize;
};

//---- Grid helpers: custom QStyledItemDelegate that renders just an icon ----------------
//
// Note: use with read only cells

class GRID_CELL_ICON_RENDERER : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_ICON_RENDERER( const QPixmap& icon );

    void paint( QPainter* painter, const QStyleOptionViewItem& option,
                const QModelIndex& index ) const override;
    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const override;

private:
    const QPixmap& m_icon;
};

//---- Grid helpers: custom QStyledItemDelegate that renders just an icon from Qt style -
//
// Note: use with read only cells

class GRID_CELL_STATUS_ICON_RENDERER : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_STATUS_ICON_RENDERER( int aStatus );

    void paint( QPainter* painter, const QStyleOptionViewItem& option,
                const QModelIndex& index ) const override;
    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const override;

private:
    int      m_status;
    QPixmap  m_bitmap;
};



//---- Grid helpers: custom QStyledItemDelegate editor ------------------------------------------
//
// Note: this implementation is an adaptation of combo box editor

class GRID_CELL_ICON_TEXT_POPUP : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_ICON_TEXT_POPUP( const std::vector<BITMAPS>& icons, const QStringList& names );

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index ) const override;
    void setEditorData( QWidget* editor, const QModelIndex& index ) const override;
    void setModelData( QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index ) const override;
    void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option,
                              const QModelIndex& index ) const override;

private slots:
    void commitAndCloseEditor();

private:
    std::vector<BITMAPS> m_icons;
    QStringList          m_names;
};


//---- Grid helpers: custom QStyledItemDelegate editor ------------------------------------------
//
// Note: This is used to mark table cell as nullable
class GRID_CELL_MARK_AS_NULLABLE : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_MARK_AS_NULLABLE() : m_isNullable( true ) {}
    GRID_CELL_MARK_AS_NULLABLE( bool aIsNullable ) : m_isNullable( aIsNullable ) {}

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index ) const override;

    bool IsNullable() { return m_isNullable; }

private:
    bool m_isNullable;
};


#endif  // GRID_ICON_TEXT_HELPERS_H
