#ifndef _GRID_TRICKS_H_
#define _GRID_TRICKS_H_


#include <bitset>
#include <functional>

#include <QTableWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QObject>
#include <QString>
#include <QAction>
#include <widgets/qt_grid.h>

#define GRIDTRICKS_MAX_COL 50

enum
{
    GRIDTRICKS_FIRST_ID = 901,
    GRIDTRICKS_ID_CUT,
    GRIDTRICKS_ID_COPY,
    GRIDTRICKS_ID_DELETE,
    GRIDTRICKS_ID_PASTE,
    GRIDTRICKS_ID_SELECT,

    GRIDTRICKS_FIRST_CLIENT_ID = 1101,  // reserve IDs for sub-classes
    GRID_TRICKS_LAST_CLIENT_ID = 2100,

    GRIDTRICKS_FIRST_SHOWHIDE,          // reserve IDs for show/hide-column-n

    GRIDTRICKS_LAST_ID = GRIDTRICKS_FIRST_SHOWHIDE + GRIDTRICKS_MAX_COL
};


class GRID_TRICKS : public QObject
{
    Q_OBJECT

public:
    explicit GRID_TRICKS( QT_GRID* aGrid );

    GRID_TRICKS( QT_GRID* aGrid, std::function<void( QAction* )> aAddHandler );

    void SetTooltipEnable( int aCol, bool aEnable = true )
    {
        m_tooltipEnabled[aCol] = aEnable;
    }

    bool GetTooltipEnabled( int aCol )
    {
        return m_tooltipEnabled[aCol];
    }

protected slots:
    void onGridCellLeftClick( int row, int col );
    void onGridCellLeftDClick( int row, int col );
    void onGridCellRightClick( int row, int col );
    void onGridLabelLeftClick( int section );
    void onGridLabelRightClick( int section );
    void onPopupSelection( QAction* action );
    void onKeyDown( QKeyEvent* event );
    void onCharHook( QKeyEvent* event );
    void onUpdateUI();
    void onGridMotion( QMouseEvent* event );

protected:
    void init();

    void getSelectedArea();

    virtual bool handleDoubleClick( int row, int col );
    virtual void showPopupMenu( QMenu& menu, int row, int col );
    virtual void doPopupSelection( QAction* action );

    bool isTextEntry( int aRow, int aCol );
    bool isCheckbox( int aRow, int aCol );
    bool isReadOnly( int aRow, int aCol );

    virtual bool toggleCell( int aRow, int aCol, bool aPreserveSelection = false );
    bool showEditor( int aRow, int aCol );

    virtual void paste_clipboard();
    virtual void paste_text( const QString& cb_text );
    virtual void cutcopy( bool doCopy, bool doDelete );

protected:
    QT_GRID* m_grid;

    int      m_sel_row_start;
    int      m_sel_col_start;
    int      m_sel_row_count;
    int      m_sel_col_count;

    std::function<void( QAction* )> m_addHandler;

    std::bitset<GRIDTRICKS_MAX_COL>        m_tooltipEnabled;

    bool                                   m_enableSingleClickEdit;
    bool                                   m_multiCellEditEnabled;
};

#endif  // _GRID_TRICKS_H_
