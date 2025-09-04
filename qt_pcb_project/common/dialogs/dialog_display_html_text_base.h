
#pragma once

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include "widgets/html_window.h"
#include "dialog_shim.h"

class DIALOG_DISPLAY_HTML_TEXT_BASE : public DIALOG_SHIM
{
	Q_OBJECT

	private:

	protected:
		HTML_WINDOW* m_htmlWindow;
		QDialogButtonBox* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;

		virtual void OnHTMLLinkClicked();


	public:

		DIALOG_DISPLAY_HTML_TEXT_BASE( QWidget* parent = nullptr, const QString& title = QString() );

		~DIALOG_DISPLAY_HTML_TEXT_BASE();

};

