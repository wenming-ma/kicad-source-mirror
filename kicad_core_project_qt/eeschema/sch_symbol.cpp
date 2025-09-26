
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#include <sch_collectors.h>
#include <sch_commit.h>
#include <sch_edit_frame.h>
#include <widgets/msgpanel.h>
#include <bitmaps.h>
#include <core/mirror.h>
#include <sch_shape.h>
#include <pgm_base.h>
// #include <sim/sim_model.h> // UNUSED_SYMBOL: SIM functionality disabled
// #include <sim/spice_generator.h> // UNUSED_SYMBOL: SIM functionality disabled
// #include <sim/sim_lib_mgr.h> // UNUSED_SYMBOL: SIM functionality disabled
#include <trace_helpers.h>
#include <trigo.h>
#include <refdes_utils.h>
#include <QLoggingCategory>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QRegularExpression>
#include <QColor>
#include <settings/settings_manager.h>
#include <sch_plotter.h>
#include <string_utils.h>
#include <sch_rule_area.h>

#include <utility>
#include <memory>
#include <validators.h>

Q_LOGGING_CATEGORY(logSchSheetPaths, "kicad.sch.sheetpaths")

std::unordered_map<TRANSFORM, int> SCH_SYMBOL::s_transformToOrientationCache;


/**
 * Convert a QString to UTF8 and replace any control characters with a ~, where a control
 * character is one of the first ASCII values up to ' ' 32d.
 */
std::string toUTFTildaText( const QString& txt )
{
    std::string ret = TO_UTF8( txt );

    for( char& c : ret )
    {
        if( (unsigned char) c <= ' ' )
            c = '~';
    }

    return ret;
}


SCH_SYMBOL::SCH_SYMBOL() :
        SYMBOL( nullptr, SCH_SYMBOL_T )
{
    Init( VECTOR2I( 0, 0 ) );
}


SCH_SYMBOL::SCH_SYMBOL( const LIB_SYMBOL& aSymbol, const LIB_ID& aLibId,
                        const SCH_SHEET_PATH* aSheet, int aUnit, int aBodyStyle,
                        const VECTOR2I& aPosition, EDA_ITEM* aParent ) :
        SYMBOL( aParent, SCH_SYMBOL_T )
{
    Init( aPosition );

    m_unit      = aUnit;
    m_bodyStyle = aBodyStyle;
    m_lib_id    = aLibId;

    std::unique_ptr< LIB_SYMBOL > part;

    part = aSymbol.Flatten();
    part->SetParent();
    SetLibSymbol( part.release() );

    // Copy fields from the library symbol
    UpdateFields( aSheet,
                  true,   /* update style */
                  false,  /* update ref */
                  false,  /* update other fields */
                  true,   /* reset ref */
                  true    /* reset other fields */ );

    m_prefix = UTIL::GetRefDesPrefix( m_part->GetReferenceField().GetText() );

    if( aSheet )
        SetRef( aSheet, UTIL::GetRefDesUnannotated( m_prefix ) );

    // Inherit the include in bill of materials and board netlist settings from flattened
    // library symbol.
    // UNUSED_SYMBOL: SIM functionality disabled
    // m_excludedFromSim = m_part->GetExcludedFromSim();
    m_excludedFromSim = false; // Default to false when SIM disabled
    m_excludedFromBOM = m_part->GetExcludedFromBOM();
    m_excludedFromBoard = m_part->GetExcludedFromBoard();
}


SCH_SYMBOL::SCH_SYMBOL( const LIB_SYMBOL& aSymbol, const SCH_SHEET_PATH* aSheet,
                        const PICKED_SYMBOL& aSel, const VECTOR2I& aPosition, EDA_ITEM* aParent ) :
        SCH_SYMBOL( aSymbol, aSel.LibId, aSheet, aSel.Unit, aSel.Convert, aPosition, aParent )
{
    // Set any fields that were modified as part of the symbol selection
    for( const std::pair<int, QString>& i : aSel.Fields )
    {
        if( i.first == REFERENCE_FIELD )
            SetRef( aSheet, i.second );
        else if( SCH_FIELD* field = GetFieldById( i.first ) )
            field->SetText( i.second );
    }
}


SCH_SYMBOL::SCH_SYMBOL( const SCH_SYMBOL& aSymbol ) :
        SYMBOL( aSymbol )
{
    m_parent      = aSymbol.m_parent;
    m_pos         = aSymbol.m_pos;
    m_unit        = aSymbol.m_unit;
    m_bodyStyle   = aSymbol.m_bodyStyle;
    m_lib_id      = aSymbol.m_lib_id;
    m_isInNetlist = aSymbol.m_isInNetlist;
    m_DNP         = aSymbol.m_DNP;

    const_cast<KIID&>( m_Uuid ) = aSymbol.m_Uuid;

    m_transform = aSymbol.m_transform;
    m_prefix = aSymbol.m_prefix;
    m_instanceReferences = aSymbol.m_instanceReferences;
    m_fields = aSymbol.m_fields;

    // Re-parent the fields, which before this had aSymbol as parent
    for( SCH_FIELD& field : m_fields )
        field.SetParent( this );

    m_pins.clear();

    // Copy (and re-parent) the pins
    for( const std::unique_ptr<SCH_PIN>& pin : aSymbol.m_pins )
    {
        m_pins.emplace_back( std::make_unique<SCH_PIN>( *pin ) );
        m_pins.back()->SetParent( this );
    }

    if( aSymbol.m_part )
        SetLibSymbol( new LIB_SYMBOL( *aSymbol.m_part ) );

    m_fieldsAutoplaced = aSymbol.m_fieldsAutoplaced;
    m_schLibSymbolName = aSymbol.m_schLibSymbolName;
}


SCH_SYMBOL::~SCH_SYMBOL()
{
}


void SCH_SYMBOL::Init( const VECTOR2I& pos )
{
    m_layer     = LAYER_DEVICE;
    m_pos       = pos;
    m_unit      = 1;  // In multi unit chip - which unit to draw.
    m_bodyStyle = BODY_STYLE::BASE;  // De Morgan Handling

    // The rotation/mirror transformation matrix. pos normal
    m_transform = TRANSFORM();

    auto addField =
            [&]( int id, SCH_LAYER_ID layer )
            {
                m_fields.emplace_back( pos, id, this, GetCanonicalFieldName( id ) );
                m_fields.back().SetLayer( layer );
            };

    // construct only the mandatory fields
    addField( REFERENCE_FIELD, LAYER_REFERENCEPART );
    addField( VALUE_FIELD, LAYER_VALUEPART );
    addField( FOOTPRINT_FIELD, LAYER_FIELDS );
    addField( DATASHEET_FIELD, LAYER_FIELDS );
    addField( DESCRIPTION_FIELD, LAYER_FIELDS );

    m_prefix = QString( "U" );
    m_isInNetlist = true;
}


EDA_ITEM* SCH_SYMBOL::Clone() const
{
    return new SCH_SYMBOL( *this );
}


bool SCH_SYMBOL::IsMissingLibSymbol() const
{
    return m_part == nullptr;
}


bool SCH_SYMBOL::IsMovableFromAnchorPoint() const
{
    // If a symbol's anchor is not grid-aligned to its pins then moving from the anchor is
    // going to end up moving the symbol's pins off-grid.

    // The minimal grid size allowed to place a pin is 25 mils
    const int min_grid_size = schIUScale.MilsToIU( 25 );

    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        if( ( ( pin->GetPosition().x - m_pos.x ) % min_grid_size ) != 0 )
            return false;

        if( ( ( pin->GetPosition().y - m_pos.y ) % min_grid_size ) != 0 )
            return false;
    }

    return true;
}


void SCH_SYMBOL::SetLibId( const LIB_ID& aLibId )
{
    m_lib_id = aLibId;
}


QString SCH_SYMBOL::GetSchSymbolLibraryName() const
{
    if( !m_schLibSymbolName.isEmpty() )
        return m_schLibSymbolName;
    else
        return m_lib_id.Format();
}


void SCH_SYMBOL::SetLibSymbol( LIB_SYMBOL* aLibSymbol )
{
    Q_ASSERT( !aLibSymbol || aLibSymbol->IsRoot() ); // aLibSymbol = nullptr on failure

    m_part.reset( aLibSymbol );
    UpdatePins();
}


QString SCH_SYMBOL::GetDescription() const
{
    if( m_part )
        return m_part->GetDescription();

    return QString();
}


QString SCH_SYMBOL::GetKeyWords() const
{
    if( m_part )
        return m_part->GetKeyWords();

    return QString();
}


QString SCH_SYMBOL::GetDatasheet() const
{
    if( m_part )
        return m_part->GetDatasheetField().GetText();

    return QString();
}


void SCH_SYMBOL::UpdatePins()
{
    std::map<QString, QString>            altPinMap;
    std::map<QString, std::set<SCH_PIN*>>  pinUuidMap;
    std::set<SCH_PIN*>                      unassignedSchPins;
    std::set<SCH_PIN*>                      unassignedLibPins;

    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        pinUuidMap[ pin->GetNumber() ].insert( pin.get() );

        unassignedSchPins.insert( pin.get() );

        if( !pin->GetAlt().isEmpty() )
            altPinMap[ pin->GetNumber() ] = pin->GetAlt();

        pin->SetLibPin( nullptr );
    }

    m_pinMap.clear();

    if( !m_part )
        return;

    for( SCH_PIN* libPin : m_part->GetPins() )
    {
        // NW: Don't filter by unit: this data-structure is used for all instances,
        // some of which might have different units.
        if( libPin->GetBodyStyle() && m_bodyStyle && m_bodyStyle != libPin->GetBodyStyle() )
            continue;

        SCH_PIN* pin = nullptr;

        auto ii = pinUuidMap.find( libPin->GetNumber() );

        if( ii == pinUuidMap.end() || ii->second.empty() )
        {
            unassignedLibPins.insert( libPin );
            continue;
        }

        auto it = ii->second.begin();
        pin = *it;
        ii->second.erase( it );
        pin->SetLibPin( libPin );
        pin->SetPosition( libPin->GetPosition() );

        unassignedSchPins.erase( pin );

        auto iii = altPinMap.find( libPin->GetNumber() );

        if( iii != altPinMap.end() )
            pin->SetAlt( iii->second );

        m_pinMap[ libPin ] = pin;
    }

    // Add any pins that were not found in the symbol
    for( SCH_PIN* libPin : unassignedLibPins )
    {
        SCH_PIN* pin = nullptr;

        // First try to re-use an existing pin
        if( !unassignedSchPins.empty() )
        {
            auto it = unassignedSchPins.begin();
            pin = *it;
            unassignedSchPins.erase( it );
        }
        else
        {
            // This is a pin that was not found in the symbol, so create a new one.
            pin = m_pins.emplace_back( std::make_unique<SCH_PIN>( SCH_PIN( this, libPin ) ) ).get();
        }

        m_pinMap[ libPin ] = pin;
        pin->SetLibPin( libPin );
        pin->SetPosition( libPin->GetPosition() );
        pin->SetNumber( libPin->GetNumber() );

        auto iii = altPinMap.find( libPin->GetNumber() );

        if( iii != altPinMap.end() )
            pin->SetAlt( iii->second );
    }

    // If we have any pins left in the symbol that were not found in the library, remove them.
    for( auto it1 = m_pins.begin(); it1 != m_pins.end() && !unassignedSchPins.empty(); )
    {
        auto it2 = unassignedSchPins.find( it1->get() );

        if( it2 != unassignedSchPins.end() )
        {
            it1 = m_pins.erase( it1 );
            unassignedSchPins.erase( it2 );
        }
        else
        {
            ++it1;
        }
    }

    // If the symbol is selected, then its pins are selected.
    if( IsSelected() )
    {
        for( std::unique_ptr<SCH_PIN>& pin : m_pins )
            pin->SetSelected();
    }

}


