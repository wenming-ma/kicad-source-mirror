
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef DIALOG_SCH_FIND_H
#define DIALOG_SCH_FIND_H

#include "dialog_schematic_find_base.h"

#include <QCloseEvent>
#include <QKeyEvent>
#include <QEvent>

class SCH_EDIT_FRAME;
class SCH_FIND_REPLACE_TOOL;
struct SCH_SEARCH_DATA;


class DIALOG_SCH_FIND : public DIALOG_SCH_FIND_BASE
{
public:
    DIALOG_SCH_FIND( SCH_EDIT_FRAME* aParent, SCH_SEARCH_DATA* aData,
                     const QPoint& aPosition = QPoint(),
                     const QSize& aSize = QSize(), int aStyle = 0 );
    ~DIALOG_SCH_FIND();

    void SetFindEntries( const QStringList& aEntries, const QString& aFindString );
    QStringList GetFindEntries() const;

    void SetReplaceEntries( const QStringList& aEntries );
    QStringList GetReplaceEntries() const { return m_comboReplace->GetStrings(); }

protected:
    // Handlers for DIALOG_SCH_FIND_BASE events.
    void OnClose( QCloseEvent* aEvent ) override;
    void OnCancel() override;
    void OnSearchForSelect() override;
    void OnSearchForText() override;
    void OnSearchForEnter() override;
    void OnReplaceWithSelect() override;
    void OnReplaceWithText() override;
    void OnReplaceWithEnter() override;
    void OnOptions() override;
    void OnUpdateReplaceUI() override;
    void OnUpdateReplaceAllUI() override;
    void OnIdle( QEvent* event ) override;
    void OnChar( QKeyEvent& aEvent );
    void OnFind() override;
    void OnReplace() override;

    // Rebuild the search flags from dialog settings
    void updateFlags();

    SCH_EDIT_FRAME*        m_frame;
    SCH_FIND_REPLACE_TOOL* m_findReplaceTool;
    SCH_SEARCH_DATA*       m_findReplaceData;
    bool                   m_findDirty;

    Q_DISABLE_COPY( DIALOG_SCH_FIND )
};


#endif // DIALOG_SCH_FIND_H
