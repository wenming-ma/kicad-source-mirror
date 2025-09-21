#pragma once

#include "widgets/html_window.h"
#include "dialog_shim.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDialog>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QEvent>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_DISPLAY_HTML_TEXT_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_DISPLAY_HTML_TEXT_BASE : public DIALOG_SHIM
{
	private:

	protected:
		HTML_WINDOW* m_htmlWindow;
		QDialogButtonBox* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;

		// Virtual event handlers, override them in your derived class
		virtual void OnHTMLLinkClicked( QEvent& event ) { event.ignore(); }


	public:

		DIALOG_DISPLAY_HTML_TEXT_BASE( QWidget* parent, int id = -1, const QString& title = QString(), const QPoint& pos = QPoint(-1, -1), const QSize& size = QSize( -1,-1 ), long style = 0 );

		~DIALOG_DISPLAY_HTML_TEXT_BASE();

};