void SCH_SYMBOL::SetBodyStyleUnconditional( int aBodyStyle )
{
    if( m_bodyStyle != aBodyStyle )
    {
        m_bodyStyle = ( m_bodyStyle == BODY_STYLE::BASE ) ? BODY_STYLE::DEMORGAN
                                                          : BODY_STYLE::BASE;

        // The body style may have a different pin layout so the update the pin map.
        UpdatePins();
    }
}


void SCH_SYMBOL::SetBodyStyle( int aBodyStyle )
{
    if( HasAlternateBodyStyle() && m_bodyStyle != aBodyStyle )
    {
        m_bodyStyle = ( m_bodyStyle == BODY_STYLE::BASE ) ? BODY_STYLE::DEMORGAN
                                                          : BODY_STYLE::BASE;

        // The body style may have a different pin layout so the update the pin map.
        UpdatePins();
    }
}


bool SCH_SYMBOL::HasAlternateBodyStyle() const
{
    if( m_part )
        return m_part->HasAlternateBodyStyle();

    return false;
}


int SCH_SYMBOL::GetUnitCount() const
{
    if( m_part )
        return m_part->GetUnitCount();

    return 0;
}


QString SCH_SYMBOL::GetUnitDisplayName( int aUnit ) const
{
    Q_ASSERT( m_part ); // Return default if no part
    if( !m_part ) return QString::asprintf( "Unit %s", SubReference( aUnit ).toStdString().c_str() );

    return m_part->GetUnitDisplayName( aUnit );
}


bool SCH_SYMBOL::HasUnitDisplayName( int aUnit ) const
{
    Q_ASSERT( m_part );
    if( !m_part ) return false;

    return m_part->HasUnitDisplayName( aUnit );
}


void SCH_SYMBOL::PrintBackground( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                                  const VECTOR2I& aOffset, bool aDimmed )
{
    Q_ASSERT( m_part );
    if( !m_part ) return;

    SCH_RENDER_SETTINGS localRenderSettings( *aSettings );
    localRenderSettings.m_Transform = m_transform;
    localRenderSettings.m_ShowVisibleFields = false;
    localRenderSettings.m_ShowHiddenFields = false;

    if( GetDNP() )
        aDimmed = true;

    m_part->PrintBackground( &localRenderSettings, m_unit, m_bodyStyle, m_pos + aOffset, aDimmed );
}


