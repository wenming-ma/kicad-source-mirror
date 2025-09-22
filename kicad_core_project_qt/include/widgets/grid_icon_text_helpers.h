
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef GRID_ICON_TEXT_HELPERS_H
#define GRID_ICON_TEXT_HELPERS_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QAbstractItemView>
#include <QtGui/QPixmap>
#include <QtCore/QStringList>
#include <QtCore/QSize>
#include <QtCore/QRect>
#include <QtGui/QPainter>
#include <vector>

class QAbstractItemView;
enum class BITMAPS : unsigned int;


//---- Grid helpers: custom QStyledItemDelegate that renders icon and a label ------------

class GRID_CELL_ICON_TEXT_RENDERER : public QStyledItemDelegate
{
public:
    /**
     * Construct a renderer that maps a list of icons from the bitmap system to a list of strings
     * @param icons is a list of possible icons to render
     * @param names is a list of names to render - must be the same length as icons
     */
    GRID_CELL_ICON_TEXT_RENDERER( const std::vector<BITMAPS>& icons, const QStringList& names );

    /**
     * Construct a renderer that renders a single icon next to the cell's value text
     * @param aIcon is the icon to render next to the cell's value
     */
    GRID_CELL_ICON_TEXT_RENDERER( const QPixmap& aIcon,
                                  QSize aPreferredIconSize = QSize() );

    void paint( QPainter* painter, const QStyleOptionViewItem& option,
                const QModelIndex& index ) const override;
    QSize sizeHint( const QStyleOptionViewItem& option,
                    const QModelIndex& index ) const override;

private:
    std::vector<BITMAPS> m_icons;
    QStringList          m_names;

    // For single-icon mode
    QPixmap m_icon;
    QSize m_iconSize;
};

//---- Grid helpers: custom QStyledItemDelegate that renders just an icon ----------------
//
// Note: use with read only cells

class GRID_CELL_ICON_RENDERER : public QStyledItemDelegate
{
public:
    GRID_CELL_ICON_RENDERER( const QPixmap& icon );

    void paint( QPainter* painter, const QStyleOptionViewItem& option,
                const QModelIndex& index ) const override;
    QSize sizeHint( const QStyleOptionViewItem& option,
                    const QModelIndex& index ) const override;
    GRID_CELL_ICON_RENDERER* clone() const;

private:
    const QPixmap& m_icon;
};

//---- Grid helpers: custom QStyledItemDelegate that renders just an icon from Qt style -
//
// Note: use with read only cells

class GRID_CELL_STATUS_ICON_RENDERER : public QStyledItemDelegate
{
public:
    GRID_CELL_STATUS_ICON_RENDERER( int aStatus );

    void paint( QPainter* painter, const QStyleOptionViewItem& option,
                const QModelIndex& index ) const override;
    QSize sizeHint( const QStyleOptionViewItem& option,
                    const QModelIndex& index ) const override;
    GRID_CELL_STATUS_ICON_RENDERER* clone() const;

private:
    int     m_status;
    QPixmap m_bitmap;
};



//---- Grid helpers: custom QStyledItemDelegate editor ------------------------------------------
//
// Note: this implementation is an adaptation of QComboBox editor

class GRID_CELL_ICON_TEXT_POPUP : public QStyledItemDelegate
{
public:
    GRID_CELL_ICON_TEXT_POPUP( const std::vector<BITMAPS>& icons, const QStringList& names );

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index ) const override;
    void setEditorData( QWidget* editor, const QModelIndex& index ) const override;
    void setModelData( QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index ) const override;
    void updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option,
                               const QModelIndex& index ) const override;

protected:
    QComboBox* Combo( QWidget* editor ) const { return static_cast<QComboBox*>( editor ); }

    std::vector<BITMAPS> m_icons;
    QStringList          m_names;
    QString              m_value;

    Q_DISABLE_COPY( GRID_CELL_ICON_TEXT_POPUP )
};


//---- Grid helpers: custom QStyledItemDelegate editor ------------------------------------------
//
// Note: This is used to mark Qt grid cell as nullable
class GRID_CELL_MARK_AS_NULLABLE : public QStyledItemDelegate
{
public:
    GRID_CELL_MARK_AS_NULLABLE() : m_isNullable( true ) {}
    GRID_CELL_MARK_AS_NULLABLE( bool aIsNullable ) : m_isNullable( aIsNullable ) {}

    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index ) const override;

    void setEditorData( QWidget* editor, const QModelIndex& index ) const override;
    void setModelData( QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index ) const override;

    bool IsNullable() { return m_isNullable; }

protected:
    bool m_isNullable;

    Q_DISABLE_COPY( GRID_CELL_MARK_AS_NULLABLE )
};


#endif  // GRID_ICON_TEXT_HELPERS_H
