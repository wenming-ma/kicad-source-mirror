// QT_TRANSFORMATION_COMPLETED

#pragma once

#include <QString>
#include <vector>

class OUTPUTFORMATTER;
class TEMPLATE_FIELDNAMES_LEXER;


enum  MANDATORY_FIELD_T {
    INVALID_FIELD = -1,
    REFERENCE_FIELD = 0,
    VALUE_FIELD,
    FOOTPRINT_FIELD,
    DATASHEET_FIELD,
    DESCRIPTION_FIELD,
    MANDATORY_FIELD_COUNT
};

#define MANDATORY_FIELDS { REFERENCE_FIELD, VALUE_FIELD, FOOTPRINT_FIELD, DATASHEET_FIELD, DESCRIPTION_FIELD }

#define DO_TRANSLATE true


QString GetDefaultFieldName( int aFieldNdx, bool aTranslateForHI );
QString GetUserFieldName( int aFieldNdx, bool aTranslateForHI );


inline QString GetCanonicalFieldName( int idx )
{
    Q_ASSERT( idx < MANDATORY_FIELD_COUNT );

    return GetDefaultFieldName( idx, !DO_TRANSLATE );
}


struct TEMPLATE_FIELDNAME
{
    TEMPLATE_FIELDNAME() :
            m_Visible( false ),
            m_URL( false )
    {
    }

    TEMPLATE_FIELDNAME( const QString& aName ) :
            m_Name( aName ),
            m_Visible( false ),
            m_URL( false )
    {
    }

    TEMPLATE_FIELDNAME( const TEMPLATE_FIELDNAME& ref )
    {
        m_Name = ref.m_Name;
        m_Visible = ref.m_Visible;
        m_URL = ref.m_URL;
    }

    void Format( OUTPUTFORMATTER* out ) const ;

    void Parse( TEMPLATE_FIELDNAMES_LEXER* aSpec );

    QString     m_Name;
    bool        m_Visible;
    bool        m_URL;
};

typedef std::vector< TEMPLATE_FIELDNAME > TEMPLATE_FIELDNAMES;


class TEMPLATES
{
public:
    TEMPLATES() :
            m_resolvedDirty( true )
    { }

    void Format( OUTPUTFORMATTER* out, bool aGlobal ) const ;

    void AddTemplateFieldName( const TEMPLATE_FIELDNAME& aFieldName, bool aGlobal );

    void AddTemplateFieldNames( const QString& aSerializedFieldNames );

    void DeleteAllFieldNameTemplates( bool aGlobal );

    const TEMPLATE_FIELDNAMES& GetTemplateFieldNames();

    const TEMPLATE_FIELDNAMES& GetTemplateFieldNames( bool aGlobal );

    const TEMPLATE_FIELDNAME* GetFieldName( const QString& aName );

protected:
    void resolveTemplates();

    void parse( TEMPLATE_FIELDNAMES_LEXER* in, bool aGlobal );

private:
    TEMPLATE_FIELDNAMES     m_globals;
    TEMPLATE_FIELDNAMES     m_project;

    TEMPLATE_FIELDNAMES     m_resolved;
    bool                    m_resolvedDirty;
};
