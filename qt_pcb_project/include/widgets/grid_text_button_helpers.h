// QT_TRANSFORMATION_COMPLETED
#ifndef GRID_TEXT_BUTTON_HELPERS_H
#define GRID_TEXT_BUTTON_HELPERS_H

#include <memory>

#include <QComboBox>
#include <QAbstractItemDelegate>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QWidget>
#include <QKeyEvent>
#include <QRect>
#include <QString>
#include <QValidator>
#include <functional>
#include <vector>


class QTableWidget;
class WX_GRID;
class DIALOG_SHIM;
class EMBEDDED_FILES;


class GRID_CELL_TEXT_BUTTON : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GRID_CELL_TEXT_BUTTON() {};

    QString GetValue() const;

    void SetSize( const QRect& aRect );

    void StartingKey( QKeyEvent& event );
    void BeginEdit( int aRow, int aCol, QTableWidget* aGrid );
    bool EndEdit( int , int , const QTableWidget* , const QString& , QString *aNewVal );
    void ApplyEdit( int aRow, int aCol, QTableWidget* aGrid );
    void Reset();

    void SetValidator( const QValidator& validator );

protected:
    QComboBox* Combo() const { return static_cast<QComboBox*>( m_control ); }

    std::unique_ptr< QValidator > m_validator;

    QString     m_value;
    QWidget*    m_control = nullptr;

    Q_DISABLE_COPY(GRID_CELL_TEXT_BUTTON)
};


class GRID_CELL_SYMBOL_ID_EDITOR : public GRID_CELL_TEXT_BUTTON
{
    Q_OBJECT

public:
    GRID_CELL_SYMBOL_ID_EDITOR( DIALOG_SHIM* aParent,
                                const QString& aPreselect = QString() ) :
            m_dlg( aParent ),
            m_preselect( aPreselect )
    { }

    QAbstractItemDelegate* Clone() const
    {
        return new GRID_CELL_SYMBOL_ID_EDITOR( m_dlg, m_preselect );
    }

    void Create( QWidget* aParent, int aId, QObject* aEventHandler );

protected:
    DIALOG_SHIM* m_dlg;
    QString     m_preselect;
};


class GRID_CELL_FPID_EDITOR : public GRID_CELL_TEXT_BUTTON
{
    Q_OBJECT

public:
    GRID_CELL_FPID_EDITOR( DIALOG_SHIM* aParent, const QString& aSymbolNetlist,
                           const QString& aPreselect = QString() ) :
            m_dlg( aParent ),
            m_preselect( aPreselect ),
            m_symbolNetlist( aSymbolNetlist )
    { }

    QAbstractItemDelegate* Clone() const
    {
        return new GRID_CELL_FPID_EDITOR( m_dlg, m_symbolNetlist );
    }

    void Create( QWidget* aParent, int aId, QObject* aEventHandler );

protected:
    DIALOG_SHIM* m_dlg;
    QString     m_preselect;
    QString     m_symbolNetlist;
};


class GRID_CELL_URL_EDITOR : public GRID_CELL_TEXT_BUTTON
{
    Q_OBJECT

public:
    GRID_CELL_URL_EDITOR( DIALOG_SHIM* aParent, SEARCH_STACK* aSearchStack = nullptr,
                          std::vector<EMBEDDED_FILES*> aFilesStack = {} ) :
            m_dlg( aParent ),
            m_searchStack( aSearchStack ),
            m_filesStack( aFilesStack )
    { }

    QAbstractItemDelegate* Clone() const
    {
        return new GRID_CELL_URL_EDITOR( m_dlg );
    }

    void Create( QWidget* aParent, int aId, QObject* aEventHandler );

protected:
    DIALOG_SHIM*                 m_dlg;
    SEARCH_STACK*                m_searchStack;     // No ownership.
    std::vector<EMBEDDED_FILES*> m_filesStack;      // No ownership.
};


class GRID_CELL_PATH_EDITOR : public GRID_CELL_TEXT_BUTTON
{
    Q_OBJECT

public:
    GRID_CELL_PATH_EDITOR( DIALOG_SHIM* aParentDialog, WX_GRID* aGrid, QString* aCurrentDir,
                           bool aNormalize, const QString& aNormalizeBasePath,
                           std::function<QString( WX_GRID* grid, int row )> aFileFilterFn ) :
            m_dlg( aParentDialog ),
            m_grid( aGrid ),
            m_currentDir( aCurrentDir ),
            m_normalize( aNormalize ),
            m_normalizeBasePath( aNormalizeBasePath ),
            m_fileFilterFn( std::move( aFileFilterFn ) )
    { }

    GRID_CELL_PATH_EDITOR( DIALOG_SHIM* aParentDialog, WX_GRID* aGrid, QString* aCurrentDir,
                           const QString& aFileFilter, bool aNormalize = false,
                           const QString& aNormalizeBasePath = QString() ) :
            m_dlg( aParentDialog ),
            m_grid( aGrid ),
            m_currentDir( aCurrentDir ),
            m_normalize( aNormalize ),
            m_normalizeBasePath( aNormalizeBasePath ),
            m_fileFilter( aFileFilter )
    { }

    QAbstractItemDelegate* Clone() const
    {
        if( m_fileFilterFn )
        {
            return new GRID_CELL_PATH_EDITOR( m_dlg, m_grid, m_currentDir, m_normalize,
                                              m_normalizeBasePath, m_fileFilterFn );
        }
        else
        {
            return new GRID_CELL_PATH_EDITOR( m_dlg, m_grid, m_currentDir, m_fileFilter,
                                              m_normalize, m_normalizeBasePath );
        }
    }

    void Create( QWidget* aParent, int aId, QObject* aEventHandler );

protected:
    DIALOG_SHIM* m_dlg;
    WX_GRID*     m_grid;
    QString*    m_currentDir;
    bool         m_normalize;
    QString     m_normalizeBasePath;

    QString                                            m_fileFilter;
    std::function<QString( WX_GRID* aGrid, int aRow )> m_fileFilterFn;
};


#endif  // GRID_TEXT_BUTTON_HELPERS_H