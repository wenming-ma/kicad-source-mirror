#include <kicommon.h>
#include <lib_id.h>
#include <json_common.h>
#include <QString>


class KICOMMON_API DESIGN_BLOCK
{
public:
    void          SetLibId( const LIB_ID& aName ) { m_lib_id = aName; }
    const LIB_ID& GetLibId() const { return m_lib_id; }

    const QString& GetLibDescription() const { return m_libDescription; }
    void            SetLibDescription( const QString& aDesc ) { m_libDescription = aDesc; }

    const QString& GetKeywords() const { return m_keywords; }
    void            SetKeywords( const QString& aKeywords ) { m_keywords = aKeywords; }

    const QString& GetSchematicFile() const { return m_schematicFile; }
    void            SetSchematicFile( const QString& aFile ) { m_schematicFile = aFile; }

    void SetFields( nlohmann::ordered_map<QString, QString>& aFields )
    {
        m_fields = std::move( aFields );
    }

    const nlohmann::ordered_map<QString, QString>& GetFields() const { return m_fields; }

    DESIGN_BLOCK() = default;

    DESIGN_BLOCK( DESIGN_BLOCK&& aOther ) = delete;

private:
    LIB_ID m_lib_id;
    QString m_schematicFile;  // File name and path for schematic symbol.
    QString m_libDescription; // File name and path for documentation file.
    QString m_keywords;       // Search keywords to find footprint in library.

    nlohmann::ordered_map<QString, QString> m_fields;
};
