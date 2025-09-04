
#include <algorithm>

#include <bitmaps.h>
#include <eda_item.h>
#include <trace_helpers.h>
#include <trigo.h>
#include <i18n_utility.h>
#include <QRegExp>
#include <QDebug>
#include <QVector>
#include <eda_pattern_match.h>

EDA_ITEM::EDA_ITEM( EDA_ITEM* parent, KICAD_T idType, bool isSCH_ITEM, bool isBOARD_ITEM ) :
        KIGFX::VIEW_ITEM( isSCH_ITEM, isBOARD_ITEM ),
        m_structType( idType ),
        m_flags( 0 ),
        m_parent( parent ),
        m_forceVisible( false ),
        m_isRollover( false )
{ }


EDA_ITEM::EDA_ITEM( KICAD_T idType, bool isSCH_ITEM, bool isBOARD_ITEM ) :
        KIGFX::VIEW_ITEM( isSCH_ITEM, isBOARD_ITEM ),
        m_structType( idType ),
        m_flags( 0 ),
        m_parent( nullptr ),
        m_forceVisible( false ),
        m_isRollover( false )
{ }


EDA_ITEM::EDA_ITEM( const EDA_ITEM& base ) :
        KIGFX::VIEW_ITEM( base.IsSCH_ITEM(), base.IsBOARD_ITEM() ),
        m_Uuid( base.m_Uuid ),
        m_structType( base.m_structType ),
        m_flags( base.m_flags ),
        m_parent( base.m_parent ),
        m_forceVisible( base.m_forceVisible ),
        m_isRollover( false )
{
    SetForcedTransparency( base.GetForcedTransparency() );
}


void EDA_ITEM::SetModified()
{
    SetFlags( IS_CHANGED );

    // If this a child object, then the parent modification state also needs to be set.
    if( m_parent )
        m_parent->SetModified();
}


const BOX2I EDA_ITEM::GetBoundingBox() const
{
    // return a zero-sized box per default. derived classes should override
    // this
    return BOX2I( VECTOR2I( 0, 0 ), VECTOR2I( 0, 0 ) );
}


EDA_ITEM* EDA_ITEM::Clone() const
{
    Q_ASSERT_X( false, "EDA_ITEM::Clone", ("Clone not implemented in derived class " + GetClass() +
                 ".  Bad programmer!").toLocal8Bit().data() );
    return nullptr;
}


// see base_struct.h
// many classes inherit this method, be careful:
INSPECT_RESULT EDA_ITEM::Visit( INSPECTOR inspector, void* testData,
                                const std::vector<KICAD_T>& aScanTypes )
{
#if 0 && defined(DEBUG)
    std::cout << GetClass().mb_str() << ' ';
#endif

    if( IsType( aScanTypes ) )
    {
        if( INSPECT_RESULT::QUIT == inspector( this, testData ) )
            return INSPECT_RESULT::QUIT;
    }

    return INSPECT_RESULT::CONTINUE;
}


QString EDA_ITEM::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    Q_ASSERT_X( false, "EDA_ITEM::GetItemDescription", ("GetItemDescription() was not overridden for schematic item type " +
                GetClass()).toLocal8Bit().data() );

    return QString( "Undefined item description for " ) + GetClass();
}


bool isWordChar( const QChar& c )
{
    return c.isLetterOrNumber() || c == '_';
}


bool EDA_ITEM::Matches( const QString& aText, const EDA_SEARCH_DATA& aSearchData ) const
{
    QString text = aText;
    QString searchText = aSearchData.findString;

    // Don't match if searching for replaceable item and the item doesn't support text replace.
    if( aSearchData.searchAndReplace && !IsReplaceable() )
        return false;

    if( !aSearchData.matchCase )
    {
        text = text.toUpper();
        searchText = searchText.toUpper();
    }

    auto isWordChar =
            []( const QChar& c )
            {
                return c.isLetterOrNumber() || c == '_';
            };

    if( aSearchData.matchMode == EDA_SEARCH_MATCH_MODE::PERMISSIVE )
    {
        EDA_COMBINED_MATCHER matcher( searchText, CTX_SEARCH );

        return matcher.Find( text );
    }
    else if( aSearchData.matchMode == EDA_SEARCH_MATCH_MODE::WHOLEWORD )
    {
        int ii = 0;

        while( ii < (int) text.length() )
        {
            int next = text.indexOf( searchText, ii );

            if( next == -1 )
                return false;

            ii = next;
            next += searchText.length();

            bool startOK = ( ii == 0 || !isWordChar( text.at( ii - 1 ) ) );
            bool endOK = ( next == (int) text.length() || !isWordChar( text.at( next ) ) );

            if( startOK && endOK )
                return true;
            else
                ii++;
        }

        return false;
    }
    else if( aSearchData.matchMode == EDA_SEARCH_MATCH_MODE::WILDCARD )
    {
        return QRegExp( searchText, Qt::CaseSensitive, QRegExp::Wildcard ).exactMatch( text );
    }
    else if( aSearchData.matchMode == EDA_SEARCH_MATCH_MODE::REGEX )
    {
        if( aSearchData.regex_string != searchText || !aSearchData.regex.isValid() )
        {
            Qt::CaseSensitivity caseSensitive = aSearchData.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
            aSearchData.regex.setPattern( searchText );
            aSearchData.regex.setCaseSensitivity( caseSensitive );
            
            if( !aSearchData.regex.isValid() )
                return false;

            aSearchData.regex_string = searchText;
        }

        return aSearchData.regex.indexIn( text ) != -1;
    }
    else
    {
        return text.indexOf( searchText ) != -1;
    }
}


