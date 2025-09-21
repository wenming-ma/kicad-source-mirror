
#ifndef SHEET_SYNCHRONIZATION_ITEM_H
#define SHEET_SYNCHRONIZATION_ITEM_H

#include <QPixmap>
#include <QString>
#include <functional>

class EDA_ITEM;
class SCH_HIERLABEL;
class SCH_SHEET_PIN;
class SCH_SHEET;
class SCH_COMMIT;
class SCH_ITEM;

enum class SHEET_SYNCHRONIZATION_ITEM_KIND
{
    HIERLABEL,
    SHEET_PIN,
    HIERLABEL_AND_SHEET_PIN
};

using SCREEN_UPDATER = std::function<void( EDA_ITEM* )>;

class SHEET_SYNCHRONIZATION_ITEM
{
public:
    virtual ~SHEET_SYNCHRONIZATION_ITEM() = default;

    virtual QString GetName() const = 0;

    virtual int GetShape() const = 0;

    virtual QPixmap& GetBitmap() const = 0;

    virtual SCH_ITEM* GetItem() const = 0;

    virtual SHEET_SYNCHRONIZATION_ITEM_KIND GetKind() const = 0;
};


class SCH_HIERLABEL_SYNCHRONIZATION_ITEM : public SHEET_SYNCHRONIZATION_ITEM
{
public:
    SCH_HIERLABEL_SYNCHRONIZATION_ITEM( SCH_HIERLABEL* aLabel, SCH_SHEET* aSheet );

    SCH_HIERLABEL* GetLabel() const { return m_label; }

    QString GetName() const override;

    int GetShape() const override;

    QPixmap& GetBitmap() const override;

    SCH_ITEM* GetItem() const override;

    SHEET_SYNCHRONIZATION_ITEM_KIND GetKind() const override;

private:
    SCH_HIERLABEL* m_label;
    SCH_SHEET*     m_sheet;
};


class SCH_SHEET_PIN_SYNCHRONIZATION_ITEM : public SHEET_SYNCHRONIZATION_ITEM
{
public:
    SCH_SHEET_PIN_SYNCHRONIZATION_ITEM( SCH_SHEET_PIN* aPin, SCH_SHEET* aSheet );

    SCH_SHEET_PIN* GetPin() const { return m_pin; }

    QString GetName() const override;

    int GetShape() const override;

    QPixmap& GetBitmap() const override;

    SCH_ITEM* GetItem() const override;

    SHEET_SYNCHRONIZATION_ITEM_KIND GetKind() const override;

private:
    SCH_SHEET_PIN* m_pin;
    SCH_SHEET*     m_sheet;
};


class ASSOCIATED_SCH_LABEL_PIN : public SHEET_SYNCHRONIZATION_ITEM
{
public:
    ASSOCIATED_SCH_LABEL_PIN( SCH_HIERLABEL* aLabel, SCH_SHEET_PIN* aPin );

    ASSOCIATED_SCH_LABEL_PIN( SCH_HIERLABEL_SYNCHRONIZATION_ITEM* aLabel,
                              SCH_SHEET_PIN_SYNCHRONIZATION_ITEM* aPin );

    SCH_HIERLABEL* GetLabel() const { return m_label; }

    SCH_SHEET_PIN* GetPin() const { return m_pin; }

    QString GetName() const override;

    int GetShape() const override;

    QPixmap& GetBitmap() const override;

    SCH_ITEM* GetItem() const override;

    SHEET_SYNCHRONIZATION_ITEM_KIND GetKind() const override;

private:
    SCH_HIERLABEL* m_label;
    SCH_SHEET_PIN* m_pin;
};


#endif
