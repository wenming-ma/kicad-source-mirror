// QT_TRANSFORMATION_COMPLETED

#pragma once

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QEvent>
#include "widgets/resettable_panel.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PANEL_DATA_COLLECTION_BASE
///////////////////////////////////////////////////////////////////////////////
class PANEL_DATA_COLLECTION_BASE : public RESETTABLE_PANEL
{
    Q_OBJECT

private:

protected:
    QLabel* m_stExplanation;
    QCheckBox* m_cbOptIn;
    QLineEdit* m_sentryUid;
    QPushButton* m_buttonResetId;

    // Virtual event handlers, override them in your derived class
    virtual void OnResetIdClick();

public:

    PANEL_DATA_COLLECTION_BASE( QWidget* parent = nullptr );

    ~PANEL_DATA_COLLECTION_BASE();

public slots:
    void onResetIdClicked();
};

