// QT_TRANSFORMATION_COMPLETED

#ifndef SCINTILLA_TRICKS_H
#define SCINTILLA_TRICKS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <Qsci/qsciscintilla.h>
#include <QKeyEvent>
#include <functional>

class SCINTILLA_TRICKS : public QObject
{
    Q_OBJECT

public:
    SCINTILLA_TRICKS( QsciScintilla* aScintilla, const QString& aBraces, bool aSingleLine,
                      std::function<void( QKeyEvent& )> onAcceptHandler =
                            []( QKeyEvent& aEvent )
                            { },
                      std::function<void( QKeyEvent& )> onCharAddedHandler =
                            []( QKeyEvent& )
                            { } );

    QsciScintilla* Scintilla() const { return m_te; }

    void DoTextVarAutocomplete(
            const std::function<void( const QString& xRef, QStringList* tokens )>& getTokensFn );

    void DoAutocomplete( const QString& aPartial, const QStringList& aTokens );

    void CancelAutocomplete();

protected:
    void setupStyles();

    int firstNonWhitespace( int aLine, int* aWhitespaceCount = nullptr );

protected slots:
    virtual void onCharHook( QKeyEvent& aEvent );
    void onChar( QKeyEvent& aEvent );
    void onModified();
    void onScintillaUpdateUI();
    void onThemeChanged();

protected:
    QsciScintilla*        m_te;
    QString               m_braces;
    int                   m_lastCaretPos;
    int                   m_lastSelStart;
    int                   m_lastSelEnd;
    bool                  m_suppressAutocomplete;
    bool                  m_singleLine;            // Treat <return> as OK, and skip special tab
                                                   //  stop handling (including monospaced font).

    // Process <return> in singleLine, and <shift> + <return> irrespective.
    std::function<void( QKeyEvent& aEvent )>         m_onAcceptFn;
    std::function<void( QKeyEvent& aEvent )>         m_onCharAddedFn;
};

#endif  // SCINTILLA_TRICKS_H
