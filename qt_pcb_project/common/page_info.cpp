
#include <page_info.h>
#include <macros.h>
#include <eda_units.h>
#include <richio.h>
#include <QPrinter>
#include <QPageSize>


// Qt paper size definitions
#define PAPER_A0   QPageSize::A0
#define PAPER_A1   QPageSize::A1


const char PAGE_INFO::A5[] = "A5";
const char PAGE_INFO::A4[] = "A4";
const char PAGE_INFO::A3[] = "A3";
const char PAGE_INFO::A2[] = "A2";
const char PAGE_INFO::A1[] = "A1";
const char PAGE_INFO::A0[] = "A0";
const char PAGE_INFO::A[]  = "A";
const char PAGE_INFO::B[]  = "B";
const char PAGE_INFO::C[]  = "C";
const char PAGE_INFO::D[]  = "D";
const char PAGE_INFO::E[]  = "E";

const char PAGE_INFO::GERBER[]   = "GERBER";
const char PAGE_INFO::USLetter[] = "USLetter";
const char PAGE_INFO::USLegal[]  = "USLegal";
const char PAGE_INFO::USLedger[] = "USLedger";
const char PAGE_INFO::Custom[]   = "User";


// Standard page sizes in mils, all constants
// see:  https://lists.launchpad.net/kicad-developers/msg07389.html
// also see: wx/defs.h

// local readability macro for millimeter size
#define MMsize( x, y ) VECTOR2D( EDA_UNIT_UTILS::Mm2mils( x ), EDA_UNIT_UTILS::Mm2mils( y ) )

// All MUST be defined as landscape.
const PAGE_INFO  PAGE_INFO::pageA5(     MMsize( 210,   148 ),   "A5",    QPageSize::A5 );
const PAGE_INFO  PAGE_INFO::pageA4(     MMsize( 297,   210 ),   "A4",    QPageSize::A4 );
const PAGE_INFO  PAGE_INFO::pageA3(     MMsize( 420,   297 ),   "A3",    QPageSize::A3 );
const PAGE_INFO  PAGE_INFO::pageA2(     MMsize( 594,   420 ),   "A2",    QPageSize::A2 );
const PAGE_INFO  PAGE_INFO::pageA1(     MMsize( 841,   594 ),   "A1",    PAPER_A1 );
const PAGE_INFO  PAGE_INFO::pageA0(     MMsize( 1189,  841 ),   "A0",    PAPER_A0 );

const PAGE_INFO  PAGE_INFO::pageA( VECTOR2D( 11000, 8500 ), "A", QPageSize::Letter );
const PAGE_INFO  PAGE_INFO::pageB( VECTOR2D( 17000, 11000 ), "B", QPageSize::Tabloid );
const PAGE_INFO  PAGE_INFO::pageC( VECTOR2D( 22000, 17000 ), "C", QPageSize::Custom );
const PAGE_INFO  PAGE_INFO::pageD( VECTOR2D( 34000, 22000 ), "D", QPageSize::Custom );
const PAGE_INFO  PAGE_INFO::pageE( VECTOR2D( 44000, 34000 ), "E", QPageSize::Custom );

const PAGE_INFO PAGE_INFO::pageGERBER( VECTOR2D( 32000, 32000 ), "GERBER", QPageSize::Custom );
const PAGE_INFO PAGE_INFO::pageUser( VECTOR2D( 17000, 11000 ), Custom, QPageSize::Custom );

// US paper sizes
const PAGE_INFO  PAGE_INFO::pageUSLetter( VECTOR2D( 11000, 8500  ),  "USLetter",
                                          QPageSize::Letter );
const PAGE_INFO PAGE_INFO::pageUSLegal( VECTOR2D( 14000, 8500 ), "USLegal", QPageSize::Legal );
const PAGE_INFO  PAGE_INFO::pageUSLedger( VECTOR2D( 17000, 11000 ), "USLedger",
                                          QPageSize::Tabloid );

// Custom paper size for next instantiation of type "User"
double PAGE_INFO::s_user_width  = 17000;
double PAGE_INFO::s_user_height = 11000;


inline void PAGE_INFO::updatePortrait()
{
    // update m_portrait based on orientation of m_size.x and m_size.y
    m_portrait = ( m_size.y > m_size.x );
}


PAGE_INFO::PAGE_INFO( const VECTOR2D& aSizeMils, const QString& aType, QPageSize::PageSizeId aPaperId ) :
    m_type( aType ), m_size( aSizeMils ), m_paper_id( aPaperId )
{
    updatePortrait();

    // This constructor is protected, and only used by const PAGE_INFO's known
    // only to class implementation, so no further changes to "this" object are
    // expected.
}