void SCH_SYMBOL::Print( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                        const VECTOR2I& aOffset, bool aForceNoFill, bool aDimmed )
{
    SCH_RENDER_SETTINGS localRenderSettings( *aSettings );
    localRenderSettings.m_Transform = m_transform;
    localRenderSettings.m_ShowVisibleFields = false;
    localRenderSettings.m_ShowHiddenFields = false;

    if( m_DNP )
        aDimmed = true;

    if( m_part )
    {
        std::vector<SCH_PIN*> libPins = m_part->GetPins( m_unit, m_bodyStyle );
        LIB_SYMBOL            tempSymbol( *m_part );
        std::vector<SCH_PIN*> tempPins = tempSymbol.GetPins( m_unit, m_bodyStyle );

        // Copy the pin info from the symbol to the temp pins
        for( unsigned i = 0; i < tempPins.size(); ++ i )
        {
            SCH_PIN* symbolPin = GetPin( libPins[ i ] );
            SCH_PIN* tempPin = tempPins[ i ];

            tempPin->SetName( symbolPin->GetShownName() );
            tempPin->SetType( symbolPin->GetType() );
            tempPin->SetShape( symbolPin->GetShape() );
        }

        for( SCH_ITEM& item : tempSymbol.GetDrawItems() )
        {
            if( EDA_TEXT* text = dynamic_cast<EDA_TEXT*>( &item ) )
            {
                // Use SCH_FIELD's text resolver
                SCH_FIELD dummy( this, -1 );
                dummy.SetText( text->GetText() );
                text->SetText( dummy.GetShownText( false ) );
            }
        }

        tempSymbol.Print( &localRenderSettings, m_unit, m_bodyStyle, m_pos + aOffset, false,
                          aDimmed );
    }
    else // Use a dummy part if the actual cannot be found.
    {
        LIB_SYMBOL::GetDummy()->Print( &localRenderSettings, 0, 0, m_pos + aOffset, aForceNoFill,
                                       aDimmed );
    }

    for( SCH_FIELD& field : m_fields )
        field.Print( &localRenderSettings, m_unit, m_bodyStyle, aOffset, aForceNoFill, aDimmed );

    if( m_DNP )
    {
        QPainter* DC = localRenderSettings.GetPrintDC();
        BOX2I    bbox = GetBodyBoundingBox();
        BOX2I    pins = GetBodyAndPinsBoundingBox();
        QColor  dnp_color = localRenderSettings.GetLayerColor( LAYER_DNP_MARKER ).ToColor();
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


bool SCH_SYMBOL::GetInstance( SCH_SYMBOL_INSTANCE& aInstance,
                              const KIID_PATH& aSheetPath, bool aTestFromEnd ) const
{
    for( const SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
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


void SCH_SYMBOL::RemoveInstance( const SCH_SHEET_PATH& aInstancePath )
{
    RemoveInstance( aInstancePath.Path() );
}


void SCH_SYMBOL::RemoveInstance( const KIID_PATH& aInstancePath )
{
    // Search for an existing path and remove it if found
    // (search from back to avoid invalidating iterator on remove)
    for( int ii = m_instanceReferences.size() - 1; ii >= 0; --ii )
    {
        if( m_instanceReferences[ii].m_Path == aInstancePath )
        {
            qCDebug(logSchSheetPaths) << QString::asprintf( "Removing symbol instance:\n"
                                                 "    sheet path %s\n"
                                                 "    reference %s, unit %d from symbol %s.",
                        aInstancePath.AsString().toStdString().c_str(),
                        m_instanceReferences[ii].m_Reference.toStdString().c_str(),
                        m_instanceReferences[ii].m_Unit,
                        m_Uuid.AsString().toStdString().c_str() );

            m_instanceReferences.erase( m_instanceReferences.begin() + ii );
        }
    }
}


void SCH_SYMBOL::AddHierarchicalReference( const KIID_PATH& aPath, const QString& aRef, int aUnit )
{
    SCH_SYMBOL_INSTANCE instance;
    instance.m_Path = aPath;
    instance.m_Reference = aRef;
    instance.m_Unit = aUnit;

    AddHierarchicalReference( instance );
}


void SCH_SYMBOL::AddHierarchicalReference( const SCH_SYMBOL_INSTANCE& aInstance )
{
    RemoveInstance( aInstance.m_Path );

    SCH_SYMBOL_INSTANCE instance = aInstance;

    qCDebug(logSchSheetPaths) << QString::asprintf( "Adding symbol '%s' instance:\n"
                                         "    sheet path '%s'\n"
                                         "    reference '%s'\n"
                                         "    unit %d\n",
                m_Uuid.AsString().toStdString().c_str(),
                instance.m_Path.AsString().toStdString().c_str(),
                instance.m_Reference.toStdString().c_str(),
                instance.m_Unit );

    m_instanceReferences.push_back( instance );

    // This should set the default instance to the first saved instance data for each symbol
    // when importing sheets.
    if( m_instanceReferences.size() == 1 )
    {
        m_fields[ REFERENCE_FIELD ].SetText( instance.m_Reference );
        m_unit = instance.m_Unit;
    }
}


const QString SCH_SYMBOL::GetRef( const SCH_SHEET_PATH* sheet, bool aIncludeUnit ) const
{
    KIID_PATH path = sheet->Path();
    QString  ref;
    QString  subRef;

    for( const SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
    {
        if( instance.m_Path == path )
        {
            ref = instance.m_Reference;
            subRef = SubReference( instance.m_Unit );
            break;
        }
    }

    // If it was not found in m_Paths array, then see if it is in m_Field[REFERENCE] -- if so,
    // use this as a default for this path.  This will happen if we load a version 1 schematic
    // file.  It will also mean that multiple instances of the same sheet by default all have
    // the same symbol references, but perhaps this is best.
    if( ref.isEmpty() && !GetField( REFERENCE_FIELD )->GetText().isEmpty() )
        ref = GetField( REFERENCE_FIELD )->GetText();

    if( ref.isEmpty() )
        ref = UTIL::GetRefDesUnannotated( m_prefix );

    if( aIncludeUnit && GetUnitCount() > 1 )
        ref += subRef;

    return ref;
}


void SCH_SYMBOL::SetRefProp( const QString& aRef )
{
    FIELD_VALIDATOR validator( REFERENCE_FIELD );

    if( validator.DoValidate( aRef, nullptr ) )
        SetRef( &Schematic()->CurrentSheet(), aRef );
}


void SCH_SYMBOL::SetRef( const SCH_SHEET_PATH* sheet, const QString& ref )
{
    KIID_PATH path = sheet->Path();
    bool      found = false;

    // check to see if it is already there before inserting it
    for( SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
    {
        if( instance.m_Path == path )
        {
            found = true;
            instance.m_Reference = ref;
            break;
        }
    }

    if( !found )
        AddHierarchicalReference( path, ref, m_unit );

    for( std::unique_ptr<SCH_PIN>& pin : m_pins )
        pin->ClearDefaultNetName( sheet );

    if( Schematic() && *sheet == Schematic()->CurrentSheet() )
        m_fields[ REFERENCE_FIELD ].SetText( ref );

    // Reinit the m_prefix member if needed
    m_prefix = UTIL::GetRefDesPrefix( ref );

    if( m_prefix.isEmpty() )
        m_prefix = "U";

    // Power symbols have references starting with # and are not included in netlists
    m_isInNetlist = ! ref.startsWith( "#" );
}


bool SCH_SYMBOL::IsAnnotated( const SCH_SHEET_PATH* aSheet ) const
{
    KIID_PATH path = aSheet->Path();

    for( const SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
    {
        if( instance.m_Path == path )
            return !instance.m_Reference.isEmpty() && instance.m_Reference.back() != '?';
    }

    return false;
}


void SCH_SYMBOL::UpdatePrefix()
{
    QString refDesignator = GetField( REFERENCE_FIELD )->GetText();

    refDesignator.replace( "~", " " );

    QString prefix = refDesignator;

    while( prefix.length() )
    {
        QChar last = prefix.back();

        if( ( last >= '0' && last <= '9' ) || last == '?' || last == '*' )
            prefix.chop(1);
        else
            break;
    }

    // Avoid a prefix containing trailing/leading spaces
    prefix = prefix.trimmed();

    if( !prefix.isEmpty() )
        SetPrefix( prefix );
}


QString SCH_SYMBOL::SubReference( int aUnit, bool aAddSeparator ) const
{
    if( SCHEMATIC* schematic = Schematic() )
        return schematic->Settings().SubReference( aUnit, aAddSeparator );

    return LIB_SYMBOL::LetterSubReference( aUnit, 'A' );
}


int SCH_SYMBOL::GetUnitSelection( const SCH_SHEET_PATH* aSheet ) const
{
    KIID_PATH path = aSheet->Path();

    for( const SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
    {
        if( instance.m_Path == path )
            return instance.m_Unit;
    }

    // If it was not found in m_Paths array, then use m_unit.  This will happen if we load a
    // version 1 schematic file.
    return m_unit;
}


void SCH_SYMBOL::SetUnitSelection( const SCH_SHEET_PATH* aSheet, int aUnitSelection )
{
    KIID_PATH path = aSheet->Path();

    // check to see if it is already there before inserting it
    for( SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
    {
        if( instance.m_Path == path )
        {
            instance.m_Unit = aUnitSelection;
            return;
        }
    }

    // didn't find it; better add it
    AddHierarchicalReference( path, UTIL::GetRefDesUnannotated( m_prefix ), aUnitSelection );
}


void SCH_SYMBOL::SetUnitSelection( int aUnitSelection )
{
    for( SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
        instance.m_Unit = aUnitSelection;
}


const QString SCH_SYMBOL::GetValue( bool aResolve, const SCH_SHEET_PATH* aPath,
                                     bool aAllowExtraText ) const
{
    if( aResolve )
        return GetField( VALUE_FIELD )->GetShownText( aPath, aAllowExtraText );

    return GetField( VALUE_FIELD )->GetText();
}


void SCH_SYMBOL::SetValueFieldText( const QString& aValue )
{
    m_fields[ VALUE_FIELD ].SetText( aValue );
}


const QString SCH_SYMBOL::GetFootprintFieldText( bool aResolve, const SCH_SHEET_PATH* aPath,
                                                  bool aAllowExtraText ) const
{
    if( aResolve )
        return GetField( FOOTPRINT_FIELD )->GetShownText( aPath, aAllowExtraText );

    return GetField( FOOTPRINT_FIELD )->GetText();
}


void SCH_SYMBOL::SetFootprintFieldText( const QString& aFootprint )
{
    m_fields[ FOOTPRINT_FIELD ].SetText( aFootprint );
}


SCH_FIELD* SCH_SYMBOL::GetField( MANDATORY_FIELD_T aFieldType )
{
    return &m_fields[aFieldType];
}


const SCH_FIELD* SCH_SYMBOL::GetField( MANDATORY_FIELD_T aFieldType ) const
{
    return &m_fields[aFieldType];
}


SCH_FIELD* SCH_SYMBOL::GetFieldById( int aFieldId )
{
    for( SCH_FIELD& field : m_fields )
    {
        if( field.GetId() == aFieldId )
            return &field;
    }

    return nullptr;
}


SCH_FIELD* SCH_SYMBOL::GetFieldByName( const QString& aFieldName )
{
    for( SCH_FIELD& field : m_fields )
    {
        if( field.GetName() == aFieldName )
            return &field;
    }

    return nullptr;
}


const SCH_FIELD* SCH_SYMBOL::GetFieldByName( const QString& aFieldName ) const
{
    for( const SCH_FIELD& field : m_fields )
    {
        if( field.GetName() == aFieldName )
            return &field;
    }

    return nullptr;
}


void SCH_SYMBOL::GetFields( std::vector<SCH_FIELD*>& aVector, bool aVisibleOnly )
{
    for( SCH_FIELD& field : m_fields )
    {
        if( aVisibleOnly )
        {
            if( !field.IsVisible() || field.GetText().isEmpty() )
                continue;
        }

        aVector.push_back( &field );
    }
}


SCH_FIELD* SCH_SYMBOL::AddField( const SCH_FIELD& aField )
{
    m_fields.push_back( aField );
    return &m_fields.back();
}


void SCH_SYMBOL::RemoveField( const QString& aFieldName )
{
    for( unsigned ii = 0; ii < m_fields.size(); ++ii )
    {
        if( m_fields[ii].IsMandatory() )
            continue;

        if( aFieldName == m_fields[ii].GetName( false ) )
        {
            m_fields.erase( m_fields.begin() + ii );
            return;
        }
    }
}


SCH_FIELD* SCH_SYMBOL::FindField( const QString& aFieldName, bool aIncludeDefaultFields,
                                  bool aCaseInsensitive )
{
    for( SCH_FIELD& field : m_fields )
    {
        if( field.IsMandatory() && !aIncludeDefaultFields )
            continue;

        if( aCaseInsensitive )
        {
            if( aFieldName.toUpper() == field.GetName( false ).toUpper() )
                return &field;
        }
        else
        {
            if( aFieldName == field.GetName( false ) )
                return &field;
        }
    }

    return nullptr;
}


void SCH_SYMBOL::UpdateFields( const SCH_SHEET_PATH* aPath, bool aUpdateStyle, bool aUpdateRef,
                               bool aUpdateOtherFields, bool aResetRef, bool aResetOtherFields )
{
    if( m_part )
    {
        std::vector<SCH_FIELD*> fields;
        m_part->GetFields( fields );

        for( const SCH_FIELD* libField : fields )
        {
            int        id = libField->GetId();
            SCH_FIELD* schField;

            if( libField->IsMandatory() )
            {
                schField = GetFieldById( id );
            }
            else
            {
                schField = FindField( libField->GetCanonicalName() );

                if( !schField )
                {
                    SCH_FIELD newField( VECTOR2I( 0, 0 ), GetNextFieldId(), this,
                                        libField->GetCanonicalName() );
                    schField = AddField( newField );
                    schField->ImportValues( *libField );
                }
            }

            schField->SetPrivate( libField->IsPrivate() );

            if( aUpdateStyle )
            {
                schField->ImportValues( *libField );
                schField->SetTextPos( m_pos + libField->GetTextPos() );
            }

            if( id == REFERENCE_FIELD && aPath )
            {
                if( aResetRef )
                    SetRef( aPath, m_part->GetReferenceField().GetText() );
                else if( aUpdateRef )
                    SetRef( aPath, libField->GetText() );
            }
            else if( id == VALUE_FIELD )
            {
                SetValueFieldText( UnescapeString( libField->GetText() ) );
            }
            else if( id == FOOTPRINT_FIELD )
            {
                if( aResetOtherFields || aUpdateOtherFields )
                    SetFootprintFieldText( libField->GetText() );
            }
            else if( id == DATASHEET_FIELD )
            {
                if( aResetOtherFields )
                    schField->SetText( GetDatasheet() ); // alias-specific value
                else if( aUpdateOtherFields )
                    schField->SetText( libField->GetText() );
            }
            else
            {
                if( aResetOtherFields || aUpdateOtherFields )
                    schField->SetText( libField->GetText() );
            }
        }
    }
}


void SCH_SYMBOL::SyncOtherUnits( const SCH_SHEET_PATH& aSourceSheet, SCH_COMMIT& aCommit,
                                 PROPERTY_BASE* aProperty )
{
    bool updateValue = true;
    bool updateExclFromBOM = true;
    bool updateExclFromBoard = true;
    bool updateDNP = true;
    bool updateOtherFields = true;
    bool updatePins = true;

    if( aProperty )
    {
        updateValue = aProperty->Name() == "Value";
        updateExclFromBoard = aProperty->Name() == "Exclude From Board";
        updateExclFromBOM = aProperty->Name() == "Exclude From Bill of Materials";
        updateDNP = aProperty->Name() == "Do not Populate";
        updateOtherFields = false;
        updatePins = false;
    }

    if( !updateValue
            && !updateExclFromBOM
            && !updateExclFromBoard
            && !updateDNP
            && !updateOtherFields
            && !updatePins )
    {
        return;
    }

    // Keep fields other than the reference, include/exclude flags, and alternate pin assignments
    // in sync in multi-unit parts.
    if( GetUnitCount() > 1 && IsAnnotated( &aSourceSheet ) )
    {
        QString ref = GetRef( &aSourceSheet );

        for( SCH_SHEET_PATH& sheet : Schematic()->Hierarchy() )
        {
            SCH_SCREEN*              screen = sheet.LastScreen();
            std::vector<SCH_SYMBOL*> otherUnits;

            CollectOtherUnits( ref, m_unit, m_lib_id, sheet, &otherUnits );

            for( SCH_SYMBOL* otherUnit : otherUnits )
            {
                aCommit.Modify( otherUnit, screen );

                if( updateValue )
                    otherUnit->SetValueFieldText( GetField( VALUE_FIELD )->GetText() );

                if( updateOtherFields )
                {
                    otherUnit->SetFootprintFieldText( GetField( FOOTPRINT_FIELD )->GetText() );

                    for( size_t ii = DATASHEET_FIELD; ii < m_fields.size(); ++ii )
                    {
                        SCH_FIELD* otherField = otherUnit->FindField( m_fields[ii].GetName() );

                        if( otherField )
                        {
                            otherField->SetText( m_fields[ii].GetText() );
                        }
                        else
                        {
                            SCH_FIELD newField( m_fields[ii] );
                            const_cast<KIID&>( newField.m_Uuid ) = KIID();

                            newField.Offset( -GetPosition() );
                            newField.Offset( otherUnit->GetPosition() );

                            newField.SetParent( otherUnit );
                            otherUnit->AddField( newField );
                        }
                    }

                    for( size_t ii = otherUnit->GetFields().size() - 1; ii > DATASHEET_FIELD; ii-- )
                    {
                        SCH_FIELD& otherField = otherUnit->GetFields().at( ii );

                        if( !FindField( otherField.GetName() ) )
                            otherUnit->GetFields().erase( otherUnit->GetFields().begin() + ii );
                    }
                }

                if( updateExclFromBOM )
                    otherUnit->SetExcludedFromBOM( m_excludedFromBOM );

                if( updateExclFromBoard )
                    otherUnit->SetExcludedFromBoard( m_excludedFromBoard );

                if( updateDNP )
                    otherUnit->SetDNP( m_DNP );

                if( updatePins )
                {
                    for( const std::unique_ptr<SCH_PIN>& model_pin : m_pins )
                    {
                        SCH_PIN* src_pin = otherUnit->GetPin( model_pin->GetNumber() );

                        if( src_pin )
                            src_pin->SetAlt( model_pin->GetAlt() );
                    }
                }
            }
        }
    }
}


void SCH_SYMBOL::RunOnChildren( const std::function<void( SCH_ITEM* )>& aFunction )
{
    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
        aFunction( pin.get() );

    for( SCH_FIELD& field : m_fields )
        aFunction( &field );
}


SCH_PIN* SCH_SYMBOL::GetPin( const QString& aNumber ) const
{
    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        if( pin->GetNumber() == aNumber )
            return pin.get();
    }

    return nullptr;
}


const SCH_PIN* SCH_SYMBOL::GetPin( const VECTOR2I& aPos ) const
{
    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        int pin_unit      = pin->GetLibPin() ? pin->GetLibPin()->GetUnit()
                                             : GetUnit();
        int pin_bodyStyle = pin->GetLibPin() ? pin->GetLibPin()->GetBodyStyle()
                                             : GetBodyStyle();

        if( pin_unit > 0 && pin_unit != GetUnit() )
            continue;

        if( pin_bodyStyle > 0 && pin_bodyStyle != GetBodyStyle() )
            continue;

        if( pin->GetPosition() == aPos )
            return pin.get();
    }

    return nullptr;
}


std::vector<SCH_PIN*> SCH_SYMBOL::GetLibPins() const
{
    if( m_part )
        return m_part->GetPins( m_unit, m_bodyStyle );

    return std::vector<SCH_PIN*>();
}


std::vector<SCH_PIN*> SCH_SYMBOL::GetAllLibPins() const
{
    if( m_part )
        return m_part->GetPins();

    return std::vector<SCH_PIN*>();
}


size_t SCH_SYMBOL::GetFullPinCount() const
{
    return m_part ? m_part->GetPinCount() : 0;
}


SCH_PIN* SCH_SYMBOL::GetPin( SCH_PIN* aLibPin ) const
{
    auto it = m_pinMap.find( aLibPin );

    if( it != m_pinMap.end() )
        return it->second;

    Q_ASSERT_X( false, __FUNCTION__, "Pin not found" );
    return nullptr;
}


std::vector<SCH_PIN*> SCH_SYMBOL::GetPins( const SCH_SHEET_PATH* aSheet ) const
{
    std::vector<SCH_PIN*> pins;
    int                   unit = m_unit;

    if( !aSheet && Schematic() )
        aSheet = &Schematic()->CurrentSheet();

    if( aSheet )
        unit = GetUnitSelection( aSheet );

    for( const std::unique_ptr<SCH_PIN>& p : m_pins )
    {
        if( unit && p->GetLibPin() && p->GetLibPin()->GetUnit()
            && ( p->GetLibPin()->GetUnit() != unit ) )
        {
            continue;
        }

        pins.push_back( p.get() );
    }

    return pins;
}


std::vector<SCH_PIN*> SCH_SYMBOL::GetPins() const
{
    return GetPins( nullptr );
}


void SCH_SYMBOL::SwapData( SCH_ITEM* aItem )
{
    SCH_ITEM::SwapFlags( aItem );

    Q_ASSERT( aItem != nullptr && aItem->Type() == SCH_SYMBOL_T );
    if( !aItem || aItem->Type() != SCH_SYMBOL_T ) return; // Cannot swap data with invalid symbol

    SCH_SYMBOL* symbol = static_cast<SCH_SYMBOL*>( aItem );

    std::swap( m_lib_id, symbol->m_lib_id );

    m_pins.swap( symbol->m_pins );      // std::vector's swap()

    for( std::unique_ptr<SCH_PIN>& pin : symbol->m_pins )
        pin->SetParent( symbol );

    for( std::unique_ptr<SCH_PIN>& pin : m_pins )
        pin->SetParent( this );

    LIB_SYMBOL* libSymbol = symbol->m_part.release();
    symbol->m_part = std::move( m_part );
    symbol->UpdatePins();
    m_part.reset( libSymbol );
    UpdatePins();

    std::swap( m_pos, symbol->m_pos );
    std::swap( m_unit, symbol->m_unit );
    std::swap( m_bodyStyle, symbol->m_bodyStyle );

    m_fields.swap( symbol->m_fields );    // std::vector's swap()

    for( SCH_FIELD& field : symbol->m_fields )
        field.SetParent( symbol );

    for( SCH_FIELD& field : m_fields )
        field.SetParent( this );

    TRANSFORM tmp = m_transform;

    m_transform = symbol->m_transform;
    symbol->m_transform = tmp;

    std::swap( m_excludedFromSim, symbol->m_excludedFromSim );
    std::swap( m_excludedFromBOM, symbol->m_excludedFromBOM );
    std::swap( m_DNP, symbol->m_DNP );
    std::swap( m_excludedFromBoard, symbol->m_excludedFromBoard );

    std::swap( m_instanceReferences, symbol->m_instanceReferences );
    std::swap( m_schLibSymbolName, symbol->m_schLibSymbolName );
}


void SCH_SYMBOL::GetContextualTextVars( QStringList* aVars ) const
{
    for( const SCH_FIELD& field : m_fields )
    {
        if( field.IsPrivate() )
            continue;

        if( field.IsMandatory() )
            aVars->append( field.GetCanonicalName().toUpper() );
        else
            aVars->append( field.GetName() );
    }

    aVars->append( "OP" );
    aVars->append( "FOOTPRINT_LIBRARY" );
    aVars->append( "FOOTPRINT_NAME" );
    aVars->append( "UNIT" );
    aVars->append( "SHORT_REFERENCE" );
    aVars->append( "SYMBOL_LIBRARY" );
    aVars->append( "SYMBOL_NAME" );
    aVars->append( "SYMBOL_DESCRIPTION" );
    aVars->append( "SYMBOL_KEYWORDS" );
    aVars->append( "EXCLUDE_FROM_BOM" );
    aVars->append( "EXCLUDE_FROM_BOARD" );
    aVars->append( "EXCLUDE_FROM_SIM" );
    aVars->append( "DNP" );
    aVars->append( "SHORT_NET_NAME(<pin_number>)" );
    aVars->append( "NET_NAME(<pin_number>)" );
    aVars->append( "NET_CLASS(<pin_number>)" );
    aVars->append( "PIN_NAME(<pin_number>)" );
}


bool SCH_SYMBOL::ResolveTextVar( const SCH_SHEET_PATH* aPath, QString* token, int aDepth ) const
{
    static QRegularExpression operatingPoint( "^"
                                        "OP"
                                        "(:[^.]*)?"      // pin
                                        "(.([0-9])?"     // precisionStr
                                        "([a-zA-Z]*))?"  // rangeStr
                                        "$" );

    Q_ASSERT( aPath );
    if( !aPath ) return false;

    SCHEMATIC* schematic = Schematic();

    if( !schematic )
        return false;

    // UNUSED_SYMBOL: SIM functionality disabled - operating point resolution removed during Qt transformation

    if( token->contains( ':' ) )
    {
        if( schematic->ResolveCrossReference( token, aDepth + 1 ) )
            return true;
    }

    QString upperToken = token->toUpper();

    for( const SCH_FIELD& field : m_fields )
    {
        QString fieldName = field.IsMandatory() ? field.GetCanonicalName()
                                                 : field.GetName();

        QString textToken = field.GetText();
        textToken.replace( " ", QString() );
        QString tokenString = "${" + fieldName + "}";

        // If the field data is just a reference to the field, don't resolve
        if( textToken.compare( tokenString, Qt::CaseInsensitive ) == 0 )
            return true;

        if( token->compare( fieldName, Qt::CaseInsensitive ) == 0 )
        {
            if( field.GetId() == REFERENCE_FIELD )
                *token = GetRef( aPath, true );
            else
                *token = field.GetShownText( aPath, false, aDepth + 1 );

            return true;
        }
    }

    // Consider missing simulation fields as empty, not un-resolved
    if( token->compare( "SIM.DEVICE", Qt::CaseInsensitive ) == 0
            || token->compare( "SIM.TYPE", Qt::CaseInsensitive ) == 0
            || token->compare( "SIM.PINS", Qt::CaseInsensitive ) == 0
            || token->compare( "SIM.PARAMS", Qt::CaseInsensitive ) == 0
            || token->compare( "SIM.LIBRARY", Qt::CaseInsensitive ) == 0
            || token->compare( "SIM.NAME", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString();
        return true;
    }

    for( const TEMPLATE_FIELDNAME& templateFieldname :
            schematic->Settings().m_TemplateFieldNames.GetTemplateFieldNames() )
    {
        if( token->compare( templateFieldname.m_Name, Qt::CaseInsensitive ) == 0
            || token->compare( templateFieldname.m_Name.toUpper(), Qt::CaseInsensitive ) == 0 )
        {
            // If we didn't find it in the fields list then it isn't set on this symbol.
            // Just return an empty string.
            *token = QString();
            return true;
        }
    }

    if( token->compare( "FOOTPRINT_LIBRARY", Qt::CaseInsensitive ) == 0 )
    {
        QString footprint = GetFootprintFieldText( true, aPath, false );

        QStringList parts = footprint.split( ':' );

        if( parts.count() > 0 )
            *token = parts[ 0 ];
        else
            *token = QString();

        return true;
    }
    else if( token->compare( "FOOTPRINT_NAME", Qt::CaseInsensitive ) == 0 )
    {
        QString footprint = GetFootprintFieldText( true, aPath, false );

        QStringList parts = footprint.split( ':' );

        if( parts.count() > 1 )
            *token = parts[ std::min( 1, (int) parts.size() - 1 ) ];
        else
            *token = QString();

        return true;
    }
    else if( token->compare( "UNIT", Qt::CaseInsensitive ) == 0 )
    {
        *token = SubReference( GetUnitSelection( aPath ) );
        return true;
    }
    else if( token->compare( "SHORT_REFERENCE", Qt::CaseInsensitive ) == 0 )
    {
        *token = GetRef( aPath, false );
        return true;
    }
    else if( token->compare( "SYMBOL_LIBRARY", Qt::CaseInsensitive ) == 0 )
    {
        *token = m_lib_id.GetUniStringLibNickname();
        return true;
    }
    else if( token->compare( "SYMBOL_NAME", Qt::CaseInsensitive ) == 0 )
    {
        *token = m_lib_id.GetUniStringLibItemName();
        return true;
    }
    else if( token->compare( "SYMBOL_DESCRIPTION", Qt::CaseInsensitive ) == 0 )
    {
        *token = GetDescription();
        return true;
    }
    else if( token->compare( "SYMBOL_KEYWORDS", Qt::CaseInsensitive ) == 0 )
    {
        *token = GetKeyWords();
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

        // UNUSED_SYMBOL: SIM functionality disabled
        // if( aPath->GetExcludedFromSim() || this->GetExcludedFromSim() )
        //     *token = _( "Excluded from simulation" );

        return true;
    }
    else if( token->compare( "DNP", Qt::CaseInsensitive ) == 0 )
    {
        *token = QString();

        if( aPath->GetDNP() || this->GetDNP() )
            *token = _( "DNP" );

        return true;
    }
    else if( token->startsWith( "SHORT_NET_NAME(" )
                 || token->startsWith( "NET_NAME(" )
                 || token->startsWith( "NET_CLASS(" )
                 || token->startsWith( "PIN_NAME(" ) )
    {
        QString pinNumber = token->mid( token->indexOf( '(' ) + 1 );
        pinNumber = pinNumber.left( pinNumber.lastIndexOf( ')' ) );

        for( SCH_PIN* pin : GetPins( aPath ) )
        {
            if( pin->GetNumber() == pinNumber )
            {
                if( token->startsWith( "PIN_NAME" ) )
                {
                    *token = pin->GetAlt().isEmpty() ? pin->GetName() : pin->GetAlt();
                    return true;
                }

                SCH_CONNECTION* conn = pin->Connection( aPath );

                if( !conn )
                    *token = QString();
                else if( token->startsWith( "SHORT_NET_NAME" ) )
                    *token = conn->LocalName();
                else if( token->startsWith( "NET_NAME" ) )
                    *token = conn->Name();
                else if( token->startsWith( "NET_CLASS" ) )
                    *token = pin->GetEffectiveNetClass( aPath )->GetName();

                return true;
            }
        }
    }

    // See if parent can resolve it (this will recurse to ancestors)
    if( aPath->Last() && aPath->Last()->ResolveTextVar( aPath, token, aDepth + 1 ) )
        return true;

    return false;
}


void SCH_SYMBOL::ClearAnnotation( const SCH_SHEET_PATH* aSheetPath, bool aResetPrefix )
{
    if( aSheetPath )
    {
        KIID_PATH path = aSheetPath->Path();

        for( SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
        {
            if( instance.m_Path == path )
            {
                if( instance.m_Reference.isEmpty() || aResetPrefix )
                    instance.m_Reference = UTIL::GetRefDesUnannotated( m_prefix );
                else
                    instance.m_Reference = UTIL::GetRefDesUnannotated( instance.m_Reference );
            }
        }
    }
    else
    {
        for( SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
        {
            if( instance.m_Reference.isEmpty() || aResetPrefix)
                instance.m_Reference = UTIL::GetRefDesUnannotated( m_prefix );
            else
                instance.m_Reference = UTIL::GetRefDesUnannotated( instance.m_Reference );
        }
    }

    for( std::unique_ptr<SCH_PIN>& pin : m_pins )
        pin->ClearDefaultNetName( aSheetPath );

    // These 2 changes do not work in complex hierarchy.
    // When a clear annotation is made, the calling function must call a
    // UpdateAllScreenReferences for the active sheet.
    // But this call cannot made here.
    QString currentReference = m_fields[REFERENCE_FIELD].GetText();

    if( currentReference.isEmpty() || aResetPrefix )
        m_fields[REFERENCE_FIELD].SetText( UTIL::GetRefDesUnannotated( m_prefix ) );
    else
        m_fields[REFERENCE_FIELD].SetText( UTIL::GetRefDesUnannotated( currentReference ) );
}


bool SCH_SYMBOL::AddSheetPathReferenceEntryIfMissing( const KIID_PATH& aSheetPath )
{
    // An empty sheet path is illegal, at a minimum the root sheet UUID must be present.
    Q_ASSERT( aSheetPath.size() > 0 );
    if( aSheetPath.size() == 0 ) return false;

    for( const SCH_SYMBOL_INSTANCE& instance : m_instanceReferences )
    {
        // if aSheetPath is found, nothing to do:
        if( instance.m_Path == aSheetPath )
            return false;
    }

    // This entry does not exist: add it, with its last-used reference
    AddHierarchicalReference( aSheetPath, m_fields[REFERENCE_FIELD].GetText(), m_unit );
    return true;
}


void SCH_SYMBOL::SetOrientation( int aOrientation )
{
    TRANSFORM temp = TRANSFORM();
    bool transform = false;

    switch( aOrientation )
    {
    case SYM_ORIENT_0:
    case SYM_NORMAL:                    // default transform matrix
        m_transform = TRANSFORM();
        break;

    case SYM_ROTATE_COUNTERCLOCKWISE:  // Rotate + (incremental rotation)
        temp.x1   = 0;
        temp.y1   = 1;
        temp.x2   = -1;
        temp.y2   = 0;
        transform = true;
        break;

    case SYM_ROTATE_CLOCKWISE:          // Rotate - (incremental rotation)
        temp.x1   = 0;
        temp.y1   = -1;
        temp.x2   = 1;
        temp.y2   = 0;
        transform = true;
        break;

    case SYM_MIRROR_Y:                  // Mirror Y (incremental transform)
        temp.x1   = -1;
        temp.y1   = 0;
        temp.x2   = 0;
        temp.y2   = 1;
        transform = true;
        break;

    case SYM_MIRROR_X:                  // Mirror X (incremental transform)
        temp.x1   = 1;
        temp.y1   = 0;
        temp.x2   = 0;
        temp.y2   = -1;
        transform = true;
        break;

    case SYM_ORIENT_90:
        SetOrientation( SYM_ORIENT_0 );
        SetOrientation( SYM_ROTATE_COUNTERCLOCKWISE );
        break;

    case SYM_ORIENT_180:
        SetOrientation( SYM_ORIENT_0 );
        SetOrientation( SYM_ROTATE_COUNTERCLOCKWISE );
        SetOrientation( SYM_ROTATE_COUNTERCLOCKWISE );
        break;

    case SYM_ORIENT_270:
        SetOrientation( SYM_ORIENT_0 );
        SetOrientation( SYM_ROTATE_CLOCKWISE );
        break;

    case ( SYM_ORIENT_0 + SYM_MIRROR_X ):
        SetOrientation( SYM_ORIENT_0 );
        SetOrientation( SYM_MIRROR_X );
        break;

    case ( SYM_ORIENT_0 + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_0 );
        SetOrientation( SYM_MIRROR_Y );
        break;

    case ( SYM_ORIENT_0 + SYM_MIRROR_X + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_0 );
        SetOrientation( SYM_MIRROR_X );
        SetOrientation( SYM_MIRROR_Y );
        break;

    case ( SYM_ORIENT_90 + SYM_MIRROR_X ):
        SetOrientation( SYM_ORIENT_90 );
        SetOrientation( SYM_MIRROR_X );
        break;

    case ( SYM_ORIENT_90 + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_90 );
        SetOrientation( SYM_MIRROR_Y );
        break;

    case ( SYM_ORIENT_90 + SYM_MIRROR_X + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_90 );
        SetOrientation( SYM_MIRROR_X );
        SetOrientation( SYM_MIRROR_Y );
        break;

    case ( SYM_ORIENT_180 + SYM_MIRROR_X ):
        SetOrientation( SYM_ORIENT_180 );
        SetOrientation( SYM_MIRROR_X );
        break;

    case ( SYM_ORIENT_180 + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_180 );
        SetOrientation( SYM_MIRROR_Y );
        break;

    case ( SYM_ORIENT_180 + SYM_MIRROR_X + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_180 );
        SetOrientation( SYM_MIRROR_X );
        SetOrientation( SYM_MIRROR_Y );
        break;

    case ( SYM_ORIENT_270 + SYM_MIRROR_X ):
        SetOrientation( SYM_ORIENT_270 );
        SetOrientation( SYM_MIRROR_X );
        break;

    case ( SYM_ORIENT_270 + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_270 );
        SetOrientation( SYM_MIRROR_Y );
        break;

    case ( SYM_ORIENT_270 + SYM_MIRROR_X + SYM_MIRROR_Y ):
        SetOrientation( SYM_ORIENT_270 );
        SetOrientation( SYM_MIRROR_X );
        SetOrientation( SYM_MIRROR_Y );
        break;

    default:
        transform = false;
        Q_ASSERT_X( false, __FUNCTION__, "Invalid schematic symbol orientation type." );
        break;
    }

    if( transform )
    {
        /* The new matrix transform is the old matrix transform modified by the
         *  requested transformation, which is the temp transform (rot,
         *  mirror ..) in order to have (in term of matrix transform):
         *     transform coord = new_m_transform * coord
         *  where transform coord is the coord modified by new_m_transform from
         *  the initial value coord.
         *  new_m_transform is computed (from old_m_transform and temp) to
         *  have:
         *     transform coord = old_m_transform * temp
         */
        TRANSFORM newTransform;

        newTransform.x1 = m_transform.x1 * temp.x1 + m_transform.x2 * temp.y1;
        newTransform.y1 = m_transform.y1 * temp.x1 + m_transform.y2 * temp.y1;
        newTransform.x2 = m_transform.x1 * temp.x2 + m_transform.x2 * temp.y2;
        newTransform.y2 = m_transform.y1 * temp.x2 + m_transform.y2 * temp.y2;
        m_transform = newTransform;
    }
}


int SCH_SYMBOL::GetOrientation() const
{
    /*
     * This is slow, but also a bizarre algorithm.  I don't feel like unteasing the algorithm right
     * now, so let's just cache it for the moment.
     */
    if( s_transformToOrientationCache.count( m_transform ) )
        return s_transformToOrientationCache.at( m_transform );

    int rotate_values[] =
    {
        SYM_ORIENT_0,
        SYM_ORIENT_90,
        SYM_ORIENT_180,
        SYM_ORIENT_270,
        SYM_MIRROR_X + SYM_ORIENT_0,
        SYM_MIRROR_X + SYM_ORIENT_90,
        SYM_MIRROR_X + SYM_ORIENT_270,
        SYM_MIRROR_Y,
        SYM_MIRROR_Y + SYM_ORIENT_0,
        SYM_MIRROR_Y + SYM_ORIENT_90,
        SYM_MIRROR_Y + SYM_ORIENT_180,
        SYM_MIRROR_Y + SYM_ORIENT_270
    };

    // Try to find the current transform option:
    TRANSFORM transform = m_transform;
    SCH_SYMBOL temp( *this );

    for( int type_rotate : rotate_values )
    {
        temp.SetOrientation( type_rotate );

        if( transform == temp.GetTransform() )
        {
            s_transformToOrientationCache[m_transform] = type_rotate;
            return type_rotate;
        }
    }

    // Error: orientation not found in list (should not happen)
    Q_ASSERT_X( false, __FUNCTION__, "Schematic symbol orientation matrix internal error." );

    return SYM_NORMAL;
}


#if defined(DEBUG)

void SCH_SYMBOL::Show( int nestLevel, std::ostream& os ) const
{
    // for now, make it look like XML:
    NestedSpace( nestLevel, os ) << '<' << GetClass().Lower().mb_str()
                                 << " ref=\"" << TO_UTF8( GetField( REFERENCE_FIELD )->GetName() )
                                 << '"' << " chipName=\""
                                 << GetLibId().Format().toStdString().c_str() << '"' << m_pos
                                 << " layer=\"" << m_layer
                                 << '"' << ">\n";

    // skip the reference, it's been output already.
    for( int i = 1; i < GetFieldCount();  ++i )
    {
        const QString& value = GetFields()[i].GetText();

        if( !value.isEmpty() )
        {
            NestedSpace( nestLevel + 1, os ) << "<field" << " name=\""
                                             << TO_UTF8( GetFields()[i].GetName() )
                                             << '"' << " value=\""
                                             << TO_UTF8( value ) << "\"/>\n";
        }
    }

    NestedSpace( nestLevel, os ) << "</" << TO_UTF8( GetClass().Lower() ) << ">\n";
}

#endif


BOX2I SCH_SYMBOL::doGetBoundingBox( bool aIncludePins, bool aIncludeFields ) const
{
    BOX2I    bBox;

    if( m_part )
        bBox = m_part->GetBodyBoundingBox( m_unit, m_bodyStyle, aIncludePins, false );
    else
        bBox = LIB_SYMBOL::GetDummy()->GetBodyBoundingBox( m_unit, m_bodyStyle, aIncludePins,
                                                           false );

    bBox = m_transform.TransformCoordinate( bBox );
    bBox.Normalize();

    bBox.Offset( m_pos );

    if( aIncludeFields )
    {
        for( const SCH_FIELD& field : m_fields )
        {
            if( field.IsVisible() )
                bBox.Merge( field.GetBoundingBox() );
        }
    }

    return bBox;
}


BOX2I SCH_SYMBOL::GetBodyBoundingBox() const
{
    try
    {
        return doGetBoundingBox( false, false );
    }
    catch( const boost::bad_pointer& e )
    {
        Q_ASSERT_X( false, __FUNCTION__, QString::asprintf( "Boost pointer exception occurred: %s", e.what() ).toStdString().c_str() );
        return BOX2I();
    }
}


BOX2I SCH_SYMBOL::GetBodyAndPinsBoundingBox() const
{
    return doGetBoundingBox( true, false );
}


const BOX2I SCH_SYMBOL::GetBoundingBox() const
{
    return doGetBoundingBox( true, true );
}


void SCH_SYMBOL::GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList )
{
    QString msg;

    SCH_EDIT_FRAME* schframe = dynamic_cast<SCH_EDIT_FRAME*>( aFrame );
    // UNUSED_SYMBOL: GetCurrentSheet - sheet path assignment commented out due to unused symbol
    // SCH_SHEET_PATH* currentSheet = schframe ? &schframe->GetCurrentSheet() : nullptr;
    SCH_SHEET_PATH* currentSheet = nullptr;

    auto addExcludes =
            [&]()
            {
                QStringList msgs;

                // UNUSED_SYMBOL: SIM functionality disabled
                // if( GetExcludedFromSim() )
                //     msgs.Add( _( "Simulation" ) );

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
            };

    // part and alias can differ if alias is not the root
    if( m_part )
    {
        if( m_part.get() != LIB_SYMBOL::GetDummy() )
        {
            if( m_part->IsPower() )
            {
                // Don't use GetShownText(); we want to see the variable references here
                // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
                // aList.emplace_back( _( "Power symbol" ),
                //                     KIUI::EllipsizeStatusText( aFrame,
                //                                                GetField( VALUE_FIELD )->GetText() ) );
                aList.emplace_back( _( "Power symbol" ), GetField( VALUE_FIELD )->GetText() ); // Direct text without ellipsis
            }
            else
            {
                aList.emplace_back( _( "Reference" ),
                                    UnescapeString( GetRef( currentSheet ) ) );

                // Don't use GetShownText(); we want to see the variable references here
                // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
                // aList.emplace_back( _( "Value" ),
                //                     KIUI::EllipsizeStatusText( aFrame,
                //                                                GetField( VALUE_FIELD )->GetText() ) );
                aList.emplace_back( _( "Value" ), GetField( VALUE_FIELD )->GetText() ); // Direct text without ellipsis
                addExcludes();
                // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
                // aList.emplace_back( _( "Name" ),
                //                     KIUI::EllipsizeStatusText( aFrame,
                //                                                GetLibId().GetLibItemName() ) );
                aList.emplace_back( _( "Name" ), GetLibId().GetLibItemName() ); // Direct text without ellipsis
            }

#if 0       // Display symbol flags, for debug only
            aList.emplace_back( _( "flags" ), QString::asprintf( "%X", GetEditFlags() ) );
#endif

            if( !m_part->IsRoot() )
            {
                msg = _( "Missing parent" );

                std::shared_ptr< LIB_SYMBOL > parent = m_part->GetParent().lock();

                if( parent )
                    msg = parent->GetName();

                aList.emplace_back( _( "Derived from" ), UnescapeString( msg ) );
            }
            else if( !m_lib_id.GetLibNickname().empty() )
            {
                aList.emplace_back( _( "Library" ), m_lib_id.GetLibNickname() );
            }
            else
            {
                aList.emplace_back( _( "Library" ), _( "Undefined!!!" ) );
            }

            // Display the current associated footprint, if exists.
            // Don't use GetShownText(); we want to see the variable references here
            // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
            // msg = KIUI::EllipsizeStatusText( aFrame, GetField( FOOTPRINT_FIELD )->GetText() );
            msg = GetField( FOOTPRINT_FIELD )->GetText(); // Direct text without ellipsis

            if( msg.isEmpty() )
                msg = _( "<Unknown>" );

            aList.emplace_back( _( "Footprint" ), msg );

            // Display description of the symbol, and keywords found in lib
            aList.emplace_back( _( "Description" ) + ": "
                                        + GetField( DESCRIPTION_FIELD )->GetText(),
                                _( "Keywords" ) + ": " + m_part->GetKeyWords() );
        }
    }
    else
    {
        aList.emplace_back( _( "Reference" ), GetRef( currentSheet ) );

        // Don't use GetShownText(); we want to see the variable references here
        // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
        // aList.emplace_back( _( "Value" ),
        //                     KIUI::EllipsizeStatusText( aFrame,
        //                                                GetField( VALUE_FIELD )->GetText() ) );
        aList.emplace_back( _( "Value" ), GetField( VALUE_FIELD )->GetText() ); // Direct text without ellipsis
        addExcludes();
        // UNUSED_SYMBOL: KIUI::EllipsizeStatusText in unused_symbols.txt - Text ellipsis disabled
        // aList.emplace_back( _( "Name" ),
        //                     KIUI::EllipsizeStatusText( aFrame, GetLibId().GetLibItemName() ) );
        aList.emplace_back( _( "Name" ), GetLibId().GetLibItemName() ); // Direct text without ellipsis

        QString libNickname = GetLibId().GetLibNickname();

        if( libNickname.isEmpty() )
            msg = _( "No library defined!" );
        else
            msg = QString( _( "Symbol not found in %1!" ) ).arg( libNickname );

        aList.emplace_back( _( "Library" ), msg );
    }
}


BITMAPS SCH_SYMBOL::GetMenuImage() const
{
    return BITMAPS::add_component;
}


EMBEDDED_FILES* SCH_SYMBOL::GetEmbeddedFiles()
{
    std::unique_ptr<LIB_SYMBOL>& libSymbolRef = GetLibSymbolRef();

    if( !libSymbolRef )
        return nullptr;

    return GetLibSymbolRef()->GetEmbeddedFiles();
}


void SCH_SYMBOL::MirrorHorizontally( int aCenter )
{
    int dx = m_pos.x;

    SetOrientation( SYM_MIRROR_Y );
    MIRROR( m_pos.x, aCenter );
    dx -= m_pos.x;     // dx,0 is the move vector for this transform

    for( SCH_FIELD& field : m_fields )
    {
        // Move the fields to the new position because the symbol itself has moved.
        VECTOR2I pos = field.GetTextPos();
        pos.x -= dx;
        field.SetTextPos( pos );
    }
}


void SCH_SYMBOL::MirrorVertically( int aCenter )
{
    int dy = m_pos.y;

    SetOrientation( SYM_MIRROR_X );
    MIRROR( m_pos.y, aCenter );
    dy -= m_pos.y;     // 0,dy is the move vector for this transform

    for( SCH_FIELD& field : m_fields )
    {
        // Move the fields to the new position because the symbol itself has moved.
        VECTOR2I pos = field.GetTextPos();
        pos.y -= dy;
        field.SetTextPos( pos );
    }
}


void SCH_SYMBOL::Rotate( const VECTOR2I& aCenter, bool aRotateCCW )
{
    VECTOR2I prev = m_pos;

    RotatePoint( m_pos, aCenter, aRotateCCW ? ANGLE_90 : ANGLE_270 );

    SetOrientation( aRotateCCW ? SYM_ROTATE_COUNTERCLOCKWISE : SYM_ROTATE_CLOCKWISE );

    for( SCH_FIELD& field : m_fields )
    {
        // Move the fields to the new position because the symbol itself has moved.
        VECTOR2I pos = field.GetTextPos();
        pos.x -= prev.x - m_pos.x;
        pos.y -= prev.y - m_pos.y;
        field.SetTextPos( pos );
    }
}


bool SCH_SYMBOL::Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const
{
    for( SCH_ITEM& drawItem : GetLibSymbolRef()->GetDrawItems() )
    {
        if( drawItem.Matches( aSearchData, aAuxData ) )
            return true;
    }

    // Symbols are searchable via the child field and pin item text.
    return false;
}


void SCH_SYMBOL::GetEndPoints( std::vector <DANGLING_END_ITEM>& aItemList )
{
    for( std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        SCH_PIN* lib_pin = pin->GetLibPin();

        if( lib_pin && lib_pin->GetUnit() && m_unit && ( m_unit != lib_pin->GetUnit() ) )
            continue;

        DANGLING_END_ITEM item( PIN_END, lib_pin, GetPinPhysicalPosition( lib_pin ), this );
        aItemList.push_back( item );
    }
}


bool SCH_SYMBOL::UpdateDanglingState( std::vector<DANGLING_END_ITEM>& aItemListByType,
                                      std::vector<DANGLING_END_ITEM>& aItemListByPos,
                                      const SCH_SHEET_PATH*           aPath )
{
    bool changed = false;

    for( std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        bool previousState = pin->IsDangling();
        pin->SetIsDangling( true );

        VECTOR2I pos = m_transform.TransformCoordinate( pin->GetLocalPosition() ) + m_pos;

        auto lower = DANGLING_END_ITEM_HELPER::get_lower_pos( aItemListByPos, pos );
        bool do_break = false;

        for( auto it = lower; it < aItemListByPos.end() && it->GetPosition() == pos; it++ )
        {
            DANGLING_END_ITEM& each_item = *it;

            // Some people like to stack pins on top of each other in a symbol to indicate
            // internal connection. While technically connected, it is not particularly useful
            // to display them that way, so skip any pins that are in the same symbol as this
            // one.
            if( each_item.GetParent() == this )
                continue;

            switch( each_item.GetType() )
            {
            case PIN_END:
            case LABEL_END:
            case SHEET_LABEL_END:
            case WIRE_END:
            case NO_CONNECT_END:
            case JUNCTION_END:
                pin->SetIsDangling( false );
                do_break = true;
                break;

            default:
                break;
            }

            if( do_break )
                break;
        }

        changed = ( changed || ( previousState != pin->IsDangling() ) );
    }

    return changed;
}


VECTOR2I SCH_SYMBOL::GetPinPhysicalPosition( const SCH_PIN* aPin ) const
{
    if( ( aPin == nullptr ) || ( aPin->Type() != SCH_PIN_T ) )
        return VECTOR2I( 0, 0 );

    return m_transform.TransformCoordinate( aPin->GetPosition() ) + m_pos;
}


bool SCH_SYMBOL::HasConnectivityChanges( const SCH_ITEM* aItem,
                                         const SCH_SHEET_PATH* aInstance ) const
{
    // Do not compare to ourself.
    if( aItem == this )
        return false;

    const SCH_SYMBOL* symbol = dynamic_cast<const SCH_SYMBOL*>( aItem );

    // Don't compare against a different SCH_ITEM.
    Q_ASSERT( symbol );
    if( !symbol ) return false;

    // The move algorithm marks any pins that are being moved without something attached
    // (during the move) as dangling. We always need to recheck connectivity in this case
    // or we will not notice changes when the user places the symbol back in the same position
    // it started.
    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        if( pin->IsDangling() )
            return true;
    }

    if( GetPosition() != symbol->GetPosition() )
        return true;

    if( GetLibId() != symbol->GetLibId() )
        return true;

    if( GetUnitSelection( aInstance ) != symbol->GetUnitSelection( aInstance ) )
        return true;

    if( GetRef( aInstance ) != symbol->GetRef( aInstance ) )
        return true;

    // Power symbol value field changes are connectivity changes.
    if( IsPower()
      && ( GetValue( true, aInstance, false ) != symbol->GetValue( true, aInstance, false ) ) )
        return true;

    if( m_pins.size() != symbol->m_pins.size() )
        return true;

    for( size_t i = 0; i < m_pins.size(); i++ )
    {
        if( m_pins[i]->HasConnectivityChanges( symbol->m_pins[i].get() ) )
            return true;
    }

    return false;
}


std::vector<VECTOR2I> SCH_SYMBOL::GetConnectionPoints() const
{
    std::vector<VECTOR2I> retval;

    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        // Collect only pins attached to the current unit and convert.
        // others are not associated to this symbol instance
        int pin_unit      = pin->GetLibPin() ? pin->GetLibPin()->GetUnit()
                                             : GetUnit();
        int pin_bodyStyle = pin->GetLibPin() ? pin->GetLibPin()->GetBodyStyle()
                                             : GetBodyStyle();

        if( pin_unit > 0 && pin_unit != GetUnit() )
            continue;

        if( pin_bodyStyle > 0 && pin_bodyStyle != GetBodyStyle() )
            continue;

        retval.push_back( m_transform.TransformCoordinate( pin->GetLocalPosition() ) + m_pos );
    }

    return retval;
}


SCH_ITEM* SCH_SYMBOL::GetDrawItem( const VECTOR2I& aPosition, KICAD_T aType )
{
    if( m_part )
    {
        // Calculate the position relative to the symbol.
        VECTOR2I libPosition = aPosition - m_pos;

        return m_part->LocateDrawItem( m_unit, m_bodyStyle, aType, libPosition, m_transform );
    }

    return nullptr;
}


QString SCH_SYMBOL::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    // UNUSED_SYMBOL: KIUI::EllipsizeMenuText in unused_symbols.txt - Menu text ellipsis disabled
    // return QString::asprintf( _( "Symbol %s [%s]" ).toStdString().c_str(),
    //                          KIUI::EllipsizeMenuText( GetField( REFERENCE_FIELD )->GetText() ),
    //                          KIUI::EllipsizeMenuText( GetLibId().GetLibItemName() ) );
    return QString::asprintf( _( "Symbol %s [%s]" ).toStdString().c_str(),
                             GetField( REFERENCE_FIELD )->GetText().toStdString().c_str(),
                             GetLibId().GetLibItemName().c_str() ); // Direct text without ellipsis
}


INSPECT_RESULT SCH_SYMBOL::Visit( INSPECTOR aInspector, void* aTestData,
                                  const std::vector<KICAD_T>& aScanTypes )
{
    for( KICAD_T scanType : aScanTypes )
    {
        if( scanType == SCH_LOCATE_ANY_T
            || ( scanType == SCH_SYMBOL_T )
            || ( scanType == SCH_SYMBOL_LOCATE_POWER_T && m_part && m_part->IsPower() ) )
        {
            if( INSPECT_RESULT::QUIT == aInspector( this, aTestData ) )
                return INSPECT_RESULT::QUIT;
        }

        if( scanType == SCH_LOCATE_ANY_T || scanType == SCH_FIELD_T )
        {
            for( SCH_FIELD& field : m_fields )
            {
                if( INSPECT_RESULT::QUIT == aInspector( &field, (void*) this ) )
                    return INSPECT_RESULT::QUIT;
            }
        }

        if( scanType == SCH_FIELD_LOCATE_REFERENCE_T )
        {
            if( INSPECT_RESULT::QUIT == aInspector( GetField( REFERENCE_FIELD ), (void*) this ) )
                return INSPECT_RESULT::QUIT;
        }

        if( scanType == SCH_FIELD_LOCATE_VALUE_T
            || ( scanType == SCH_SYMBOL_LOCATE_POWER_T && m_part && m_part->IsPower() ) )
        {
            if( INSPECT_RESULT::QUIT == aInspector( GetField( VALUE_FIELD ), (void*) this ) )
                return INSPECT_RESULT::QUIT;
        }

        if( scanType == SCH_FIELD_LOCATE_FOOTPRINT_T )
        {
            if( INSPECT_RESULT::QUIT == aInspector( GetField( FOOTPRINT_FIELD ), (void*) this ) )
                return INSPECT_RESULT::QUIT;
        }

        if( scanType == SCH_FIELD_LOCATE_DATASHEET_T )
        {
            if( INSPECT_RESULT::QUIT == aInspector( GetField( DATASHEET_FIELD ), (void*) this ) )
                return INSPECT_RESULT::QUIT;
        }

        if( scanType == SCH_LOCATE_ANY_T || scanType == SCH_PIN_T )
        {
            for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
            {
                // Collect only pins attached to the current unit and convert.
                // others are not associated to this symbol instance
                int pin_unit      = pin->GetLibPin() ? pin->GetLibPin()->GetUnit()
                                                     : GetUnit();
                int pin_bodyStyle = pin->GetLibPin() ? pin->GetLibPin()->GetBodyStyle()
                                                     : GetBodyStyle();

                if( pin_unit > 0 && pin_unit != GetUnit() )
                    continue;

                if( pin_bodyStyle > 0 && pin_bodyStyle != GetBodyStyle() )
                    continue;

                if( INSPECT_RESULT::QUIT == aInspector( pin.get(), (void*) this ) )
                    return INSPECT_RESULT::QUIT;
            }
        }
    }

    return INSPECT_RESULT::CONTINUE;
}


bool SCH_SYMBOL::operator <( const SCH_ITEM& aItem ) const
{
    if( Type() != aItem.Type() )
        return Type() < aItem.Type();

    const SCH_SYMBOL* symbol = static_cast<const SCH_SYMBOL*>( &aItem );

    BOX2I rect = GetBodyAndPinsBoundingBox();

    if( rect.GetArea() != symbol->GetBodyAndPinsBoundingBox().GetArea() )
        return rect.GetArea() < symbol->GetBodyAndPinsBoundingBox().GetArea();

    if( m_pos.x != symbol->m_pos.x )
        return m_pos.x < symbol->m_pos.x;

    if( m_pos.y != symbol->m_pos.y )
        return m_pos.y < symbol->m_pos.y;

    return m_Uuid < aItem.m_Uuid;       // Ensure deterministic sort
}


bool SCH_SYMBOL::operator==( const SCH_SYMBOL& aSymbol ) const
{
    if( GetFieldCount() !=  aSymbol.GetFieldCount() )
        return false;

    for( int ii = 0; ii < GetFieldCount(); ii++ )
    {
        if( ii == REFERENCE_FIELD )
            continue;

        if( GetFields()[ii].GetText().compare( aSymbol.GetFields()[ii].GetText() ) != 0 )
            return false;
    }

    return true;
}


bool SCH_SYMBOL::operator!=( const SCH_SYMBOL& aSymbol ) const
{
    return !( *this == aSymbol );
}


SCH_SYMBOL& SCH_SYMBOL::operator=( const SCH_SYMBOL& aSymbol )
{
    Q_ASSERT( Type() == aSymbol.Type() );
    if( Type() != aSymbol.Type() ) return *this; // Cannot assign object type

    if( &aSymbol != this )
    {
        SYMBOL::operator=( aSymbol );

        m_lib_id    = aSymbol.m_lib_id;
        m_part.reset( aSymbol.m_part ? new LIB_SYMBOL( *aSymbol.m_part ) : nullptr );
        m_pos       = aSymbol.m_pos;
        m_unit      = aSymbol.m_unit;
        m_bodyStyle = aSymbol.m_bodyStyle;
        m_transform = aSymbol.m_transform;

        m_instanceReferences = aSymbol.m_instanceReferences;

        m_fields    = aSymbol.m_fields;    // std::vector's assignment operator

        // Reparent fields after assignment to new symbol.
        for( SCH_FIELD& field : m_fields )
            field.SetParent( this );

        UpdatePins();
    }

    return *this;
}


bool SCH_SYMBOL::HitTest( const VECTOR2I& aPosition, int aAccuracy ) const
{
    BOX2I bBox = GetBodyBoundingBox();
    bBox.Inflate( aAccuracy / 2 );

    if( bBox.Contains( aPosition ) )
        return true;

    return false;
}


bool SCH_SYMBOL::HitTest( const BOX2I& aRect, bool aContained, int aAccuracy ) const
{
    if( m_flags & STRUCT_DELETED || m_flags & SKIP_STRUCT )
        return false;

    BOX2I rect = aRect;

    rect.Inflate( aAccuracy / 2 );

    if( aContained )
        return rect.Contains( GetBodyBoundingBox() );

    return rect.Intersects( GetBodyBoundingBox() );
}


bool SCH_SYMBOL::doIsConnected( const VECTOR2I& aPosition ) const
{
    VECTOR2I new_pos = m_transform.InverseTransform().TransformCoordinate( aPosition - m_pos );

    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        if( pin->GetType() == ELECTRICAL_PINTYPE::PT_NC )
            continue;

        // Collect only pins attached to the current unit and convert.
        // others are not associated to this symbol instance
        int pin_unit      = pin->GetLibPin() ? pin->GetLibPin()->GetUnit()
                                             : GetUnit();
        int pin_bodyStyle = pin->GetLibPin() ? pin->GetLibPin()->GetBodyStyle()
                                             : GetBodyStyle();

        if( pin_unit > 0 && pin_unit != GetUnit() )
            continue;

        if( pin_bodyStyle > 0 && pin_bodyStyle != GetBodyStyle() )
            continue;

        if( pin->GetLocalPosition() == new_pos )
            return true;
    }

    return false;
}


bool SCH_SYMBOL::IsInNetlist() const
{
    return m_isInNetlist;
}


void SCH_SYMBOL::Plot( PLOTTER* aPlotter, bool aBackground, const SCH_PLOT_OPTS& aPlotOpts,
                       int aUnit, int aBodyStyle, const VECTOR2I& aOffset, bool aDimmed )
{
    if( aBackground )
        return;

    if( m_part )
    {
        std::vector<SCH_PIN*> libPins = m_part->GetPins( GetUnit(), GetBodyStyle() );

        // Copy the source so we can re-orient and translate it.
        LIB_SYMBOL            tempSymbol( *m_part );
        std::vector<SCH_PIN*> tempPins = tempSymbol.GetPins( GetUnit(), GetBodyStyle() );

        // Copy the pin info from the symbol to the temp pins
        for( unsigned i = 0; i < tempPins.size(); ++ i )
        {
            SCH_PIN* symbolPin = GetPin( libPins[ i ] );
            SCH_PIN* tempPin = tempPins[ i ];

            tempPin->SetName( symbolPin->GetShownName() );
            tempPin->SetType( symbolPin->GetType() );
            tempPin->SetShape( symbolPin->GetShape() );

            if( symbolPin->IsDangling() )
                tempPin->SetFlags( IS_DANGLING );
        }

        for( SCH_ITEM& item : tempSymbol.GetDrawItems() )
        {
            if( EDA_TEXT* text = dynamic_cast<EDA_TEXT*>( &item ) )
            {
                // Use SCH_FIELD's text resolver
                SCH_FIELD dummy( this, -1 );
                dummy.SetText( text->GetText() );
                text->SetText( dummy.GetShownText( false ) );
            }
        }

        SCH_RENDER_SETTINGS* renderSettings = getRenderSettings( aPlotter );
        TRANSFORM            savedTransform = renderSettings->m_Transform;
        renderSettings->m_Transform = GetTransform();
        aPlotter->StartBlock( nullptr );

        for( bool local_background : { true, false } )
        {
            tempSymbol.Plot( aPlotter, local_background, aPlotOpts, GetUnit(), GetBodyStyle(),
                             m_pos, GetDNP() );

            for( SCH_FIELD field : m_fields )
            {
                field.ClearRenderCache();
                field.Plot( aPlotter, local_background, aPlotOpts, GetUnit(), GetBodyStyle(),
                            m_pos, GetDNP() );
            }
        }

        if( m_DNP )
            PlotDNP( aPlotter );

        SCH_SHEET_PATH* sheet = &Schematic()->CurrentSheet();

        // Plot attributes to a hypertext menu
        if( aPlotOpts.m_PDFPropertyPopups )
        {
            std::vector<QString> properties;

            for( const SCH_FIELD& field : GetFields() )
            {
                QString text_field = field.GetShownText( sheet, false);

                if( text_field.isEmpty() )
                    continue;

                properties.emplace_back( QString::asprintf( "!%s = %s",
                                                           field.GetName(), text_field ) );
            }

            if( !m_part->GetKeyWords().isEmpty() )
            {
                properties.emplace_back( QString::asprintf( "!%s = %s",
                                                           _( "Keywords" ),
                                                           m_part->GetKeyWords() ) );
            }

            aPlotter->HyperlinkMenu( GetBoundingBox(), properties );
        }

        aPlotter->EndBlock( nullptr );
        renderSettings->m_Transform = savedTransform;

        if( !m_part->IsPower() )
            aPlotter->Bookmark( GetBoundingBox(), GetRef( sheet ), _( "Symbols" ) );
    }
}


void SCH_SYMBOL::PlotDNP( PLOTTER* aPlotter ) const
{
    BOX2I           bbox = GetBodyBoundingBox();
    BOX2I           pins = GetBodyAndPinsBoundingBox();
    VECTOR2D        margins( std::max( bbox.GetX() - pins.GetX(),
                                       pins.GetEnd().x - bbox.GetEnd().x ),
                             std::max( bbox.GetY() - pins.GetY(),
                                       pins.GetEnd().y - bbox.GetEnd().y ) );
    int             strokeWidth = 3.0 * schIUScale.MilsToIU( DEFAULT_LINE_WIDTH_MILS );

    margins.x = std::max( margins.x * 0.6, margins.y * 0.3 );
    margins.y = std::max( margins.y * 0.6, margins.x * 0.3 );
    bbox.Inflate( KiROUND( margins.x ), KiROUND( margins.y ) );

    SCH_RENDER_SETTINGS* renderSettings = getRenderSettings( aPlotter );
    aPlotter->SetColor( renderSettings->GetLayerColor( LAYER_DNP_MARKER ) );

    aPlotter->ThickSegment( bbox.GetOrigin(), bbox.GetEnd(), strokeWidth, FILLED, nullptr );

    aPlotter->ThickSegment( bbox.GetOrigin() + VECTOR2I( bbox.GetWidth(), 0 ),
                            bbox.GetOrigin() + VECTOR2I( 0, bbox.GetHeight() ),
                            strokeWidth, FILLED, nullptr );
}


void SCH_SYMBOL::PlotPins( PLOTTER* aPlotter ) const
{
    if( m_part )
    {
        SCH_RENDER_SETTINGS* renderSettings = getRenderSettings( aPlotter );
        TRANSFORM            savedTransform = renderSettings->m_Transform;
        renderSettings->m_Transform = GetTransform();

        std::vector<SCH_PIN*> libPins = m_part->GetPins( GetUnit(), GetBodyStyle() );

        // Copy the source to stay const
        LIB_SYMBOL            tempSymbol( *m_part );
        std::vector<SCH_PIN*> tempPins = tempSymbol.GetPins( GetUnit(), GetBodyStyle() );
        SCH_PLOT_OPTS         plotOpts;

        // Copy the pin info from the symbol to the temp pins
        for( unsigned i = 0; i < tempPins.size(); ++ i )
        {
            SCH_PIN* symbolPin = GetPin( libPins[ i ] );
            SCH_PIN* tempPin = tempPins[ i ];

            tempPin->SetName( symbolPin->GetShownName() );
            tempPin->SetType( symbolPin->GetType() );
            tempPin->SetShape( symbolPin->GetShape() );
            tempPin->Plot( aPlotter, false, plotOpts, GetUnit(), GetBodyStyle(), m_pos, GetDNP() );
        }

        renderSettings->m_Transform = savedTransform;
    }
}


bool SCH_SYMBOL::HasBrightenedPins()
{
    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        if( pin->IsBrightened() )
            return true;
    }

    return false;
}


void SCH_SYMBOL::ClearBrightenedPins()
{
    for( std::unique_ptr<SCH_PIN>& pin : m_pins )
        pin->ClearBrightened();
}


/*
 * When modified at the schematic level, we still store the values of these flags in the
 * associated m_part.  If m_part now diverges from other usages, a new derived LIB_SYMBOL
 * will be created and stored locally in the schematic.
 */
bool SCH_SYMBOL::GetShowPinNames() const
{
    return m_part && m_part->GetShowPinNames();
}


void SCH_SYMBOL::SetShowPinNames( bool aShow )
{
    if( m_part )
        m_part->SetShowPinNames( aShow );
}


bool SCH_SYMBOL::GetShowPinNumbers() const
{
    return m_part && m_part->GetShowPinNumbers();
}


void SCH_SYMBOL::SetShowPinNumbers( bool aShow )
{
    if( m_part )
        m_part->SetShowPinNumbers( aShow );
}


bool SCH_SYMBOL::IsPointClickableAnchor( const VECTOR2I& aPos ) const
{
    for( const std::unique_ptr<SCH_PIN>& pin : m_pins )
    {
        int pin_unit      = pin->GetLibPin() ? pin->GetLibPin()->GetUnit()
                                             : GetUnit();
        int pin_bodyStyle = pin->GetLibPin() ? pin->GetLibPin()->GetBodyStyle()
                                             : GetBodyStyle();

        if( pin_unit > 0 && pin_unit != GetUnit() )
            continue;

        if( pin_bodyStyle > 0 && pin_bodyStyle != GetBodyStyle() )
            continue;

        if( pin->IsPointClickableAnchor( aPos ) )
            return true;
    }

    return false;
}


bool SCH_SYMBOL::IsSymbolLikePowerGlobalLabel() const
{
    // return true if the symbol is equivalent to a global label:
    // It is a Power symbol
    // It has only one pin type Power input

    if( !GetLibSymbolRef() || !GetLibSymbolRef()->IsPower() )
        return false;

    std::vector<SCH_PIN*> pin_list = GetAllLibPins();

    if( pin_list.size() != 1 )
        return false;

    return pin_list[0]->GetType() == ELECTRICAL_PINTYPE::PT_POWER_IN;
}


bool SCH_SYMBOL::IsPower() const
{
    if( !m_part )
        return false;

    return m_part->IsPower();
}


bool SCH_SYMBOL::IsNormal() const
{
    Q_ASSERT( m_part );
    if( !m_part ) return false;

    return m_part->IsNormal();
}


std::unordered_set<QString> SCH_SYMBOL::GetComponentClassNames( const SCH_SHEET_PATH* aPath ) const
{
    std::unordered_set<QString> componentClass;

    auto getComponentClassFields = [&]( const auto& fields )
    {
        for( const SCH_FIELD& field : fields )
        {
            if( field.GetCanonicalName() == "Component Class" )
            {
                if( field.GetShownText( aPath, false ) != QString() )
                    componentClass.insert( field.GetShownText( aPath, false ) );
            }
        }
    };

    // First get component classes set on the symbol itself
    getComponentClassFields( m_fields );

    // Now get component classes set on any enclosing rule areas
    for( const SCH_RULE_AREA* ruleArea : m_rule_areas_cache )
    {
        for( const SCH_DIRECTIVE_LABEL* label : ruleArea->GetDirectives() )
        {
            getComponentClassFields( label->GetFields() );
        }
    }

    return componentClass;
}


bool SCH_SYMBOL::operator==( const SCH_ITEM& aOther ) const
{
    if( Type() != aOther.Type() )
        return false;

    const SCH_SYMBOL& symbol = static_cast<const SCH_SYMBOL&>( aOther );

    if( GetLibId() != symbol.GetLibId() )
        return false;

    if( GetPosition() != symbol.GetPosition() )
        return false;

    if( GetUnit() != symbol.GetUnit() )
        return false;

    if( GetBodyStyle() != symbol.GetBodyStyle() )
        return false;

    if( GetTransform() != symbol.GetTransform() )
        return false;

    if( GetFields() != symbol.GetFields() )
        return false;

    if( m_pins.size() != symbol.m_pins.size() )
        return false;

    for( unsigned i = 0; i < m_pins.size(); ++i )
    {
        if( *m_pins[i] != *symbol.m_pins[i] )
            return false;
    }

    return true;
}


double SCH_SYMBOL::Similarity( const SCH_ITEM& aOther ) const
{
    if( Type() != aOther.Type() )
        return 0.0;

    const SCH_SYMBOL& symbol = static_cast<const SCH_SYMBOL&>( aOther );

    if( GetLibId() != symbol.GetLibId() )
        return 0.0;

    if( GetPosition() == symbol.GetPosition() )
        return 1.0;

    return 0.0;
}


static struct SCH_SYMBOL_DESC
{
    SCH_SYMBOL_DESC()
    {
        ENUM_MAP<SYMBOL_ORIENTATION_PROP>::Instance()
                .Map( SYMBOL_ANGLE_0,   "0" )
                .Map( SYMBOL_ANGLE_90,  "90" )
                .Map( SYMBOL_ANGLE_180, "180" )
                .Map( SYMBOL_ANGLE_270, "270" );

        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        REGISTER_TYPE( SCH_SYMBOL );
        propMgr.InheritsAfter( TYPE_HASH( SCH_SYMBOL ), TYPE_HASH( SYMBOL ) );

        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, int>( "Position X",
                    &SCH_SYMBOL::SetX, &SCH_SYMBOL::GetX, PROPERTY_DISPLAY::PT_COORD,
                    ORIGIN_TRANSFORMS::ABS_X_COORD ) );
        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, int>( "Position Y",
                    &SCH_SYMBOL::SetY, &SCH_SYMBOL::GetY, PROPERTY_DISPLAY::PT_COORD,
                    ORIGIN_TRANSFORMS::ABS_Y_COORD ) );

        propMgr.AddProperty( new PROPERTY_ENUM<SCH_SYMBOL, SYMBOL_ORIENTATION_PROP>(
                    "Orientation",
                    &SCH_SYMBOL::SetOrientationProp, &SCH_SYMBOL::GetOrientationProp ) );
        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, bool>( "Mirror X",
                    &SCH_SYMBOL::SetMirrorX, &SCH_SYMBOL::GetMirrorX ) );
        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, bool>( "Mirror Y",
                    &SCH_SYMBOL::SetMirrorY, &SCH_SYMBOL::GetMirrorY ) );

        auto hasLibPart =
                []( INSPECTABLE* aItem ) -> bool
                {
                    if( SCH_SYMBOL* symbol = dynamic_cast<SCH_SYMBOL*>( aItem ) )
                        return symbol->GetLibSymbolRef() != nullptr;

                    return false;
                };

        propMgr.AddProperty( new PROPERTY<SYMBOL, bool>( "Pin numbers",
                    &SYMBOL::SetShowPinNumbers, &SYMBOL::GetShowPinNumbers ) )
                .SetAvailableFunc( hasLibPart );

        propMgr.AddProperty( new PROPERTY<SYMBOL, bool>( "Pin names",
                    &SYMBOL::SetShowPinNames, &SYMBOL::GetShowPinNames ) )
                .SetAvailableFunc( hasLibPart );

        const QString groupFields = "Fields";

        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, QString>( "Reference",
                    &SCH_SYMBOL::SetRefProp, &SCH_SYMBOL::GetRefProp ),
                    groupFields );
        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, QString>( "Value",
                    &SCH_SYMBOL::SetValueProp, &SCH_SYMBOL::GetValueProp ),
                    groupFields );
        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, QString>( "Library Link",
                    NO_SETTER( SCH_SYMBOL, QString ), &SCH_SYMBOL::GetSymbolIDAsString ),
                    groupFields );
        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, QString>( "Library Description",
                    NO_SETTER( SCH_SYMBOL, QString ), &SCH_SYMBOL::GetDescription ),
                    groupFields );
        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, QString>( "Keywords",
                    NO_SETTER( SCH_SYMBOL, QString ), &SCH_SYMBOL::GetKeyWords ),
                    groupFields );

        auto multiUnit =
                [=]( INSPECTABLE* aItem ) -> bool
                {
                    if( SCH_SYMBOL* symbol = dynamic_cast<SCH_SYMBOL*>( aItem ) )
                        return symbol->IsMulti();

                    return false;
                };

        auto multiBodyStyle =
                [=]( INSPECTABLE* aItem ) -> bool
                {
                    if( SCH_SYMBOL* symbol = dynamic_cast<SCH_SYMBOL*>( aItem ) )
                        return symbol->HasAlternateBodyStyle();

                    return false;
                };

        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, QString>( "Unit",
                    &SCH_SYMBOL::SetUnitProp, &SCH_SYMBOL::GetUnitProp ) )
                .SetAvailableFunc( multiUnit );

        propMgr.AddProperty( new PROPERTY<SCH_SYMBOL, int>( "Body Style",
                    &SCH_SYMBOL::SetBodyStyleProp, &SCH_SYMBOL::GetBodyStyleProp ) )
                .SetAvailableFunc( multiBodyStyle );

        const QString groupAttributes = "Attributes";

        propMgr.AddProperty( new PROPERTY<SYMBOL, bool>( "Exclude From Board",
                    &SYMBOL::SetExcludedFromBoard, &SYMBOL::GetExcludedFromBoard ),
                    groupAttributes );
        // UNUSED_SYMBOL: SIM functionality disabled
        // propMgr.AddProperty( new PROPERTY<SYMBOL, bool>( "Exclude From Simulation",
        //             &SYMBOL::SetExcludedFromSim, &SYMBOL::GetExcludedFromSim ),
        //             groupAttributes );
        propMgr.AddProperty( new PROPERTY<SYMBOL, bool>( "Exclude From Bill of Materials",
                    &SYMBOL::SetExcludedFromBOM, &SYMBOL::GetExcludedFromBOM ),
                    groupAttributes );
        propMgr.AddProperty( new PROPERTY<SYMBOL, bool>( "Do not Populate",
                    &SYMBOL::SetDNP, &SYMBOL::GetDNP ),
                    groupAttributes );
    }
} _SCH_SYMBOL_DESC;

// Qt Transformation Completed: wxWidgets to Qt framework migration finished
// All wxString -> QString, wxWidgets containers -> Qt containers
// All wx macros removed, Q_ASSERT used instead of wxASSERT
// Commented legacy wxWidgets SIM code removed

IMPLEMENT_ENUM_TO_QVARIANT( SYMBOL_ORIENTATION_PROP )
