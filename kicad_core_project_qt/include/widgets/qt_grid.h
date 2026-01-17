#ifndef QT_GRID_H
#define QT_GRID_H

#include <QTableWidget>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <vector>
#include <QWidget>
#include <QAbstractItemView>

class QT_GRID : public QTableWidget
{
    Q_OBJECT

public:
    // Selection modes matching wxGrid behavior
    enum SelectionMode {
        SelectCells = 0,
        SelectRows = 1,
        SelectColumns = 2
    };

    explicit QT_GRID(QWidget* parent = nullptr);
    ~QT_GRID() override;

    // Grid dimensions
    int GetNumberRows() const { return rowCount(); }
    int GetNumberCols() const { return columnCount(); }

    // Cell operations
    QString GetCellValue(int row, int col) const;
    void SetCellValue(int row, int col, const QString& value);
    bool CanGetValueAs(int row, int col, const QString& typeName) const;
    bool CanSetValueAs(int row, int col, const QString& typeName) const;
    void SetValueAsBool(int row, int col, bool value);
    bool GetValueAsBool(int row, int col) const;

    // Cell editor operations
    QWidget* GetCellEditor(int row, int col) const;
    QWidget* GetCurrentEditor() const;
    bool IsTextEntry(int row, int col) const;
    bool IsCheckbox(int row, int col) const;
    bool IsEditable() const { return editTriggers() != QAbstractItemView::NoEditTriggers; }
    bool IsReadOnly(int row, int col) const;
    bool IsEditorEditable(QWidget* editor) const;
    void WriteTextToEditor(QWidget* editor, const QString& text);

    // Grid cursor
    int GetGridCursorRow() const { return currentRow(); }
    int GetGridCursorCol() const { return currentColumn(); }
    void SetGridCursor(int row, int col);

    // Selection operations
    SelectionMode GetSelectionMode() const { return m_selectionMode; }
    void SetSelectionMode(SelectionMode mode);
    std::vector<int> GetSelectedRows() const;
    std::vector<int> GetSelectedCols() const;
    std::vector<std::vector<int>> GetSelectedCells() const;
    std::vector<std::vector<int>> GetSelectionBlockTopLeft() const;
    std::vector<std::vector<int>> GetSelectionBlockBottomRight() const;
    void SelectRow(int row);
    void SelectAll();

    // Column operations
    QString GetColLabelValue(int col) const;
    bool IsColShown(int col) const;
    void HideCol(int col);
    void ShowCol(int col);

    // Coordinate conversion
    int XToCol(int x) const;
    int YToRow(int y) const;
    QPoint CalcScrolledPosition(const QPoint& pt) const;

    // Edit control
    bool IsCellEditControlShown() const;
    bool CommitPendingChanges(bool quietMode = true);
    void CancelPendingChanges();
    void ShowEditorOnMouseUp();
    void ForceRefresh() { update(); }

    // Event emission
    void EmitCellChanged(int row, int col, const QString& newValue);

    // Modifier state
    bool HasModifiers() const { return QApplication::keyboardModifiers() != Qt::NoModifier; }

signals:
    void cellClicked(int row, int col);
    void cellDoubleClicked(int row, int col);
    void cellRightClicked(int row, int col);
    void labelLeftClicked(int section);
    void labelRightClicked(int section);
    void keyPressed(QKeyEvent* event);
    void charHook(QKeyEvent* event);
    void updateUI();
    void mouseMotion(QMouseEvent* event);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onHeaderClicked(int logicalIndex);
    void onHeaderRightClicked(const QPoint& pos);
    void onCurrentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    SelectionMode m_selectionMode;
    bool m_waitForSlowClick;
    
    void setupConnections();
    QModelIndexList getSelectedIndexes() const;
};

#endif // QT_GRID_H