PAGE_INFO::PAGE_INFO( const QString& aType, bool aIsPortrait )
{
    SetType( aType, aIsPortrait );
}


bool PAGE_INFO::SetType( const QString& aType, bool aIsPortrait )
{
    bool rc = true;

    // all are landscape initially
    if( aType == pageA5.GetType() )
        *this = pageA5;
    else if( aType == pageA4.GetType() )
        *this = pageA4;
    else if( aType == pageA3.GetType() )
        *this = pageA3;
    else if( aType == pageA2.GetType() )
        *this = pageA2;
    else if( aType == pageA1.GetType() )
        *this = pageA1;
    else if( aType == pageA0.GetType() )
        *this = pageA0;
    else if( aType == pageA.GetType() )
        *this = pageA;
    else if( aType == pageB.GetType() )
        *this = pageB;
    else if( aType == pageC.GetType() )
        *this = pageC;
    else if( aType == pageD.GetType() )
        *this = pageD;
    else if( aType == pageE.GetType() )
        *this = pageE;
    else if( aType == pageGERBER.GetType() )
        *this = pageGERBER;
    else if( aType == pageUSLetter.GetType() )
        *this = pageUSLetter;
    else if( aType == pageUSLegal.GetType() )
        *this = pageUSLegal;
    else if( aType == pageUSLedger.GetType() )
        *this = pageUSLedger;
    else if( aType == pageUser.GetType() )
    {
        // pageUser is const, and may not and does not hold the custom size,
        // so customize *this later
        *this  = pageUser;

        // customize:
        m_size.x = s_user_width;
        m_size.y = s_user_height;

        updatePortrait();
    }
    else
        rc = false;

    if( aIsPortrait )
    {
        // all private PAGE_INFOs are landscape, must swap x and y
        std::swap( m_size.y, m_size.x );
        updatePortrait();
    }

    return rc;
}


bool PAGE_INFO::IsCustom() const
{
    return m_type == Custom;
}


void PAGE_INFO::SetPortrait( bool aIsPortrait )
{
    if( m_portrait != aIsPortrait )
    {
        // swap x and y in m_size
        std::swap( m_size.y, m_size.x );

        m_portrait = aIsPortrait;

        // margins are not touched, do that if you want
    }
}


static double clampWidth( double aWidthInMils )
{
/*  was giving EESCHEMA single component SVG plotter grief
    However a minimal test is made to avoid values that crashes KiCad
    if( aWidthInMils < 4000 )       // 4" is about a baseball card
        aWidthInMils = 4000;
    else if( aWidthInMils > 44000 ) //44" is plotter size
        aWidthInMils = 44000;
*/
    if( aWidthInMils < 10 )
        aWidthInMils = 10;

    return aWidthInMils;
}


static double clampHeight( double aHeightInMils )
{
/*  was giving EESCHEMA single component SVG plotter grief
    clamping is best done at the UI, i.e. dialog, levels
    However a minimal test is made to avoid values that crashes KiCad
    if( aHeightInMils < 4000 )
        aHeightInMils = 4000;
    else if( aHeightInMils > 44000 )
        aHeightInMils = 44000;
*/
    if( aHeightInMils < 10.0 )
        aHeightInMils = 10.0;

    return aHeightInMils;
}


void PAGE_INFO::SetCustomWidthMils( double aWidthInMils )
{
    s_user_width = clampWidth( aWidthInMils );
}


void PAGE_INFO::SetCustomHeightMils( double aHeightInMils )
{
    s_user_height = clampHeight( aHeightInMils );
}


void PAGE_INFO::SetWidthMils( double aWidthInMils )
{
    if( m_size.x != aWidthInMils )
    {
        m_size.x = clampWidth( aWidthInMils );

        m_type = Custom;
        m_paper_id = QPageSize::Custom;

        updatePortrait();
    }
}


void PAGE_INFO::SetHeightMils( double aHeightInMils )
{
    if( m_size.y != aHeightInMils )
    {
        m_size.y = clampHeight( aHeightInMils );

        m_type = Custom;
        m_paper_id = QPageSize::Custom;

        updatePortrait();
    }
}


void PAGE_INFO::Format( OUTPUTFORMATTER* aFormatter ) const
{
    aFormatter->Print( "(paper %s", aFormatter->Quotew( GetType() ).c_str() );

    // The page dimensions are only required for user defined page sizes.
    // Internally, the page size is in mils
    if( GetType() == PAGE_INFO::Custom )
    {
        aFormatter->Print( " %g %g",
                           GetWidthMils() * 25.4 / 1000.0,
                           GetHeightMils() * 25.4 / 1000.0 );
    }

    if( !IsCustom() && IsPortrait() )
        aFormatter->Print( " portrait" );

    aFormatter->Print( ")" );
}
