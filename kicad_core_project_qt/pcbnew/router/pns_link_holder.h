
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCBNEW_ROUTER_PNS_LINK_HOLDER_H_
#define PCBNEW_ROUTER_PNS_LINK_HOLDER_H_

#include <QtGlobal>
#include <core/kicad_algo.h>
#include "pns_item.h"
#include "pns_linked_item.h"

namespace PNS
{
class LINK_HOLDER : public ITEM
{
public:
    LINK_HOLDER( PnsKind aKind ) :
        ITEM( aKind )
    {}

    ///< Add a reference to an item registered in a #NODE that is a part of this line.
    void Link( LINKED_ITEM* aLink )
    {
        Q_ASSERT_X( !alg::contains( m_links, aLink ), "Link", 
                    "Trying to link an item that is already linked" );
        m_links.push_back( aLink );
    }

    void Unlink( const LINKED_ITEM* aLink )
    {
        Q_ASSERT_X( alg::contains( m_links, aLink ), "Unlink",
                    "Trying to unlink an item that is not linked" );
        alg::delete_matching( m_links, aLink );
    }

    ///< Return the list of links from the owning node that constitute this
    ///< line (or NULL if the line is not linked).
    std::vector<LINKED_ITEM*>& Links() { return m_links; }
    const std::vector<LINKED_ITEM*>& Links() const { return m_links; }

    bool IsLinked() const
    {
        return m_links.size() != 0;
    }

    ///< Check if the segment aLink is a part of the line.
    bool ContainsLink( const LINKED_ITEM* aItem ) const
    {
        return alg::contains( m_links, aItem );
    }

    LINKED_ITEM* GetLink( int aIndex ) const
    {
        if( aIndex < 0 )
            aIndex += m_links.size();

        return m_links[aIndex];
    }

    ///< Erase the linking information. Used to detach the line from the owning node.
    virtual void ClearLinks()
    {
        m_links.clear();
    }

    ///< Return the number of segments that were assembled together to form this line.
    int LinkCount() const
    {
        return m_links.size();
    }

    void ShowLinks() const
    {
#if 0 /// @todo move outside header.
        if( !IsLinked() )
        {
            // TODO: Replace with Qt logging
            // wxLogTrace( wxT( "PNS" ), wxT( "item %p: no links" ), this );
            return;
        }

        // TODO: Replace with Qt logging
        // wxLogTrace( wxT( "PNS" ), wxT( "item %p: %d links" ), this, (int) m_links.size() );

        for( int i = 0; i < (int) m_links.size(); i++ )
            // TODO: Replace with Qt logging
            // wxLogTrace( wxT( "PNS" ), wxT( "item %d: %p\n" ), i, m_links[i] );
#endif
    }

protected:
    ///< Copy m_links from the line \a aParent.
    void copyLinks( const LINK_HOLDER* aParent )
    {
        m_links = aParent->m_links;
    }

    ///< List of segments in the owning NODE (ITEM::m_owner) that constitute this line, or NULL
    ///< if the line is not a part of any node.
    std::vector<LINKED_ITEM*> m_links;
};

} // namespace PNS
#endif /* PCBNEW_ROUTER_PNS_LINK_HOLDER_H_ */
