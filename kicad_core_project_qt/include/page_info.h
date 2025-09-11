// QT_TRANSFORMATION_COMPLETED

#ifndef PAGE_INFO_H
#define PAGE_INFO_H

#include <kicommon.h>
#include <QString>
#include <math/vector2d.h>

// Qt includes for paper size and orientation
#include <QPrinter>
#include <QPageSize>
#include <QPageLayout>

/// Min and max page sizes for clamping, in mils.
#define MIN_PAGE_SIZE_MILS          1000
#define MAX_PAGE_SIZE_PCBNEW_MILS   48000
#define MAX_PAGE_SIZE_EESCHEMA_MILS 120000

/// Min and max page sizes for clamping, in mm.
#define MIN_PAGE_SIZE_MM            25.4
#define MAX_PAGE_SIZE_PCBNEW_MM     48000*.0254
#define MAX_PAGE_SIZE_EESCHEMA_MM   120000*.0254

class OUTPUTFORMATTER;

// Describe the page size and margins of a paper page on which to eventually print or plot.
class KICOMMON_API PAGE_INFO
{
public:
    PAGE_INFO( const QString& aType = PAGE_INFO::A3, bool IsPortrait = false );

    // paper size names which are part of the public API, pass to SetType() or
    // above constructor.

    // these were once wxStrings, but it caused static construction sequence problems:
    static const char A5[];
    static const char A4[];
    static const char A3[];
    static const char A2[];
    static const char A1[];
    static const char A0[];
    static const char A[];
    static const char B[];
    static const char C[];
    static const char D[];
    static const char E[];
    static const char GERBER[];
    static const char USLetter[];
    static const char USLegal[];
    static const char USLedger[];
    static const char Custom[];     // "User" defined page type


    bool SetType( const QString& aStandardPageDescriptionName, bool aIsPortrait = false );
    const QString& GetType() const { return m_type; }

    // Return True if the object has the default page settings which are A3, landscape.
    bool IsDefault() const { return m_type == PAGE_INFO::A3 && !m_portrait; }

    // Return true if the type is Custom.
    bool IsCustom() const;

    // Rotate the paper page 90 degrees.
    void SetPortrait( bool aIsPortrait );
    bool IsPortrait() const { return m_portrait; }

    // Return Qt style printing orientation.
    QPageLayout::Orientation  GetQtOrientation() const { return IsPortrait() ? QPageLayout::Portrait : QPageLayout::Landscape; }

    // Return Qt style paper size associated with page type name.
    QPageSize::PageSizeId GetPaperId() const { return m_paper_id; }

    void SetWidthMM(  double aWidthInMM ) { SetWidthMils( aWidthInMM * 1000 / 25.4 ); }
    void SetWidthMils(  double aWidthInMils );
    double GetWidthMils() const { return m_size.x; }
    double GetWidthMM() const { return m_size.x * 25.4 / 1000; }

    void SetHeightMM( double aHeightInMM ) { SetHeightMils( aHeightInMM * 1000 / 25.4 ); }
    void SetHeightMils( double aHeightInMils );
    double GetHeightMils() const { return m_size.y; }
    double GetHeightMM() const { return m_size.y * 25.4 / 1000; }

    const VECTOR2D& GetSizeMils() const { return m_size; }

    // Gets the page width in IU
    int GetWidthIU( double aIUScale ) const { return aIUScale * GetWidthMils(); }

    // Gets the page height in IU
    int GetHeightIU( double aIUScale ) const { return aIUScale * GetHeightMils(); }

    // Gets the page size in internal units
    const VECTOR2D GetSizeIU( double aIUScale ) const
    {
        return VECTOR2D( GetWidthIU( aIUScale ), GetHeightIU( aIUScale ) );
    }

    // Set the width of Custom page in mils for any custom page constructed or made via SetType() after making this call.
    static void SetCustomWidthMils( double aWidthInMils );

    // Set the height of Custom page in mils for any custom page constructed or made via SetType() after making this call.
    static void SetCustomHeightMils( double aHeightInMils );

    // Return custom paper width in mils.
    static double GetCustomWidthMils() { return s_user_width; }

    // Return custom paper height in mils.
    static double GetCustomHeightMils() { return s_user_height; }

    // Output the page class to aFormatter in s-expression form.
    void Format( OUTPUTFORMATTER* aFormatter ) const;

protected:
    // only the class implementation(s) may use this constructor
    PAGE_INFO( const VECTOR2D& aSizeMils, const QString& aName, QPageSize::PageSizeId aPaperId );

private:
    // standard pre-defined sizes
    static const PAGE_INFO pageA5;
    static const PAGE_INFO pageA4;
    static const PAGE_INFO pageA3;
    static const PAGE_INFO pageA2;
    static const PAGE_INFO pageA1;
    static const PAGE_INFO pageA0;
    static const PAGE_INFO pageA;
    static const PAGE_INFO pageB;
    static const PAGE_INFO pageC;
    static const PAGE_INFO pageD;
    static const PAGE_INFO pageE;
    static const PAGE_INFO pageGERBER;

    static const PAGE_INFO pageUSLetter;
    static const PAGE_INFO pageUSLegal;
    static const PAGE_INFO pageUSLedger;

    static const PAGE_INFO pageUser;

    // all dimensions here are in mils

    QString     m_type;             // paper type: A4, A3, etc.
    VECTOR2D    m_size;             // mils

    bool        m_portrait;         // true if portrait, false if landscape

    QPageSize::PageSizeId m_paper_id;  // Qt style paper id.

    static double s_user_height;
    static double s_user_width;

    void    updatePortrait();

    void    setMargins();
};

#endif  // PAGE_INFO_H
