
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef GRID_TEXT_BUTTON_HELPERS_H
#define GRID_TEXT_BUTTON_HELPERS_H

#include <memory>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QAbstractItemDelegate>
#include <QtWidgets/QStyledItemDelegate>
#include <QtCore/QString>
#include <QtCore/QRect>
#include <QtWidgets/QWidget>
#include <QtGui/QValidator>
#include <QtGui/QKeyEvent>


class QTableWidget;
class DIALOG_SHIM;
class EMBEDDED_FILES;
class SEARCH_STACK;


class GRID_CELL_TEXT_BUTTON : public QStyledItemDelegate
{
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
    QWidget*    m_control;

    Q_DISABLE_COPY( GRID_CELL_TEXT_BUTTON );
};


class GRID_CELL_SYMBOL_ID_EDITOR : public GRID_CELL_TEXT_BUTTON
{
public:
    GRID_CELL_SYMBOL_ID_EDITOR( DIALOG_SHIM* aParent,
                                const QString& aPreselect = QString() ) :
            m_dlg( aParent ),
            m_preselect( aPreselect )
    { }

    GRID_CELL_SYMBOL_ID_EDITOR* Clone() const
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
public:
    GRID_CELL_FPID_EDITOR( DIALOG_SHIM* aParent, const QString& aSymbolNetlist,
                           const QString& aPreselect = QString() ) :
            m_dlg( aParent ),
            m_preselect( aPreselect ),
            m_symbolNetlist( aSymbolNetlist )
    { }

    GRID_CELL_FPID_EDITOR* Clone() const
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
public:
    GRID_CELL_URL_EDITOR( DIALOG_SHIM* aParent, SEARCH_STACK* aSearchStack = nullptr,
                          std::vector<EMBEDDED_FILES*> aFilesStack = {} ) :
            m_dlg( aParent ),
            m_searchStack( aSearchStack ),
            m_filesStack( aFilesStack )
    { }

    GRID_CELL_URL_EDITOR* Clone() const
    {
        return new GRID_CELL_URL_EDITOR( m_dlg );
    }

    void Create( QWidget* aParent, int aId, QObject* aEventHandler );

protected:
    DIALOG_SHIM*                 m_dlg;
    SEARCH_STACK*                m_searchStack;     // No ownership.
    std::vector<EMBEDDED_FILES*> m_filesStack;      // No ownership.
};


// Editor for QTableWidget cells that adds a file/folder browser to the grid input field
class GRID_CELL_PATH_EDITOR : public GRID_CELL_TEXT_BUTTON
{
public:
    /**
     * Constructor
     *
     * @param aCurrentDir is current directory the path editor will open at
     * @param aNormalize indicates whether to normalize the selected path (replace part of path
     *                   with variables or relative path)
     * @param aNormalizeBasePath is the path to use when trying to base variables (generally
     *                           current project path)
     * @param aFileFilterFn a callback which provides a file extension(s) filter.
     */
    GRID_CELL_PATH_EDITOR( DIALOG_SHIM* aParentDialog, QTableWidget* aGrid, QString* aCurrentDir,
                           bool aNormalize, const QString& aNormalizeBasePath,
                           std::function<QString( QTableWidget* grid, int row )> aFileFilterFn ) :
            m_dlg( aParentDialog ),
            m_grid( aGrid ),
            m_currentDir( aCurrentDir ),
            m_normalize( aNormalize ),
            m_normalizeBasePath( aNormalizeBasePath ),
            m_fileFilterFn( std::move( aFileFilterFn ) )
    { }

    /**
     * Constructor
     *
     * @param aCurrentDir is current directory the path editor will open at
     * @param aFileFilter is the file extension(s) to filter by. If empty, the path editor will
     *                    switch to folder mode instead of file.
     * @param aNormalize indicates whether to normalize the selected path (replace part of path
     *                   with variables or relative path)
     * @param aNormalizeBasePath is the path to use when trying to base variables (generally
     *                           current project path)
     */
    GRID_CELL_PATH_EDITOR( DIALOG_SHIM* aParentDialog, QTableWidget* aGrid, QString* aCurrentDir,
                           const QString& aFileFilter, bool aNormalize = false,
                           const QString& aNormalizeBasePath = QString() ) :
            m_dlg( aParentDialog ),
            m_grid( aGrid ),
            m_currentDir( aCurrentDir ),
            m_normalize( aNormalize ),
            m_normalizeBasePath( aNormalizeBasePath ),
            m_fileFilter( aFileFilter )
    { }

    GRID_CELL_PATH_EDITOR* Clone() const
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
    DIALOG_SHIM*    m_dlg;
    QTableWidget*   m_grid;
    QString*    m_currentDir;
    bool         m_normalize;
    QString     m_normalizeBasePath;

    QString                                            m_fileFilter;
    std::function<QString( QTableWidget* aGrid, int aRow )> m_fileFilterFn;
};


#endif  // GRID_TEXT_BUTTON_HELPERS_H
