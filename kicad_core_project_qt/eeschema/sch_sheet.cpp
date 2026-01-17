
#include <cstdlib>

#include <bitmaps.h>
#include <core/mirror.h>
#include <core/kicad_algo.h>
#include <sch_draw_panel.h>
#include <trigo.h>
#include <sch_edit_frame.h>
#include <plotters/plotter.h>
#include <sch_plotter.h>
#include <string_utils.h>
#include <widgets/msgpanel.h>
#include <math/util.h>      // for KiROUND
#include <sch_sheet.h>
#include <sch_sheet_path.h>
#include <sch_sheet_pin.h>
#include <sch_symbol.h>
#include <sch_painter.h>
#include <schematic.h>
#include <settings/color_settings.h>
#include <settings/settings_manager.h>
#include <trace_helpers.h>
#include <pgm_base.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <i18n_utility.h>
#include <gr_basic.h>

// N.B. Do not change these values without transitioning the file format
#define SHEET_NAME_CANONICAL "Sheetname"
#define SHEET_FILE_CANONICAL "Sheetfile"
#define USER_FIELD_CANONICAL "Field%d"

static QString s_CanonicalSheetName( SHEET_NAME_CANONICAL );
static QString s_CanonicalSheetFile( SHEET_FILE_CANONICAL );

const QString SCH_SHEET::GetDefaultFieldName( int aFieldNdx, bool aTranslated )
{
    if( !aTranslated )
    {
        switch( aFieldNdx )
        {
        case  SHEETNAME:     return s_CanonicalSheetName;
        case  SHEETFILENAME: return s_CanonicalSheetFile;
        default:             return QString( USER_FIELD_CANONICAL ).arg( aFieldNdx );
        }
    }
    else
    {
        switch( aFieldNdx )
        {
        case  SHEETNAME:     return _( SHEET_NAME_CANONICAL );
        case  SHEETFILENAME: return _( SHEET_FILE_CANONICAL );
        default:             return QString( _( USER_FIELD_CANONICAL ) ).arg( aFieldNdx );
        }
    }
}


SCH_SHEET::SCH_SHEET( EDA_ITEM* aParent, const VECTOR2I& aPos, VECTOR2I aSize ) :
        SCH_ITEM( aParent, SCH_SHEET_T ),
        m_excludedFromSim( false ),
        m_excludedFromBOM( false ),
        m_excludedFromBoard( false ),
        m_DNP( false )
{
    m_layer = LAYER_SHEET;
    m_pos = aPos;
    m_size = aSize;
    m_screen = nullptr;

    m_borderWidth = 0;
    m_borderColor = COLOR4D::UNSPECIFIED;
    m_backgroundColor = COLOR4D::UNSPECIFIED;
    m_fieldsAutoplaced = AUTOPLACE_AUTO;

    m_fields.emplace_back( aPos, SHEETNAME, this, GetDefaultFieldName( SHEETNAME, DO_TRANSLATE ) );
    m_fields.back().SetVisible( true );
    m_fields.back().SetLayer( LAYER_SHEETNAME );

    m_fields.emplace_back( aPos, SHEETFILENAME, this,
                           GetDefaultFieldName( SHEETFILENAME, DO_TRANSLATE ) );
    m_fields.back().SetVisible( true );
    m_fields.back().SetLayer( LAYER_SHEETFILENAME );

    AutoplaceFields( nullptr, m_fieldsAutoplaced );
}


SCH_SHEET::SCH_SHEET( const SCH_SHEET& aSheet ) :
    SCH_ITEM( aSheet )
{
    m_pos = aSheet.m_pos;
    m_size = aSheet.m_size;
    m_layer = aSheet.m_layer;
    const_cast<KIID&>( m_Uuid ) = aSheet.m_Uuid;
    m_fields = aSheet.m_fields;
    m_fieldsAutoplaced = aSheet.m_fieldsAutoplaced;
    m_screen = aSheet.m_screen;

    m_excludedFromSim = aSheet.m_excludedFromSim;
    m_excludedFromBOM = aSheet.m_excludedFromBOM;
    m_excludedFromBoard = aSheet.m_excludedFromBoard;
    m_DNP = aSheet.m_DNP;

    m_borderWidth = aSheet.m_borderWidth;
    m_borderColor = aSheet.m_borderColor;
    m_backgroundColor = aSheet.m_backgroundColor;
    m_instances = aSheet.m_instances;

    for( SCH_SHEET_PIN* pin : aSheet.m_pins )
    {
        m_pins.emplace_back( new SCH_SHEET_PIN( *pin ) );
        m_pins.back()->SetParent( this );
    }

    for( SCH_FIELD& field : m_fields )
        field.SetParent( this );

    if( m_screen )
        m_screen->IncRefCount();
}


SCH_SHEET::~SCH_SHEET()
{
    // also, look at the associated sheet & its reference count
    // perhaps it should be deleted also.
    if( m_screen )
    {
        m_screen->DecRefCount();

        if( m_screen->GetRefCount() == 0 )
            delete m_screen;
    }

    // We own our pins; delete them
    for( SCH_SHEET_PIN* pin : m_pins )
        delete pin;
}


EDA_ITEM* SCH_SHEET::Clone() const
{
    return new SCH_SHEET( *this );
}


void SCH_SHEET::SetScreen( SCH_SCREEN* aScreen )
{
    if( aScreen == m_screen )
        return;

    if( m_screen != nullptr )
    {
        m_screen->DecRefCount();

        if( m_screen->GetRefCount() == 0 )
        {
            delete m_screen;
            m_screen = nullptr;
        }
    }

    m_screen = aScreen;

    if( m_screen )
        m_screen->IncRefCount();
}


int SCH_SHEET::GetScreenCount() const
{
    if( m_screen == nullptr )
        return 0;

    return m_screen->GetRefCount();
}


bool SCH_SHEET::IsRootSheet() const
{
    Q_ASSERT_X( Schematic(), "SCH_SHEET::IsRootSheet", "Can't call IsRootSheet without setting a schematic" );
    if( !Schematic() ) return false;

    return &Schematic()->Root() == this;
}


void SCH_SHEET::GetContextualTextVars( QStringList* aVars ) const
{
    auto add =
            [&]( const QString& aVar )
            {
                if( !aVars->contains( aVar ) )
                    aVars->append( aVar );
            };

    for( const SCH_FIELD& field : m_fields )
    {
        if( field.IsMandatory() )
            add( field.GetCanonicalName().toUpper() );
        else
            add( field.GetName() );
    }

    SCH_SHEET_PATH sheetPath = findSelf();

    if( sheetPath.size() >= 2 )
    {
        sheetPath.pop_back();
        sheetPath.Last()->GetContextualTextVars( aVars );
    }
    else if( Schematic() )
    {
        Schematic()->GetContextualTextVars( aVars );
    }

    add( "#" );
    add( "##" );
    add( "SHEETPATH" );
    add( "EXCLUDE_FROM_BOM" );
    add( "EXCLUDE_FROM_BOARD" );
    add( "EXCLUDE_FROM_SIM" );
    add( "DNP" );
    add( "ERC_ERROR <message_text>" );
    add( "ERC_WARNING <message_text>" );

    m_screen->GetTitleBlock().GetContextualTextVars( aVars );
}


