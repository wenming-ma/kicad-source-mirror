// QT_TRANSFORMATION_COMPLETED

#ifndef KICAD_GRID_TEXT_HELPERS_H
#define KICAD_GRID_TEXT_HELPERS_H

#include <functional>
#include <memory>
#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QStyledItemDelegate>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QRect>
#include <QtGui/QPainter>
#include <QtGui/QValidator>
#include <QtGui/QKeyEvent>
#include <QtGui/QFocusEvent>
#include <QtCore/QSize>

class QTableWidget;
class QTextEdit;
#ifdef HAVE_QSCINTILLA
class SCINTILLA_TRICKS;
#endif

class GRID_CELL_TEXT_EDITOR : public QItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_TEXT_EDITOR();

    void setGeometry(QWidget* editor, const QStyleOptionViewItem& option, 
                     const QModelIndex& index) const;

    virtual void setValidator(const QValidator& validator);
    virtual void startingKey(QKeyEvent& event);

protected:
    std::unique_ptr<QValidator> m_validator;
};

class GRID_CELL_ESCAPED_TEXT_RENDERER : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_ESCAPED_TEXT_RENDERER();

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#ifdef HAVE_QSCINTILLA
class GRID_CELL_STC_EDITOR : public QItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_STC_EDITOR(bool ignoreCase, bool singleLine,
                         std::function<void(QKeyEvent&, SCINTILLA_TRICKS*)> onCharFn);

    void setGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                     const QModelIndex& index) const;
    
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;

    QString getValue() const;

    virtual void startingKey(QKeyEvent& event);
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;

protected slots:
    void onFocusLoss();

protected:
    QTextEdit* stc_ctrl() const;

protected:
    SCINTILLA_TRICKS* m_scintillaTricks;
    bool              m_ignoreCase;
    bool              m_singleLine;
    QString           m_value;

    std::function<void(QKeyEvent&, SCINTILLA_TRICKS*)> m_onCharFn;
};
#endif // HAVE_QSCINTILLA

#endif // KICAD_GRID_TEXT_HELPERS_H