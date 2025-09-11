
#include "template_fieldnames.h"

#include <mutex>

#include <template_fieldnames_lexer.h>
#include <string_utils.h>
#include <i18n_utility.h>

using namespace TFIELD_T;

// N.B. Do not change these values without transitioning the file format
#define REFERENCE_CANONICAL "Reference"
#define VALUE_CANONICAL "Value"
#define FOOTPRINT_CANONICAL "Footprint"
#define DATASHEET_CANONICAL "Datasheet"
#define DESCRIPTION_CANONICAL "Description"
#define USER_FIELD_CANONICAL "Field%d"

static QString s_CanonicalReference( REFERENCE_CANONICAL );
static QString s_CanonicalValue( VALUE_CANONICAL );
static QString s_CanonicalFootprint( FOOTPRINT_CANONICAL );
static QString s_CanonicalDatasheet( DATASHEET_CANONICAL );
static QString s_CanonicalDescription( DESCRIPTION_CANONICAL );


QString GetDefaultFieldName( int aFieldNdx, bool aTranslateForHI )
{
    if( !aTranslateForHI )
    {
        switch( aFieldNdx )
        {
        case REFERENCE_FIELD:   return s_CanonicalReference;   // The symbol reference, R1, C1, etc.
        case VALUE_FIELD:       return s_CanonicalValue;       // The symbol value
        case FOOTPRINT_FIELD:   return s_CanonicalFootprint;   // The footprint for use with Pcbnew
        case DATASHEET_FIELD:   return s_CanonicalDatasheet;   // Link to a datasheet for symbol
        case DESCRIPTION_FIELD: return s_CanonicalDescription; // The symbol description
        default:                return GetUserFieldName( aFieldNdx, aTranslateForHI );
        }
    }
    else
    {
        switch( aFieldNdx )
        {
        case REFERENCE_FIELD:   return _( REFERENCE_CANONICAL );   // The symbol reference, R1, C1, etc.
        case VALUE_FIELD:       return _( VALUE_CANONICAL );       // The symbol value
        case FOOTPRINT_FIELD:   return _( FOOTPRINT_CANONICAL );   // The footprint for use with Pcbnew
        case DATASHEET_FIELD:   return _( DATASHEET_CANONICAL );   // Link to a datasheet for symbol
        case DESCRIPTION_FIELD: return _( DESCRIPTION_CANONICAL ); // The symbol description
        default:                return GetUserFieldName( aFieldNdx, aTranslateForHI );
        }
    }
}


QString GetUserFieldName( int aFieldNdx, bool aTranslateForHI )
{
    if( !aTranslateForHI )
        return QString( USER_FIELD_CANONICAL ).arg( aFieldNdx );
    else
        return QString( _( USER_FIELD_CANONICAL ) ).arg( aFieldNdx );
}


void TEMPLATE_FIELDNAME::Format( OUTPUTFORMATTER* out ) const
{
    out->Print( "(field (name %s)",  out->Quotew( m_Name ).c_str() );

    if( m_Visible )
        out->Print( " visible" );

    if( m_URL )
        out->Print( " url" );

    out->Print( ")" );
}


void TEMPLATE_FIELDNAME::Parse( TEMPLATE_FIELDNAMES_LEXER* in )
{
    T    tok;

    in->NeedLEFT();     // begin (name ...)

    if( ( tok = in->NextTok() ) != T_name )
        in->Expecting( T_name );

    in->NeedSYMBOLorNUMBER();

    m_Name = From_UTF8( in->CurText() );

    in->NeedRIGHT();    // end (name ...)

    while( (tok = in->NextTok() ) != T_RIGHT && tok != T_EOF )
    {
        // "visible" has no '(' prefix, "value" does, so T_LEFT is optional.
        if( tok == T_LEFT )
            tok = in->NextTok();

        switch( tok )
        {
        case T_value:
            // older format; silently skip
            in->NeedSYMBOLorNUMBER();
            in->NeedRIGHT();
            break;

        case T_visible:
            m_Visible = true;
            break;

        case T_url:
            m_URL = true;
            break;

        default:
            in->Expecting( "value|url|visible" );
            break;
        }
    }
}


