
#pragma once

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QCheckBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

class PANEL_PACKAGES_AND_UPDATES_BASE : public QWidget
{
	Q_OBJECT

	private:

	protected:
		QLabel* m_generalLabel;
		QFrame* m_staticline3;
		QCheckBox* m_cbKicadUpdate;
		QLabel* m_pcmLabel;
		QFrame* m_staticline1;
		QCheckBox* m_cbPcmUpdate;
		QLabel* m_staticText4;
		QFrame* m_staticline2;
		QCheckBox* m_libAutoAdd;
		QCheckBox* m_libAutoRemove;
		QLabel* m_staticText1;
		QLineEdit* m_libPrefix;

	public:

		PANEL_PACKAGES_AND_UPDATES_BASE( QWidget* parent = nullptr );

		~PANEL_PACKAGES_AND_UPDATES_BASE();

};

