
#ifndef _BUS_ALIAS_H
#define _BUS_ALIAS_H

#include <memory>
#include <vector>
#include <QString>


class SCH_SCREEN;


class BUS_ALIAS
{
public:
    BUS_ALIAS( SCH_SCREEN* aParent = nullptr ) :
        m_parent( aParent )
    { }

    ~BUS_ALIAS()
    { }

    std::shared_ptr<BUS_ALIAS> Clone() const
    {
        return std::make_shared<BUS_ALIAS>( *this );
    }

    QString GetName() { return m_name; }
    void SetName( const QString& aName ) { m_name = aName; }

    const std::vector<QString>& Members() const { return m_members; }
    std::vector<QString>& Members() { return m_members; }

    SCH_SCREEN* GetParent() { return m_parent; }
    void SetParent( SCH_SCREEN* aParent ) { m_parent = aParent; }

protected:
    QString              m_name;
    std::vector<QString> m_members;

    /**
     * Schematic Setup can edit aliases from all sheets, so we have to store a reference back
     * to our parent so that the dialog can update the parent if aliases are changed or removed.
     */
    SCH_SCREEN* m_parent;
};

#endif
