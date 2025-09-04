// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-03
#ifndef KICAD_WX_GRID_H
#define KICAD_WX_GRID_H

#include <bitset>
#include <memory>
#include <utility>
#include <vector>

#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QLineEdit>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QFont>
#include <QPainter>
#include <QWidget>
#include <QString>

#include <libeval/numeric_evaluator.h>
#include <units_provider.h>

class QLineEdit;


class WX_GRID_TABLE_BASE : public QAbstractTableModel
{
protected:
    QStyledItemDelegate* enhanceAttr( QStyledItemDelegate* aInputAttr, int aRow, int aCol,
                                      int aKind  );
};


class WX_GRID : public QTableWidget
{
public:
    // Constructor has to be Qt Designer-compatible
    WX_GRID( QWidget *parent = nullptr, int id = -1,
             const QPoint& pos = QPoint(), const QSize& size = QSize(),
             long style = 0, const QString& name = QString() );

    ~WX_GRID() override;

    void SetColLabelSize( int aHeight );

    void SetLabelFont( const QFont& aFont );

    void EnableAlternateRowColors( bool aEnable = true );

    QString GetShownColumnsAsString();
    std::bitset<64> GetShownColumns();

    void ShowHideColumns( const QString& shownColumns );

    void ShowHideColumns( const std::bitset<64>& aShownColumns );

    void SetTable( QAbstractTableModel* table, bool aTakeOwnership = false );

    void DestroyTable( QAbstractTableModel* aTable );

    bool CommitPendingChanges( bool aQuietMode = false );
    bool CancelPendingChanges();

    void SetUnitsProvider( UNITS_PROVIDER* aProvider, int aCol = 0 );

    void SetAutoEvalCols( const std::vector<int>& aCols ) { m_autoEvalCols = aCols; }

    int GetUnitValue( int aRow, int aCol );

    std::optional<int> GetOptionalUnitValue( int aRow, int aCol );

    void SetUnitValue( int aRow, int aCol, int aValue );

    void SetOptionalUnitValue( int aRow, int aCol, std::optional<int> aValue );

    int GetVisibleWidth( int aCol, bool aHeader = true, bool aContents = true, bool aKeep = false );

    void EnsureColLabelsVisible();

    void ShowEditorOnMouseUp() { m_waitForSlowClick = true; }
    void CancelShowEditorOnMouseUp() { m_waitForSlowClick = false; }

    void ClearRows()
    {
        if( rowCount() )
            removeRows( 0, rowCount() );
    }

    static void CellEditorSetMargins( QLineEdit* aEntry );

    static void CellEditorTransformSizeRect( QRect& aRect );

    void OverrideMinSize( double aXPct, double aYPct )
    {
        QSize size = sizeHint();
        m_minSizeOverride = QSize( KiROUND( size.width() * aXPct ), KiROUND( size.height() * aYPct ) );
    }

    QSize sizeHint() const override
    {
        if( m_minSizeOverride )
            return m_minSizeOverride.value();
        else
            return QTableWidget::sizeHint();
    }

protected:
    void paintEvent( QPaintEvent* event ) override;

    void onGridColMove( int logicalIndex, int oldVisualIndex, int newVisualIndex );
    void onGridCellSelect( int row, int column );
    void onCellEditorShown( int row, int column );
    void onCellEditorHidden( int row, int column );

    bool                               m_weOwnTable;
    bool                               m_waitForSlowClick;

    std::map<int, UNITS_PROVIDER*>     m_unitsProviders;
    std::unique_ptr<NUMERIC_EVALUATOR> m_eval;
    std::vector<int>                   m_autoEvalCols;

    std::map< std::pair<int, int>, std::pair<QString, QString> > m_evalBeforeAfter;

    std::optional<QSize>               m_minSizeOverride;
};

#endif //KICAD_WX_GRID_H
