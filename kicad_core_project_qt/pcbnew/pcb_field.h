#ifndef PCB_FIELD_H
#define PCB_FIELD_H

#include <QString>
#include <pcb_text.h>
#include <template_fieldnames.h>

class BOARD_DESIGN_SETTINGS;

class PCB_FIELD : public PCB_TEXT
{
public:
    PCB_FIELD( FOOTPRINT* aParent, int aFieldId, const QString& aName = QString() );

    PCB_FIELD( const PCB_TEXT& aText, int aFieldId, const QString& aName = QString() );

    //void Serialize( google::protobuf::Any &aContainer ) const override;
    //bool Deserialize( const google::protobuf::Any &aContainer ) override;

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_FIELD_T == aItem->Type();
    }

    QString GetClass() const override { return QStringLiteral( "PCB_FIELD" ); }

    bool IsType( const std::vector<KICAD_T>& aScanTypes ) const override
    {
        if( BOARD_ITEM::IsType( aScanTypes ) )
            return true;

        for( KICAD_T scanType : aScanTypes )
        {
            if( scanType == PCB_FIELD_LOCATE_REFERENCE_T && m_id == REFERENCE_FIELD )
                return true;
            else if( scanType == PCB_FIELD_LOCATE_VALUE_T && m_id == VALUE_FIELD )
                return true;
            else if( scanType == PCB_FIELD_LOCATE_DATASHEET_T && m_id == DATASHEET_FIELD )
                return true;
        }

        return false;
    }

    bool IsReference() const { return m_id == REFERENCE_FIELD; }
    bool IsValue() const { return m_id == VALUE_FIELD; }
    bool IsDatasheet() const { return m_id == DATASHEET_FIELD; }
    bool IsComponentClass() const { return GetName() == QStringLiteral( "Component Class" ); }

    bool IsMandatory() const;

    bool IsHypertext() const;

    QString GetTextTypeDescription() const override;

    bool Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const override;

    QString GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const override;

    double ViewGetLOD( int aLayer, const KIGFX::VIEW* aView ) const override;

    EDA_ITEM* Clone() const override;

    /**
     * Same as Clone, but returns a PCB_FIELD item.
     *
     * Useful mainly for python scripts, because Clone returns an EDA_ITEM.
     */
    PCB_FIELD* CloneField() const { return (PCB_FIELD*) Clone(); }

    QString GetName( bool aUseDefaultName = true ) const;

    QString GetCanonicalName() const;

    void SetName( const QString& aName ) { m_name = aName; }

    int GetId() const { return m_id; }
    void SetId( int aId ) { m_id = aId; }

    double Similarity( const BOARD_ITEM& aOther ) const override;

    bool operator==( const PCB_FIELD& aOther ) const;
    bool operator==( const BOARD_ITEM& aOther ) const override;

protected:
    void swapData( BOARD_ITEM* aImage ) override;

private:
    void setId( int aId ) { m_id = aId; }

    int m_id; ///< Field index, @see enum MANDATORY_FIELD_T

    QString m_name;
};

#endif
