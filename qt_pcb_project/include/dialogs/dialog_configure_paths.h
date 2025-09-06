
#ifndef _DIALOG_CONFIGURE_PATHS_H_
#define _DIALOG_CONFIGURE_PATHS_H_

#include <memory>

#include <QString>
#include <QValidator>

#include <../common/dialogs/dialog_configure_paths_base.h>


class HTML_WINDOW;
class QT_GRID_AUTOSIZER;


class DIALOG_CONFIGURE_PATHS: public DIALOG_CONFIGURE_PATHS_BASE
{
public:
    DIALOG_CONFIGURE_PATHS(  QWidget* aParent );
    ~DIALOG_CONFIGURE_PATHS() override;

    bool TransferDataToWindow();
    bool TransferDataFromWindow() override;

protected:
    // Various button callbacks
    void OnUpdateUI() override;
    void OnGridCellChanging( QEvent& event );
    void OnAddEnvVar() override;
    void OnRemoveEnvVar() override;
    void OnHelp() override;

    void AppendEnvVar( const QString& aName, const QString& aPath, bool isExternal );
    void AppendSearchPath( const QString& aName, const QString& aPath, const QString& aDesc );

private:
    QString             m_errorMsg;
    QTableWidget*       m_errorGrid;
    int                 m_errorRow;
    int                 m_errorCol;

    QString             m_curdir;
    QValidator*         m_aliasValidator;

    std::unique_ptr<QT_GRID_AUTOSIZER> m_gridAutosizer;

    HTML_WINDOW*        m_helpBox;
    int                 m_heightBeforeHelp;
};

#endif    // _DIALOG_CONFIGURE_PATHS_H_
