// Qt dialog base class for reorderable list functionality

#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QString>
class STD_BITMAP_BUTTON;

#include "dialog_shim.h"

// Base class for reorderable list dialog
class EDA_REORDERABLE_LIST_DIALOG_BASE : public DIALOG_SHIM
{
	private:

	protected:
		QLabel* m_availableListLabel;
		QLabel* m_enabledListLabel;
		QListWidget* m_availableListBox;
		QPushButton* m_btnAdd;
		QPushButton* m_btnRemove;
		QListWidget* m_enabledListBox;
		STD_BITMAP_BUTTON* m_btnUp;
		STD_BITMAP_BUTTON* m_btnDown;
		QDialogButtonBox* m_sdbSizer;
		QPushButton* m_sdbSizerOK;
		QPushButton* m_sdbSizerCancel;

		// Virtual event handlers, override them in your derived class
		virtual void onAvailableListItemSelected() = 0;
		virtual void onAddItem() = 0;
		virtual void onRemoveItem() = 0;
		virtual void onEnabledListItemSelected() = 0;
		virtual void onMoveUp() = 0;
		virtual void onMoveDown() = 0;


	public:
		QHBoxLayout* m_ButtonsSizer;

		EDA_REORDERABLE_LIST_DIALOG_BASE( QWidget* parent = nullptr, const QString& title = QString() );

		~EDA_REORDERABLE_LIST_DIALOG_BASE();

};
