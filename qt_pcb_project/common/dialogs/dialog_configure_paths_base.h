
#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include "dialog_shim.h"

class STD_BITMAP_BUTTON;
class WX_GRID;

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

		virtual void OnUpdateUI();
		virtual void OnAddEnvVar();
		virtual void OnRemoveEnvVar();
		virtual void OnHelp();


	public:

		DIALOG_CONFIGURE_PATHS_BASE( QWidget* parent = nullptr, const QString& title = "Configure Paths" );

		~DIALOG_CONFIGURE_PATHS_BASE();

};

