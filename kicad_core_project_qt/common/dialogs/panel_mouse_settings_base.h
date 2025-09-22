#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QPushButton>
#include <QtCore/QString>
#include "widgets/resettable_panel.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PANEL_MOUSE_SETTINGS_BASE
///////////////////////////////////////////////////////////////////////////////
class PANEL_MOUSE_SETTINGS_BASE : public RESETTABLE_PANEL
{
	Q_OBJECT

private:

protected:
		QLabel* m_panZoomLabel;
		QFrame* m_staticline1;
		QCheckBox* m_checkZoomCenter;
		QCheckBox* m_checkAutoPan;
		QCheckBox* m_checkZoomAcceleration;
		QVBoxLayout* m_zoomSizer;
		QLabel* m_staticText1;
		QSlider* m_zoomSpeed;
		QCheckBox* m_checkAutoZoomSpeed;
		QVBoxLayout* m_panSizer;
		QLabel* m_staticText22;
		QSlider* m_autoPanSpeed;
		QLabel* m_dragLabel;
		QFrame* m_staticline3;
		QLabel* m_leftButtonDragLabel;
		QComboBox* m_choiceLeftButtonDrag;
		QLabel* m_staticText3;
		QComboBox* m_choiceMiddleButtonDrag;
		QLabel* m_staticText31;
		QComboBox* m_choiceRightButtonDrag;
		QLabel* m_scrollLabel;
		QFrame* m_staticline2;
		QLabel* m_staticText21;
		QLabel* m_scrollWarning;
		QLabel* m_staticText19;
		QLabel* m_staticText17;
		QLabel* m_lblCtrl;
		QLabel* m_staticText8;
		QLabel* m_lblAlt;
		QLabel* m_staticText18;
		QLabel* m_staticText10;
		QRadioButton* m_rbZoomNone;
		QRadioButton* m_rbZoomCtrl;
		QRadioButton* m_rbZoomShift;
		QRadioButton* m_rbZoomAlt;
		QCheckBox* m_checkZoomReverse;
		QLabel* m_staticText11;
		QRadioButton* m_rbPanVNone;
		QRadioButton* m_rbPanVCtrl;
		QRadioButton* m_rbPanVShift;
		QRadioButton* m_rbPanVAlt;
		QLabel* m_staticText211;
		QLabel* m_staticText20;
		QRadioButton* m_rbPanHNone;
		QRadioButton* m_rbPanHCtrl;
		QRadioButton* m_rbPanHShift;
		QRadioButton* m_rbPanHAlt;
		QCheckBox* m_checkPanHReverse;
		QCheckBox* m_checkEnablePanH;
		QPushButton* m_mouseDefaults;
		QPushButton* m_trackpadDefaults;

		// Virtual event handlers, override them in your derived class
		virtual void OnScrollRadioButton() {}
		virtual void onMouseDefaults() {}
		virtual void onTrackpadDefaults() {}


public:

		PANEL_MOUSE_SETTINGS_BASE( QWidget* parent = nullptr );

		~PANEL_MOUSE_SETTINGS_BASE();

};

