#include <memory>
#include <set>
#include <vector>
#include <algorithm>

#include <eda_units.h>
#include <string_utils.h>
#include <QDebug>

#ifdef DEBUG
#include <cstdarg>
#endif

#include <libeval_compiler/libeval_compiler.h>

namespace LIBEVAL
{

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include <libeval_compiler/grammar.c>
#include <libeval_compiler/grammar.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


#define libeval_dbg(level, fmt, ...) \
    qDebug() << QString::asprintf(fmt, __VA_ARGS__);


TREE_NODE* newNode( LIBEVAL::COMPILER* compiler, int op, const T_TOKEN_VALUE& value )
{
    TREE_NODE* t2    = new TREE_NODE();

    t2->valid        = true;
    t2->value.str    = value.str ? new QString( *value.str ) : nullptr;
    t2->value.num    = value.num;
    t2->value.idx    = value.idx;
    t2->op           = op;
    t2->leaf[0]      = nullptr;
    t2->leaf[1]      = nullptr;
    t2->isTerminal   = false;
    t2->srcPos       = compiler->GetSourcePos();
    t2->uop          = nullptr;

    libeval_dbg(10, " ostr %p nstr %p nnode %p op %d", value.str, t2->value.str, t2, t2->op );

    if(t2->value.str)
        compiler->GcItem( t2->value.str );

    compiler->GcItem( t2 );

    return t2;
}


static const QString formatOpName( int op )
{
    static const struct
    {
        int      op;
        QString mnemonic;
    }
    simpleOps[] =
    {
        { TR_OP_MUL, "MUL" },
        { TR_OP_DIV, "DIV" },
        { TR_OP_ADD, "ADD" },
        { TR_OP_SUB, "SUB" },
        { TR_OP_LESS, "LESS" },
        { TR_OP_GREATER, "GREATER" },
        { TR_OP_LESS_EQUAL, "LESS_EQUAL" },
        { TR_OP_GREATER_EQUAL, "GREATER_EQUAL" },
        { TR_OP_EQUAL, "EQUAL" },
        { TR_OP_NOT_EQUAL, "NEQUAL" },
        { TR_OP_BOOL_AND, "AND" },
        { TR_OP_BOOL_OR, "OR" },
        { TR_OP_BOOL_NOT, "NOT" },
        { -1, "" }
    };

    for( int i = 0; simpleOps[i].op >= 0; i++ )
    {
        if( simpleOps[i].op == op )
            return simpleOps[i].mnemonic;
    }

    return "???";
}


bool VALUE::EqualTo( CONTEXT* aCtx, const VALUE* b ) const
{
    if( m_type == VT_UNDEFINED || b->m_type == VT_UNDEFINED )
        return false;

    if( m_type == VT_NULL && b->m_type == VT_NULL )
        return true;

    if( m_type == VT_NUMERIC && b->m_type == VT_NUMERIC )
    {
        return AsDouble() == b->AsDouble();
    }
    else if( m_type == VT_STRING && b->m_type == VT_STRING )
    {
        if( b->m_stringIsWildcard )
            return WildCompareString( b->AsString(), AsString(), false );
        else
            return AsString().compare( b->AsString(), Qt::CaseInsensitive ) == 0;
    }

    return false;
}


bool VALUE::NotEqualTo( CONTEXT* aCtx, const VALUE* b ) const
{
    if( m_type == VT_UNDEFINED || b->m_type == VT_UNDEFINED )
        return false;

    return !EqualTo( aCtx, b );
}


QString UOP::Format() const
{
    QString str;

    switch( m_op )
    {
    case TR_UOP_PUSH_VAR:
        str = QString::asprintf( "PUSH VAR [%p]", m_ref.get() );
        break;

    case TR_UOP_PUSH_VALUE:
        if( !m_value )
            str = QString::asprintf( "PUSH nullptr" );
        else if( m_value->GetType() == VT_NUMERIC )
            str = QString::asprintf( "PUSH NUM [%.10f]", m_value->AsDouble() );
        else
            str = QString::asprintf( "PUSH STR [%s]", m_value->AsString().toUtf8().data() );
        break;

    case TR_OP_METHOD_CALL:
        str = QString::asprintf( "MCALL" );
        break;

    case TR_OP_FUNC_CALL:
        str = QString::asprintf( "FCALL" );
        break;

    default:
        str = QString::asprintf( "%s %d", formatOpName( m_op ).toUtf8().data(), m_op );
        break;
    }

    return str;
}


UCODE::~UCODE()
{
    for( UOP* op : m_ucode )
        delete op;
}


QString UCODE::Dump() const
{
    QString rv;

    for( UOP* op : m_ucode )
    {
        rv += op->Format();
        rv += "\n";
    }

    return rv;
};


QString TOKENIZER::GetString()
{
    QString rv;

    while( m_pos < m_str.length() && m_str[ m_pos ] != '\'' )
    {
        if( m_str[ m_pos ] == '\\' && m_pos + 1 < m_str.length() && m_str[ m_pos + 1 ] == '\'' )
            m_pos++;

        rv.append( m_str[ m_pos++ ] );
    }

    m_pos++;

    return rv;
}


QString TOKENIZER::GetChars( const std::function<bool( QChar )>& cond ) const
{
    QString rv;
    size_t      p = m_pos;

    while( p < m_str.length() && cond( m_str[p] ) )
    {
        rv.append( m_str[p] );
        p++;
    }

    return rv;
}


bool TOKENIZER::MatchAhead( const QString& match,
                            const std::function<bool( QChar )>& stopCond ) const
{
    int remaining = (int) m_str.length() - m_pos;

    if( remaining < (int) match.length() )
        return false;

    if( m_str.mid( m_pos, match.length() ) == match )
        return ( remaining == (int) match.length() || stopCond( m_str[m_pos + match.length()] ) );

    return false;
}


COMPILER::COMPILER() :
        m_lexerState( COMPILER::LS_DEFAULT )
{
    m_localeDecimalSeparator = '.';
    m_sourcePos = 0;
    m_parseFinished = false;
    m_unitResolver = std::make_unique<UNIT_RESOLVER>();
    m_parser = LIBEVAL::ParseAlloc( malloc );
    m_tree = nullptr;
    m_errorStatus.pendingError = false;
}


COMPILER::~COMPILER()
{
    LIBEVAL::ParseFree( m_parser, free );

    if( m_tree )
    {
        freeTree( m_tree );
        m_tree = nullptr;
    }

    m_parser = nullptr;

    Clear();
}


void COMPILER::Clear()
{
    m_tokenizer.Clear();

    if( m_tree )
    {
        freeTree( m_tree );
        m_tree = nullptr;
    }

    m_tree = nullptr;

    for( TREE_NODE* tok : m_gcItems )
        delete tok;

    for( QString* tok: m_gcStrings )
        delete tok;

    m_gcItems.clear();
    m_gcStrings.clear();
}


void COMPILER::parseError( const char* s )
{
    reportError( CST_PARSE, s );
}


void COMPILER::parseOk()
{
    m_parseFinished = true;
}


bool COMPILER::Compile( const QString& aString, UCODE* aCode, CONTEXT* aPreflightContext )
{
    newString( aString );

    if( m_tree )
    {
        freeTree( m_tree );
        m_tree = nullptr;
    }

    m_tree = nullptr;
    m_parseFinished = false;
    T_TOKEN tok( defaultToken );

    libeval_dbg(0, "str: '%s' empty: %d\n", aString.toUtf8().data(), !!aString.isEmpty() );

    if( aString.isEmpty() )
    {
        m_parseFinished = true;
        return generateUCode( aCode, aPreflightContext );
    }

    do
    {
        m_sourcePos = m_tokenizer.GetPos();

        tok = getToken();

        if( tok.value.str )
            GcItem( tok.value.str );

        libeval_dbg(10, "parse: tok %d valstr %p\n", tok.token, tok.value.str );
        Parse( m_parser, tok.token, tok, this );

        if ( m_errorStatus.pendingError )
            return false;

        if( m_parseFinished || tok.token == G_ENDS )
        {
            Parse( m_parser, 0, tok, this );
            break;
        }
    } while( tok.token );

    return generateUCode( aCode, aPreflightContext );
}


void COMPILER::newString( const QString& aString )
{
    Clear();

    m_lexerState = LS_DEFAULT;
    m_tokenizer.Restart( aString );
    m_parseFinished = false;
}


T_TOKEN COMPILER::getToken()
{
    T_TOKEN rv;
    rv.value = defaultTokenValue;

    bool    done = false;

    do
    {
        switch( m_lexerState )
        {
        case LS_DEFAULT:
            done = lexDefault( rv );
            break;

        case LS_STRING:
            done = lexString( rv );
            break;
        }
    } while( !done );

    return rv;
}


bool COMPILER::lexString( T_TOKEN& aToken )
{
    aToken.token = G_STRING;
    aToken.value.str = new QString( m_tokenizer.GetString() );

    m_lexerState = LS_DEFAULT;
    return true;
}


int COMPILER::resolveUnits()
{
    int unitId = 0;

    for( const QString& unitName : m_unitResolver->GetSupportedUnits() )
    {
        if( m_tokenizer.MatchAhead( unitName,
                                    []( QChar c ) -> bool
                                    {
                                        return !c.isLetterOrNumber();
                                    } ) )
        {
            libeval_dbg(10, "Match unit '%s'\n", unitName.toUtf8().data() );
            m_tokenizer.NextChar( unitName.length() );
            return unitId;
        }

        unitId++;
    }

    return -1;
}


bool COMPILER::lexDefault( T_TOKEN& aToken )
{
    T_TOKEN  retval;
    QString current;
    int      convertFrom;
    QString msg;

    retval.value.str = nullptr;
    retval.value.num = 0.0;
    retval.value.idx = -1;
    retval.token = G_ENDS;

    if( m_tokenizer.Done() )
    {
        aToken = retval;
        return true;
    }

    auto isDecimalSeparator =
            [&]( QChar ch ) -> bool
            {
                return ( ch == m_localeDecimalSeparator || ch == '.' || ch == ',' );
            };

    auto extractNumber =
            [&]()
            {
                bool      haveSeparator = false;
                QChar ch = QChar(m_tokenizer.GetChar());

                do
                {
                    if( isDecimalSeparator( ch ) && haveSeparator )
                        break;

                    current.append( ch );

                    if( isDecimalSeparator( ch ) )
                        haveSeparator = true;

                    m_tokenizer.NextChar();
                    ch = QChar(m_tokenizer.GetChar());
                } while( ch.isDigit() || isDecimalSeparator( ch ) );

                for( int i = current.length(); i; i-- )
                {
                    if( isDecimalSeparator( current[i - 1] ) )
                        current[i - 1] = QChar(m_localeDecimalSeparator);
                }
            };

    QChar ch;

    for( ;; )
    {
        ch = QChar(m_tokenizer.GetChar());

        if( ch == ' ' )
            m_tokenizer.NextChar();
        else
            break;
    }

    libeval_dbg(10, "LEX ch '%c' pos %lu\n", ch.toLatin1(), (unsigned long)m_tokenizer.GetPos() );

    if( ch == 0 )
    {
        /* End of input */
    }
    else if( ch.isDigit() )
    {
        extractNumber();
        retval.token = G_VALUE;
        retval.value.str = new QString( current );
    }
    else if( ( convertFrom = resolveUnits() ) >= 0 )
    {
        retval.token            = G_UNIT;
        retval.value.idx        = convertFrom;
    }
    else if( ch == '\'' )
    {
        m_lexerState = LS_STRING;
        m_tokenizer.NextChar();
        return false;
    }
    else if( ch.isLetter() || ch == '_' )
    {
        current = m_tokenizer.GetChars( []( QChar c ) -> bool { return c.isLetterOrNumber() || c == '_'; } );
        retval.token = G_IDENTIFIER;
        retval.value.str = new QString( current );
        m_tokenizer.NextChar( current.length() );
    }
    else if( m_tokenizer.MatchAhead( "==", []( QChar c ) -> bool { return c != '='; } ) )
    {
        retval.token = G_EQUAL;
        m_tokenizer.NextChar( 2 );
    }
    else if( m_tokenizer.MatchAhead( "!=", []( QChar c ) -> bool { return c != '='; } ) )
    {
        retval.token = G_NOT_EQUAL;
        m_tokenizer.NextChar( 2 );
    }
    else if( m_tokenizer.MatchAhead( "<=", []( QChar c ) -> bool { return c != '='; } ) )
    {
        retval.token = G_LESS_EQUAL_THAN;
        m_tokenizer.NextChar( 2 );
    }
    else if( m_tokenizer.MatchAhead( ">=", []( QChar c ) -> bool { return c != '='; } ) )
    {
        retval.token = G_GREATER_EQUAL_THAN;
        m_tokenizer.NextChar( 2 );
    }
    else if( m_tokenizer.MatchAhead( "&&", []( QChar c ) -> bool { return c != '&'; } ) )
    {
        retval.token = G_BOOL_AND;
        m_tokenizer.NextChar( 2 );
    }
    else if( m_tokenizer.MatchAhead( "||", []( QChar c ) -> bool { return c != '|'; } ) )
    {
        retval.token = G_BOOL_OR;
        m_tokenizer.NextChar( 2 );
    }
    else
    {
        switch( ch.toLatin1() )
        {
        case '+': retval.token = G_PLUS;         break;
        case '!': retval.token = G_BOOL_NOT;     break;
        case '-': retval.token = G_MINUS;        break;
        case '*': retval.token = G_MULT;         break;
        case '/': retval.token = G_DIVIDE;       break;
        case '<': retval.token = G_LESS_THAN;    break;
        case '>': retval.token = G_GREATER_THAN; break;
        case '(': retval.token = G_PARENL;       break;
        case ')': retval.token = G_PARENR;       break;
        case ';': retval.token = G_SEMCOL;       break;
        case '.': retval.token = G_STRUCT_REF;   break;
        case ',': retval.token = G_COMMA;        break;

        default:
            reportError( CST_PARSE, QString::asprintf( "Unrecognized character '%c'", (char) ch.toLatin1() ) );
            break;
        }

        m_tokenizer.NextChar();
    }

    aToken = retval;
    return true;
}


const QString formatNode( TREE_NODE* node )
{
    return node->value.str ? *(node->value.str) : QString();
}


void dumpNode( QString& buf, TREE_NODE* tok, int depth = 0 )
{
    QString str;

    if( !tok )
        return;

    str = QString::asprintf( "\n[%p L0:%-20p L1:%-20p] ", tok, tok->leaf[0], tok->leaf[1] );
    buf += str;

    for( int i = 0; i < 2 * depth; i++ )
        buf += "  ";

    if( tok->op & TR_OP_BINARY_MASK )
    {
        buf += formatOpName( tok->op );
        dumpNode( buf, tok->leaf[0], depth + 1 );
        dumpNode( buf, tok->leaf[1], depth + 1 );
    }

    switch( tok->op )
    {
    case TR_NUMBER:
        buf += "NUMERIC: ";
        buf += formatNode( tok );

        if( tok->leaf[0] )
            dumpNode( buf, tok->leaf[0], depth + 1 );

        break;

    case TR_ARG_LIST:
        buf += "ARG_LIST: ";
        buf += formatNode( tok );

        if( tok->leaf[0] )
            dumpNode( buf, tok->leaf[0], depth + 1 );
        if( tok->leaf[1] )
            dumpNode( buf, tok->leaf[1], depth + 1 );

        break;

    case TR_STRING:
        buf += "STRING: ";
        buf +=  formatNode( tok );
        break;

    case TR_IDENTIFIER:
        buf += "ID: ";
        buf += formatNode( tok );
        break;

    case TR_STRUCT_REF:
        buf += "SREF: ";
        dumpNode( buf, tok->leaf[0], depth + 1 );
        dumpNode( buf, tok->leaf[1], depth + 1 );
        break;

     case TR_OP_FUNC_CALL:
        buf += "CALL '";
        buf += formatNode( tok->leaf[0] );
        buf += "': ";
        dumpNode( buf, tok->leaf[1], depth + 1 );
        break;

    case TR_UNIT:
        str = QString::asprintf( "UNIT: %d ", tok->value.idx );
        buf += str;
        break;
    }
}


void CONTEXT::ReportError( const QString& aErrorMsg )
{
    if( m_errorCallback )
        m_errorCallback( aErrorMsg, -1 );
}


void COMPILER::reportError( COMPILATION_STAGE stage, const QString& aErrorMsg, int aPos )
{
    if( aPos == -1 )
        aPos = m_sourcePos;

    m_errorStatus.pendingError = true;
    m_errorStatus.stage = stage;
    m_errorStatus.message = aErrorMsg;
    m_errorStatus.srcPos = aPos;

    if( m_errorCallback )
        m_errorCallback( aErrorMsg, aPos );
}


void COMPILER::setRoot( TREE_NODE *root )
{
    m_tree = root;
}


void COMPILER::freeTree( LIBEVAL::TREE_NODE *tree )
{
    if ( tree->leaf[0] )
        freeTree( tree->leaf[0] );

    if ( tree->leaf[1] )
        freeTree( tree->leaf[1] );

    delete tree->uop;
    tree->uop = nullptr;
}


void TREE_NODE::SetUop( int aOp, double aValue )
{
    delete uop;

    std::unique_ptr<VALUE> val = std::make_unique<VALUE>( aValue );
    uop = new UOP( aOp, std::move( val ) );
}


void TREE_NODE::SetUop( int aOp, const QString& aValue, bool aStringIsWildcard )
{
    delete uop;

    std::unique_ptr<VALUE> val = std::make_unique<VALUE>( aValue, aStringIsWildcard );
    uop = new UOP( aOp, std::move( val ) );
}


void TREE_NODE::SetUop( int aOp, std::unique_ptr<VAR_REF> aRef )
{
    delete uop;

    uop = new UOP( aOp, std::move( aRef ) );
}


void TREE_NODE::SetUop( int aOp, FUNC_CALL_REF aFunc, std::unique_ptr<VAR_REF> aRef )
{
    delete uop;

    uop = new UOP( aOp, std::move( aFunc ), std::move( aRef ) );
}


static void prepareTree( LIBEVAL::TREE_NODE *node )
{
    node->isVisited = false;

    if( node->op == TR_OP_FUNC_CALL && node->leaf[0] )
    {
        node->leaf[0]->leaf[0] = nullptr;
        node->leaf[0]->leaf[1] = nullptr;
    }

    if ( node->leaf[0] )
        prepareTree( node->leaf[0] );

    if ( node->leaf[1] )
        prepareTree( node->leaf[1] );
}


static std::vector<TREE_NODE*> squashParamList( TREE_NODE* root )
{
    std::vector<TREE_NODE*> args;

    if( !root )
        return args;

    if( root->op != TR_ARG_LIST && root->op != TR_NULL )
    {
        args.push_back( root );
    }
    else
    {
        TREE_NODE *n = root;
        do
        {
            if( n->leaf[1] )
                args.push_back(n->leaf[1]);

            n = n->leaf[0];
        } while ( n && n->op == TR_ARG_LIST );

        if( n )
            args.push_back( n );
    }

    std::reverse( args.begin(), args.end() );

    for( size_t i = 0; i < args.size(); i++ )
        libeval_dbg( 10, "squash arg%d: %s\n", int( i ), formatNode( args[i] ).toUtf8().data() );

    return args;
}


bool COMPILER::generateUCode( UCODE* aCode, CONTEXT* aPreflightContext )
{
    std::vector<TREE_NODE*> stack;
    QString                msg;
    int                     numericValueCount = 0;
    QString                missingUnitsMsg;
    int                     missingUnitsSrcPos = 0;

    if( !m_tree )
    {
        std::unique_ptr<VALUE> val = std::make_unique<VALUE>( 1.0 );
        aCode->AddOp( new UOP( TR_UOP_PUSH_VALUE, std::move(val) ) );
        return true;
    }

    prepareTree( m_tree );

    stack.push_back( m_tree );

    QString dump;

    dumpNode( dump, m_tree, 0 );
    libeval_dbg( 3, "Tree dump:\n%s\n\n", dump.toUtf8().data() );

    while( !stack.empty() )
    {
        TREE_NODE* node = stack.back();

        libeval_dbg( 4, "process node %p [op %d] [stack %lu]\n", node, node->op, (unsigned long)stack.size() );

        switch( node->op )
        {
        case TR_OP_FUNC_CALL:
            if( !node->uop )
                reportError( CST_CODEGEN,  "Unknown parent of function parameters", node->srcPos );

            node->isTerminal = true;
            break;

        case TR_STRUCT_REF:
        {
            if( node->leaf[0]->op != TR_IDENTIFIER )
            {
                int pos = node->leaf[0]->srcPos;

                if( node->leaf[0]->value.str )
                    pos -= static_cast<int>( formatNode( node->leaf[0] ).length() );

                reportError( CST_CODEGEN,  "Unknown parent of property", pos );

                node->leaf[0]->isVisited = true;
                node->leaf[1]->isVisited = true;

                node->SetUop( TR_UOP_PUSH_VALUE, 0.0 );
                node->isTerminal = true;
                break;
            }

            switch( node->leaf[1]->op )
            {
                case TR_IDENTIFIER:
                {
                    QString itemName = formatNode( node->leaf[0] );
                    QString propName = formatNode( node->leaf[1] );
                    std::unique_ptr<VAR_REF> vref = aCode->CreateVarRef( itemName, propName );

                    if( !vref )
                    {
                        msg = QString::asprintf( "Unrecognized item '%s'", itemName.toUtf8().data() );
                        reportError( CST_CODEGEN, msg, node->leaf[0]->srcPos - (int) itemName.length() );
                    }
                    else if( vref->GetType() == VT_PARSE_ERROR )
                    {
                        msg = QString::asprintf( "Unrecognized property '%s'", propName.toUtf8().data() );
                        reportError( CST_CODEGEN, msg, node->leaf[1]->srcPos - (int) propName.length() );
                    }

                    node->leaf[0]->isVisited = true;
                    node->leaf[1]->isVisited = true;

                    node->SetUop( TR_UOP_PUSH_VAR, std::move( vref ) );
                    node->isTerminal = true;
                    break;
                }
                case TR_OP_FUNC_CALL:
                {
                    QString                 itemName = formatNode( node->leaf[0] );
                    std::unique_ptr<VAR_REF> vref = aCode->CreateVarRef( itemName, "" );

                    if( !vref )
                    {
                        msg = QString::asprintf( "Unrecognized item '%s'", itemName.toUtf8().data() );
                        reportError( CST_CODEGEN, msg, node->leaf[0]->srcPos - (int) itemName.length() );
                    }

                    QString functionName = formatNode( node->leaf[1]->leaf[0] );
                    auto  func = aCode->CreateFuncCall( functionName );
                    std::vector<TREE_NODE*> params = squashParamList( node->leaf[1]->leaf[1] );

                    libeval_dbg( 10, "emit func call: %s\n", functionName.toUtf8().data() );

                    if( !func )
                    {
                        msg = QString::asprintf( "Unrecognized function '%s'", functionName.toUtf8().data() );
                        reportError( CST_CODEGEN, msg, node->leaf[0]->srcPos + 1 );
                    }

                    if( func )
                    {
                        for( TREE_NODE* pnode : params )
                        {
                            VALUE*   param = aPreflightContext->AllocValue();
                            param->Set( formatNode( pnode ) );
                            aPreflightContext->Push( param );
                        }

                        aPreflightContext->SetErrorCallback(
                                [&]( const QString& aMessage, int aOffset )
                                {
                                    size_t loc = node->leaf[1]->leaf[1]->srcPos;
                                    reportError( CST_CODEGEN, aMessage, (int) loc - 1 );
                                } );

                        try
                        {
                            func( aPreflightContext, vref.get() );
                            aPreflightContext->Pop();
                        }
                        catch( ... )
                        {
                        }
                    }

                    node->leaf[0]->isVisited = true;
                    node->leaf[1]->isVisited = true;
                    node->leaf[1]->leaf[0]->isVisited = true;
                    node->leaf[1]->leaf[1]->isVisited = true;

                    stack.pop_back();
                    stack.push_back( node->leaf[1] );

                    for( TREE_NODE* pnode : params )
                        stack.push_back( pnode );

                    node->leaf[1]->SetUop( TR_OP_METHOD_CALL, func, std::move( vref ) );
                    node->isTerminal = false;
                    break;
                }

                default:
                    QString itemName = formatNode( node->leaf[0] );
                    QString propName = formatNode( node->leaf[1] );
                    std::unique_ptr<VAR_REF> vref = aCode->CreateVarRef( itemName, propName );

                    if( !vref )
                    {
                        msg = QString::asprintf( "Unrecognized item '%s'", itemName.toUtf8().data() );
                        reportError( CST_CODEGEN, msg, node->leaf[0]->srcPos - (int) itemName.length() );
                    }

                    msg = QString::asprintf( "Unrecognized property '%s'", propName.toUtf8().data() );
                    reportError( CST_CODEGEN, msg, node->leaf[0]->srcPos + 1 );

                    node->leaf[0]->isVisited = true;
                    node->leaf[1]->isVisited = true;

                    node->SetUop( TR_UOP_PUSH_VALUE, 0.0 );
                    node->isTerminal = true;
                    break;
            }

            break;
        }

        case TR_NUMBER:
        {
            TREE_NODE* son = node->leaf[0];
            double     value;

            if( !node->value.str )
            {
                value = 0.0;
            }
            else if( son && son->op == TR_UNIT )
            {
                if( m_unitResolver->GetSupportedUnits().empty() )
                {
                    msg = QString::asprintf( "Unexpected units for '%s'", formatNode( node ).toUtf8().data() );
                    reportError( CST_CODEGEN, msg, node->srcPos );
                }

                int units = son->value.idx;
                value =  m_unitResolver->Convert( formatNode( node ), units );
                son->isVisited = true;
            }
            else
            {
                if( !m_unitResolver->GetSupportedUnitsMessage().isEmpty() )
                {
                    missingUnitsMsg = QString::asprintf( "Missing units for '%s'| (%s)",
                                            formatNode( node ).toUtf8().data(),
                                            m_unitResolver->GetSupportedUnitsMessage().toUtf8().data() );
                    missingUnitsSrcPos = node->srcPos;
                }

                value = EDA_UNIT_UTILS::UI::DoubleValueFromString( formatNode( node ) );
            }

            node->SetUop( TR_UOP_PUSH_VALUE, value );
            node->isTerminal = true;
            numericValueCount++;
            break;
        }

        case TR_STRING:
        {
            QString str = formatNode( node );
            bool isWildcard = str.contains("?") || str.contains("*");
            node->SetUop( TR_UOP_PUSH_VALUE, str, isWildcard );
            node->isTerminal = true;
            break;
        }

        case TR_IDENTIFIER:
        {
            std::unique_ptr<VAR_REF> vref = aCode->CreateVarRef( formatNode( node ), "" );

            if( !vref )
            {
                msg = QString::asprintf( "Unrecognized item '%s'", formatNode( node ).toUtf8().data() );
                reportError( CST_CODEGEN, msg, node->srcPos - (int) formatNode( node ).length() );
            }

            node->SetUop( TR_UOP_PUSH_VAR, std::move( vref ) );
            node->isTerminal = true;
            break;
        }

        default:
            node->SetUop( node->op );
            node->isTerminal = ( !node->leaf[0] || node->leaf[0]->isVisited )
                                    && ( !node->leaf[1] || node->leaf[1]->isVisited );
            break;
        }

        if( !node->isTerminal )
        {
            if( node->leaf[0] && !node->leaf[0]->isVisited )
            {
                stack.push_back( node->leaf[0] );
                node->leaf[0]->isVisited = true;
                continue;
            }
            else if( node->leaf[1] && !node->leaf[1]->isVisited )
            {
                stack.push_back( node->leaf[1] );
                node->leaf[1]->isVisited = true;
            }

            continue;
        }

        node->isVisited = true;

        if( node->uop )
        {
            aCode->AddOp( node->uop );
            node->uop = nullptr;
        }

        stack.pop_back();
    }

    if( !missingUnitsMsg.isEmpty() && numericValueCount == 1 )
        reportError( CST_CODEGEN, missingUnitsMsg, missingUnitsSrcPos );

    libeval_dbg(2,"dump: \n%s\n", aCode->Dump().toUtf8().data() );

    return true;
}


void UOP::Exec( CONTEXT* ctx )
{
    switch( m_op )
    {
    case TR_UOP_PUSH_VAR:
    {
        VALUE* value = nullptr;

        if( m_ref )
            value = ctx->StoreValue( m_ref->GetValue( ctx ) );
        else
            value = ctx->AllocValue();

        ctx->Push( value );
        break;
    }

    case TR_UOP_PUSH_VALUE:
        ctx->Push( m_value.get() );
        return;

    case TR_OP_METHOD_CALL:
        m_func( ctx, m_ref.get() );
        return;

    default:
        break;
    }

#define AS_DOUBLE( arg ) ( arg ? arg->AsDouble() : 0.0 )

    if( m_op & TR_OP_BINARY_MASK )
    {
        LIBEVAL::VALUE* arg2 = ctx->Pop();
        LIBEVAL::VALUE* arg1 = ctx->Pop();
        double          result;

        if( ctx->HasErrorCallback() )
        {
            if( arg1 && arg1->GetType() == VT_STRING && arg2 && arg2->GetType() == VT_NUMERIC )
            {
                ctx->ReportError( QString::asprintf( "Type mismatch between '%s' and %lf",
                                                    arg1->AsString().toUtf8().data(),
                                                    arg2->AsDouble() ) );
            }
            else if( arg1 && arg1->GetType() == VT_NUMERIC && arg2 && arg2->GetType() == VT_STRING )
            {
                ctx->ReportError( QString::asprintf( "Type mismatch between %lf and '%s'",
                                                    arg1->AsDouble(),
                                                    arg2->AsString().toUtf8().data() ) );
            }
        }

        switch( m_op )
        {
        case TR_OP_ADD:
            result = AS_DOUBLE( arg1 ) + AS_DOUBLE( arg2 );
            break;

        case TR_OP_SUB:
            result = AS_DOUBLE( arg1 ) - AS_DOUBLE( arg2 );
            break;

        case TR_OP_MUL:
            result = AS_DOUBLE( arg1 ) * AS_DOUBLE( arg2 );
            break;

        case TR_OP_DIV:
            result = AS_DOUBLE( arg1 ) / AS_DOUBLE( arg2 );
            break;

        case TR_OP_LESS_EQUAL:
            result = AS_DOUBLE( arg1 ) <= AS_DOUBLE( arg2 ) ? 1 : 0;
            break;

        case TR_OP_GREATER_EQUAL:
            result = AS_DOUBLE( arg1 ) >= AS_DOUBLE( arg2 ) ? 1 : 0;
            break;

        case TR_OP_LESS:
            result = AS_DOUBLE( arg1 ) < AS_DOUBLE( arg2 ) ? 1 : 0;
            break;
        case TR_OP_GREATER:
            result = AS_DOUBLE( arg1 ) > AS_DOUBLE( arg2 ) ? 1 : 0;
            break;

        case TR_OP_EQUAL:
            if( !arg1 || !arg2 )
                result = arg1 == arg2 ? 1 : 0;
            else if( arg2->GetType() == VT_UNDEFINED )
                result = arg2->EqualTo( ctx, arg1 ) ? 1 : 0;
            else
                result = arg1->EqualTo( ctx, arg2 ) ? 1 : 0;
            break;

        case TR_OP_NOT_EQUAL:
            if( !arg1 || !arg2 )
                result = arg1 != arg2 ? 1 : 0;
            else if( arg2->GetType() == VT_UNDEFINED )
                result = arg2->NotEqualTo( ctx, arg1 ) ? 1 : 0;
            else
                result = arg1->NotEqualTo( ctx, arg2 ) ? 1 : 0;
            break;

        case TR_OP_BOOL_AND:
            result = AS_DOUBLE( arg1 ) != 0.0 && AS_DOUBLE( arg2 ) != 0.0 ? 1 : 0;
            break;

        case TR_OP_BOOL_OR:
            result = AS_DOUBLE( arg1 ) != 0.0 || AS_DOUBLE( arg2 ) != 0.0 ? 1 : 0;
            break;

        default:
            result = 0.0;
            break;
        }

        VALUE* rp = ctx->AllocValue();
        rp->Set( result );
        ctx->Push( rp );
        return;
    }
    else if( m_op & TR_OP_UNARY_MASK )
    {
        LIBEVAL::VALUE* arg1 = ctx->Pop();
        double          ARG1VALUE = arg1 ? arg1->AsDouble() : 0.0;
        double          result;

        switch( m_op )
        {
        case TR_OP_BOOL_NOT:
            result = ARG1VALUE != 0.0 ? 0 : 1;
            break;
        default:
            result = ARG1VALUE != 0.0 ? 1 : 0;
            break;
        }

        VALUE* rp = ctx->AllocValue();
        rp->Set( result );
        ctx->Push( rp );
        return;
    }
}


VALUE* UCODE::Run( CONTEXT* ctx )
{
    try
    {
        for( UOP* op : m_ucode )
            op->Exec( ctx );
    }
    catch(...)
    {
        std::unique_ptr<VALUE> temp_false = std::make_unique<VALUE>( 0 );
        return ctx->StoreValue( temp_false.get() );
    }

    if( ctx->SP() == 1 )
    {
        return ctx->Pop();
    }
    else
    {
        Q_ASSERT( ctx->SP() == 1 );

        std::unique_ptr<VALUE> temp_false = std::make_unique<VALUE>( 0 );
        return ctx->StoreValue( temp_false.get() );
    }
}


} // namespace LIBEVAL