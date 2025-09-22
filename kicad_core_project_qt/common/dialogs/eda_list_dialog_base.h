#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtGui/QResizeEvent>
#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EDA_LIST_DIALOG_BASE
///////////////////////////////////////////////////////////////////////////////
class EDA_LIST_DIALOG_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QLabel* m_listLabel;
		QListWidget* m_listBox;
		QLineEdit* m_filterBox;
		QHBoxLayout* m_sdbSizer;
		QPushButton* m_sdbSizerOK;
		QPushButton* m_sdbSizerCancel;

		// Virtual event handlers, override them in your derived class
		virtual void onSize( QResizeEvent* event ) = 0;
		virtual void onListItemActivated( ) = 0;
		virtual void textChangeInFilterBox( ) = 0;


	public:
		QVBoxLayout* m_ButtonsSizer;

		EDA_LIST_DIALOG_BASE( QWidget* parent, int id = -1, const QString& title = QString(), const QPoint& pos = QPoint(), const QSize& size = QSize(), Qt::WindowFlags style = Qt::WindowFlags() );

		~EDA_LIST_DIALOG_BASE();

};