bool EDA_ITEM::Replace( const EDA_SEARCH_DATA& aSearchData, QString& aText )
{
    QString text = aText;
    QString searchText = aSearchData.findString;
    QString result;
    bool     replaced = false;

    if( aSearchData.matchMode == EDA_SEARCH_MATCH_MODE::REGEX )
    {
        if( aSearchData.regex_string != searchText || !aSearchData.regex.isValid() )
        {
            Qt::CaseSensitivity caseSensitive = aSearchData.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive;
            aSearchData.regex.setPattern( searchText );
            aSearchData.regex.setCaseSensitivity( caseSensitive );
            
            if( !aSearchData.regex.isValid() )
                return false;

            aSearchData.regex_string = searchText;
        }

        QString newText = text.replace( aSearchData.regex, aSearchData.replaceString );
        if( newText == text )
            return false;
            
        aText = newText;
        return true;
    }

    if( !aSearchData.matchCase )
    {
        text = text.toUpper();
        searchText = searchText.toUpper();
    }

    int ii = 0;

    while( ii < (int) text.length() )
    {
        int next = text.indexOf( searchText, ii );

        if( next == -1 )
        {
            result += aText.mid( ii );
            break;
        }

        if( next > ii )
            result += aText.mid( ii, next - ii );

        ii = next;
        next += searchText.length();

        bool startOK;
        bool endOK;

        if( aSearchData.matchMode == EDA_SEARCH_MATCH_MODE::WHOLEWORD )
        {
            startOK = ( ii == 0 || !isWordChar( text.at( ii - 1 ) ) );
            endOK = ( next == (int) text.length() || !isWordChar( text.at( next ) ) );
        }
        else
        {
            startOK = true;
            endOK = true;
        }

        if( startOK && endOK )
        {
            result += aSearchData.replaceString;
            replaced = true;
            ii = next;
        }
        else
        {
            result += aText.at( ii );
            ii++;
        }
    }

    aText = result;
    return replaced;
}


bool EDA_ITEM::operator<( const EDA_ITEM& aItem ) const
{
    Q_ASSERT_X( false, "EDA_ITEM::operator<", QString( "Less than operator not defined for item type %1." )
                                  .arg( GetClass() ).toLocal8Bit().data() );

    return false;
}


EDA_ITEM& EDA_ITEM::operator=( const EDA_ITEM& aItem )
{
    // do not call initVars()

    m_structType   = aItem.m_structType;
    m_flags        = aItem.m_flags;
    m_parent       = aItem.m_parent;
    m_forceVisible = aItem.m_forceVisible;

    SetForcedTransparency( aItem.GetForcedTransparency() );

    return *this;
}


const BOX2I EDA_ITEM::ViewBBox() const
{
    // Basic fallback
    return GetBoundingBox();
}


QVector<int> EDA_ITEM::ViewGetLayers() const
{
    // Basic fallback
    QVector<int> layers{ 1 };
    return layers;
}


BITMAPS EDA_ITEM::GetMenuImage() const
{
    return BITMAPS::dummy_item;
}


#if defined( DEBUG )

void EDA_ITEM::ShowDummy( std::ostream& os ) const
{
    QString s = GetClass();

    os << '<' << s.toLower().toLocal8Bit().data() << ">"
       << " Need ::Show() override for this class "
       << "</" << s.toLower().toLocal8Bit().data() << ">\n";
}


std::ostream& EDA_ITEM::NestedSpace( int nestLevel, std::ostream& os )
{
    for( int i = 0; i<nestLevel; ++i )
        os << "  ";

    // number of spaces here controls indent per nest level

    return os;
}

#endif


QString EDA_ITEM::GetTypeDesc() const
{
    //@see EDA_ITEM_DESC for definition of ENUM_MAP<KICAD_T>
    QString typeDescr = ENUM_MAP<KICAD_T>::Instance().ToString( Type() );

    return QObject::tr( typeDescr.toLocal8Bit().data() );
}


QString EDA_ITEM::GetFriendlyName() const
{
    return GetTypeDesc();
}


