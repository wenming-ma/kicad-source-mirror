#include <inspectable.h>
#include <properties/property_mgr.h>
#include <properties/property.h>

#include <algorithm>
#include <utility>
#include <QString>

static QString EMPTY_STRING;


void PROPERTY_MANAGER::RegisterType( TYPE_ID aType, const QString& aName )
{
    Q_ASSERT( m_classNames.count( aType ) == 0 );
    m_classNames.emplace( aType, aName );
}


const QString& PROPERTY_MANAGER::ResolveType( TYPE_ID aType ) const
{
    auto it = m_classNames.find( aType );
    return it != m_classNames.end() ? it->second : EMPTY_STRING;
}


PROPERTY_BASE* PROPERTY_MANAGER::GetProperty( TYPE_ID aType, const QString& aProperty ) const
{
    if( m_dirty )
        const_cast<PROPERTY_MANAGER*>( this )->Rebuild();

    auto it = m_classes.find( aType );

    if( it == m_classes.end() )
        return nullptr;

    const CLASS_DESC& classDesc = it->second;

    for( PROPERTY_BASE* property : classDesc.m_allProperties )
    {
        if( aProperty.compare( property->Name(), Qt::CaseInsensitive ) == 0 )
            return property;
    }

    return nullptr;
}


const PROPERTY_LIST& PROPERTY_MANAGER::GetProperties( TYPE_ID aType ) const
{
    if( m_dirty )
        const_cast<PROPERTY_MANAGER*>( this )->Rebuild();

    static const PROPERTY_LIST empty;
    auto it = m_classes.find( aType );

    if( it == m_classes.end() )
        return empty;

    return it->second.m_allProperties;
}


const PROPERTY_DISPLAY_ORDER& PROPERTY_MANAGER::GetDisplayOrder( TYPE_ID aType ) const
{
    if( m_dirty )
        const_cast<PROPERTY_MANAGER*>( this )->Rebuild();

    static const PROPERTY_DISPLAY_ORDER empty;
    auto it = m_classes.find( aType );

    if( it == m_classes.end() )
        return empty;

    return it->second.m_displayOrder;
}


const QVector<QString>& PROPERTY_MANAGER::GetGroupDisplayOrder( TYPE_ID aType ) const
{
    if( m_dirty )
        const_cast<PROPERTY_MANAGER*>( this )->Rebuild();

    static const QVector<QString> empty;
    auto it = m_classes.find( aType );

    if( it == m_classes.end() )
        return empty;

    return it->second.m_groupDisplayOrder;
}


const void* PROPERTY_MANAGER::TypeCast( const void* aSource, TYPE_ID aBase, TYPE_ID aTarget ) const
{
    if( aBase == aTarget )
        return aSource;

    auto classDesc = m_classes.find( aBase );

    if( classDesc == m_classes.end() )
        return aSource;

    auto& converters = classDesc->second.m_typeCasts;
    auto converter = converters.find( aTarget );

    if( converter == converters.end() )     // explicit type cast not found
        return IsOfType( aBase, aTarget ) ? aSource : nullptr;

    return (*converter->second)( aSource );
}


PROPERTY_BASE& PROPERTY_MANAGER::AddProperty( PROPERTY_BASE* aProperty, const QString& aGroup )
{
    const QString& name = aProperty->Name();
    TYPE_ID hash = aProperty->OwnerHash();
    CLASS_DESC& classDesc = getClass( hash );
    classDesc.m_ownProperties.emplace( name, aProperty );
    classDesc.m_ownDisplayOrder.emplace_back( aProperty );

    aProperty->SetGroup( aGroup );

    if( !classDesc.m_groups.count( aGroup ) )
    {
        classDesc.m_groupDisplayOrder.emplace_back( aGroup );
        classDesc.m_groups.insert( aGroup );
    }

    m_dirty = true;
    return *aProperty;
}


PROPERTY_BASE& PROPERTY_MANAGER::ReplaceProperty( size_t aBase, const QString& aName,
                                                  PROPERTY_BASE* aNew, const QString& aGroup )
{
    CLASS_DESC& classDesc = getClass( aNew->OwnerHash() );
    classDesc.m_replaced.insert( std::make_pair( aBase, aName ) );
    return AddProperty( aNew, aGroup );
}


void PROPERTY_MANAGER::AddTypeCast( TYPE_CAST_BASE* aCast )
{
    TYPE_ID derivedHash = aCast->DerivedHash();
    CLASS_DESC& classDesc = getClass( aCast->BaseHash() );
    auto& typeCasts = classDesc.m_typeCasts;
    Q_ASSERT_X( typeCasts.count( derivedHash ) == 0, "AddTypeCast", "Such converter already exists" );
    typeCasts.emplace( derivedHash, aCast );
}


