
#ifndef KICAD_FONTCONFIG_H
#define KICAD_FONTCONFIG_H

#include <fontconfig/fontconfig.h>

#include <kicommon.h>
#include <QString>
#include <vector>
#include <map>
#include <unordered_map>
#include <font/fontinfo.h>

class REPORTER;
namespace fontconfig
{

struct FONTCONFIG_PAT;

class KICOMMON_API FONTCONFIG
{
public:
    FONTCONFIG();

    static QString Version();

    enum class FF_RESULT
    {
        FF_OK,
        FF_ERROR,
        FF_SUBSTITUTE,
        FF_MISSING_BOLD,
        FF_MISSING_ITAL,
        FF_MISSING_BOLD_ITAL
    };

    FF_RESULT FindFont( const QString& aFontName, QString& aFontFile, int& aFaceIndex, bool aBold,
                        bool aItalic, const std::vector<QString>* aEmbeddedFiles = nullptr );

    void ListFonts( std::vector<std::string>& aFonts, const std::string& aDesiredLang,
                    const std::vector<QString>* aEmbeddedFiles = nullptr, bool aForce = false );

    static void SetReporter( REPORTER* aReporter );

private:
    std::map<std::string, FONTINFO> m_fontInfoCache;
    QString                     m_fontCacheLastLang;
    static REPORTER*            s_reporter;

    bool isLanguageMatch( const QString& aSearchLang, const QString& aSupportedLang );

    void getAllFamilyStrings( FONTCONFIG_PAT& aPat,
                              std::unordered_map<std::string, std::string>& aFamStringMap );

    std::string getFamilyStringByLang( FONTCONFIG_PAT& APat, const QString& aDesiredLang );

    std::string getFcString( FONTCONFIG_PAT& aPat, const char* aObj, int aIdx );
};

} // namespace fontconfig


KICOMMON_API fontconfig::FONTCONFIG* Fontconfig();


#endif //KICAD_FONTCONFIG_H
