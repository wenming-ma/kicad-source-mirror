
#ifndef _LIB_ID_H_
#define _LIB_ID_H_

#include <kicommon.h>
#include <core/utf8.h>
#include <QString>

class KICOMMON_API LIB_ID
{
public:
    LIB_ID() {}

    // NOTE: don't define any constructors which call Parse() on their arguments.  We want it
    // to be obvious to callers that parsing is involved (and that valid IDs are guaranteed in
    // the presence of disallowed characters, malformed ids, etc.).

    LIB_ID( const QString& aLibraryName, const QString& aItemName );

    int Parse( const UTF8& aId, bool aFix = false );

    const UTF8& GetLibNickname() const { return m_libraryName; }
    const QString GetUniStringLibNickname() const { return QString::fromStdString(m_libraryName); }

    int SetLibNickname( const UTF8& aLibNickname );

    const UTF8& GetLibItemName() const { return m_itemName; }

    const QString GetUniStringLibItemName() const { return QString::fromStdString(m_itemName); }

    int SetLibItemName( const UTF8& aLibItemName );

    UTF8 GetSubLibraryName() const { return m_subLibraryName; }
    void SetSubLibraryName( const UTF8& aName ) { m_subLibraryName = aName; }
    const QString GetUniStringSubLibraryName() const { return QString::fromStdString(m_subLibraryName); }

    const QString GetFullLibraryName() const;

    UTF8 Format() const;

    QString GetUniStringLibId() const
    {
        return QString::fromStdString(Format());
    }

    static UTF8 Format( const UTF8& aLibraryName, const UTF8& aLibItemName );

    bool IsValid() const
    {
        return !m_libraryName.empty() && !m_itemName.empty();
    }

    bool IsLegacy() const
    {
        return m_libraryName.empty() && !m_itemName.empty();
    }

    void clear();

    bool empty() const
    {
        return m_libraryName.empty() && m_itemName.empty();
    }

    int compare( const LIB_ID& aLibId ) const;

    bool operator < ( const LIB_ID& aLibId ) const { return this->compare( aLibId ) < 0; }
    bool operator > ( const LIB_ID& aLibId ) const { return this->compare( aLibId ) > 0; }
    bool operator ==( const LIB_ID& aLibId ) const { return this->compare( aLibId ) == 0; }
    bool operator !=( const LIB_ID& aLibId ) const { return !(*this == aLibId); }

    static int HasIllegalChars( const UTF8& aLibItemName );

    static UTF8 FixIllegalChars( const UTF8& aLibItemName, bool aLib );

    static unsigned FindIllegalLibraryNameChar( const UTF8& aLibraryName );

protected:
    static bool isLegalChar( unsigned aUniChar );

    static bool isLegalLibraryNameChar( unsigned aUniChar );

    UTF8    m_libraryName;
    UTF8    m_itemName;
    UTF8    m_subLibraryName;
};


#endif // _LIB_ID_H_
