
#ifndef BOARD_COMMIT_H
#define BOARD_COMMIT_H

#include <commit.h>
#include <math/box2.h>
#include <QString>

class BOARD_ITEM;
class ZONE;
class BOARD;
class PICKED_ITEMS_LIST;
class PCB_TOOL_BASE;
class TOOL_MANAGER;
class EDA_DRAW_FRAME;
class TOOL_BASE;

#define SKIP_UNDO          0x0001
#define APPEND_UNDO        0x0002
#define SKIP_SET_DIRTY     0x0004
#define SKIP_CONNECTIVITY  0x0008
#define ZONE_FILL_OP       0x0010
#define SKIP_TEARDROPS     0x0020

class BOARD_COMMIT : public COMMIT
{
public:
    BOARD_COMMIT( EDA_DRAW_FRAME* aFrame );
    BOARD_COMMIT( TOOL_BASE* aTool );
    BOARD_COMMIT( TOOL_MANAGER* aMgr );
    BOARD_COMMIT( TOOL_MANAGER* aMgr, bool aIsBoardEditor );

    virtual ~BOARD_COMMIT() {}

    BOARD* GetBoard() const;

    virtual void Push( const QString& aMessage = QString(), int aCommitFlags = 0 ) override;

    virtual void Revert() override;
    COMMIT&      Stage( EDA_ITEM* aItem, CHANGE_TYPE aChangeType,
                        BASE_SCREEN* aScreen = nullptr ) override;
    COMMIT&      Stage( std::vector<EDA_ITEM*>& container, CHANGE_TYPE aChangeType,
                        BASE_SCREEN* aScreen = nullptr ) override;
    COMMIT&      Stage( const PICKED_ITEMS_LIST& aItems,
                        UNDO_REDO aModFlag = UNDO_REDO::UNSPECIFIED,
                        BASE_SCREEN* aScreen = nullptr ) override;

    static EDA_ITEM* MakeImage( EDA_ITEM* aItem );

private:
    EDA_ITEM* parentObject( EDA_ITEM* aItem ) const override;

    EDA_ITEM* makeImage( EDA_ITEM* aItem ) const override;

    void propagateDamage( BOARD_ITEM* aItem, std::vector<ZONE*>* aStaleZones,
                          std::vector<BOX2I>& aStaleRuleAreas );

private:
    TOOL_MANAGER*  m_toolMgr;
    bool           m_isBoardEditor;
    bool           m_isFootprintEditor;
};

#endif
