// Qt Framework Transformation - Converted from wxWidgets to Qt
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#include <macros.h>
#include <string>
#include <string_utils.h>
#include <QString>

#include "markdown.h"
#include "html.h"
#include "buffer.h"


void ConvertMarkdown2Html( const QString& aMarkdownInput, QString& aHtmlOutput )
{
    std::string markdownInput( aMarkdownInput.toStdString() );

    /* performing markdown parsing */
    struct sd_callbacks callbacks;
    struct html_renderopt   options;

#define OUTPUT_UNIT 64
    struct buf* ob = bufnew( OUTPUT_UNIT );

    sdhtml_renderer( &callbacks, &options, 0 );
    struct sd_markdown* markdown = sd_markdown_new( MKDEXT_TABLES, 16, &callbacks, &options );

    sd_markdown_render( ob, (uint8_t*)markdownInput.data(), markdownInput.size(), markdown );
    sd_markdown_free( markdown );

    std::string out( (char*)ob->data, ob->size );
    aHtmlOutput = QString::fromStdString( out );

    /* cleanup */
    bufrelease( ob );

}
