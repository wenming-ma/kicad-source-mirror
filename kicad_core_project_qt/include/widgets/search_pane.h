
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef SEARCH_PANE_H
#define SEARCH_PANE_H

#include <memory>
#include <vector>

#include <widgets/search_pane_base.h>
#include <QAbstractItemView>
#include <QString>
#include <QEvent>


class QCloseEvent;
class ACTION_MENU;
class EDA_DRAW_FRAME;
class SEARCH_PANE_TAB;

class SEARCH_HANDLER
{
public:
    SEARCH_HANDLER( const QString& aName ) :
            m_name( aName )
    {}

    virtual ~SEARCH_HANDLER()
    {}

    QString GetName() const { return m_name; }

    std::vector<std::tuple<QString, int, QAbstractItemView::SelectionBehavior>> GetColumns() const
    {
        return m_columns;
    }

    virtual int Search( const QString& string ) = 0;
    virtual QString GetResultCell( int row, int col ) = 0;
    virtual void Sort( int aCol, bool aAscending, std::vector<long>* aSelection ) = 0;

    virtual void SelectItems( std::vector<long>& aItemRows ) {}
    virtual void ActivateItem( long aItemRow ) {}

protected:
    QString                                                   m_name;
    std::vector<std::tuple<QString, int, QAbstractItemView::SelectionBehavior>> m_columns;
};


class SEARCH_PANE : public SEARCH_PANE_BASE
{
public:
    SEARCH_PANE( EDA_DRAW_FRAME* aFrame );
    virtual ~SEARCH_PANE();

    // We own at least one list of raw pointers.  Don't let the compiler fill in copy c'tors that
    // will only land us in trouble.
    SEARCH_PANE( const SEARCH_PANE& ) = delete;
    SEARCH_PANE& operator=( const SEARCH_PANE& ) = delete;

    void AddSearcher( const std::shared_ptr<SEARCH_HANDLER>& aHandler );
    void OnSearchTextEntry( QEvent& aEvent ) override;
    void OnNotebookPageChanged( QEvent& aEvent ) override;

    void RefreshSearch();
    void FocusSearch();
    void ClearAllResults();

    void OnCharHook( QKeyEvent& aEvent );

protected:
    void             OnLanguageChange( QEvent& aEvent );
    SEARCH_PANE_TAB* GetCurrentTab() const;
    void             OnClosed( QCloseEvent& aEvent );

private:
    std::vector<std::shared_ptr<SEARCH_HANDLER>> m_handlers;
    std::vector<SEARCH_PANE_TAB*>                m_tabs;
    QString                                     m_lastQuery;
    EDA_DRAW_FRAME*                              m_frame;
    ACTION_MENU*                                 m_menu;
};

#endif
