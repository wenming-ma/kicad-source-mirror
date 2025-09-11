
#ifndef _REPORTER_H_
#define _REPORTER_H_

#include <memory>
#include <map>

#include <eda_units.h>
#include <widgets/report_severity.h>
#include <kicommon.h>


class QString;
class QStatusBar;
class QTextEdit;
class WX_HTML_REPORT_PANEL;
class WX_INFOBAR;



class KICOMMON_API REPORTER
{
public:
    REPORTER() :
            m_reportedSeverityMask( 0 )
    { }

    virtual ~REPORTER()
    { }

    enum LOCATION {
        LOC_HEAD = 0,
        LOC_BODY,
        LOC_TAIL
    };


    virtual REPORTER& Report( const QString& aText,
                              SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED )
    {
        m_reportedSeverityMask |= aSeverity;
        return *this;
    }

    virtual REPORTER& ReportTail( const QString& aText,
                                  SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED )
    {
        return Report( aText, aSeverity );
    }

    virtual REPORTER& ReportHead( const QString& aText,
                                  SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED )
    {
        return Report( aText, aSeverity );
    }

    REPORTER& Report( const char* aText, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED );

    REPORTER& operator <<( const QString& aText ) { return Report( aText ); }

    virtual bool HasMessage() const
    {
        return m_reportedSeverityMask != 0;
    }

    virtual bool HasMessageOfSeverity( int aSeverityMask ) const
    {
        return ( m_reportedSeverityMask & aSeverityMask ) != 0;
    }

    virtual EDA_UNITS GetUnits() const
    {
        return EDA_UNITS::MM;
    }

    virtual void Clear()
    {
        m_reportedSeverityMask = 0;
    }

private:
    int m_reportedSeverityMask;
};


class KICOMMON_API QT_TEXT_EDIT_REPORTER : public REPORTER
{
public:
    QT_TEXT_EDIT_REPORTER( QTextEdit* aTextEdit ) :
        REPORTER(),
        m_textEdit( aTextEdit )
    {
    }

    virtual ~QT_TEXT_EDIT_REPORTER()
    {
    }

    REPORTER& Report( const QString& aText,
                      SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;

private:
    QTextEdit* m_textEdit;
};


class KICOMMON_API QT_STRING_REPORTER : public REPORTER
{
public:
    QT_STRING_REPORTER() :
            REPORTER()
    { }

    virtual ~QT_STRING_REPORTER()
    { }

    REPORTER& Report( const QString& aText, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;

    const QString& GetMessages() const;
    void            Clear() override;

private:
    QString m_string;
};


class KICOMMON_API NULL_REPORTER : public REPORTER
{
public:
    NULL_REPORTER()
    { }

    virtual ~NULL_REPORTER()
    { }

    static REPORTER& GetInstance();

    REPORTER& Report( const QString& aText,
                      SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;
};


class KICOMMON_API CLI_REPORTER : public REPORTER
{
public:
    CLI_REPORTER()
    { }

    virtual ~CLI_REPORTER()
    { }

    static REPORTER& GetInstance();

    REPORTER& Report( const QString& aMsg, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;
};


class KICOMMON_API STDOUT_REPORTER : public REPORTER
{
public:
    STDOUT_REPORTER()
    { }

    virtual ~STDOUT_REPORTER()
    { }

    static REPORTER& GetInstance();

    REPORTER& Report( const QString& aMsg, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;
};


class KICOMMON_API QTLOG_REPORTER : public REPORTER
{
public:
    QTLOG_REPORTER()
    { }

    virtual ~QTLOG_REPORTER()
    { }

    static REPORTER& GetInstance();

    REPORTER& Report( const QString& aMsg, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;
};


class KICOMMON_API REDIRECT_REPORTER : public REPORTER
{
public:
    REDIRECT_REPORTER( REPORTER* aRedirectTarget ) : m_redirectTarget( aRedirectTarget ) {}

    REPORTER& Report( const QString& aMsg, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;

    REPORTER* m_redirectTarget;
};


class KICOMMON_API STATUSBAR_REPORTER : public REPORTER
{
public:
    STATUSBAR_REPORTER( QStatusBar* aStatusBar, int aPosition = 0 )
            : REPORTER(),
              m_statusBar( aStatusBar ),
              m_position( aPosition )
    { }

    REPORTER& Report( const QString& aText, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;

private:
    QStatusBar* m_statusBar;
    int          m_position;
};

#endif     // _REPORTER_H_
