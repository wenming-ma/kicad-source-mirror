#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class WX_UNIT_ENTRY_DIALOG_BASE
///////////////////////////////////////////////////////////////////////////////
class WX_UNIT_ENTRY_DIALOG_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QVBoxLayout* bSizerMain;
		QLabel* m_label;
		QLineEdit* m_textCtrl;
		QLabel* m_unit_label;
		QDialogButtonBox* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;
		QPushButton* m_sdbSizer1Cancel;

	public:

		WX_UNIT_ENTRY_DIALOG_BASE( QWidget* parent, int id = -1, const QString& title = "Title", const QPoint& pos = QPoint(), const QSize& size = QSize( -1,-1 ), long style = 0 );

		~WX_UNIT_ENTRY_DIALOG_BASE();

};

///////////////////////////////////////////////////////////////////////////////
/// Class WX_PT_ENTRY_DIALOG_BASE
///////////////////////////////////////////////////////////////////////////////
class WX_PT_ENTRY_DIALOG_BASE : public DIALOG_SHIM
{
	Q_OBJECT

	private:

	protected:
		QVBoxLayout* bSizerMain;
		QLabel* m_labelX;
		QLineEdit* m_textCtrlX;
		QLabel* m_unitsX;
		QLabel* m_labelY;
		QLineEdit* m_textCtrlY;
		QLabel* m_unitsY;
		QPushButton* m_ButtonReset;
		QDialogButtonBox* m_sdbSizer1;
		QPushButton* m_sdbSizer1OK;
		QPushButton* m_sdbSizer1Cancel;

	public slots:
		virtual void ResetValues();


	public:

		WX_PT_ENTRY_DIALOG_BASE( QWidget* parent, int id = -1, const QString& title = "Move Point to Location", const QPoint& pos = QPoint(), const QSize& size = QSize( -1,-1 ), long style = 0 );

		~WX_PT_ENTRY_DIALOG_BASE();

};