void TEMPLATES::Format( OUTPUTFORMATTER* out, bool aGlobal ) const
{
    // We'll keep this general, and include the \n, even though the only known
    // use at this time will not want the newlines or the indentation.
    out->Print( "(templatefields" );

    const TEMPLATE_FIELDNAMES& source = aGlobal ? m_globals : m_project;

    for( const TEMPLATE_FIELDNAME& temp : source )
    {
        if( !temp.m_Name.isEmpty() )
            temp.Format( out );
    }

    out->Print( ")" );
}


void TEMPLATES::parse( TEMPLATE_FIELDNAMES_LEXER* in, bool aGlobal )
{
    T  tok;

    while( ( tok = in->NextTok() ) != T_RIGHT && tok != T_EOF )
    {
        if( tok == T_LEFT )
            tok = in->NextTok();

        switch( tok )
        {
        case T_templatefields:  // a token indicating class TEMPLATES.

            // Be flexible regarding the starting point of the TEMPLATE_FIELDNAMES_LEXER
            // stream.  Caller may not have read the first two tokens out of the
            // stream: T_LEFT and T_templatefields, so ignore them if seen here.
            break;

        case T_field:
            {
                // instantiate on stack, so if exception is thrown,
                // destructor runs
                TEMPLATE_FIELDNAME  field;

                field.Parse( in );

                // add the field
                if( !field.m_Name.isEmpty() )
                    AddTemplateFieldName( field, aGlobal );
            }
            break;

        default:
            in->Unexpected( in->CurText() );
            break;
        }
    }
}


void TEMPLATES::resolveTemplates()
{
    m_resolved = m_project;

    // Note: order N^2 algorithm.  Would need changing if fieldname template sets ever
    // get large.

    for( const TEMPLATE_FIELDNAME& global : m_globals )
    {
        bool overriddenInProject = false;

        for( const TEMPLATE_FIELDNAME& project : m_project )
        {
            if( global.m_Name == project.m_Name )
            {
                overriddenInProject = true;
                break;
            }
        }

        if( !overriddenInProject )
            m_resolved.push_back( global );
    }

    m_resolvedDirty = false;
}


void TEMPLATES::AddTemplateFieldName( const TEMPLATE_FIELDNAME& aFieldName, bool aGlobal )
{
    // Ensure that the template fieldname does not match a fixed fieldname.
    for( int fieldId : MANDATORY_FIELDS )
    {
        if( GetCanonicalFieldName( fieldId ) == aFieldName.m_Name )
            return;
    }

    TEMPLATE_FIELDNAMES& target = aGlobal ? m_globals : m_project;

    // ensure uniqueness, overwrite any template fieldname by the same name.
    for( TEMPLATE_FIELDNAME& temp : target )
    {
        if( temp.m_Name == aFieldName.m_Name )
        {
            temp = aFieldName;
            m_resolvedDirty = true;
            return;
        }
    }

    // the name is legal and not previously added to the config container, append
    // it and return its index within the container.
    target.push_back( aFieldName );
    m_resolvedDirty = true;
}


void TEMPLATES::AddTemplateFieldNames( const QString& aSerializedFieldNames )
{
    TEMPLATE_FIELDNAMES_LEXER field_lexer( std::string( TO_UTF8( aSerializedFieldNames ) ) );

    try
    {
        parse( &field_lexer, true );
    }
    catch( const IO_ERROR& )
    {
    }
}


void TEMPLATES::DeleteAllFieldNameTemplates( bool aGlobal )
{
    if( aGlobal )
    {
        m_globals.clear();
        m_resolved = m_project;
    }
    else
    {
        m_project.clear();
        m_resolved = m_globals;
    }

    m_resolvedDirty = false;
}


const TEMPLATE_FIELDNAMES& TEMPLATES::GetTemplateFieldNames()
{
    if( m_resolvedDirty )
        resolveTemplates();

    return m_resolved;
}


const TEMPLATE_FIELDNAMES& TEMPLATES::GetTemplateFieldNames( bool aGlobal )
{
    if( aGlobal )
        return m_globals;
    else
        return m_project;
}


const TEMPLATE_FIELDNAME* TEMPLATES::GetFieldName( const QString& aName )
{
    if( m_resolvedDirty )
        resolveTemplates();

    for( const TEMPLATE_FIELDNAME& field : m_resolved )
    {
        if( field.m_Name == aName )
            return &field;
    }

    return nullptr;
}

