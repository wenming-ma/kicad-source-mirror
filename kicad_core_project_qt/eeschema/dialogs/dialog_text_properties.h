
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef DIALOG_TEXT_PROPERTIES_H
#define DIALOG_TEXT_PROPERTIES_H

#include <widgets/unit_binder.h>
#include <sch_text.h>
#include <dialog_text_properties_base.h>


class SCH_EDIT_FRAME;
class SCH_TEXT;
#ifdef HAVE_QSCINTILLA
class SCINTILLA_TRICKS;
#endif
class HTML_MESSAGE_BOX;


class DIALOG_TEXT_PROPERTIES : public DIALOG_TEXT_PROPERTIES_BASE
{
public:
    DIALOG_TEXT_PROPERTIES( SCH_BASE_FRAME* parent, SCH_ITEM* aTextItem );
    ~DIALOG_TEXT_PROPERTIES() override;

private:
    void getContextualTextVars( const QString& aCrossRef, QStringList* aTokens );

    void onHAlignButton( QEvent &aEvent );
    void onVAlignButton( QEvent &aEvent );
    void onTextAngleButton( QEvent &aEvent );
    void onBorderChecked() override;
    void onFillChecked() override;
    void onHyperlinkChecked() override;
    void onHyperlinkText() override;
    void onHyperlinkCombo() override;

    void OnFormattingHelp() override;
    void onMultiLineTCLostFocus( QFocusEvent* event ) override;

    bool TransferDataToWindow();
    bool TransferDataFromWindow() override;

    SCH_BASE_FRAME*       m_frame;
    bool                  m_isSymbolEditor;
    SCH_ITEM*             m_currentItem;
    EDA_TEXT*             m_currentText;
    UNIT_BINDER           m_textSize;
    UNIT_BINDER           m_borderWidth;
#ifdef HAVE_QSCINTILLA
    SCINTILLA_TRICKS*     m_scintillaTricks;
#endif
    std::vector<QString> m_pageNumbers;

    HTML_MESSAGE_BOX*     m_helpWindow;

    QString              m_lastLink;
};



#endif // DIALOG_TEXT_PROPERTIES_H
