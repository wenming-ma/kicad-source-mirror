
#ifndef KIID_H
#define KIID_H

#include <kicommon.h>
#include <boost/uuid/uuid.hpp>
#include <macros_swig.h>
#include <nlohmann/json_fwd.hpp>

#include <string>

class QString;

// timestamp_t is our type to represent unique IDs for all kinds of elements
typedef uint32_t timestamp_t;

class KICOMMON_API KIID
{
public:
    KIID();
    KIID( int null );
    KIID( const std::string& aString );
    KIID( const char* aString );
    KIID( const QString& aString );
    KIID( timestamp_t aTimestamp );

    void Clone( const KIID& aUUID );

    size_t Hash() const;

    bool        IsLegacyTimestamp() const;
    timestamp_t AsLegacyTimestamp() const;

    QString AsString() const;
    QString AsLegacyTimestampString() const;
    std::string AsStdString() const;

    // Returns true if a string has the correct formatting to be a KIID
    static bool SniffTest( const QString& aCandidate );

    // A performance optimization which disables/enables the generation of pseudo-random UUIDs
    // NB: uses a global.  Not thread safe!
    static void CreateNilUuids( bool aNil = true );

    // Re-initialize the UUID generator with a given seed (for testing or QA purposes)
    // WARNING: Do not call this function from within KiCad or via a Python action plugin
    static void SeedGenerator( unsigned int aSeed );

    // Change an existing time stamp based UUID into a true UUID
    void ConvertTimestampToUuid();

    // Generates a deterministic replacement for a given ID
    void Increment();

    bool operator==( KIID const& rhs ) const
    {
        return m_uuid == rhs.m_uuid;
    }

    bool operator!=( KIID const& rhs ) const
    {
        return m_uuid != rhs.m_uuid;
    }

    bool operator<( KIID const& rhs ) const
    {
        return m_uuid < rhs.m_uuid;
    }

    bool operator>( KIID const& rhs ) const
    {
        return m_uuid > rhs.m_uuid;
    }

private:
    boost::uuids::uuid m_uuid;
};


extern KICOMMON_API KIID niluuid;

KICOMMON_API KIID& NilUuid();

// declare KIID_VECT_LIST as std::vector<KIID> both for c++ and swig:
DECL_VEC_FOR_SWIG( KIID_VECT_LIST, KIID )

class KICOMMON_API KIID_PATH : public KIID_VECT_LIST
{
public:
    KIID_PATH()
    {
    }

    KIID_PATH( const QString& aString );

    bool MakeRelativeTo( const KIID_PATH& aPath );

    // Test if aPath from the last path towards the first path
    bool EndsWith( const KIID_PATH& aPath ) const;

    QString AsString() const;

    bool operator==( KIID_PATH const& rhs ) const
    {
        if( size() != rhs.size() )
            return false;

        for( size_t i = 0; i < size(); ++i )
        {
            if( at( i ) != rhs.at( i ) )
                return false;
        }

        return true;
    }

    bool operator<( KIID_PATH const& rhs ) const
    {
        if( size() != rhs.size() )
            return size() < rhs.size();

        for( size_t i = 0; i < size(); ++i )
        {
            if( at( i ) < rhs.at( i ) )
                return true;

            if( at( i ) != rhs.at( i ) )
                return false;
        }

        return false;
    }

    bool operator>( KIID_PATH const& rhs ) const
    {
        if( size() != rhs.size() )
            return size() > rhs.size();

        for( size_t i = 0; i < size(); ++i )
        {
            if( at( i ) > rhs.at( i ) )
                return true;

            if( at( i ) != rhs.at( i ) )
                return false;
        }

        return false;
    }

    KIID_PATH& operator+=( const KIID_PATH& aRhs )
    {
        for( const KIID& kiid : aRhs )
            emplace_back( kiid );

        return *this;
    }

    friend KIID_PATH operator+( KIID_PATH aLhs, const KIID_PATH& aRhs )
    {
        aLhs += aRhs;
        return aLhs;
    }
};

// RAII class to safely set/reset nil KIIDs for use in footprint/symbol loading
class KICOMMON_API KIID_NIL_SET_RESET
{
public:
    KIID_NIL_SET_RESET()
    {
        KIID::CreateNilUuids( true );
    };

    ~KIID_NIL_SET_RESET()
    {
        KIID::CreateNilUuids( false );
    }
};

KICOMMON_API void to_json( nlohmann::json& aJson, const KIID& aKIID );

KICOMMON_API void from_json( const nlohmann::json& aJson, KIID& aKIID );

template<> struct KICOMMON_API std::hash<KIID>
{
    std::size_t operator()( const KIID& aId ) const
    {
        return aId.Hash();
    }
};

#endif // KIID_H
