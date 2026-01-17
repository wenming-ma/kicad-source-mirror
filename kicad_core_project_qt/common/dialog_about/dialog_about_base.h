#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QString>
#include <QtGui/QPixmap>

class DIALOG_ABOUT_BASE : public QDialog
{
	private:

	protected:
		QLabel* m_bitmapApp;
		QLabel* m_staticTextAppTitle;
		QLabel* m_staticTextBuildVersion;
		QLabel* m_staticTextLibVersion;
		QPushButton* m_btCopyVersionInfo;
		QPushButton* m_btReportBug;
		QPushButton* m_btDonate;
		QTabWidget* m_notebook;
		QPushButton* m_btOk;

		// Virtual event handlers, override them in your derived class
		virtual void onCopyVersionInfo() {}
		virtual void onReportBug() {}
		virtual void onDonateClick() {}
		virtual void OnNotebookPageChanged(int) {}


	public:

		DIALOG_ABOUT_BASE( QWidget* parent = nullptr, const QString& title = "About" );

		~DIALOG_ABOUT_BASE();

};

