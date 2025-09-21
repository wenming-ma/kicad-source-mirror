///////////////////////////////////////////////////////////////////////////
// C++ code generated with Qt Designer equivalent
// Transformed from wxFormBuilder to Qt framework
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtCore/QString>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtGui/QFocusEvent>
#include <QtGui/QResizeEvent>
class STD_BITMAP_BUTTON;

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class SEARCH_PANE_BASE
///////////////////////////////////////////////////////////////////////////////
class SEARCH_PANE_BASE : public QWidget
{
	private:

	protected:
		QVBoxLayout* m_sizerOuter;
		QLineEdit* m_searchCtrl1;
		QFrame* m_staticline1;
		STD_BITMAP_BUTTON* m_menuButton;
		QTabWidget* m_notebook;

		// Virtual event handlers, override them in your derived class
		virtual void OnSetFocus( QFocusEvent* event ) { event->accept(); }
		virtual void OnSize( QResizeEvent* event ) { event->accept(); }
		virtual void OnSearchTextEntry( const QString& text ) { Q_UNUSED(text); }
		virtual void OnNotebookPageChanged( int index ) { Q_UNUSED(index); }


	public:

		SEARCH_PANE_BASE( QWidget* parent = nullptr, const QPoint& pos = QPoint(), const QSize& size = QSize( 284,110 ), const QString& name = QString() );

		~SEARCH_PANE_BASE();

};
