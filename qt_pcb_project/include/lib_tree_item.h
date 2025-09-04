
#ifndef LIB_TREE_ITEM_H
#define LIB_TREE_ITEM_H

#include <QMap>
#include <lib_id.h>
#include <import_export.h>
#include <eda_pattern_match.h>


class APIEXPORT LIB_TREE_ITEM
{
public:
    virtual ~LIB_TREE_ITEM()
    {
    }

    virtual LIB_ID GetLIB_ID() const = 0;

    virtual QString GetName() const = 0;
    virtual QString GetLibNickname() const = 0;
    virtual QString GetDesc() = 0;

    virtual void GetChooserFields( QMap<QString , QString>& aColumnMap ) {}

    virtual std::vector<SEARCH_TERM> GetSearchTerms() { return std::vector<SEARCH_TERM>(); }

    virtual bool IsRoot() const { return true; }

    virtual QString GetFootprint() { return QString(); }

    virtual int GetPinCount() { return 0; }

    virtual int GetSubUnitCount() const { return 0; }

    virtual QString GetUnitReference( int aUnit ) { return QString(); }

    virtual QString GetUnitDisplayName( int aUnit ) { return QString(); }

    virtual bool HasUnitDisplayName( int aUnit ) { return false; }
};

#endif //LIB_TREE_ITEM_H
