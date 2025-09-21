#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialog>
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class EDA_VIEW_SWITCHER_BASE
///////////////////////////////////////////////////////////////////////////////
class EDA_VIEW_SWITCHER_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QLabel* m_stTitle;
		QListWidget* m_listBox;

	public:

		EDA_VIEW_SWITCHER_BASE( QWidget* parent, int id = -1, const QString& title = "View Preset Switcher", const QPoint& pos = QPoint(), const QSize& size = QSize(), Qt::WindowFlags style = Qt::WindowStaysOnTopHint );

		~EDA_VIEW_SWITCHER_BASE();

};
