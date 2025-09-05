// QT_TRANSFORMATION_COMPLETED
#ifndef QT_GRID_AUTOSIZER_H
#define QT_GRID_AUTOSIZER_H

#include <map>
#include <QTableWidget>
#include <QResizeEvent>

class QT_GRID_AUTOSIZER
{
public:
    using COL_MIN_WIDTHS = std::map<int, int>;

    QT_GRID_AUTOSIZER( QTableWidget& aGrid, COL_MIN_WIDTHS aAutosizedCols, unsigned aFlexibleCol );

private:
    void recomputeGridWidths();

    void onSizeEvent( QResizeEvent* aEvent );

    QTableWidget&  m_grid;
    COL_MIN_WIDTHS m_autosizedCols;
    int            m_flexibleCol;

    bool m_gridWidthsDirty = true;
    int  m_gridWidth = 0;
};

#endif // QT_GRID_AUTOSIZER_H