void PROPERTY_MANAGER::InheritsAfter( TYPE_ID aDerived, TYPE_ID aBase )
{
    Q_ASSERT_X( aDerived != aBase, "InheritsAfter", "Class cannot inherit from itself" );

    CLASS_DESC& derived = getClass( aDerived );
    CLASS_DESC& base = getClass( aBase );
    derived.m_bases.push_back( base );
    m_dirty = true;

    Q_ASSERT_X( derived.m_bases.size() == 1 || derived.m_typeCasts.count( aBase ) == 1,
                "InheritsAfter", "You need to add a TYPE_CAST for classes inheriting from multiple bases" );
}


void PROPERTY_MANAGER::Mask( TYPE_ID aDerived, TYPE_ID aBase, const QString& aName )
{
    Q_ASSERT_X( aDerived != aBase, "Mask", "Class cannot mask from itself" );

    CLASS_DESC& derived = getClass( aDerived );
    derived.m_maskedBaseProperties.insert( std::make_pair( aBase, aName ) );
    m_dirty = true;
}


void PROPERTY_MANAGER::OverrideAvailability( TYPE_ID aDerived, TYPE_ID aBase,
                                             const QString& aName,
                                             std::function<bool( INSPECTABLE* )> aFunc )
{
    Q_ASSERT_X( aDerived != aBase, "OverrideAvailability", "Class cannot override from itself" );

    CLASS_DESC& derived = getClass( aDerived );
    derived.m_availabilityOverrides[std::make_pair( aBase, aName )] = std::move( aFunc );
    m_dirty = true;
}


void PROPERTY_MANAGER::OverrideWriteability( TYPE_ID aDerived, TYPE_ID aBase,
                                             const QString& aName,
                                             std::function<bool( INSPECTABLE* )> aFunc )
{
    Q_ASSERT_X( aDerived != aBase, "OverrideWriteability", "Class cannot override from itself" );

    CLASS_DESC& derived = getClass( aDerived );
    derived.m_writeabilityOverrides[std::make_pair( aBase, aName )] = std::move( aFunc );
    m_dirty = true;
}


bool PROPERTY_MANAGER::IsAvailableFor( TYPE_ID aItemClass, PROPERTY_BASE* aProp,
                                       INSPECTABLE* aItem )
{
    if( !aProp->Available( aItem ) )
        return false;

    CLASS_DESC& derived = getClass( aItemClass );

    auto it = derived.m_availabilityOverrides.find( std::make_pair( aProp->BaseHash(),
                                                                    aProp->Name() ) );

    if( it != derived.m_availabilityOverrides.end() )
        return it->second( aItem );

    return true;
}


bool PROPERTY_MANAGER::IsWriteableFor( TYPE_ID aItemClass, PROPERTY_BASE* aProp,
                                       INSPECTABLE* aItem )
{
    if( !aProp->Writeable( aItem ) )
        return false;

    CLASS_DESC& derived = getClass( aItemClass );

    auto it = derived.m_writeabilityOverrides.find( std::make_pair( aProp->BaseHash(),
                                                                    aProp->Name() ) );

    if( it != derived.m_writeabilityOverrides.end() )
        return it->second( aItem );

    return true;
}


bool PROPERTY_MANAGER::IsOfType( TYPE_ID aDerived, TYPE_ID aBase ) const
{
    if( aDerived == aBase )
        return true;

    auto derived = m_classes.find( aDerived );
    if( derived == m_classes.end() )
        return false;

    // traverse the hierarchy seeking for the base class
    for( auto& base : derived->second.m_bases )
    {
        if( IsOfType( base.get().m_id, aBase ) )
            return true;
    }

    return false;
}


void PROPERTY_MANAGER::Rebuild()
{
    for( std::pair<const TYPE_ID, CLASS_DESC>& classEntry : m_classes )
        classEntry.second.rebuild();

    m_dirty = false;
}


PROPERTY_MANAGER::CLASS_DESC& PROPERTY_MANAGER::getClass( TYPE_ID aTypeId )
{
    auto it = m_classes.find( aTypeId );

    if( it == m_classes.end() )
        tie( it, std::ignore ) = m_classes.emplace( aTypeId, CLASS_DESC( aTypeId ) );

    return it->second;
}


