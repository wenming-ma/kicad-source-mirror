
#include <utility>
#include <QString>
#include "i18n_utility.h"


#include <component_class_manager.h>


void COMPONENT_CLASS::AddConstituentClass( COMPONENT_CLASS* componentClass )
{
    m_constituentClasses.push_back( componentClass );
}


bool COMPONENT_CLASS::ContainsClassName( const QString& className ) const
{
    if( m_constituentClasses.size() == 0 )
        return false;

    if( m_constituentClasses.size() == 1 )
        return m_name == className;

    return std::any_of( m_constituentClasses.begin(), m_constituentClasses.end(),
                        [&className]( const COMPONENT_CLASS* testClass )
                        {
                            return testClass->GetFullName() == className;
                        } );
}


QString COMPONENT_CLASS::GetName() const
{
    if( m_constituentClasses.size() == 0 )
        return QStringLiteral( "<None>" );

    if( m_constituentClasses.size() == 1 )
        return m_name;

    Q_ASSERT( m_constituentClasses.size() >= 2 );

    QString name;

    if( m_constituentClasses.size() == 2 )
    {
        name = _( "%1 and %2" ).arg( m_constituentClasses[0]->GetName() )
                                .arg( m_constituentClasses[1]->GetName() );
    }
    else if( m_constituentClasses.size() == 3 )
    {
        name = _( "%1, %2 and %3" ).arg( m_constituentClasses[0]->GetName() )
                                    .arg( m_constituentClasses[1]->GetName() )
                                    .arg( m_constituentClasses[2]->GetName() );
    }
    else if( m_constituentClasses.size() > 3 )
    {
        name = _( "%1, %2 and %3 more" ).arg( m_constituentClasses[0]->GetName() )
                                         .arg( m_constituentClasses[1]->GetName() )
                                         .arg( static_cast<int>( m_constituentClasses.size() - 2 ) );
    }

    return name;
}


bool COMPONENT_CLASS::IsEmpty() const
{
    return m_constituentClasses.size() == 0;
}


COMPONENT_CLASS_MANAGER::COMPONENT_CLASS_MANAGER()
{
    m_noneClass = std::make_unique<COMPONENT_CLASS>( QString() );
}


COMPONENT_CLASS* COMPONENT_CLASS_MANAGER::GetEffectiveComponentClass(
        const std::unordered_set<QString>& classNames )
{
    if( classNames.size() == 0 )
        return m_noneClass.get();

    // Lambda to handle finding constituent component classes. This first checks the cache,
    // and if found moves the class to the primary classes map. If not found, it either returns
    // an existing class in the primary list or creates a new class.
    auto getOrCreateClass = [this]( const QString& className )
    {
        if( m_classesCache.count( className ) )
        {
            auto existingClass = m_classesCache.extract( className );
            m_classes.insert( std::move( existingClass ) );
        }
        else if( !m_classes.count( className ) )
        {
            std::unique_ptr<COMPONENT_CLASS> newClass =
                    std::make_unique<COMPONENT_CLASS>( className );
            newClass->AddConstituentClass( newClass.get() );
            m_classes[className] = std::move( newClass );
        }

        return m_classes[className].get();
    };

    // Handle single-assignment component classes
    if( classNames.size() == 1 )
        return getOrCreateClass( *classNames.begin() );

    // Handle composite component classes
    std::vector<QString> sortedClassNames( classNames.begin(), classNames.end() );

    std::sort( sortedClassNames.begin(), sortedClassNames.end(),
               []( const QString& str1, const QString& str2 )
               {
                   return str1.compare( str2 ) < 0;
               } );

    QString fullName = GetFullClassNameForConstituents( sortedClassNames );

    if( m_effectiveClassesCache.count( fullName ) )
    {
        // The effective class was previously constructed - copy it across to the new live map
        auto             existingClass = m_effectiveClassesCache.extract( fullName );
        COMPONENT_CLASS* effClass = existingClass.mapped().get();
        m_effectiveClasses.insert( std::move( existingClass ) );

        // Ensure that all constituent component classes are copied to the live map
        for( COMPONENT_CLASS* constClass : effClass->GetConstituentClasses() )
        {
            if( m_classesCache.count( constClass->GetFullName() ) )
            {
                auto constClassNode = m_classesCache.extract( constClass->GetFullName() );
                m_classes.insert( std::move( constClassNode ) );
            }
        }
    }
    else if( !m_effectiveClasses.count( fullName ) )
    {
        // The effective class was not previously constructed
        std::unique_ptr<COMPONENT_CLASS> effClass = std::make_unique<COMPONENT_CLASS>( fullName );

        for( const QString& className : sortedClassNames )
            effClass->AddConstituentClass( getOrCreateClass( className ) );

        m_effectiveClasses[fullName] = std::move( effClass );
    }

    return m_effectiveClasses[fullName].get();
}


void COMPONENT_CLASS_MANAGER::InitNetlistUpdate()
{
    m_classesCache = std::move( m_classes );
    m_effectiveClassesCache = std::move( m_effectiveClasses );
}


void COMPONENT_CLASS_MANAGER::FinishNetlistUpdate()
{
    m_classesCache.clear();
    m_effectiveClassesCache.clear();
}


QString COMPONENT_CLASS_MANAGER::GetFullClassNameForConstituents(
        const std::unordered_set<QString>& classNames )
{
    std::vector<QString> sortedClassNames( classNames.begin(), classNames.end() );

    std::sort( sortedClassNames.begin(), sortedClassNames.end(),
               []( const QString& str1, const QString& str2 )
               {
                   return str1.compare( str2 ) < 0;
               } );

    return GetFullClassNameForConstituents( sortedClassNames );
}


QString
COMPONENT_CLASS_MANAGER::GetFullClassNameForConstituents( const std::vector<QString>& classNames )
{
    if( classNames.size() == 0 )
        return QString();

    QString fullName = classNames[0];

    for( std::size_t i = 1; i < classNames.size(); ++i )
    {
        fullName += ",";
        fullName += classNames[i];
    }

    return fullName;
}
