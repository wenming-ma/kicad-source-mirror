#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtCore/QSize>
#include <QtCore/QPoint>

class STD_BITMAP_BUTTON;
class WX_GRID;

#include "dialog_shim.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_CONFIGURE_PATHS_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_CONFIGURE_PATHS_BASE : public DIALOG_SHIM
{
	Q_OBJECT

	private:

	protected:
		WX_GRID* m_EnvVars;
		STD_BITMAP_BUTTON* m_btnAddEnvVar;
		STD_BITMAP_BUTTON* m_btnDeleteEnvVar;
		QDialogButtonBox* m_sdbSizer;
		QPushButton* m_sdbSizerOK;
		QPushButton* m_sdbSizerCancel;
		QPushButton* m_sdbSizerHelp;

		// Virtual event handlers, override them in your derived class
		virtual void OnUpdateUI() {}
		virtual void OnAddEnvVar() {}
		virtual void OnRemoveEnvVar() {}
		virtual void OnHelp() {}

	protected slots:
		virtual void onUpdateUI();
		virtual void onAddEnvVar();
		virtual void onRemoveEnvVar();
		virtual void onHelp();

	public:

		DIALOG_CONFIGURE_PATHS_BASE( QWidget* parent, int id = -1, const QString& title = "Configure Paths", const QPoint& pos = QPoint(), const QSize& size = QSize( -1,-1 ), long style = 0 );

		~DIALOG_CONFIGURE_PATHS_BASE();

};

