
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#include <pcb_field.h>
#include <footprint.h>
#include <board_design_settings.h>
#include <i18n_utility.h>
#include <pcb_painter.h>
// #include <api/board/board_types.pb.h> // DISABLED FOR MINIMAL BUILD
#include <string_utils.h>
#include <QString>
#include <QDebug>


PCB_FIELD::PCB_FIELD( FOOTPRINT* aParent, int aFieldId, const QString& aName ) :
        PCB_TEXT( aParent, PCB_FIELD_T ),
        m_id( aFieldId ),
        m_name( aName )
{
}


PCB_FIELD::PCB_FIELD( const PCB_TEXT& aText, int aFieldId, const QString& aName ) :
        PCB_TEXT( aText.GetParent(), PCB_FIELD_T ),
        m_id( aFieldId ),
        m_name( aName )
{
    // Copy the text properties from the PCB_TEXT
    SetText( aText.GetText() );
    SetVisible( aText.IsVisible() );
    SetLayer( aText.GetLayer() );
    SetPosition( aText.GetPosition() );
    SetAttributes( aText.GetAttributes() );
}


//void PCB_FIELD::Serialize( google::protobuf::Any &aContainer ) const
//{
//    kiapi::board::types::Field field;
//
//    google::protobuf::Any anyText;
//    PCB_TEXT::Serialize( anyText );
//    anyText.UnpackTo( field.mutable_text() );
//
//    field.set_name( GetCanonicalName().toStdString() );
//    field.mutable_id()->set_id( GetId() );
//    field.set_visible( IsVisible() );
//
//    aContainer.PackFrom( field );
//}
//
//
//bool PCB_FIELD::Deserialize( const google::protobuf::Any &aContainer )
//{
//    kiapi::board::types::Field field;
//
//    if( !aContainer.UnpackTo( &field ) )
//        return false;
//
//    if( field.has_id() )
//        setId( field.id().id() );
//
//    // Mandatory fields have a blank Name in the KiCad object
//    if( !IsMandatory() )
//        SetName( QString::fromUtf8( field.name().c_str() ) );
//
//    if( field.has_text() )
//    {
//        google::protobuf::Any anyText;
//        anyText.PackFrom( field.text() );
//        PCB_TEXT::Deserialize( anyText );
//    }
//
//    SetVisible( field.visible() );
//
//    if( field.text().layer() == kiapi::board::types::BoardLayer::BL_UNKNOWN )
//        SetLayer( F_SilkS );
//
//    return true;
//}


QString PCB_FIELD::GetName( bool aUseDefaultName ) const
{
    if( m_parent && m_parent->Type() == PCB_FOOTPRINT_T )
    {
        if( IsMandatory() )
            return GetCanonicalFieldName( m_id );
        else if( m_name.isEmpty() && aUseDefaultName )
            return GetUserFieldName( m_id, !DO_TRANSLATE );
        else
            return m_name;
    }
    else
    {
        qDebug() << "Unhandled field owner type.";
        return m_name;
    }
}


QString PCB_FIELD::GetCanonicalName() const
{
    if( m_parent && m_parent->Type() == PCB_FOOTPRINT_T )
    {
        if( IsMandatory() )
            return GetCanonicalFieldName( m_id );
        else
            return m_name;
    }
    else
    {
        if( m_parent )
        {
            qDebug() << QString::asprintf( "Unhandled field owner type (id %d, parent type %d).",
                                          m_id, m_parent->Type() );
        }

        return m_name;
    }
}


bool PCB_FIELD::IsMandatory() const
{
    return m_id == REFERENCE_FIELD
        || m_id == VALUE_FIELD
        || m_id == DATASHEET_FIELD
        || m_id == DESCRIPTION_FIELD;
}


bool PCB_FIELD::IsHypertext() const
{
    return IsURL( GetShownText( false ) );
}


QString PCB_FIELD::GetTextTypeDescription() const
{
    if( IsMandatory() )
        return GetCanonicalFieldName( m_id );
    else
        return _( "User Field" );
}


bool PCB_FIELD::Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const
{
    if( !IsVisible() && !aSearchData.searchAllFields )
        return false;

    return PCB_TEXT::Matches( aSearchData, aAuxData );
}


