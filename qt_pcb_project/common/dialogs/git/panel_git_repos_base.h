#pragma once

#include "widgets/resettable_panel.h"
#include <QtWidgets/QWidget>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFrame>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtCore/QString>

class PANEL_GIT_REPOS_BASE : public RESETTABLE_PANEL
{
private:

protected:
    QCheckBox* m_enableGit;
    QVBoxLayout* m_gitSizer;
    QLabel* m_staticText6;
    QFrame* m_staticline2;
    QLabel* m_updateLabel;
    QSpinBox* m_updateInterval;
    QLabel* m_staticText7;
    QLabel* m_staticText12;
    QFrame* m_staticline31;
    QCheckBox* m_cbDefault;
    QLabel* m_authorLabel;
    QLineEdit* m_author;
    QLabel* m_authorEmailLabel;
    QLineEdit* m_authorEmail;

    // Virtual event handlers, override them in your derived class
    virtual void onEnableGitClick() {}
    virtual void onDefaultClick() {}

public:
    PANEL_GIT_REPOS_BASE(QWidget* parent = nullptr);
    ~PANEL_GIT_REPOS_BASE();
};