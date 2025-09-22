#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLineEdit>
#include <QtCore/QString>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PANEL_PACKAGES_AND_UPDATES_BASE
///////////////////////////////////////////////////////////////////////////////
class PANEL_PACKAGES_AND_UPDATES_BASE : public QWidget
{
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

