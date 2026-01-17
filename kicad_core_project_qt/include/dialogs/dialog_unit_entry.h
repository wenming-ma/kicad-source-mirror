
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#pragma once

#include <dialogs/dialog_unit_entry_base.h>
#include <widgets/unit_binder.h>


/**
 * An extension of WX_TEXT_ENTRY_DIALOG that uses UNIT_BINDER to request a dimension
 * (e.g. mm, inches, etc) from the user according to the selected units
 */
class WX_UNIT_ENTRY_DIALOG : public WX_UNIT_ENTRY_DIALOG_BASE
{
public:
    WX_UNIT_ENTRY_DIALOG( EDA_DRAW_FRAME* aParent, const QString& aCaption, const QString& aLabel,
                          long long int aDefaultValue );

    /**
     * Return the value in internal units.
     */
    int GetValue();

private:
    UNIT_BINDER m_unit_binder;
};


class WX_PT_ENTRY_DIALOG : public WX_PT_ENTRY_DIALOG_BASE
{
public:
    WX_PT_ENTRY_DIALOG( EDA_DRAW_FRAME* aParent, const QString& aCaption, const QString& aLabelX,
                        const QString& aLabelY, const VECTOR2I& aDefaultValue, bool aShowResetButt );

    /**
     * Return the value in internal units.
     */
    VECTOR2I GetValue();

	void ResetValues() override;

private:
    UNIT_BINDER m_unit_binder_x;
    UNIT_BINDER m_unit_binder_y;
};