static struct EDA_ITEM_DESC
{
    EDA_ITEM_DESC()
    {
        ENUM_MAP<KICAD_T>::Instance()
            .Undefined( TYPE_NOT_INIT )
            .Map( NOT_USED,                QString( "<not used>" ) )
            .Map( SCREEN_T,                _HKI( "Screen" ) )

            .Map( PCB_FOOTPRINT_T,         _HKI( "Footprint" ) )
            .Map( PCB_PAD_T,               _HKI( "Pad" ) )
            .Map( PCB_SHAPE_T,             _HKI( "Graphic" ) )
            .Map( PCB_REFERENCE_IMAGE_T,   _HKI( "Reference Image" ) )
            .Map( PCB_GENERATOR_T,         _HKI( "Generator" ) )
            .Map( PCB_FIELD_T,             _HKI( "Text" ) )
            .Map( PCB_TEXT_T,              _HKI( "Text" ) )
            .Map( PCB_TEXTBOX_T,           _HKI( "Text Box" ) )
            .Map( PCB_TABLE_T,             _HKI( "Table" ) )
            .Map( PCB_TABLECELL_T,         _HKI( "Table Cell" ) )
            .Map( PCB_TRACE_T,             _HKI( "Track" ) )
            .Map( PCB_ARC_T,               _HKI( "Track" ) )
            .Map( PCB_VIA_T,               _HKI( "Via" ) )
            .Map( PCB_MARKER_T,            _HKI( "Marker" ) )
            .Map( PCB_DIM_ALIGNED_T,       _HKI( "Dimension" ) )
            .Map( PCB_DIM_ORTHOGONAL_T,    _HKI( "Dimension" ) )
            .Map( PCB_DIM_CENTER_T,        _HKI( "Dimension" ) )
            .Map( PCB_DIM_RADIAL_T,        _HKI( "Dimension" ) )
            .Map( PCB_DIM_LEADER_T,        _HKI( "Leader" ) )
            .Map( PCB_TARGET_T,            _HKI( "Target" ) )
            .Map( PCB_ZONE_T,              _HKI( "Zone" ) )
            .Map( PCB_ITEM_LIST_T,         _HKI( "ItemList" ) )
            .Map( PCB_NETINFO_T,           _HKI( "NetInfo" ) )
            .Map( PCB_GROUP_T,             _HKI( "Group" ) )

            .Map( SCH_MARKER_T,            _HKI( "Marker" ) )
            .Map( SCH_JUNCTION_T,          _HKI( "Junction" ) )
            .Map( SCH_NO_CONNECT_T,        _HKI( "No-Connect Flag" ) )
            .Map( SCH_BUS_WIRE_ENTRY_T,    _HKI( "Wire Entry" ) )
            .Map( SCH_BUS_BUS_ENTRY_T,     _HKI( "Bus Entry" ) )
            .Map( SCH_LINE_T,              _HKI( "Line" ) )
            .Map( SCH_BITMAP_T,            _HKI( "Bitmap" ) )
            .Map( SCH_SHAPE_T,             _HKI( "Graphic" ) )
            .Map( SCH_RULE_AREA_T,         _HKI( "Rule Area" ) )
            .Map( SCH_TEXT_T,              _HKI( "Text" ) )
            .Map( SCH_TEXTBOX_T,           _HKI( "Text Box" ) )
            .Map( SCH_TABLE_T,             _HKI( "Table" ) )
            .Map( SCH_TABLECELL_T,         _HKI( "Table Cell" ) )
            .Map( SCH_LABEL_T,             _HKI( "Net Label" ) )
            .Map( SCH_DIRECTIVE_LABEL_T,   _HKI( "Directive Label" ) )
            .Map( SCH_GLOBAL_LABEL_T,      _HKI( "Global Label" ) )
            .Map( SCH_HIER_LABEL_T,        _HKI( "Hierarchical Label" ) )
            .Map( SCH_FIELD_T,             _HKI( "Field" ) )
            .Map( SCH_SYMBOL_T,            _HKI( "Symbol" ) )
            .Map( SCH_PIN_T,               _HKI( "Pin" ) )
            .Map( SCH_SHEET_PIN_T,         _HKI( "Sheet Pin" ) )
            .Map( SCH_SHEET_T,             _HKI( "Sheet" ) )

            // Synthetic search tokens don't need to be included...
            //.Map( SCH_FIELD_LOCATE_REFERENCE_T, _HKI( "Field Locate Reference" ) )
            //.Map( SCH_FIELD_LOCATE_VALUE_T,     _HKI( "Field Locate Value" ) )
            //.Map( SCH_FIELD_LOCATE_FOOTPRINT_T, _HKI( "Field Locate Footprint" ) )

            .Map( SCH_SCREEN_T,            _HKI( "SCH Screen" ) )

            .Map( LIB_SYMBOL_T,            _HKI( "Symbol" ) )

            .Map( GERBER_LAYOUT_T,         _HKI( "Gerber Layout" ) )
            .Map( GERBER_DRAW_ITEM_T,      _HKI( "Draw Item" ) )
            .Map( GERBER_IMAGE_T,          _HKI( "Image" ) );

        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        REGISTER_TYPE( EDA_ITEM );

        propMgr.AddProperty( new PROPERTY_ENUM<EDA_ITEM, KICAD_T>( QString( "Type" ),
                             NO_SETTER( EDA_ITEM, KICAD_T ), &EDA_ITEM::Type ) )
                .SetIsHiddenFromPropertiesManager();
    }
} _EDA_ITEM_DESC;

ENUM_TO_WXANY( KICAD_T );
