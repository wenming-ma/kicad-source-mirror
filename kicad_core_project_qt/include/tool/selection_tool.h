#pragma once
#ifndef INCLUDE_TOOL_SELECTION_TOOL_H_
#define INCLUDE_TOOL_SELECTION_TOOL_H_

#include <math/vector2d.h>
#include <tool/tool_interactive.h>
#include <QTimer>
#include <QObject>
#include <eda_item.h>

class COLLECTOR;
class KIID;


class SELECTION_TOOL : public TOOL_INTERACTIVE, public QObject
{
public:
    SELECTION_TOOL( const std::string& aName );
    ~SELECTION_TOOL(){};

    int UpdateMenu( const TOOL_EVENT& aEvent );

    int AddItemToSel( const TOOL_EVENT& aEvent );
    void AddItemToSel( EDA_ITEM* aItem, bool aQuietMode = false );
    int AddItemsToSel( const TOOL_EVENT& aEvent );
    void AddItemsToSel( EDA_ITEMS* aList, bool aQuietMode = false );

    int RemoveItemFromSel( const TOOL_EVENT& aEvent );
    void RemoveItemFromSel( EDA_ITEM* aItem, bool aQuietMode = false );
    int RemoveItemsFromSel( const TOOL_EVENT& aEvent );
    void RemoveItemsFromSel( EDA_ITEMS* aList, bool aQuietMode = false );

    int ReselectItem( const TOOL_EVENT& aEvent );

    void RemoveItemsFromSel( std::vector<KIID>* aList, bool aQuietMode = false );

    void BrightenItem( EDA_ITEM* aItem );
    void UnbrightenItem( EDA_ITEM* aItem );

    int SelectionMenu( const TOOL_EVENT& aEvent );

    SELECTION& GetSelection() { return selection(); }

protected:
    virtual SELECTION& selection() = 0;

   void onDisambiguationExpire();

    virtual void select( EDA_ITEM* aItem ) = 0;

    virtual void unselect( EDA_ITEM* aItem ) = 0;

    virtual void highlight( EDA_ITEM* aItem, int aHighlightMode, SELECTION* aGroup = nullptr ) = 0;

    virtual void unhighlight( EDA_ITEM* aItem, int aHighlightMode,
                              SELECTION* aGroup = nullptr ) = 0;

    void setModifiersState( bool aShiftState, bool aCtrlState, bool aAltState );

    bool hasModifier();

    virtual bool ctrlClickHighlights() { return false; }

    bool doSelectionMenu( COLLECTOR* aCollector );

protected:
    bool            m_additive;          ///< Items should be added to sel (instead of replacing).
    bool            m_subtractive;       ///< Items should be removed from selection.
    bool            m_exclusive_or;      ///< Items' selection state should be toggled.
    bool            m_multiple;          ///< Multiple selection mode is active.

    /// Show disambiguation menu for all items under the cursor rather than trying to narrow
    /// them down first using heuristics.
    bool            m_skip_heuristics;
    bool            m_highlight_modifier;///< Select highlight net on left click.
    bool            m_drag_additive;     ///< Add multiple items to selection.
    bool            m_drag_subtractive;  ///< Remove multiple from selection.

    bool            m_canceledMenu;      ///< Sets to true if the disambiguation menu was canceled.

    QTimer*         m_disambiguateTimer; ///< Timer to show the disambiguate menu.

    VECTOR2I        m_originalCursor;    ///< Location of original cursor when starting click.
};

#endif /* INCLUDE_TOOL_SELECTION_TOOL_H_ */
