
#ifndef SCHEMATIC_COMMIT_H
#define SCHEMATIC_COMMIT_H

#include <commit.h>
#include <QString>

class PICKED_ITEMS_LIST;
class TOOL_MANAGER;
class SCH_EDIT_FRAME;
class SCH_BASE_FRAME;
class EDA_DRAW_FRAME;
class TOOL_BASE;

template<class T>
class SCH_TOOL_BASE;

#define SKIP_UNDO          0x0001
#define APPEND_UNDO        0x0002
#define SKIP_SET_DIRTY     0x0004

class SCH_COMMIT : public COMMIT
{
public:
    SCH_COMMIT( TOOL_MANAGER* aToolMgr );
    SCH_COMMIT( EDA_DRAW_FRAME* aFrame );
    SCH_COMMIT( SCH_TOOL_BASE<SCH_BASE_FRAME>* aFrame );

    virtual ~SCH_COMMIT();

    virtual void Push( const QString& aMessage = "A commit",
                       int aCommitFlags = 0 ) override;

    virtual void Revert() override;
    COMMIT& Stage( EDA_ITEM *aItem, CHANGE_TYPE aChangeType,
                   BASE_SCREEN *aScreen = nullptr ) override;
    COMMIT& Stage( std::vector<EDA_ITEM*> &container, CHANGE_TYPE aChangeType,
                   BASE_SCREEN *aScreen = nullptr ) override;
    COMMIT& Stage( const PICKED_ITEMS_LIST &aItems, UNDO_REDO aModFlag = UNDO_REDO::UNSPECIFIED,
                   BASE_SCREEN *aScreen = nullptr ) override;

private:
    EDA_ITEM* parentObject( EDA_ITEM* aItem ) const override;

    EDA_ITEM* makeImage( EDA_ITEM* aItem ) const override;

    void pushLibEdit(  const QString& aMessage, int aCommitFlags );
    void pushSchEdit(  const QString& aMessage, int aCommitFlags );

    void revertLibEdit();

private:
    TOOL_MANAGER*  m_toolMgr;
    bool           m_isLibEditor;
};

#endif
