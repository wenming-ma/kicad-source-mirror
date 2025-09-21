
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef SYM_TREE_PANE_H
#define SYM_TREE_PANE_H

#include <QWidget>
#include <QAbstractItemView>
#include <vector>

class LIB_TREE;
class SYMBOL_EDIT_FRAME;
class LIB_SYMBOL_LIBRARY_MANAGER;
class QBoxLayout;

/**
 * Library Editor pane with symbol tree and symbol library table selector.
 */
class SYMBOL_TREE_PANE : public QWidget
{
public:
    SYMBOL_TREE_PANE( SYMBOL_EDIT_FRAME* aParent, LIB_SYMBOL_LIBRARY_MANAGER* aLibMgr );
    ~SYMBOL_TREE_PANE();

    LIB_TREE* GetLibTree() const
    {
        return m_tree;
    }

protected:
    void onSymbolSelected();
    void onUpdateUI();

    /**
     * Handle parent menu events to block preview updates while the menu is open.
     */
    void onMenuOpen();
    void onMenuClose();

    SYMBOL_EDIT_FRAME*          m_symbolEditFrame;
    LIB_TREE*                   m_tree;             ///< symbol search tree widget
    LIB_SYMBOL_LIBRARY_MANAGER* m_libMgr;
};

#endif /* SYM_TREE_PANE_H */
