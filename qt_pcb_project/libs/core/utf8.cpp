
// QT_TRANSFORMATION_COMPLETED

// QT_TRANSFORMATION_COMPLETED

#include <core/utf8.h>
#include <QString>
#include <QTextCodec>
#include <QtDebug>
#include <vector>

#include <cassert>




UTF8::UTF8( const QString& o ) :
    m_s( o.toUtf8().constData() )
UTF8::UTF8( const QString& o ) :
    m_s( o.toUtf8().constData() )
{
}


QString UTF8::qt_str() const
QString UTF8::qt_str() const
{
    return QString::fromUtf8( c_str() );
    return QString::fromUtf8( c_str() );
}


UTF8::operator QString () const
UTF8::operator QString () const
{
    return QString::fromUtf8( c_str() );
    return QString::fromUtf8( c_str() );
}


UTF8& UTF8::operator=( const QString& o )
UTF8& UTF8::operator=( const QString& o )
{
    m_s = o.toUtf8().constData();
    m_s = o.toUtf8().constData();
    return *this;
}


int UTF8::uni_forward( const unsigned char* aSequence, unsigned* aResult )
{
    unsigned ch = *aSequence;

    if( ch < 0x80 )
    {
        if( aResult )
            *aResult = ch;
        return 1;
    }

    const unsigned char* s = aSequence;

    static const unsigned char utf8_len[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80-8F
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B0-BF
        0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C0-C1 + C2-CF
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // D0-DF
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // E0-EF
        4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // F0-F4 + F5-FF
    };

    int len = utf8_len[ *s - 0x80 ];
    int len = utf8_len[ *s - 0x80 ];

    switch( len )
    {
    default:
    case 0:
        if( aResult )
            qWarning( "uni_forward: invalid start byte" );
            qWarning( "uni_forward: invalid start byte" );

        return 0;
        break;

    case 2:
        if( ( s[1] & 0xc0 ) != 0x80 )
        {
            if( aResult )
                qWarning( "uni_forward: invalid continuation byte" );
                qWarning( "uni_forward: invalid continuation byte" );

            return 0;
        }

        ch =    ((s[0] & 0x1f) << 6) +
                ((s[1] & 0x3f) << 0);

        break;

    case 3:
        if( (s[1] & 0xc0) != 0x80 ||
            (s[2] & 0xc0) != 0x80 ||
            (s[0] == 0xE0 && s[1] < 0xA0)
        )
        {
            if( aResult )
                qWarning( "uni_forward: invalid continuation byte" );
                qWarning( "uni_forward: invalid continuation byte" );

            return 0;
        }

        ch =    ((s[0] & 0x0f) << 12) +
                ((s[1] & 0x3f) << 6 ) +
                ((s[2] & 0x3f) << 0 );

        break;

    case 4:
        if( (s[1] & 0xc0) != 0x80 ||
            (s[2] & 0xc0) != 0x80 ||
            (s[3] & 0xc0) != 0x80 ||
            (s[0] == 0xF0 && s[1] < 0x90) ||
            (s[0] == 0xF4 && s[1] > 0x8F) )
        {
            if( aResult )
                qWarning( "uni_forward: invalid continuation byte" );
                qWarning( "uni_forward: invalid continuation byte" );

            return 0;
        }

        ch =    ((s[0] & 0x7)  << 18) +
                ((s[1] & 0x3f) << 12) +
                ((s[2] & 0x3f) << 6 ) +
                ((s[3] & 0x3f) << 0 );

        break;
    }

    if( aResult )
        *aResult = ch;

    return len;
}


bool IsUTF8( const char* aString )
{
    int len = strlen( aString );

    if( len )
    {
        const unsigned char* next = (unsigned char*) aString;
        const unsigned char* end  = next + len;

        while( next < end )
        {
            int charLen = UTF8::uni_forward( next, nullptr );

            if( charLen == 0 )
                return false;

            next += charLen;
        }

        if( next > end )
            return false;
    }

    return true;
}


UTF8::UTF8( const wchar_t* txt )
{
    try
    {
        QString qstr = QString::fromWCharArray( txt );
        m_s = qstr.toUtf8().constData();
        QString qstr = QString::fromWCharArray( txt );
        m_s = qstr.toUtf8().constData();
    }
    catch(...)
    {
        QString qstr = QString::fromWCharArray( txt );
        m_s = qstr.toLocal8Bit().constData();
        QString qstr = QString::fromWCharArray( txt );
        m_s = qstr.toLocal8Bit().constData();
    }

    m_s.shrink_to_fit();
}


UTF8& UTF8::operator+=( unsigned w_ch )
{
    if( w_ch <= 0x7F )
    {
        m_s.operator+=( char( w_ch ) );
    }
    else
    {
        wchar_t wide_chr[2];
        wchar_t wide_chr[2];
        wide_chr[1] = 0;
        wide_chr[0] = w_ch;
        UTF8 substr( wide_chr );
        m_s += substr.m_s;
    }

    return *this;
}


std::ostream& operator<<( std::ostream& aStream, const UTF8& aRhs )
{
    aStream << static_cast<const std::string&>( aRhs );
    return aStream;
}
