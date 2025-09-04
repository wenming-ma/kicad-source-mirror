
#include <font/version_info.h>
#include <font/fontconfig.h>
#include <harfbuzz/hb.h>
#ifdef _MSC_VER
#include <ft2build.h>
#else
#include <freetype2/ft2build.h>
#endif
#include FT_FREETYPE_H

using namespace KIFONT;

QString VERSION_INFO::FreeType()
{
    FT_Library library;

    FT_Int major = 0;
    FT_Int minor = 0;
    FT_Int patch = 0;
    FT_Init_FreeType( &library );
    FT_Library_Version( library, &major, &minor, &patch );
    FT_Done_FreeType( library );

    return QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
}


QString VERSION_INFO::HarfBuzz()
{
    return QString::fromUtf8( HB_VERSION_STRING );
}


QString VERSION_INFO::FontConfig()
{
    return fontconfig::FONTCONFIG::Version();
}


QString VERSION_INFO::FontLibrary()
{
    return QString("FreeType %1 HarfBuzz %2").arg(FreeType()).arg(HarfBuzz());
}