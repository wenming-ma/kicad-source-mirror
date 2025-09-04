
// QT_TRANSFORMATION_COMPLETED

#ifndef UTF8_H_
#define UTF8_H_

#include <iostream>
#include <string>
#include <QString>

#if defined(DEBUG)
 #define UTF8_VERIFY
#endif

bool IsUTF8( const char* aString );


#if defined(UTF8_VERIFY)
 #define MAYBE_VERIFY_UTF8(x)       Q_ASSERT( IsUTF8(x) )
#else
 #define MAYBE_VERIFY_UTF8(x)       // nothing
#endif


class UTF8
{
public:
    UTF8( const QString& o );

    UTF8( const char* txt ) :
        m_s( txt )
    {
        MAYBE_VERIFY_UTF8( c_str() );
    }

    UTF8( const wchar_t* txt );

    UTF8( const std::string& o ) :
        m_s( o )
    {
        MAYBE_VERIFY_UTF8( c_str() );
    }

    UTF8()
    {
    }

    ~UTF8()
    {
    }

    const char* c_str()                         const   { return m_s.c_str(); }
    bool empty()                                const   { return m_s.empty(); }

    std::string::size_type find( char c )       const   { return m_s.find( c ); }
    std::string::size_type find( char c, size_t s )     const   { return m_s.find( c, s ); }

    void clear()                                        { m_s.clear(); }
    std::string::size_type length()             const   { return m_s.length(); }
    std::string::size_type size()               const   { return m_s.size(); }
    int compare( const std::string& s )         const   { return m_s.compare( s ); }

    bool operator==( const UTF8& rhs )          const   { return m_s == rhs.m_s; }
    bool operator==( const std::string& rhs )   const   { return m_s == rhs; }
    bool operator==( const char* s )            const   { return m_s == s; }

    bool operator!=( const UTF8& rhs ) const { return !( operator==( rhs ) ); }
    bool operator<( const UTF8& rhs ) const { return m_s < rhs.m_s;  }
    bool operator>( const UTF8& rhs ) const { return m_s > rhs.m_s;  }

    std::string::size_type find_first_of( const std::string& str,
                                          std::string::size_type pos = 0 ) const
    {
        return m_s.find_first_of( str, pos );
    }

    UTF8& operator+=( const UTF8& str )
    {
        m_s += str.m_s;
        MAYBE_VERIFY_UTF8( c_str() );
        return *this;
    }

    UTF8& operator+=( char ch )
    {
        m_s.operator+=( ch );
        MAYBE_VERIFY_UTF8( c_str() );
        return *this;
    }

    UTF8& operator+=( const char* s )
    {
        m_s.operator+=( s );
        MAYBE_VERIFY_UTF8( c_str() );
        return *this;
    }

    UTF8& operator+=( unsigned w_ch );

    static constexpr std::string::size_type npos = -1;

    UTF8& operator=( const QString& o );

    UTF8& operator=( const std::string& o )
    {
        m_s = o;
        MAYBE_VERIFY_UTF8( c_str() );
        return *this;
    }

    UTF8& operator=( const char* s )
    {
        m_s = s;
        MAYBE_VERIFY_UTF8( c_str() );
        return *this;
    }

    UTF8& operator=( char c )
    {
        m_s = c;
        MAYBE_VERIFY_UTF8( c_str() );
        return *this;
    }

    std::string substr( size_t pos = 0, size_t len = npos ) const
    {
        return m_s.substr( pos, len );
    }

    operator const std::string& () const    { return m_s; }

    QString qt_str() const;
    operator QString () const;

    std::string::const_iterator begin()         const   { return m_s.begin(); }
    std::string::const_iterator end()           const   { return m_s.end(); }

#ifndef SWIG
    class uni_iter
    {
    public:
        uni_iter()
        {
            it = nullptr;
        }

        uni_iter( const uni_iter& o )
        {
            it = o.it;
        }

        const uni_iter& operator++()
        {
            it += uni_forward( it );
            return *this;
        }

        uni_iter operator++( int )
        {
            uni_iter ret = *this;

            it += uni_forward( it );
            return ret;
        }

        unsigned operator->() const
        {
            unsigned    result;

            uni_forward( it, &result );
            return result;
        }

        unsigned operator*() const
        {
            unsigned    result;

            uni_forward( it, &result );
            return result;
        }

        uni_iter operator-( int aVal ) const { return uni_iter( (char*) it - aVal ); }

        bool operator==( const uni_iter& other ) const  { return it == other.it; }
        bool operator!=( const uni_iter& other ) const  { return it != other.it; }

        bool operator< ( const uni_iter& other ) const  { return it <  other.it; }
        bool operator<=( const uni_iter& other ) const  { return it <= other.it; }
        bool operator> ( const uni_iter& other ) const  { return it >  other.it; }
        bool operator>=( const uni_iter& other ) const  { return it >= other.it; }

    private:
        friend class UTF8;

        const unsigned char* it;

        uni_iter( const char* start ) :
            it( (const unsigned char*) start )
        {
        }
    };

    uni_iter ubegin() const
    {
        return uni_iter( m_s.data() );
    }

    uni_iter uend() const
    {
        return uni_iter( m_s.data() + m_s.size() );
    }

    static int uni_forward( const unsigned char* aSequence, unsigned* aResult = nullptr );
#endif

protected:
    std::string m_s;
};


std::ostream& operator<<( std::ostream& aStream, const UTF8& aRhs );

#endif // UTF8_H_
