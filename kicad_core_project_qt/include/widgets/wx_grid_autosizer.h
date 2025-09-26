// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#ifndef WX_GRID_AUTOSIZER_H
#define WX_GRID_AUTOSIZER_H

#include <map>

#include <QTableWidget>
#include <QResizeEvent>

/**
 * Class that manages autosizing of columns in a QTableWidget.
 *
 * The class will automatically resize the columns in the grid to fit the content,
 * with one column being flexible and taking up the remaining space.
 */
class WX_GRID_AUTOSIZER
{
public:
    /**
     * Map of column indices to minimum widths.
     *
     * Use 0 to indicate that a column should be autosized to fit content,
     * but without a minimum width.
     */
    using COL_MIN_WIDTHS = std::map<int, int>;

    /**
     * @param aGrid The grid to manage.
     * @param aAutosizedCols A map of columns to autosize: these will sized to fit,
     *                       but not smaller than the width specified.
     * @param aFlexibleCol The column that will take up the remaining space,
     *                     with a minimum width if given in the aAutosizedCols map.
     */
    WX_GRID_AUTOSIZER( QTableWidget& aGrid, COL_MIN_WIDTHS aAutosizedCols, unsigned aFlexibleCol );

private:
    void recomputeGridWidths();

    void onSizeEvent( QResizeEvent& aEvent );

    QTableWidget&  m_grid;
    COL_MIN_WIDTHS m_autosizedCols;
    int            m_flexibleCol;

    bool m_gridWidthsDirty = true;
    int  m_gridWidth = 0;
};

#endif // WX_GRID_AUTOSIZER_H