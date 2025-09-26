// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
// Qt-based dialog header for schematic find/replace functionality
// Transformed from wxWidgets to Qt framework

#pragma once

#include "dialog_shim.h"
#include <QString>
#include <QLabel>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QCloseEvent>
#include <QEvent>
#include <QWidget>
#include <QDialog>
#include <QPoint>
#include <QSize>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DIALOG_SCH_FIND_BASE
///////////////////////////////////////////////////////////////////////////////
class DIALOG_SCH_FIND_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QLabel* m_staticText1;
		QComboBox* m_comboFind;
		QLabel* m_staticReplace;
		QComboBox* m_comboReplace;
		QLabel* m_staticDirection;
		QRadioButton* m_radioForward;
		QRadioButton* m_radioBackward;
		QCheckBox* m_checkMatchCase;
		QCheckBox* m_checkWholeWord;
		QCheckBox* m_checkRegexMatch;
		QCheckBox* m_checkAllPins;
		QCheckBox* m_checkAllFields;
		QCheckBox* m_checkCurrentSheetOnly;
		QCheckBox* m_checkSelectedOnly;
		QCheckBox* m_checkReplaceReferences;
		QCheckBox* m_checkConnections;
		QPushButton* m_buttonFind;
		QPushButton* m_buttonReplace;
		QPushButton* m_buttonReplaceAll;
		QPushButton* m_buttonCancel;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( QCloseEvent* event ) { event->ignore(); }
		virtual void OnIdle( QEvent* event ) { event->ignore(); }
		virtual void OnSearchForSelect() {}
		virtual void OnSearchForText() {}
		virtual void OnSearchForEnter() {}
		virtual void OnUpdateDrcUI() {}
		virtual void OnReplaceWithSelect() {}
		virtual void OnReplaceWithText() {}
		virtual void OnReplaceWithEnter() {}
		virtual void OnOptions() {}
		virtual void OnFind() {}
		virtual void OnReplace() {}
		virtual void OnUpdateReplaceUI() {}
		virtual void OnUpdateReplaceAllUI() {}
		virtual void OnCancel() {}


	public:

		DIALOG_SCH_FIND_BASE( QWidget* parent = nullptr, const QString& title = "Find", const QPoint& pos = QPoint(), const QSize& size = QSize( -1,-1 ) );

		~DIALOG_SCH_FIND_BASE();

};

