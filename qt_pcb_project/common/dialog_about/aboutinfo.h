
#ifndef ABOUTAPPINFO_H
#define ABOUTAPPINFO_H

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

#include <bitmaps/bitmap_types.h>

class CONTRIBUTOR;

using CONTRIBUTORS = QVector<CONTRIBUTOR*>;


class ABOUT_APP_INFO
{
public:
    ABOUT_APP_INFO() {};
    virtual ~ABOUT_APP_INFO() {};

    void AddDeveloper( const CONTRIBUTOR* developer )
    {
        if( developer != nullptr )
            mDevelopers.push_back( developer );
    }

    void AddDocWriter( const CONTRIBUTOR* docwriter )
    {
        if( docwriter != nullptr )
            mDocWriters.push_back( docwriter );
    }

    void AddLibrarian( const CONTRIBUTOR* aLibrarian )
    {
        if( aLibrarian )
            mLibrarians.push_back( aLibrarian );
    }

    void AddArtist( const CONTRIBUTOR* artist )
    {
        if( artist != nullptr )
            mArtists.push_back( artist );
    }

    void AddTranslator( const CONTRIBUTOR* translator )
    {
        if( translator != nullptr )
            mTranslators.push_back( translator );
    }

    void AddPackager( const CONTRIBUTOR* packager )
    {
        if( packager != nullptr )
            mPackagers.push_back( packager );
    }

    CONTRIBUTORS GetDevelopers()  { return mDevelopers; }
    CONTRIBUTORS GetDocWriters()  { return mDocWriters; }
    CONTRIBUTORS GetLibrarians()  { return mLibrarians; }
    CONTRIBUTORS GetArtists()     { return mArtists; }
    CONTRIBUTORS GetTranslators() { return mTranslators; }
    CONTRIBUTORS GetPackagers()   { return mPackagers; }

    void SetDescription( const QString& text ) { description = text; }
    QString& GetDescription() { return description; }

    void SetLicense( const QString& text ) { license = text; }
    QString& GetLicense() { return license; }

    void SetAppName( const QString& name ) { appName = name; }
    QString& GetAppName() { return appName; }

    void SetBuildVersion( const QString& version ) { buildVersion = version; }
    QString& GetBuildVersion() { return buildVersion; }

    void SetBuildDate( const QString& date ) { buildDate = date; }
    QString& GetBuildDate() { return buildDate; }

    void SetLibVersion( const QString& version ) { libVersion = version; }
    QString& GetLibVersion() { return libVersion; }

    void SetAppIcon( const QIcon& aIcon ) { m_appIcon = aIcon; }
    QIcon& GetAppIcon() { return m_appIcon; }

    QPixmap* CreateKiBitmap( BITMAPS aBitmap )
    {
        m_bitmaps.emplace_back( std::make_unique<QPixmap>( KiBitmapNew( aBitmap ) ) );
        return m_bitmaps.back().get();
    }

private:
    CONTRIBUTORS mDevelopers;
    CONTRIBUTORS mDocWriters;
    CONTRIBUTORS mLibrarians;
    CONTRIBUTORS mArtists;
    CONTRIBUTORS mTranslators;
    CONTRIBUTORS mPackagers;

    QString     description;
    QString     license;

    QString     appName;
    QString     buildVersion;
    QString     buildDate;
    QString     libVersion;

    QIcon       m_appIcon;

    std::vector<std::unique_ptr<QPixmap>> m_bitmaps;
};


class CONTRIBUTOR
{
public:
    CONTRIBUTOR( const QString& aName, const QString& aCategory,
                 const QString& aUrl = QString() )
    {
        m_checked = false;
        m_name = aName;
        m_url = aUrl,
        m_category = aCategory;
    }

    virtual ~CONTRIBUTOR() {}

    QString& GetName()     { return m_name; }
    QString& GetUrl()      { return m_url; }
    QString& GetCategory() { return m_category; }
    void SetChecked( bool status ) { m_checked = status; }
    bool IsChecked() { return m_checked; }

private:
    QString  m_name;
    QString  m_url;
    QString  m_category;
    bool      m_checked;
};

#endif // ABOUTAPPINFO_H
