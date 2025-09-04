
#ifndef DIALOG_ABOUT_H
#define DIALOG_ABOUT_H

#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtCore/QString>

#include "aboutinfo.h"
#include "dialog_about_base.h"

// Used for the notebook image list
enum class IMAGES {
    INFORMATION,
    VERSION,
    DEVELOPERS,
    DOCWRITERS,
    LIBRARIANS,
    ARTISTS,
    TRANSLATORS,
    PACKAGERS,
    LICENSE
};

class DIALOG_ABOUT : public DIALOG_ABOUT_BASE
{
public:
    DIALOG_ABOUT( EDA_BASE_FRAME* aParent, ABOUT_APP_INFO& aAppInfo );
    ~DIALOG_ABOUT();

protected:
    void OnNotebookPageChanged( int aIndex ) override;

private:
    void onHtmlLinkClicked( const QString& url );

    void onCopyVersionInfo() override;

    void onReportBug() override;

    void onDonateClick() override;

    // Notebook pages
    void createNotebooks();
    void createNotebookPageByCategory( QTabWidget* aParent, const QString& aCaption,
                                       IMAGES aIconIndex, const CONTRIBUTORS& aContributors );
    void createNotebookHtmlPage( QTabWidget* aParent, const QString& aCaption,
                                 IMAGES aIconIndex, const QString& aHtmlMessage,
                                 bool aSelection = false );

    QLabel* createStaticTextRef( QScrollArea* aParent, const QString& aReference );
    QLabel*  createStaticBitmap( QScrollArea* aParent, const QPixmap& icon );

private:
    QVector<QPixmap> m_images;
    QString         m_titleName;
    QString         m_untranslatedTitleName;

    ABOUT_APP_INFO& m_info;
};

#endif // DIALOG_ABOUT_H
