
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef DIALOG_TEXT_PROPERTIES_H
#define DIALOG_TEXT_PROPERTIES_H

#include <widgets/unit_binder.h>
#include <sch_text.h>
#include <dialog_text_properties_base.h>


class SCH_EDIT_FRAME;
class SCH_TEXT;
class SCINTILLA_TRICKS;
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
    void onBorderChecked( QEvent& aEvent ) override;
    void onFillChecked( QEvent& aEvent ) override;
    void onHyperlinkChecked( QEvent& aEvent ) override;
    void onHyperlinkText( QEvent& aEvent ) override;
    void onHyperlinkCombo( QEvent& aEvent ) override;

    void OnFormattingHelp( QEvent& aEvent ) override;
    void onMultiLineTCLostFocus( QFocusEvent& event ) override;

    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

    SCH_BASE_FRAME*       m_frame;
    bool                  m_isSymbolEditor;
    SCH_ITEM*             m_currentItem;
    EDA_TEXT*             m_currentText;
    UNIT_BINDER           m_textSize;
    UNIT_BINDER           m_borderWidth;
    SCINTILLA_TRICKS*     m_scintillaTricks;
    std::vector<QString> m_pageNumbers;

    HTML_MESSAGE_BOX*     m_helpWindow;

    QString              m_lastLink;
};



#endif // DIALOG_TEXT_PROPERTIES_H
