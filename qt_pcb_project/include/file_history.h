// QT_TRANSFORMATION_COMPLETED

#ifndef FILEHISTORY_H_
#define FILEHISTORY_H_

#include <tool/action_menu.h>
#include <tool/selection_conditions.h>
#include <QObject>
#include <QString>
#include <QMenu>
#include <QAction>
#include <QStringList>
#include <vector>
#include <string>


class APP_SETTINGS_BASE;

class FILE_HISTORY : public QObject
{
    Q_OBJECT

public:
    FILE_HISTORY( size_t aMaxFiles, int aBaseFileId, int aClearId,
            QString aClearText = QString() );

    void Load( const APP_SETTINGS_BASE& aSettings );
    void Load( const std::vector<std::string>& aList );
    void Save( APP_SETTINGS_BASE& aSettings );
    void Save( std::vector<std::string>* aList );

    void AddFileToHistory( const QString& aFile );

    void AddFilesToMenu();
    void AddFilesToMenu( QMenu* aMenu );

    void SetMaxFiles( size_t aMaxFiles );

    void SetClearText( QString aClearText )
    {
        m_clearText = aClearText;
    }

    void UpdateClearText( QMenu* aMenu, QString aClearText );
    void ClearFileHistory();

    size_t GetCount() const;
    QString GetHistoryFile( size_t index ) const;
    void RemoveFileFromHistory( size_t index );
    void AddFileMenu( QMenu* aMenu );
    void RemoveFileMenu( QMenu* aMenu );

    static SELECTION_CONDITION FileHistoryNotEmpty( const FILE_HISTORY& aHistory );

protected:
    void doRemoveClearitem( QMenu* aMenu );
    void doAddClearItem( QMenu* aMenu );

private:
    static bool isHistoryNotEmpty( const SELECTION& aSelection, const FILE_HISTORY& aHistory );

    QStringList             m_fileHistory;
    std::vector<QMenu*>     m_fileMenus;
    size_t      m_maxFiles;
    int         m_baseFileId;
    int         m_clearId;
    QString     m_clearText;
};

#endif
