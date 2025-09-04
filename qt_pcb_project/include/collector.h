
#ifndef COLLECTOR_H
#define COLLECTOR_H

#include <QVector>
#include <core/kicad_algo.h>
#include <eda_item.h>


class EDA_ITEM;


class COLLECTOR
{
public:
    COLLECTOR() :
            m_Threshold( 0 ),
            m_MenuCancelled( false ),
            m_scanTypes( {} )
    {
        m_inspector = [this]( EDA_ITEM* aItem, void* aTestData )
        {
            return this->Inspect( aItem, aTestData );
        };
    }

    virtual ~COLLECTOR() {}

    virtual INSPECT_RESULT Inspect( EDA_ITEM* aTestItem, void* aTestData )
    {
        return INSPECT_RESULT::QUIT;
    };

    using ITER = QVector<EDA_ITEM*>::iterator;
    using CITER = QVector<EDA_ITEM*>::const_iterator;

    ITER begin() { return m_list.begin(); }
    ITER end() { return m_list.end(); }
    CITER begin() const { return m_list.constBegin(); }
    CITER end() const { return m_list.constEnd(); }

    int GetCount() const
    {
        return (int) m_list.size();
    }

    void Empty()
    {
        m_list.clear();
    }

    void Append( EDA_ITEM* item )
    {
        m_list.append( item );
    }

    void Remove( int aIndex )
    {
        m_list.remove( aIndex );
    }

    void Remove( const EDA_ITEM* aItem )
    {
        alg::delete_if( m_list, [&aItem]( const EDA_ITEM* aCandidate )
                                {
                                    return aCandidate == aItem;
                                } );
    }

    bool HasAdditionalItems()
    {
        return !m_backupList.empty();
    }

    void Combine()
    {
        m_list += m_backupList;
        m_backupList.clear();
    }

    void Transfer( int aIndex )
    {
        m_backupList.append( m_list[aIndex] );
        m_list.remove( aIndex );
    }

    void Transfer( EDA_ITEM* aItem )
    {
        for( size_t i = 0; i < m_list.size(); i++ )
        {
            if( m_list[i] == aItem )
            {
                m_list.remove( i );
                m_backupList.append( aItem );
                return;
            }
        }
    }

    virtual EDA_ITEM* operator[]( int aIndex ) const
    {
        if( (unsigned)aIndex < (unsigned)GetCount() )  // (unsigned) excludes aIndex<0 also
            return m_list[ aIndex ];

        return nullptr;
    }

    bool HasItem( const EDA_ITEM* aItem ) const
    {
        for( size_t i = 0; i < m_list.size(); i++ )
        {
            if( m_list[i] == aItem )
                return true;
        }

        return false;
    }

    void SetScanTypes( const QVector<KICAD_T>& aTypes ) { m_scanTypes = aTypes; }

    void SetRefPos( const VECTOR2I& aRefPos ) { m_refPos = aRefPos; }

    int CountType( KICAD_T aType )
    {
        int cnt = 0;

        for( size_t i = 0; i < m_list.size(); i++ )
        {
            if( m_list[i]->Type() == aType )
                cnt++;
        }

        return cnt;
    }

    int            m_Threshold;

    QString        m_MenuTitle;
    bool           m_MenuCancelled;

protected:
    QVector<EDA_ITEM*> m_list;
    QVector<EDA_ITEM*> m_backupList;

    QVector<KICAD_T>   m_scanTypes;
    INSPECTOR_FUNC     m_inspector;

    VECTOR2I           m_refPos;
};

#endif  // COLLECTOR_H
