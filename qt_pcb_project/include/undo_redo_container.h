
#ifndef _CLASS_UNDOREDO_CONTAINER_H
#define _CLASS_UNDOREDO_CONTAINER_H

#include <core/typeinfo.h>
#include <eda_item_flags.h>
#include <functional>
#include <kiid.h>
#include <vector>
#include <QString>

class EDA_ITEM;
class PICKED_ITEMS_LIST;
class BASE_SCREEN;


// Undo Redo considerations:
// Basically we have 3 cases
//      New item
//      Deleted item
//      Modified item
// there is also a specific case in Eeschema, when wires are modified
// If an item is modified, a copy of the "old" item parameters value is held.
// When an item is deleted or added (new item) the pointer points the item, and there is
// no other copy.

// Type of undo/redo operations.
// Each type must be redo/undone by a specific operation.
enum class UNDO_REDO {
    UNSPECIFIED = 0,    // illegal
    CHANGED,            // params of items have a value changed: undo is made by exchange
                        // values with a copy of these values
    NEWITEM,            // new item, undo by changing in deleted
    DELETED,            // deleted item, undo by changing in deleted
    LIBEDIT,            // Specific to the component editor (symbol_editor creates a full copy
                        // of the current component when changed)
    LIB_RENAME,         // As LIBEDIT, but old copy should be removed from library
    DRILLORIGIN,        // origin changed (like CHANGED, contains the origin and a copy)
    GRIDORIGIN,         // origin changed (like CHANGED, contains the origin and a copy)
    PAGESETTINGS,       // page settings or title block changes
    REGROUP,            // new group of items created (NB: can't use GROUP because of collision
                        // with a header on msys2)
    UNGROUP,            // existing group destroyed (items not destroyed)
    REPEAT_ITEM         // storage entry for the editor's global repeatItems list
};


class ITEM_PICKER
{
public:
//    ITEM_PICKER( EDA_ITEM* aItem = NULL, UNDO_REDO aStatus = UNSPECIFIED );
    ITEM_PICKER();
    ITEM_PICKER( BASE_SCREEN* aScreen, EDA_ITEM* aItem,
                 UNDO_REDO aStatus = UNDO_REDO::UNSPECIFIED );

    EDA_ITEM* GetItem() const { return m_pickedItem; }

    void SetItem( EDA_ITEM* aItem );

    KICAD_T GetItemType() const { return m_pickedItemType; }

    void SetStatus( UNDO_REDO aStatus ) { m_undoRedoStatus = aStatus; }

    UNDO_REDO GetStatus() const { return m_undoRedoStatus; }

    void SetFlags( EDA_ITEM_FLAGS aFlags ) { m_pickerFlags = aFlags; }

    EDA_ITEM_FLAGS GetFlags() const { return m_pickerFlags; }

    void SetLink( EDA_ITEM* aItem ) { m_link = aItem; }

    EDA_ITEM* GetLink() const { return m_link; }

    KIID GetGroupId() const { return m_groupId; }

    void SetGroupId( KIID aId ) { m_groupId = aId; }

    BASE_SCREEN* GetScreen() const { return m_screen; }

private:
    EDA_ITEM_FLAGS m_pickerFlags;      /* A copy of m_flags member. Currently used only to flag
                                        * transient items. */
    UNDO_REDO      m_undoRedoStatus;   /* type of operation to undo/redo for this item */
    EDA_ITEM*      m_pickedItem;       /* Pointer on the schematic or board item that is concerned
                                        * (picked), or in undo redo commands, the copy of an
                                        * edited item. */
    KICAD_T        m_pickedItemType;   /* type of schematic or board item that is concerned */

    EDA_ITEM*      m_link;             /* Pointer on another item. Used in undo redo command
                                        * used when a duplicate exists i.e. when an item is
                                        * modified, and the copy of initial item exists (the
                                        * duplicate) m_Item points the duplicate (i.e the old
                                        * copy of an active item) and m_Link points the active
                                        * item in schematic */
    KIID           m_groupId;           /* Id of the group of items in case this is a
                                         * group/ungroup command */

    BASE_SCREEN*   m_screen;           /* For new and deleted items the screen the item should
                                        * be added to/removed from. */

};


// A holder to handle information on schematic or board items.
// The information held is a pointer on each item, and the command made.
class PICKED_ITEMS_LIST
{
public:
    PICKED_ITEMS_LIST();
    ~PICKED_ITEMS_LIST();

    void PushItem( const ITEM_PICKER& aItem );

    ITEM_PICKER PopItem();

    bool ContainsItem( const EDA_ITEM* aItem ) const;

    bool ContainsItemType( KICAD_T aItemType ) const;

    int FindItem( const EDA_ITEM* aItem ) const;

    void ClearItemsList();

    void ClearListAndDeleteItems( std::function<void(EDA_ITEM*)> aItemDeleter );

    unsigned GetCount() const
    {
        return m_ItemsList.size();
    }

    void ReversePickersListOrder();

    ITEM_PICKER GetItemWrapper( unsigned int aIdx ) const;

    EDA_ITEM* GetPickedItem( unsigned int aIdx ) const;

    BASE_SCREEN* GetScreenForItem( unsigned int aIdx ) const;

    EDA_ITEM* GetPickedItemLink( unsigned int aIdx ) const;

    UNDO_REDO GetPickedItemStatus( unsigned int aIdx ) const;

    KIID GetPickedItemGroupId( unsigned int aIdx ) const;

    EDA_ITEM_FLAGS GetPickerFlags( unsigned aIdx ) const;

    bool SetPickedItem( EDA_ITEM* aItem, unsigned aIdx );

    bool SetPickedItem( EDA_ITEM* aItem, UNDO_REDO aStatus, unsigned aIdx );

    bool SetPickedItemLink( EDA_ITEM* aLink, unsigned aIdx );

    bool SetPickedItemGroupId( KIID aId, unsigned aIdx );

    bool SetPickedItemStatus( UNDO_REDO aStatus, unsigned aIdx );

    bool SetPickerFlags( EDA_ITEM_FLAGS aFlags, unsigned aIdx );

    bool RemovePicker( unsigned aIdx );

    void CopyList( const PICKED_ITEMS_LIST& aSource );

    QString GetDescription() const                     { return m_description; }
    void SetDescription( const QString& aDescription ) { m_description = aDescription; }

private:
    QString                  m_description;
    std::vector<ITEM_PICKER> m_ItemsList;
};


// A holder to handle a list of undo (or redo) commands.
class UNDO_REDO_CONTAINER
{
public:
    UNDO_REDO_CONTAINER();
    ~UNDO_REDO_CONTAINER();

    // We own at least one list of raw pointers.  Don't let the compiler fill in copy c'tors that
    // will only land us in trouble.
    UNDO_REDO_CONTAINER( const UNDO_REDO_CONTAINER& ) = delete;
    UNDO_REDO_CONTAINER& operator=( const UNDO_REDO_CONTAINER& ) = delete;

    void PushCommand( PICKED_ITEMS_LIST* aCommand );

    PICKED_ITEMS_LIST* PopCommand();

    void ClearCommandList();

    std::vector <PICKED_ITEMS_LIST*> m_CommandsList;   // the list of possible undo/redo commands
};


#endif      // _CLASS_UNDOREDO_CONTAINER_H