bool SCH_SHEET::ResolveTextVar( const SCH_SHEET_PATH* aPath, QString* token, int aDepth ) const
{
    Q_ASSERT( aPath );
    if( !aPath ) return false;

    SCHEMATIC* schematic = Schematic();

    if( !schematic )
        return false;

    if( token->contains( ':' ) )
    {
        if( schematic->ResolveCrossReference( token, aDepth + 1 ) )
            return true;
    }

    for( const SCH_FIELD& field : m_fields )
    {
        QString fieldName = field.IsMandatory() ? field.GetCanonicalName().toUpper()
                                                 : field.GetName();

        if( token->compare( fieldName, Qt::CaseInsensitive ) == 0 )
        {
            *token = field.GetShownText( aPath, false, aDepth + 1 );
            return true;
        }
    }

    PROJECT* project = &schematic->Prj();

    // We cannot resolve text variables initially on load as we need to first load the screen and
    // then parse the hierarchy.  So skip the resolution if the screen isn't set yet
    if( m_screen && m_screen->GetTitleBlock().TextVarResolver( token, project ) )
    {
        return true;
    }

    if( token->compare( "#", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString::asprintf( "%s", aPath->GetPageNumber().toStdString().c_str() );
        return true;
    }
    else if( token->compare( "##", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString::asprintf( "%d", (int) schematic->Hierarchy().size() );
        return true;
    }
    else if( token->compare( "SHEETPATH", Qt::CaseInsensitive ) == 0 )
    {
        *token = aPath->PathHumanReadable();
        return true;
    }
    else if( token->compare( "EXCLUDE_FROM_BOM", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString();

        if( aPath->GetExcludedFromBOM() || this->GetExcludedFromBOM() )
            *token = _( "Excluded from BOM" );

        return true;
    }
    else if( token->compare( "EXCLUDE_FROM_BOARD", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString();

        if( aPath->GetExcludedFromBoard() || this->GetExcludedFromBoard() )
            *token = _( "Excluded from board" );

        return true;
    }
    else if( token->compare( "EXCLUDE_FROM_SIM", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString();

        if( aPath->GetExcludedFromSim() || this->GetExcludedFromSim() )
            *token = _( "Excluded from simulation" );

        return true;
    }
    else if( token->compare( "DNP", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString();

        if( aPath->GetDNP() || this->GetDNP() )
            *token = _( "DNP" );

        return true;
    }

    // See if parent can resolve it (these will recurse to ancestors)

    if( aPath->size() >= 2 )
    {
        SCH_SHEET_PATH path = *aPath;
        path.pop_back();

        if( path.Last()->ResolveTextVar( &path, token, aDepth + 1 ) )
            return true;
    }
    else
    {
        if( schematic->ResolveTextVar( aPath, token, aDepth + 1 ) )
            return true;
    }

    return false;
}


void SCH_SHEET::SwapData( SCH_ITEM* aItem )
{
    SCH_ITEM::SwapFlags( aItem );

    Q_ASSERT_X( aItem->Type() == SCH_SHEET_T, "SCH_SHEET::SwapData",
                QString::asprintf( "SCH_SHEET object cannot swap data with %s object.",
                                   aItem->GetClass().toStdString().c_str() ).toStdString().c_str() );
    if( aItem->Type() != SCH_SHEET_T ) return;

    SCH_SHEET* sheet = ( SCH_SHEET* ) aItem;

    std::swap( m_pos, sheet->m_pos );
    std::swap( m_size, sheet->m_size );
    m_fields.swap( sheet->m_fields );
    std::swap( m_fieldsAutoplaced, sheet->m_fieldsAutoplaced );
    m_pins.swap( sheet->m_pins );

    // Update parent pointers after swapping.
    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->SetParent( this );

    for( SCH_SHEET_PIN* sheetPin : sheet->m_pins )
        sheetPin->SetParent( sheet );

    for( SCH_FIELD& field : m_fields )
        field.SetParent( this );

    for( SCH_FIELD& field : sheet->m_fields )
        field.SetParent( sheet );

    std::swap( m_excludedFromSim, sheet->m_excludedFromSim );
    std::swap( m_excludedFromBOM, sheet->m_excludedFromBOM );
    std::swap( m_excludedFromBoard, sheet->m_excludedFromBoard );
    std::swap( m_DNP, sheet->m_DNP );

    std::swap( m_borderWidth, sheet->m_borderWidth );
    std::swap( m_borderColor, sheet->m_borderColor );
    std::swap( m_backgroundColor, sheet->m_backgroundColor );
    std::swap( m_instances, sheet->m_instances );
}


void SCH_SHEET::SetFields( const std::vector<SCH_FIELD>& aFields )
{
    m_fields = aFields;

    // Ensure that mandatory fields are at the beginning
    std::sort( m_fields.begin(), m_fields.end(),
               []( const SCH_FIELD& a, const SCH_FIELD& b )
               {
                   return a.GetId() < b.GetId();
               } );

    // After mandatory fields, the rest should be sequential user fields
    for( int ii = 0; ii < static_cast<int>( m_fields.size() ); ++ii )
    {
        if( !m_fields[ii].IsMandatory() )
            m_fields[ii].SetId( ii );
    }

    // Make sure that we get the UNIX variant of the file path
    SetFileName( m_fields[SHEETFILENAME].GetText() );
}


void SCH_SHEET::AddPin( SCH_SHEET_PIN* aSheetPin )
{
    Q_ASSERT( aSheetPin != nullptr );
    Q_ASSERT( aSheetPin->Type() == SCH_SHEET_PIN_T );

    aSheetPin->SetParent( this );
    m_pins.push_back( aSheetPin );
    renumberPins();
}


void SCH_SHEET::RemovePin( const SCH_SHEET_PIN* aSheetPin )
{
    Q_ASSERT( aSheetPin != nullptr );
    Q_ASSERT( aSheetPin->Type() == SCH_SHEET_PIN_T );

    for( auto i = m_pins.begin(); i < m_pins.end(); ++i )
    {
        if( *i == aSheetPin )
        {
            m_pins.erase( i );
            renumberPins();
            return;
        }
    }
}


bool SCH_SHEET::HasPin( const QString& aName ) const
{
    for( SCH_SHEET_PIN* pin : m_pins )
    {
        if( pin->GetText().compare( aName ) == 0 )
            return true;
    }

    return false;
}


bool SCH_SHEET::doIsConnected( const VECTOR2I& aPosition ) const
{
    for( SCH_SHEET_PIN* sheetPin : m_pins )
    {
        if( sheetPin->GetPosition() == aPosition )
            return true;
    }

    return false;
}


bool SCH_SHEET::IsVerticalOrientation() const
{
    int leftRight = 0;
    int topBottom = 0;

    for( SCH_SHEET_PIN* pin : m_pins )
    {
        switch( pin->GetSide() )
        {
        case SHEET_SIDE::LEFT:   leftRight++; break;
        case SHEET_SIDE::RIGHT:  leftRight++; break;
        case SHEET_SIDE::TOP:    topBottom++; break;
        case SHEET_SIDE::BOTTOM: topBottom++; break;
        default:                              break;
        }
    }

    return topBottom > 0 && leftRight == 0;
}


bool SCH_SHEET::HasUndefinedPins() const
{
    for( SCH_SHEET_PIN* pin : m_pins )
    {
        /* Search the schematic for a hierarchical label corresponding to this sheet label. */
        const SCH_HIERLABEL* HLabel = nullptr;

        for( SCH_ITEM* aItem : m_screen->Items().OfType( SCH_HIER_LABEL_T ) )
        {
            if( pin->GetText().compare( static_cast<SCH_HIERLABEL*>( aItem )->GetText() ) == 0 )
            {
                HLabel = static_cast<SCH_HIERLABEL*>( aItem );
                break;
            }
        }

        if( HLabel == nullptr ) // Corresponding hierarchical label not found.
            return true;
    }

    return false;
}


int bumpToNextGrid( const int aVal, const int aDirection )
{
    constexpr int gridSize = schIUScale.MilsToIU( 50 );

    int base = aVal / gridSize;
    int excess = abs( aVal % gridSize );

    if( aDirection > 0 )
    {
        return ( base + 1 ) * gridSize;
    }
    else if( excess > 0 )
    {
        return ( base ) * gridSize;
    }
    else
    {
        return ( base - 1 ) * gridSize;
    }
}


int SCH_SHEET::GetMinWidth( bool aFromLeft ) const
{
    int pinsLeft = m_pos.x + m_size.x;
    int pinsRight = m_pos.x;

    for( size_t i = 0; i < m_pins.size();  i++ )
    {
        SHEET_SIDE edge = m_pins[i]->GetSide();

        if( edge == SHEET_SIDE::TOP || edge == SHEET_SIDE::BOTTOM )
        {
            BOX2I pinRect = m_pins[i]->GetBoundingBox();

            pinsLeft = std::min( pinsLeft, pinRect.GetLeft() );
            pinsRight = std::max( pinsRight, pinRect.GetRight() );
        }
    }

    pinsLeft = bumpToNextGrid( pinsLeft, -1 );
    pinsRight = bumpToNextGrid( pinsRight, 1 );

    int pinMinWidth;

    if( pinsLeft >= pinsRight )
        pinMinWidth = 0;
    else if( aFromLeft )
        pinMinWidth = pinsRight - m_pos.x;
    else
        pinMinWidth = m_pos.x + m_size.x - pinsLeft;

    return std::max( pinMinWidth, schIUScale.MilsToIU( MIN_SHEET_WIDTH ) );
}


int SCH_SHEET::GetMinHeight( bool aFromTop ) const
{
    int pinsTop = m_pos.y + m_size.y;
    int pinsBottom = m_pos.y;

    for( size_t i = 0; i < m_pins.size();  i++ )
    {
        SHEET_SIDE edge = m_pins[i]->GetSide();

        if( edge == SHEET_SIDE::RIGHT || edge == SHEET_SIDE::LEFT )
        {
            BOX2I pinRect = m_pins[i]->GetBoundingBox();

            pinsTop = std::min( pinsTop, pinRect.GetTop() );
            pinsBottom = std::max( pinsBottom, pinRect.GetBottom() );
        }
    }

    pinsTop = bumpToNextGrid( pinsTop, -1 );
    pinsBottom = bumpToNextGrid( pinsBottom, 1 );

    int pinMinHeight;

    if( pinsTop >= pinsBottom )
        pinMinHeight = 0;
    else if( aFromTop )
        pinMinHeight = pinsBottom - m_pos.y;
    else
        pinMinHeight = m_pos.y + m_size.y - pinsTop;

    return std::max( pinMinHeight, schIUScale.MilsToIU( MIN_SHEET_HEIGHT ) );
}


void SCH_SHEET::CleanupSheet()
{
    std::vector<SCH_SHEET_PIN*> pins = m_pins;

    m_pins.clear();

    for( SCH_SHEET_PIN* pin : pins )
    {
        /* Search the schematic for a hierarchical label corresponding to this sheet label. */
        const SCH_HIERLABEL* HLabel = nullptr;

        for( SCH_ITEM* aItem : m_screen->Items().OfType( SCH_HIER_LABEL_T ) )
        {
            if( pin->GetText().compare( static_cast<SCH_HIERLABEL*>( aItem )->GetText(), Qt::CaseInsensitive ) == 0 )
            {
                HLabel = static_cast<SCH_HIERLABEL*>( aItem );
                break;
            }
        }

        if( HLabel )
            m_pins.push_back( pin );
    }
}


SCH_SHEET_PIN* SCH_SHEET::GetPin( const VECTOR2I& aPosition )
{
    for( SCH_SHEET_PIN* pin : m_pins )
    {
        if( pin->HitTest( aPosition ) )
            return pin;
    }

    return nullptr;
}


int SCH_SHEET::GetPenWidth() const
{
    if( GetBorderWidth() > 0 )
        return GetBorderWidth();

    if( Schematic() )
        return Schematic()->Settings().m_DefaultLineWidth;

    return schIUScale.MilsToIU( DEFAULT_LINE_WIDTH_MILS );
}


void SCH_SHEET::AutoplaceFields( SCH_SCREEN* aScreen, AUTOPLACE_ALGO aAlgo )
{
    VECTOR2I textSize = m_fields[SHEETNAME].GetTextSize();
    int      borderMargin = KiROUND( GetPenWidth() / 2.0 ) + 4;
    int      margin = borderMargin + KiROUND( std::max( textSize.x, textSize.y ) * 0.5 );

    if( IsVerticalOrientation() )
    {
        m_fields[SHEETNAME].SetTextPos( m_pos + VECTOR2I( -margin, m_size.y ) );
        m_fields[ SHEETNAME ].SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
        m_fields[ SHEETNAME ].SetVertJustify( GR_TEXT_V_ALIGN_BOTTOM );
        m_fields[ SHEETNAME ].SetTextAngle( ANGLE_VERTICAL );
    }
    else
    {
        m_fields[SHEETNAME].SetTextPos( m_pos + VECTOR2I( 0, -margin ) );
        m_fields[ SHEETNAME ].SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
        m_fields[ SHEETNAME ].SetVertJustify( GR_TEXT_V_ALIGN_BOTTOM );
        m_fields[ SHEETNAME ].SetTextAngle( ANGLE_HORIZONTAL );
    }

    textSize = m_fields[ SHEETFILENAME ].GetTextSize();
    margin = borderMargin + KiROUND( std::max( textSize.x, textSize.y ) * 0.4 );

    if( IsVerticalOrientation() )
    {
        m_fields[SHEETFILENAME].SetTextPos( m_pos + VECTOR2I( m_size.x + margin, m_size.y ) );
        m_fields[ SHEETFILENAME ].SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
        m_fields[ SHEETFILENAME ].SetVertJustify( GR_TEXT_V_ALIGN_TOP );
        m_fields[ SHEETFILENAME ].SetTextAngle( ANGLE_VERTICAL );
    }
    else
    {
        m_fields[SHEETFILENAME].SetTextPos( m_pos + VECTOR2I( 0, m_size.y + margin ) );
        m_fields[ SHEETFILENAME ].SetHorizJustify( GR_TEXT_H_ALIGN_LEFT );
        m_fields[ SHEETFILENAME ].SetVertJustify( GR_TEXT_V_ALIGN_TOP );
        m_fields[ SHEETFILENAME ].SetTextAngle( ANGLE_HORIZONTAL );
    }

    if( aAlgo == AUTOPLACE_AUTO || aAlgo == AUTOPLACE_MANUAL )
        m_fieldsAutoplaced = aAlgo;
}


std::vector<int> SCH_SHEET::ViewGetLayers() const
{
    // Sheet pins are drawn by their parent sheet, so the parent needs to draw to LAYER_DANGLING
    return { LAYER_DANGLING,    LAYER_HIERLABEL, LAYER_SHEETNAME,        LAYER_SHEETFILENAME,
             LAYER_SHEETFIELDS, LAYER_SHEET,     LAYER_SHEET_BACKGROUND, LAYER_SELECTION_SHADOWS };
}


const BOX2I SCH_SHEET::GetBodyBoundingBox() const
{
    VECTOR2I end;
    BOX2I    box( m_pos, m_size );
    int      lineWidth = GetPenWidth();
    int      textLength = 0;

    // Calculate bounding box X size:
    end.x = std::max( m_size.x, textLength );

    // Calculate bounding box pos:
    end.y = m_size.y;
    end += m_pos;

    box.SetEnd( end );
    box.Inflate( lineWidth / 2 );

    return box;
}


const BOX2I SCH_SHEET::GetBoundingBox() const
{
    BOX2I bbox = GetBodyBoundingBox();

    for( const SCH_FIELD& field : m_fields )
        bbox.Merge( field.GetBoundingBox() );

    return bbox;
}


VECTOR2I SCH_SHEET::GetRotationCenter() const
{
    BOX2I box( m_pos, m_size );
    return box.GetCenter();
}


int SCH_SHEET::SymbolCount() const
{
    int n = 0;

    if( m_screen )
    {
        for( SCH_ITEM* aItem : m_screen->Items().OfType( SCH_SYMBOL_T ) )
        {
            SCH_SYMBOL* symbol = (SCH_SYMBOL*) aItem;

            if( symbol->GetField( VALUE_FIELD )->GetText().at( 0 ) != '#' )
                n++;
        }

        for( SCH_ITEM* aItem : m_screen->Items().OfType( SCH_SHEET_T ) )
            n += static_cast<const SCH_SHEET*>( aItem )->SymbolCount();
    }

    return n;
}


bool SCH_SHEET::SearchHierarchy( const QString& aFilename, SCH_SCREEN** aScreen )
{
    if( m_screen )
    {
        // Only check the root sheet once and don't recurse.
        if( !GetParent() )
        {
            if( m_screen && m_screen->GetFileName().compare( aFilename ) == 0 )
            {
                *aScreen = m_screen;
                return true;
            }
        }

        for( SCH_ITEM* aItem : m_screen->Items().OfType( SCH_SHEET_T ) )
        {
            SCH_SHEET*  sheet  = static_cast<SCH_SHEET*>( aItem );
            SCH_SCREEN* screen = sheet->m_screen;

            // Must use the screen's path (which is always absolute) rather than the
            // sheet's (which could be relative).
            if( screen && screen->GetFileName().compare( aFilename ) == 0 )
            {
                *aScreen = screen;
                return true;
            }

            if( sheet->SearchHierarchy( aFilename, aScreen ) )
                return true;
        }
    }

    return false;
}


bool SCH_SHEET::LocatePathOfScreen( SCH_SCREEN* aScreen, SCH_SHEET_PATH* aList )
{
    if( m_screen )
    {
        aList->push_back( this );

        if( m_screen == aScreen )
            return true;

        for( EDA_ITEM* item : m_screen->Items().OfType( SCH_SHEET_T ) )
        {
            SCH_SHEET* sheet = static_cast<SCH_SHEET*>( item );

            if( sheet->LocatePathOfScreen( aScreen, aList ) )
                return true;
        }

        aList->pop_back();
    }

    return false;
}


int SCH_SHEET::CountSheets( const QString& aFilename ) const
{
    int count = 0;

    if( m_screen )
    {
        if( m_screen->GetFileName().compare( aFilename ) == 0 )
            count++;

        for( SCH_ITEM* aItem : m_screen->Items().OfType( SCH_SHEET_T ) )
            count += static_cast<SCH_SHEET*>( aItem )->CountSheets( aFilename );
    }

    return count;
}


int SCH_SHEET::CountSheets() const
{
    int count = 1; //1 = this!!

    if( m_screen )
    {
        for( SCH_ITEM* aItem : m_screen->Items().OfType( SCH_SHEET_T ) )
            count += static_cast<SCH_SHEET*>( aItem )->CountSheets();
    }

    return count;
}


void SCH_SHEET::GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList )
{
    // Don't use GetShownText(); we want to see the variable references here
    // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
    // aList.emplace_back( _( "Sheet Name" ),
    //                     KIUI::EllipsizeStatusText( aFrame, m_fields[ SHEETNAME ].GetText() ) );
    aList.emplace_back( _( "Sheet Name" ), m_fields[ SHEETNAME ].GetText() ); // Direct text without ellipsis

    if( SCH_EDIT_FRAME* schframe = dynamic_cast<SCH_EDIT_FRAME*>( aFrame ) )
    {
        // UNUSED_SYMBOL: GetCurrentSheet - sheet path creation commented out due to unused symbol
        // SCH_SHEET_PATH path = schframe->GetCurrentSheet();
        SCH_SHEET_PATH path;
        path.push_back( this );

        aList.emplace_back( _( "Hierarchical Path" ), path.PathHumanReadable( false, true ) );
    }

    // Don't use GetShownText(); we want to see the variable references here
    // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
    // aList.emplace_back( _( "File Name" ),
    //                     KIUI::EllipsizeStatusText( aFrame, m_fields[ SHEETFILENAME ].GetText() ) );
    aList.emplace_back( _( "File Name" ), m_fields[ SHEETFILENAME ].GetText() ); // Direct text without ellipsis

    QStringList msgs;
    QString      msg;

    if( GetExcludedFromSim() )
        msgs.append( _( "Simulation" ) );

    if( GetExcludedFromBOM() )
        msgs.append( _( "BOM" ) );

    if( GetExcludedFromBoard() )
        msgs.append( _( "Board" ) );

    if( GetDNP() )
        msgs.append( _( "DNP" ) );

    msg = msgs.join( '|' );
    msg.replace( '|', ", " );

    if( !msg.isEmpty() )
        aList.emplace_back( _( "Exclude from" ), msg );
}


void SCH_SHEET::SetPositionIgnoringPins( const VECTOR2I& aPosition )
{
    VECTOR2I delta = aPosition - m_pos;

    m_pos = aPosition;

    for( SCH_FIELD& field : m_fields )
        field.Move( delta );
}


void SCH_SHEET::Move( const VECTOR2I& aMoveVector )
{
    m_pos += aMoveVector;

    for( SCH_SHEET_PIN* pin : m_pins )
        pin->Move( aMoveVector );

    for( SCH_FIELD& field : m_fields )
        field.Move( aMoveVector );
}


void SCH_SHEET::Rotate( const VECTOR2I& aCenter, bool aRotateCCW )
{
    VECTOR2I prev = m_pos;

    RotatePoint( m_pos, aCenter, aRotateCCW ? ANGLE_90 : ANGLE_270 );
    RotatePoint( &m_size.x, &m_size.y, aRotateCCW ? ANGLE_90 : ANGLE_270 );

    if( m_size.x < 0 )
    {
        m_pos.x += m_size.x;
        m_size.x = -m_size.x;
    }

    if( m_size.y < 0 )
    {
        m_pos.y += m_size.y;
        m_size.y = -m_size.y;
    }

    // Pins must be rotated first as that's how we determine vertical vs horizontal
    // orientation for auto-placement
    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->Rotate( aCenter, aRotateCCW );

    if( m_fieldsAutoplaced == AUTOPLACE_AUTO || m_fieldsAutoplaced == AUTOPLACE_MANUAL )
    {
        AutoplaceFields( nullptr, m_fieldsAutoplaced );
    }
    else
    {
        // Move the fields to the new position because the parent itself has moved.
        for( SCH_FIELD& field : m_fields )
        {
            VECTOR2I pos = field.GetTextPos();
            pos.x -= prev.x - m_pos.x;
            pos.y -= prev.y - m_pos.y;
            field.SetTextPos( pos );
        }
    }
}


void SCH_SHEET::MirrorVertically( int aCenter )
{
    int dy = m_pos.y;

    MIRROR( m_pos.y, aCenter );
    m_pos.y -= m_size.y;
    dy -= m_pos.y;     // 0,dy is the move vector for this transform

    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->MirrorVertically( aCenter );

    for( SCH_FIELD& field : m_fields )
    {
        VECTOR2I pos = field.GetTextPos();
        pos.y -= dy;
        field.SetTextPos( pos );
    }
}


void SCH_SHEET::MirrorHorizontally( int aCenter )
{
    int dx = m_pos.x;

    MIRROR( m_pos.x, aCenter );
    m_pos.x -= m_size.x;
    dx -= m_pos.x;     // dx,0 is the move vector for this transform

    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->MirrorHorizontally( aCenter );

    for( SCH_FIELD& field : m_fields )
    {
        VECTOR2I pos = field.GetTextPos();
        pos.x -= dx;
        field.SetTextPos( pos );
    }
}


void SCH_SHEET::SetPosition( const VECTOR2I& aPosition )
{
    // Remember the sheet and all pin sheet positions must be
    // modified. So use Move function to do that.
    Move( aPosition - m_pos );
}


void SCH_SHEET::Resize( const VECTOR2I& aSize )
{
    if( aSize == m_size )
        return;

    m_size = aSize;

    // Move the fields if we're in autoplace mode
    if( m_fieldsAutoplaced == AUTOPLACE_AUTO || m_fieldsAutoplaced == AUTOPLACE_MANUAL )
        AutoplaceFields( nullptr, m_fieldsAutoplaced );

    // Move the sheet labels according to the new sheet size.
    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->ConstrainOnEdge( sheetPin->GetPosition(), false );
}


bool SCH_SHEET::Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const
{
    // Sheets are searchable via the child field and pin item text.
    return false;
}


void SCH_SHEET::renumberPins()
{
    int id = 2;

    for( SCH_SHEET_PIN* pin : m_pins )
    {
        pin->SetNumber( id );
        id++;
    }
}


SCH_SHEET_PATH SCH_SHEET::findSelf() const
{
    Q_ASSERT_X( Schematic(), "SCH_SHEET::findSelf", "Can't call findSelf without a schematic" );
    if( !Schematic() ) return SCH_SHEET_PATH();

    SCH_SHEET_PATH sheetPath = Schematic()->CurrentSheet();

    while( !sheetPath.empty() && sheetPath.Last() != this )
        sheetPath.pop_back();

    if( sheetPath.empty() )
    {
        // If we weren't in the hierarchy, then we must be a child of the current sheet.
        sheetPath = Schematic()->CurrentSheet();
        sheetPath.push_back( const_cast<SCH_SHEET*>( this ) );
    }

    return sheetPath;
}


void SCH_SHEET::GetEndPoints( std::vector <DANGLING_END_ITEM>& aItemList )
{
    for( SCH_SHEET_PIN* sheetPin : m_pins )
    {
        Q_ASSERT_X( sheetPin->Type() == SCH_SHEET_PIN_T, "SCH_SHEET::GetEndPoints",
                    "Invalid item in schematic sheet pin list.  Bad programmer!" );
        if( sheetPin->Type() != SCH_SHEET_PIN_T ) continue;

        sheetPin->GetEndPoints( aItemList );
    }
}


bool SCH_SHEET::UpdateDanglingState( std::vector<DANGLING_END_ITEM>& aItemListByType,
                                     std::vector<DANGLING_END_ITEM>& aItemListByPos,
                                     const SCH_SHEET_PATH*           aPath )
{
    bool changed = false;

    for( SCH_SHEET_PIN* sheetPin : m_pins )
        changed |= sheetPin->UpdateDanglingState( aItemListByType, aItemListByPos );

    return changed;
}


bool SCH_SHEET::HasConnectivityChanges( const SCH_ITEM* aItem,
                                        const SCH_SHEET_PATH* aInstance ) const
{
    // Do not compare to ourself.
    if( aItem == this )
        return false;

    const SCH_SHEET* sheet = dynamic_cast<const SCH_SHEET*>( aItem );

    // Don't compare against a different SCH_ITEM.
    Q_ASSERT( sheet );
    if( !sheet ) return false;

    if( GetPosition() != sheet->GetPosition() )
        return true;

    // Technically this cannot happen because undo/redo does not support reloading sheet
    // file association changes.  This was just added so that it doesn't get missed should
    // we ever fix the undo/redo issue.
    if( ( GetFileName() != sheet->GetFileName() ) || ( GetName() != sheet->GetName() ) )
        return true;

    if( m_pins.size() != sheet->m_pins.size() )
        return true;

    for( size_t i = 0; i < m_pins.size(); i++ )
    {
        if( m_pins[i]->HasConnectivityChanges( sheet->m_pins[i] ) )
            return true;
    }

    return false;
}


std::vector<VECTOR2I> SCH_SHEET::GetConnectionPoints() const
{
    std::vector<VECTOR2I> retval;

    for( SCH_SHEET_PIN* sheetPin : m_pins )
        retval.push_back( sheetPin->GetPosition() );

    return retval;
}


INSPECT_RESULT SCH_SHEET::Visit( INSPECTOR aInspector, void* testData,
                                 const std::vector<KICAD_T>& aScanTypes )
{
    for( KICAD_T scanType : aScanTypes )
    {
        // If caller wants to inspect my type
        if( scanType == SCH_LOCATE_ANY_T || scanType == Type() )
        {
            if( INSPECT_RESULT::QUIT == aInspector( this, nullptr ) )
                return INSPECT_RESULT::QUIT;
        }

        if( scanType == SCH_LOCATE_ANY_T || scanType == SCH_FIELD_T )
        {
            // Test the sheet fields.
            for( SCH_FIELD& field : m_fields )
            {
                if( INSPECT_RESULT::QUIT == aInspector( &field, this ) )
                    return INSPECT_RESULT::QUIT;
            }
        }

        if( scanType == SCH_LOCATE_ANY_T || scanType == SCH_SHEET_PIN_T )
        {
            // Test the sheet labels.
            for( SCH_SHEET_PIN* sheetPin : m_pins )
            {
                if( INSPECT_RESULT::QUIT == aInspector( sheetPin, this ) )
                    return INSPECT_RESULT::QUIT;
            }
        }
    }

    return INSPECT_RESULT::CONTINUE;
}


void SCH_SHEET::RunOnChildren( const std::function<void( SCH_ITEM* )>& aFunction )
{
    for( SCH_FIELD& field : m_fields )
        aFunction( &field );

    for( SCH_SHEET_PIN* pin : m_pins )
        aFunction( pin );
}


QString SCH_SHEET::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    return QString( _( "Hierarchical Sheet %s" ) ).arg(
                             aFull ? m_fields[ SHEETNAME ].GetShownText( false )
                                   // UNUSED_SYMBOL: KIUI::EllipsizeMenuText in unused_symbols.txt - Menu text ellipsis disabled
                                  // : KIUI::EllipsizeMenuText( m_fields[ SHEETNAME ].GetText() ) );
                                  : m_fields[ SHEETNAME ].GetText() ); // Direct text without ellipsis
}


BITMAPS SCH_SHEET::GetMenuImage() const
{
    return BITMAPS::add_hierarchical_subsheet;
}


bool SCH_SHEET::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    BOX2I rect = GetBodyBoundingBox();

    rect.Inflate( aAccuracy );

    return rect.Contains( aPosition );
}


bool SCH_SHEET::HitTest( const BOX2I& aRect, bool aContained, int aAccuracy ) const
{
    BOX2I rect = aRect;

    rect.Inflate( aAccuracy );

    if( aContained )
        return rect.Contains( GetBodyBoundingBox() );

    return rect.Intersects( GetBodyBoundingBox() );
}


void SCH_SHEET::Plot( PLOTTER* aPlotter, bool aBackground, const SCH_PLOT_OPTS& aPlotOpts,
                      int aUnit, int aBodyStyle, const VECTOR2I& aOffset, bool aDimmed )
{
    if( aBackground && !aPlotter->GetColorMode() )
        return;

    SCH_RENDER_SETTINGS* renderSettings = getRenderSettings( aPlotter );
    COLOR4D              borderColor = GetBorderColor();
    COLOR4D              backgroundColor = GetBackgroundColor();

    if( renderSettings->m_OverrideItemColors || borderColor == COLOR4D::UNSPECIFIED )
        borderColor = aPlotter->RenderSettings()->GetLayerColor( LAYER_SHEET );

    if( renderSettings->m_OverrideItemColors || backgroundColor == COLOR4D::UNSPECIFIED )
        backgroundColor = aPlotter->RenderSettings()->GetLayerColor( LAYER_SHEET_BACKGROUND );

    if( aBackground && backgroundColor.a > 0.0 )
    {
        aPlotter->SetColor( backgroundColor );
        aPlotter->Rect( m_pos, m_pos + m_size, FILL_T::FILLED_SHAPE, 1 );
    }
    else
    {
        aPlotter->SetColor( borderColor );

        int penWidth = GetEffectivePenWidth( getRenderSettings( aPlotter ) );
        aPlotter->Rect( m_pos, m_pos + m_size, FILL_T::NO_FILL, penWidth );
    }

    // Make the sheet object a clickable hyperlink (e.g. for PDF plotter)
    if( aPlotOpts.m_PDFHierarchicalLinks )
    {
        aPlotter->HyperlinkBox( GetBoundingBox(),
                                EDA_TEXT::GotoPageHref( findSelf().GetPageNumber() ) );
    }
    else if( aPlotOpts.m_PDFPropertyPopups )
    {
        std::vector<QString> properties;

        properties.emplace_back( EDA_TEXT::GotoPageHref( findSelf().GetPageNumber() ) );

        for( const SCH_FIELD& field : GetFields() )
        {
            properties.emplace_back( QString::asprintf( "!%s = %s", field.GetName().toStdString().c_str(),
                                                       field.GetShownText( false ).toStdString().c_str() ) );
        }

        aPlotter->HyperlinkMenu( GetBoundingBox(), properties );
    }

    // Plot sheet pins
    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->Plot( aPlotter, aBackground, aPlotOpts, aUnit, aBodyStyle, aOffset, aDimmed );

    // Plot the fields
    for( SCH_FIELD& field : m_fields )
        field.Plot( aPlotter, aBackground, aPlotOpts, aUnit, aBodyStyle, aOffset, aDimmed );

    if( GetDNP() )
    {
        COLOR_SETTINGS* colors = Pgm().GetSettingsManager().GetColorSettings();
        BOX2I           bbox = GetBodyBoundingBox();
        BOX2I           pins = GetBoundingBox();
        VECTOR2D        margins( std::max( bbox.GetX() - pins.GetX(),
                                           pins.GetEnd().x - bbox.GetEnd().x ),
                                 std::max( bbox.GetY() - pins.GetY(),
                                           pins.GetEnd().y - bbox.GetEnd().y ) );
        int             strokeWidth = 3.0 * schIUScale.MilsToIU( DEFAULT_LINE_WIDTH_MILS );

        margins.x = std::max( margins.x * 0.6, margins.y * 0.3 );
        margins.y = std::max( margins.y * 0.6, margins.x * 0.3 );
        bbox.Inflate( KiROUND( margins.x ), KiROUND( margins.y ) );

        aPlotter->SetColor( colors->GetColor( LAYER_DNP_MARKER ) );

        aPlotter->ThickSegment( bbox.GetOrigin(), bbox.GetEnd(), strokeWidth, FILLED, nullptr );

        aPlotter->ThickSegment( bbox.GetOrigin() + VECTOR2I( bbox.GetWidth(), 0 ),
                                bbox.GetOrigin() + VECTOR2I( 0, bbox.GetHeight() ),
                                strokeWidth, FILLED, nullptr );
    }
}


void SCH_SHEET::Print( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                       const VECTOR2I& aOffset, bool aForceNoFill, bool aDimmed )
{
    QPainter*        DC = aSettings->GetPrintDC();
    VECTOR2I pos = m_pos + aOffset;
    int      lineWidth = GetEffectivePenWidth( aSettings );
    COLOR4D  border = GetBorderColor();
    COLOR4D  background = GetBackgroundColor();

    if( aSettings->m_OverrideItemColors || border == COLOR4D::UNSPECIFIED )
        border = aSettings->GetLayerColor( LAYER_SHEET );

    if( aSettings->m_OverrideItemColors || background == COLOR4D::UNSPECIFIED )
        background = aSettings->GetLayerColor( LAYER_SHEET_BACKGROUND );

    if( GetGRForceBlackPenState() )     // printing in black & white
        background = COLOR4D::UNSPECIFIED;

    if( background.a > 0.0 )
        GRFilledRect( DC, pos, pos + m_size, 0, background, background );

    GRRect( DC, pos, pos + m_size, lineWidth, border );

    for( SCH_FIELD& field : m_fields )
        field.Print( aSettings, aUnit, aBodyStyle, aOffset, aForceNoFill, aDimmed );

    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->Print( aSettings, aUnit, aBodyStyle, aOffset, aForceNoFill, aDimmed );

    if( GetDNP() )
    {
        BOX2I    bbox = GetBodyBoundingBox();
        BOX2I    pins = GetBoundingBox();
        COLOR4D  dnp_color = aSettings->GetLayerColor( LAYER_DNP_MARKER );
        VECTOR2D margins( std::max( bbox.GetX() - pins.GetX(), pins.GetEnd().x - bbox.GetEnd().x ),
                          std::max( bbox.GetY() - pins.GetY(),
                                    pins.GetEnd().y - bbox.GetEnd().y ) );

        margins.x = std::max( margins.x * 0.6, margins.y * 0.3 );
        margins.y = std::max( margins.y * 0.6, margins.x * 0.3 );
        bbox.Inflate( KiROUND( margins.x ), KiROUND( margins.y ) );

        GRFilledSegment( DC, bbox.GetOrigin(), bbox.GetEnd(),
                             3.0 * schIUScale.MilsToIU( DEFAULT_LINE_WIDTH_MILS ),
                             dnp_color );

        GRFilledSegment( DC, bbox.GetOrigin() + VECTOR2I( bbox.GetWidth(), 0 ),
                             bbox.GetOrigin() + VECTOR2I( 0, bbox.GetHeight() ),
                             3.0 * schIUScale.MilsToIU( DEFAULT_LINE_WIDTH_MILS ),
                             dnp_color );
    }
}


SCH_SHEET& SCH_SHEET::operator=( const SCH_ITEM& aItem )
{
    Q_ASSERT_X( Type() == aItem.Type(), "SCH_SHEET::operator=",
                QString( "Cannot assign object type " + aItem.GetClass() + " to type " + GetClass() ).toStdString().c_str() );
    if( Type() != aItem.Type() ) return *this;

    if( &aItem != this )
    {
        SCH_ITEM::operator=( aItem );

        SCH_SHEET* sheet = (SCH_SHEET*) &aItem;

        m_pos = sheet->m_pos;
        m_size = sheet->m_size;
        m_fields = sheet->m_fields;

        for( SCH_SHEET_PIN* pin : sheet->m_pins )
        {
            m_pins.emplace_back( new SCH_SHEET_PIN( *pin ) );
            m_pins.back()->SetParent( this );
        }

        for( const SCH_SHEET_INSTANCE& instance : sheet->m_instances )
            m_instances.emplace_back( instance );
    }

    return *this;
}


bool SCH_SHEET::operator <( const SCH_ITEM& aItem ) const
{
    if( Type() != aItem.Type() )
        return Type() < aItem.Type();

    auto sheet = static_cast<const SCH_SHEET*>( &aItem );

    if (m_fields[ SHEETNAME ].GetText() != sheet->m_fields[ SHEETNAME ].GetText() )
        return m_fields[ SHEETNAME ].GetText() < sheet->m_fields[ SHEETNAME ].GetText();

    if (m_fields[ SHEETFILENAME ].GetText() != sheet->m_fields[ SHEETFILENAME ].GetText() )
        return m_fields[ SHEETFILENAME ].GetText() < sheet->m_fields[ SHEETFILENAME ].GetText();

    return false;
}


void SCH_SHEET::RemoveInstance( const KIID_PATH& aInstancePath )
{
    // Search for an existing path and remove it if found (should not occur)
    // (search from back to avoid invalidating iterator on remove)
    for( int ii = m_instances.size() - 1; ii >= 0; --ii )
    {
        if( m_instances[ii].m_Path == aInstancePath )
        {
            qDebug() << "Removing sheet instance:\n"
                     << "    sheet path" << aInstancePath.AsString().toStdString().c_str() << "\n"
                     << "    page" << m_instances[ii].m_PageNumber.toStdString().c_str()
                     << ", from project" << m_instances[ii].m_ProjectName.toStdString().c_str();

            m_instances.erase( m_instances.begin() + ii );
        }
    }
}


void SCH_SHEET::AddInstance( const SCH_SHEET_INSTANCE& aInstance )
{
    SCH_SHEET_INSTANCE oldInstance;

    if( getInstance( oldInstance, aInstance.m_Path ) )
        RemoveInstance( aInstance.m_Path );

    m_instances.emplace_back( aInstance );

}


bool SCH_SHEET::addInstance( const KIID_PATH& aPath )
{
    for( const SCH_SHEET_INSTANCE& instance : m_instances )
    {
        // if aSheetPath is found, nothing to do:
        if( instance.m_Path == aPath )
            return false;
    }

    qDebug() << "Adding instance" << aPath.AsString().toStdString().c_str()
             << "to sheet" << ( GetName().isEmpty() ? QString( "root" ) : GetName() ).toStdString().c_str();

    SCH_SHEET_INSTANCE instance;

    instance.m_Path = aPath;

    // This entry does not exist: add it with an empty page number.
    m_instances.emplace_back( instance );
    return true;
}


bool SCH_SHEET::getInstance( SCH_SHEET_INSTANCE& aInstance, const KIID_PATH& aSheetPath,
                             bool aTestFromEnd ) const
{
    for( const SCH_SHEET_INSTANCE& instance : m_instances )
    {
        if( !aTestFromEnd )
        {
            if( instance.m_Path == aSheetPath )
            {
                aInstance = instance;
                return true;
            }
        }
        else if( instance.m_Path.EndsWith( aSheetPath ) )
        {
            aInstance = instance;
            return true;
        }
    }

    return false;
}


bool SCH_SHEET::HasRootInstance() const
{
    for( const SCH_SHEET_INSTANCE& instance : m_instances )
    {
        if( instance.m_Path.size() == 0 )
            return true;
    }

    return false;
}


const SCH_SHEET_INSTANCE& SCH_SHEET::GetRootInstance() const
{
    for( const SCH_SHEET_INSTANCE& instance : m_instances )
    {
        if( instance.m_Path.size() == 0 )
            return instance;
    }

    Q_ASSERT_X( false, "SCH_SHEET::GetRootInstance", "No root instance found" );

    static SCH_SHEET_INSTANCE dummy;

    return dummy;
}


QString SCH_SHEET::getPageNumber( const KIID_PATH& aPath ) const
{
    QString pageNumber;

    for( const SCH_SHEET_INSTANCE& instance : m_instances )
    {
        if( instance.m_Path == aPath )
        {
            pageNumber = instance.m_PageNumber;
            break;
        }
    }

    return pageNumber;
}


void SCH_SHEET::setPageNumber( const KIID_PATH& aPath, const QString& aPageNumber )
{
    for( SCH_SHEET_INSTANCE& instance : m_instances )
    {
        if( instance.m_Path == aPath )
        {
            instance.m_PageNumber = aPageNumber;
            break;
        }
    }
}


bool SCH_SHEET::HasPageNumberChanges( const SCH_SHEET& aOther ) const
{
    // Avoid self comparison.
    if( &aOther == this )
        return false;

    // A difference in the instance data count implies a page numbering change.
    if( GetInstances().size() != aOther.GetInstances().size() )
        return true;

    std::vector<SCH_SHEET_INSTANCE> instances = GetInstances();
    std::vector<SCH_SHEET_INSTANCE> otherInstances = aOther.GetInstances();

    // Sorting may not be necessary but there is no guarantee that sheet
    // instance data will be in the correct KIID_PATH order.  We should
    // probably use a std::map instead of a std::vector to store the sheet
    // instance data.
    std::sort( instances.begin(), instances.end(),
               []( const SCH_SHEET_INSTANCE& aLhs, const SCH_SHEET_INSTANCE& aRhs )
               {
                   if( aLhs.m_Path > aRhs.m_Path )
                       return true;

                   return false;
               } );
    std::sort( otherInstances.begin(), otherInstances.end(),
               []( const SCH_SHEET_INSTANCE& aLhs, const SCH_SHEET_INSTANCE& aRhs )
               {
                   if( aLhs.m_Path > aRhs.m_Path )
                       return true;

                   return false;
               } );

    auto itThis = instances.begin();
    auto itOther = otherInstances.begin();

    while( itThis != instances.end() )
    {
        if( ( itThis->m_Path == itOther->m_Path )
          && ( itThis->m_PageNumber != itOther->m_PageNumber ) )
        {
            return true;
        }

        itThis++;
        itOther++;
    }

    return false;
}


int SCH_SHEET::ComparePageNum( const QString& aPageNumberA, const QString& aPageNumberB )
{
    if( aPageNumberA == aPageNumberB )
        return 0; // A == B

    // First sort numerically if the page numbers are integers
    long pageA, pageB;
    bool isIntegerPageA;
    bool isIntegerPageB;
    pageA = aPageNumberA.toLong( &isIntegerPageA );
    pageB = aPageNumberB.toLong( &isIntegerPageB );

    if( isIntegerPageA && isIntegerPageB )
    {
        if( pageA < pageB )
            return -1; //A < B
        else
            return 1; // A > B
    }

    // Numerical page numbers always before strings
    if( isIntegerPageA )
        return -1; //A < B
    else if( isIntegerPageB )
        return 1; // A > B

    // If not numeric, then sort as strings using natural sort
    int result = StrNumCmp( aPageNumberA, aPageNumberB );

    // Divide by zero bad.
    Q_ASSERT( result != 0 );
    if( result == 0 ) return 0;

    result = result / std::abs( result );

    return result;
}


bool SCH_SHEET::operator==( const SCH_ITEM& aOther ) const
{
    if( Type() != aOther.Type() )
        return false;

    const SCH_SHEET* other = static_cast<const SCH_SHEET*>( &aOther );

    if( m_pos != other->m_pos )
        return false;

    if( m_size != other->m_size )
        return false;

    if( GetExcludedFromSim() != other->GetExcludedFromSim() )
        return false;

    if( GetExcludedFromBOM() != other->GetExcludedFromBOM() )
        return false;

    if( GetExcludedFromBoard() != other->GetExcludedFromBoard() )
        return false;

    if( GetDNP() != other->GetDNP() )
        return false;

    if( GetBorderColor() != other->GetBorderColor() )
        return false;

    if( GetBackgroundColor() != other->GetBackgroundColor() )
        return false;

    if( GetBorderWidth() != other->GetBorderWidth() )
        return false;

    if( GetFields().size() != other->GetFields().size() )
        return false;

    for( size_t i = 0; i < GetFields().size(); ++i )
    {
        if( !( GetFields()[i] == other->GetFields()[i] ) )
            return false;
    }

    return true;
}


double SCH_SHEET::Similarity( const SCH_ITEM& aOther ) const
{
    if( Type() != aOther.Type() )
        return 0.0;

    const SCH_SHEET* other = static_cast<const SCH_SHEET*>( &aOther );

    if( m_screen->GetFileName() == other->m_screen->GetFileName() )
        return 1.0;

    return 0.0;
}


#if defined(DEBUG)

void SCH_SHEET::Show( int nestLevel, std::ostream& os ) const
{
    // XML output:
    QString s = GetClass();

    NestedSpace( nestLevel, os ) << '<' << s.toLower().toStdString().c_str() << ">" << " sheet_name=\""
                                 << m_fields[ SHEETNAME ].GetText().toStdString().c_str() << '"' << ">\n";

    // show all the pins, and check the linked list integrity
    for( SCH_SHEET_PIN* sheetPin : m_pins )
        sheetPin->Show( nestLevel + 1, os );

    NestedSpace( nestLevel, os ) << "</" << s.toLower().toStdString().c_str() << ">\n" << std::flush;
}

#endif

static struct SCH_SHEET_DESC
{
    SCH_SHEET_DESC()
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        REGISTER_TYPE( SCH_SHEET );
        propMgr.InheritsAfter( TYPE_HASH( SCH_SHEET ), TYPE_HASH( SCH_ITEM ) );

        propMgr.AddProperty( new PROPERTY<SCH_SHEET, QString>( _HKI( "Sheet Name" ),
                             &SCH_SHEET::SetName, &SCH_SHEET::GetName ) );

        propMgr.AddProperty( new PROPERTY<SCH_SHEET, int>( _HKI( "Border Width" ),
                             &SCH_SHEET::SetBorderWidth, &SCH_SHEET::GetBorderWidth,
                             PROPERTY_DISPLAY::PT_SIZE ) );

        propMgr.AddProperty( new PROPERTY<SCH_SHEET, COLOR4D>( _HKI( "Border Color" ),
                             &SCH_SHEET::SetBorderColor, &SCH_SHEET::GetBorderColor ) );

        propMgr.AddProperty( new PROPERTY<SCH_SHEET, COLOR4D>( _HKI( "Background Color" ),
                             &SCH_SHEET::SetBackgroundColor, &SCH_SHEET::GetBackgroundColor ) );

        const QString groupAttributes = _HKI( "Attributes" );

        propMgr.AddProperty( new PROPERTY<SCH_SHEET, bool>( _HKI( "Exclude From Board" ),
                    &SCH_SHEET::SetExcludedFromBoard, &SCH_SHEET::GetExcludedFromBoard ),
                    groupAttributes );
        propMgr.AddProperty( new PROPERTY<SCH_SHEET, bool>( _HKI( "Exclude From Simulation" ),
                    &SCH_SHEET::SetExcludedFromSim, &SCH_SHEET::GetExcludedFromSim ),
                    groupAttributes );
        propMgr.AddProperty(
                new PROPERTY<SCH_SHEET, bool>( _HKI( "Exclude From Bill of Materials" ),
                                               &SCH_SHEET::SetExcludedFromBOM,
                                               &SCH_SHEET::GetExcludedFromBOM ),
                groupAttributes );
        propMgr.AddProperty( new PROPERTY<SCH_SHEET, bool>( _HKI( "Do not Populate" ),
                    &SCH_SHEET::SetDNP, &SCH_SHEET::GetDNP ),
                    groupAttributes );
    }
} _SCH_SHEET_DESC;

// Qt transformation completed - all wxWidgets dependencies have been replaced with Qt equivalents
