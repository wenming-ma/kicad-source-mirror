
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
#pragma once

#include <widgets/unit_binder.h>
#include <string_utils.h>
#include <dialog_lib_new_symbol_base.h>
#include <QStringList>

class EDA_DRAW_FRAME;

class DIALOG_LIB_NEW_SYMBOL : public DIALOG_LIB_NEW_SYMBOL_BASE
{
public:
    DIALOG_LIB_NEW_SYMBOL( EDA_DRAW_FRAME* aParent, const QStringList& aSymbolNames,
                           const QString&                         aInheritFromSymbolName,
                           std::function<bool( QString newName )> aValidator );

    ~DIALOG_LIB_NEW_SYMBOL();

    void SetName( const QString& name )
    {
        m_textName->setText( UnescapeString( name ) );
    }

    QString GetName() const
    {
        QString name = EscapeString( m_textName->text(), CTX_LIBID );

        // Currently, symbol names cannot include a space, that breaks libraries:
        name.replace( " ", "_" );

        return name;
    }

    QString GetParentSymbolName() const
    {
        return EscapeString( m_comboInheritanceSelect->currentText(), CTX_LIBID );
    }

    void SetReference( const QString& reference ) { m_textReference->setText( reference ); }
    QString GetReference() { return m_textReference->text(); }

    void SetPartCount( int count ) { m_spinPartCount->setValue( count ); }
    int GetUnitCount() { return m_spinPartCount->value(); }

    void SetAlternateBodyStyle( bool enable ) { m_checkHasAlternateBodyStyle->setChecked( enable ); }
    bool GetAlternateBodyStyle()  { return m_checkHasAlternateBodyStyle->isChecked(); }

    void SetPowerSymbol( bool enable ) { m_checkIsPowerSymbol->setChecked( enable ); }
    bool GetPowerSymbol() { return m_checkIsPowerSymbol->isChecked(); }

    void SetUnitsInterchangeable( bool enable ) { m_checkUnitsInterchangeable->setChecked( enable ); }
    bool GetUnitsInterchangeable() { return m_checkUnitsInterchangeable->isChecked(); }

    void SetIncludeInBom( bool aInclude ) { m_excludeFromBomCheckBox->setChecked( !aInclude ); }
    bool GetIncludeInBom() const { return !m_excludeFromBomCheckBox->isChecked(); }

    void SetIncludeOnBoard( bool aInclude ) { m_excludeFromBoardCheckBox->setChecked( !aInclude ); }
    bool GetIncludeOnBoard() const { return !m_excludeFromBoardCheckBox->isChecked(); }

    void SetPinTextPosition( int position ) { m_pinTextPosition.SetValue( position ); }
    int GetPinTextPosition() { return m_pinTextPosition.GetIntValue(); }

    void SetShowPinNumber( bool show ) { m_checkShowPinNumber->setChecked( show ); }
    bool GetShowPinNumber() { return m_checkShowPinNumber->isChecked(); }

    void SetShowPinName( bool show ) { m_checkShowPinName->setChecked( show ); }
    bool GetShowPinName() { return m_checkShowPinName->isChecked(); }

    void SetPinNameInside( bool show ) { m_checkShowPinNameInside->setChecked( show ); }
    bool GetPinNameInside() { return m_checkShowPinNameInside->isChecked(); }

protected:
    bool TransferDataFromWindow() override;

    virtual void onPowerCheckBox() override;

private:
    void onParentSymbolSelect( QEvent& aEvent );

    void syncControls( bool aIsDerivedPart );

private:
    UNIT_BINDER                             m_pinTextPosition;
    std::function<bool( QString newName )> m_validator;
    bool                                    m_nameIsDefaulted;
};
