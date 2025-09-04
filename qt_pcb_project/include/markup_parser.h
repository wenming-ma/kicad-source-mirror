
#ifndef MARKUP_PARSER_H
#define MARKUP_PARSER_H

#include <thirdparty/pegtl/pegtl.hpp>
#include <thirdparty/pegtl/pegtl/contrib/parse_tree.hpp>
#include <iostream>
#include <string>
#include <QString>
#include <core/utf8.h>
#include <kicommon.h>


namespace MARKUP
{
using namespace tao::pegtl;

struct subscript;
struct superscript;
struct overbar;

struct KICOMMON_API NODE : parse_tree::basic_node<NODE>
{
    std::string asString() const;

    std::string typeString() const;

    QString asQString() const;

    bool isOverbar() const     { return is_type<MARKUP::overbar>(); }
    bool isSubscript() const   { return is_type<MARKUP::subscript>(); }
    bool isSuperscript() const { return is_type<MARKUP::superscript>(); }
};

struct markup : sor< subscript,
                     superscript,
                     overbar > {};

// anyString = a run of characters that do not start a command sequence, or if they do, they do not start a complete command prefix (command char + open brace)
struct anyString : plus< seq< not_at< markup >,
                              utf8::any > > {};

struct escapeSequence : seq< string<'{'>, identifier, string<'}'> > {};

struct anyStringWithinBraces : plus< sor< seq< not_at< markup >,
                                               escapeSequence >,
                                          seq< not_at< markup >,
                                               utf8::not_one<'}'> > > > {};

template< typename ControlChar >
struct braces : seq< seq< ControlChar,
                          string<'{'> >,
                     until< string<'}'>,
                            sor< anyStringWithinBraces,
                                 subscript,
                                 superscript,
                                 overbar > > > {};

struct superscript : braces< string<'^'> > {};
struct subscript   : braces< string<'_'> > {};
struct overbar     : braces< string<'~'> > {};

// Finally, the full grammar
struct anything : sor< anyString,
                       subscript,
                       superscript,
                       overbar > {};

struct grammar : until< tao::pegtl::eof, anything > {};

template <typename Rule>
using selector = parse_tree::selector< Rule,
                                       parse_tree::store_content::on< anyStringWithinBraces,
                                                                      anyString >,
                                       parse_tree::discard_empty::on< superscript,
                                                                      subscript,
                                                                      overbar > >;

class KICOMMON_API MARKUP_PARSER
{
public:
    MARKUP_PARSER( const std::string& source ) :
            in( std::make_unique<string_input<>>( source, "from_input" ) ),
            mem_in()
    {}

    MARKUP_PARSER( const std::string* source ) :
            in(),
            mem_in( std::make_unique<memory_input<>>( *source, "from_input" ) )
    {}

    std::unique_ptr<NODE> Parse();

private:
    std::unique_ptr<string_input<>> in;
    std::unique_ptr<memory_input<>> mem_in;
};

} // namespace MARKUP


#endif //MARKUP_PARSER_H
