
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

// file class_DCodeSelectionbox.h

#ifndef DCODESELECTIONBOX_H
#define DCODESELECTIONBOX_H

#include <QComboBox>
#include <QToolBar>
#include <QPoint>
#include <QSize>
#include <QStringList>

// Define event type for DCODE_SELECTION_BOX
// Qt uses signals/slots - currentIndexChanged signal replaces EVT_COMBOBOX


/**
 * Helper to display a DCode list and select a DCode id.
 */
class DCODE_SELECTION_BOX : public QComboBox
{
public:
    DCODE_SELECTION_BOX( QToolBar* aParent, int aId,
                         const QPoint& aLocation, const QSize& aSize,
                         const QStringList* aChoices = nullptr );
    ~DCODE_SELECTION_BOX();

    /**
     * @return the current selected DCode Id or 0 if no dcode
     */
    int GetSelectedDCodeId();

    /**
     * @param aDCodeId is the DCode Id to select or <= 0 to select "no dcode".
     */
    void SetDCodeSelection( int aDCodeId );

    /**
     * @param aChoices is the DCode Id list to add to the combo box.
     */
    void AppendDCodeList( const QStringList& aChoices );
};

#endif //DCODESELECTIONBOX_H
