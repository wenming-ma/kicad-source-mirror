#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include "widgets/html_window.h"
#include "dialog_shim.h"

class DIALOG_DISPLAY_HTML_TEXT_BASE : public DIALOG_SHIM
{
	private:

	protected:
		HTML_WINDOW* m_htmlWindow;
		QHBoxLayout* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;

		// Virtual event handlers, override them in your derived class
		virtual void OnHTMLLinkClicked( const QUrl& url ) {}


	public:

		DIALOG_DISPLAY_HTML_TEXT_BASE( QWidget* parent = nullptr, const QString& title = QString(), Qt::WindowFlags flags = Qt::WindowFlags() );

		~DIALOG_DISPLAY_HTML_TEXT_BASE();

};

