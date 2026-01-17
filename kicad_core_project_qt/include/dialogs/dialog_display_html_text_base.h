// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
// Dialog base class for displaying HTML text content
// Transformed from wxWidgets to Qt framework

#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include "widgets/html_window.h"
#include "dialog_shim.h"

// Base class for HTML text display dialog
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

		DIALOG_DISPLAY_HTML_TEXT_BASE( QWidget* parent, int id = -1, const QString& title = QString(), const QPoint& pos = QPoint(), const QSize& size = QSize( -1,-1 ), Qt::WindowFlags style = Qt::Dialog );

		~DIALOG_DISPLAY_HTML_TEXT_BASE();

};