void PROPERTY_MANAGER::CLASS_DESC::rebuild()
{
    PROPERTY_SET replaced;
    PROPERTY_SET masked;
    m_allProperties.clear();
    collectPropsRecur( m_allProperties, replaced, m_displayOrder, masked );

    // We need to keep properties sorted to be able to use std::set_* functions
    sort( m_allProperties.begin(), m_allProperties.end() );

    QVector<QString> displayOrder;
    std::set<QString> groups;

    auto collectGroups =
            [&]( std::set<QString>& aSet, QVector<QString>& aResult )
    {
        auto collectGroupsRecursive =
                []( auto& aSelf, std::set<QString>& aSetR, QVector<QString>& aResultR,
                    const CLASS_DESC& aClassR ) -> void
        {
            for( const QString& group : aClassR.m_groupDisplayOrder )
            {
                if( !aSetR.count( group ) )
                {
                    aSetR.insert( group );
                    aResultR.emplace_back( group );
                }
            }

            for( const CLASS_DESC& base : aClassR.m_bases )
                aSelf( aSelf, aSetR, aResultR, base );
        };

        collectGroupsRecursive( collectGroupsRecursive, aSet, aResult, *this );
    };

    collectGroups( groups, displayOrder );
    m_groupDisplayOrder = displayOrder;
}


void PROPERTY_MANAGER::CLASS_DESC::collectPropsRecur( PROPERTY_LIST& aResult,
                                                      PROPERTY_SET& aReplaced,
                                                      PROPERTY_DISPLAY_ORDER& aDisplayOrder,
                                                      PROPERTY_SET& aMasked ) const
{
    for( const std::pair<size_t, QString>& replacedEntry : m_replaced )
        aReplaced.emplace( replacedEntry );

    for( const std::pair<size_t, QString>& maskedEntry : m_maskedBaseProperties )
        aMasked.emplace( maskedEntry );

    int displayOrderStart = 0;

    if( !aDisplayOrder.empty() )
    {
        int firstSoFar = std::min_element( aDisplayOrder.begin(), aDisplayOrder.end(),
                                           []( const std::pair<PROPERTY_BASE*, int>& aFirst,
                                               const std::pair<PROPERTY_BASE*, int>& aSecond )
                                           {
                                               return aFirst.second < aSecond.second;
                                           } )->second;

        displayOrderStart = firstSoFar - m_ownProperties.size();
    }

    int idx = 0;

    for( PROPERTY_BASE* property : m_ownDisplayOrder )
    {
        PROPERTY_SET::key_type propertyKey = std::make_pair( property->OwnerHash(),
                                                             property->Name() );
        // Do not store replaced properties
        if( aReplaced.count( propertyKey ) )
            continue;

        // Do not store masked properties
        if( aMasked.count( propertyKey ) )
            continue;

        aDisplayOrder[property] = displayOrderStart + idx++;
        aResult.push_back( property );
    }

    // Iterate backwards so that replaced properties appear before base properties
    for( auto it = m_bases.rbegin(); it != m_bases.rend(); ++it )
        it->get().collectPropsRecur( aResult, aReplaced, aDisplayOrder, aMasked );
}


std::vector<TYPE_ID> PROPERTY_MANAGER::GetMatchingClasses( PROPERTY_BASE* aProperty )
{
    std::vector<TYPE_ID> ids;


    return ids;
}


PROPERTY_MANAGER::CLASSES_INFO PROPERTY_MANAGER::GetAllClasses()
{
    CLASSES_INFO rv;

    for( std::pair<const TYPE_ID, CLASS_DESC>& classEntry : m_classes )
    {
        CLASS_INFO info;

        info.type = classEntry.first;
        info.name = m_classNames[classEntry.first];

        for( PROPERTY_BASE* prop : classEntry.second.m_allProperties )
            info.properties.push_back( prop );

        rv.push_back( info );
    }

    return rv;
}


void PROPERTY_MANAGER::PropertyChanged( INSPECTABLE* aObject, PROPERTY_BASE* aProperty )
{
    auto callListeners =
            [&]( TYPE_ID typeId )
            {
                auto listeners = m_listeners.find( typeId );

                if( listeners != m_listeners.end() )
                {
                    for( const PROPERTY_LISTENER& listener : listeners->second )
                        listener( aObject, aProperty, m_managedCommit );
                }
            };

    CLASS_DESC& objectClass = getClass( TYPE_HASH( *aObject ) );

    callListeners( objectClass.m_id );

    for( CLASS_DESC& superClass : objectClass.m_bases )
        callListeners( superClass.m_id );
}


PROPERTY_COMMIT_HANDLER::PROPERTY_COMMIT_HANDLER( COMMIT* aCommit )
{
    if( PROPERTY_MANAGER::Instance().m_managedCommit != nullptr )
        return;

    PROPERTY_MANAGER::Instance().m_managedCommit = aCommit;
}


PROPERTY_COMMIT_HANDLER::~PROPERTY_COMMIT_HANDLER()
{
    Q_ASSERT_X( PROPERTY_MANAGER::Instance().m_managedCommit != nullptr,
                "~PROPERTY_COMMIT_HANDLER", "Something went wrong: m_managedCommit already null!" );

    PROPERTY_MANAGER::Instance().m_managedCommit = nullptr;
}