QString PCB_FIELD::GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const
{
    // UNUSED_SYMBOL: KIUI::EllipsizeMenuText in unused_symbols.txt - Menu text ellipsis disabled
    // QString content = aFull ? GetShownText( false ) : KIUI::EllipsizeMenuText( GetText() );
    QString content = aFull ? GetShownText( false ) : GetText(); // Direct text without ellipsis
    QString ref = GetParentFootprint()->GetReference();

    switch( m_id )
    {
    case REFERENCE_FIELD:
        return _( "Reference field of %1" ).arg( ref );

    case VALUE_FIELD:
        return _( "Value field of %1 (%2)" ).arg( ref ).arg( content );

    case FOOTPRINT_FIELD:
        return _( "Footprint field of %1 (%2)" ).arg( ref ).arg( content );

    case DATASHEET_FIELD:
        return _( "Datasheet field of %1 (%2)" ).arg( ref ).arg( content );

    default:
        if( GetName().isEmpty() )
            return _( "Field of %1 (%2)" ).arg( ref ).arg( content );
        else
            return _( "%1 field of %2 (%3)" ).arg( GetName() ).arg( ref ).arg( content );
    }
}


double PCB_FIELD::ViewGetLOD( int aLayer, const KIGFX::VIEW* aView ) const
{
    if( !aView )
        return LOD_SHOW;

    KIGFX::PCB_PAINTER*         painter = static_cast<KIGFX::PCB_PAINTER*>( aView->GetPainter() );
    KIGFX::PCB_RENDER_SETTINGS* renderSettings = painter->GetSettings();

    if( GetParentFootprint() && GetParentFootprint()->IsSelected()
            && renderSettings->m_ForceShowFieldsWhenFPSelected )
    {
        return LOD_SHOW;
    }

    // Handle Render tab switches
    if( IsValue() && !aView->IsLayerVisible( LAYER_FP_VALUES ) )
        return LOD_HIDE;

    if( IsReference() && !aView->IsLayerVisible( LAYER_FP_REFERENCES ) )
        return LOD_HIDE;

    return PCB_TEXT::ViewGetLOD( aLayer, aView );
}


EDA_ITEM* PCB_FIELD::Clone() const
{
    return new PCB_FIELD( *this );
}


void PCB_FIELD::swapData( BOARD_ITEM* aImage )
{
    assert( aImage->Type() == PCB_FIELD_T );

    std::swap( *((PCB_FIELD*) this), *((PCB_FIELD*) aImage) );
}


bool PCB_FIELD::operator==( const BOARD_ITEM& aOther ) const
{
    if( aOther.Type() != Type() )
        return false;

    const PCB_FIELD& other = static_cast<const PCB_FIELD&>( aOther );

    return *this == other;
}


bool PCB_FIELD::operator==( const PCB_FIELD& aOther ) const
{
    return m_id == aOther.m_id && m_name == aOther.m_name && EDA_TEXT::operator==( aOther );
}


double PCB_FIELD::Similarity( const BOARD_ITEM& aOther ) const
{
    if( m_Uuid == aOther.m_Uuid )
        return 1.0;

    if( aOther.Type() != Type() )
        return 0.0;

    const PCB_FIELD& other = static_cast<const PCB_FIELD&>( aOther );

    if( IsMandatory() || other.IsMandatory() )
    {
        if( m_id == other.m_id )
            return 1.0;
        else
            return 0.0;
    }

    if( m_name == other.m_name )
        return 1.0;

    return EDA_TEXT::Similarity( other );
}

static struct PCB_FIELD_DESC
{
    PCB_FIELD_DESC()
    {
        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        REGISTER_TYPE( PCB_FIELD );
        propMgr.AddTypeCast( new TYPE_CAST<PCB_FIELD, PCB_TEXT> );
        propMgr.AddTypeCast( new TYPE_CAST<PCB_FIELD, BOARD_ITEM> );
        propMgr.AddTypeCast( new TYPE_CAST<PCB_FIELD, EDA_TEXT> );
        propMgr.InheritsAfter( TYPE_HASH( PCB_FIELD ), TYPE_HASH( BOARD_ITEM ) );
        propMgr.InheritsAfter( TYPE_HASH( PCB_FIELD ), TYPE_HASH( PCB_TEXT ) );
        propMgr.InheritsAfter( TYPE_HASH( PCB_FIELD ), TYPE_HASH( EDA_TEXT ) );

        propMgr.AddProperty( new PROPERTY<PCB_FIELD, QString>( _HKI( "Name" ),
                     NO_SETTER( PCB_FIELD, QString ), &PCB_FIELD::GetCanonicalName ) )
                .SetIsHiddenFromLibraryEditors()
                .SetIsHiddenFromPropertiesManager();

        // These properties, inherited from EDA_TEXT, have no sense for the board editor
        propMgr.Mask( TYPE_HASH( PCB_FIELD ), TYPE_HASH( EDA_TEXT ), _HKI( "Hyperlink" ) );
        propMgr.Mask( TYPE_HASH( PCB_FIELD ), TYPE_HASH( EDA_TEXT ), _HKI( "Color" ) );
    }
} _PCB_FIELD_DESC;
