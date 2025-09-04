
#pragma once

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "widgets/resettable_panel.h"

class PANEL_DATA_COLLECTION_BASE : public RESETTABLE_PANEL
{
	Q_OBJECT

	private:

	protected:
		QLabel* m_stExplanation;
		QCheckBox* m_cbOptIn;
		QLineEdit* m_sentryUid;
		QPushButton* m_buttonResetId;

		virtual void OnResetIdClick();


	public:

		PANEL_DATA_COLLECTION_BASE( QWidget* parent = nullptr );

		~PANEL_DATA_COLLECTION_BASE();

};

