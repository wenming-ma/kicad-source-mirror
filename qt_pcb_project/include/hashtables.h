// QT_TRANSFORMATION_COMPLETED

#ifndef HASHTABLES_H_
#define HASHTABLES_H_

#include <unordered_map>

#include <QString>
#include <QHash>

// Qt 6.9.1 already provides std::hash<QString> specialization
// No need to define it manually

// First some utility classes and functions

// Equality test for "const char*" type used in very specialized KEYWORD_MAP below
struct iequal_to
{
    bool operator()( const char* x, const char* y ) const
    {
        return !strcmp( x, y );
    }
};


// Very fast and efficient hash function for "const char*" type, used in specialized
// KEYWORD_MAP below.
// taken from: http://www.boost.org/doc/libs/1_53_0/libs/unordered/examples/fnv1.hpp
struct fnv_1a
{
    std::size_t operator()( const char* it ) const
    {
        std::size_t hash = 2166136261u;

        for( ; *it; ++it )
        {
            hash ^= (unsigned char) *it;
            hash *= 16777619;
        }
        return hash;
    }
};


// Declare a std::unordered_map
#define DECL_HASH_FOR_SWIG( TypeName, KeyType, ValueType )          \
    typedef std::unordered_map<KeyType, ValueType> TypeName;


// A hashtable made of a const char* and an int.
// The use of this type outside very specific circumstances is foolish since there is
// no storage provided for the actual C string itself.
//
// This type assumes use with type KEYWORD that is created by CMake and that table creates
// constant storage for C strings (and pointers to those C strings).  Here we are only
// interested in the C strings themselves and only the pointers are duplicated within the
// hashtable.  If the strings were not constant and fixed, this type would not work.  Also
// note that normally a hashtable (i.e. unordered_map) using a const char* key would simply
// compare the 32 bit or 64 bit pointers themselves, rather than the C strings which they
// are known to point to in this context.  I force the latter behavior by supplying both
// "hash" and "equality" overloads to the hashtable (unordered_map) template.
typedef std::unordered_map< const char*, int, fnv_1a, iequal_to > KEYWORD_MAP;


#endif // HASHTABLES_H_
