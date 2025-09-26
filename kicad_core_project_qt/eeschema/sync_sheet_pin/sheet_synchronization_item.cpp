
#include "sheet_synchronization_item.h"
#include "bitmaps/bitmap_types.h"
#include "sync_sheet_pin/sheet_synchronization_item.h"
#include "sync_sheet_pin/sync_sheet_pin_preference.h"
#include <sch_label.h>
#include <sch_sheet_pin.h>
#include <bitmaps/bitmaps_list.h>
#include <sch_sheet.h>
#include <sch_screen.h>
#include <QPixmap>
#include <QImage>
#include <QSize>
#include <QPainter>


SCH_HIERLABEL_SYNCHRONIZATION_ITEM::SCH_HIERLABEL_SYNCHRONIZATION_ITEM( SCH_HIERLABEL* aLabel,
                                                                        SCH_SHEET*     aSheet ) :
        m_label( aLabel ),
        m_sheet( aSheet )
{
}


QString SCH_HIERLABEL_SYNCHRONIZATION_ITEM::GetName() const
{
    return m_label->GetShownText( true );
}


int SCH_HIERLABEL_SYNCHRONIZATION_ITEM::GetShape() const
{
    return m_label->GetShape();
}


QPixmap& SCH_HIERLABEL_SYNCHRONIZATION_ITEM::GetBitmap() const
{
    static QPixmap bitMap =
            KiBitmap( BITMAPS::add_hierarchical_label, SYNC_SHEET_PIN_PREFERENCE::NORMAL_HEIGHT );
    return bitMap;
}


SCH_ITEM* SCH_HIERLABEL_SYNCHRONIZATION_ITEM::GetItem() const
{
    return m_label;
}


SHEET_SYNCHRONIZATION_ITEM_KIND SCH_HIERLABEL_SYNCHRONIZATION_ITEM::GetKind() const
{
    return SHEET_SYNCHRONIZATION_ITEM_KIND::HIERLABEL;
}


SCH_SHEET_PIN_SYNCHRONIZATION_ITEM::SCH_SHEET_PIN_SYNCHRONIZATION_ITEM( SCH_SHEET_PIN* aPin,
                                                                        SCH_SHEET*     aSheet ) :
        m_pin( aPin ),
        m_sheet( aSheet )
{
}


QString SCH_SHEET_PIN_SYNCHRONIZATION_ITEM::GetName() const
{
    return m_pin->GetShownText( true );
}


int SCH_SHEET_PIN_SYNCHRONIZATION_ITEM::GetShape() const
{
    return m_pin->GetShape();
}


QPixmap& SCH_SHEET_PIN_SYNCHRONIZATION_ITEM::GetBitmap() const
{
    static QPixmap bitMap =
            KiBitmap( BITMAPS::add_hierar_pin, SYNC_SHEET_PIN_PREFERENCE::NORMAL_HEIGHT );
    return bitMap;
}


SCH_ITEM* SCH_SHEET_PIN_SYNCHRONIZATION_ITEM::GetItem() const
{
    return m_pin;
}


SHEET_SYNCHRONIZATION_ITEM_KIND SCH_SHEET_PIN_SYNCHRONIZATION_ITEM::GetKind() const
{
    return SHEET_SYNCHRONIZATION_ITEM_KIND::SHEET_PIN;
}


ASSOCIATED_SCH_LABEL_PIN::ASSOCIATED_SCH_LABEL_PIN( SCH_HIERLABEL* aLabel, SCH_SHEET_PIN* aPin ) :
        m_label( aLabel ), m_pin( aPin )
{
}


ASSOCIATED_SCH_LABEL_PIN::ASSOCIATED_SCH_LABEL_PIN( SCH_HIERLABEL_SYNCHRONIZATION_ITEM* aLabel,
                                                    SCH_SHEET_PIN_SYNCHRONIZATION_ITEM* aPin ) :
        ASSOCIATED_SCH_LABEL_PIN( aLabel->GetLabel(), aPin->GetPin() )
{
}


QString ASSOCIATED_SCH_LABEL_PIN::GetName() const
{
    return m_label->GetShownText( true );
}


int ASSOCIATED_SCH_LABEL_PIN::GetShape() const
{
    return m_label->GetShape();
}


QPixmap& ASSOCIATED_SCH_LABEL_PIN::GetBitmap() const
{
    static auto label_and_pin_icon = ( []{
        QPixmap left =  KiBitmap( BITMAPS::add_hierar_pin,
                                   SYNC_SHEET_PIN_PREFERENCE::NORMAL_HEIGHT );
        QPixmap right =
                KiBitmap( BITMAPS::add_hierarchical_label,
                          SYNC_SHEET_PIN_PREFERENCE::NORMAL_HEIGHT );
        QImage img( SYNC_SHEET_PIN_PREFERENCE::NORMAL_WIDTH * 2,
                    SYNC_SHEET_PIN_PREFERENCE::NORMAL_HEIGHT,
                    QImage::Format_ARGB32 );
        QPainter painter( &img );
        painter.drawImage( 0, 0, left.toImage() );
        painter.drawImage( SYNC_SHEET_PIN_PREFERENCE::NORMAL_WIDTH, 0, right.toImage() );
        return QPixmap::fromImage( img );
    } )();

    return label_and_pin_icon;
}


SCH_ITEM* ASSOCIATED_SCH_LABEL_PIN::GetItem() const
{
    return nullptr;
}


SHEET_SYNCHRONIZATION_ITEM_KIND ASSOCIATED_SCH_LABEL_PIN::GetKind() const
{
    return SHEET_SYNCHRONIZATION_ITEM_KIND::HIERLABEL_AND_SHEET_PIN;
}
