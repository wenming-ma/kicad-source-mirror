#include <QString>
#include <QRegExp>
#include <board.h>
#include <zones.h>
#include <drc/drc_rule_parser.h>
#include <drc/drc_rule_condition.h>
#include <drc_rules_lexer.h>
#include <pcbexpr_evaluator.h>
#include <reporter.h>

using namespace DRCRULE_T;


DRC_RULES_PARSER::DRC_RULES_PARSER( const QString& aSource, const QString& aSourceDescr ) :
        DRC_RULES_LEXER( aSource.toStdString(), aSourceDescr ),
        m_requiredVersion( 0 ),
        m_tooRecent( false ),
        m_reporter( nullptr )
{
}


DRC_RULES_PARSER::DRC_RULES_PARSER( FILE* aFile, const QString& aFilename ) :
        DRC_RULES_LEXER( aFile, aFilename ),
        m_requiredVersion( 0 ),
        m_tooRecent( false ),
        m_reporter( nullptr )
{
}


void DRC_RULES_PARSER::reportError( const QString& aMessage )
{
    QString rest;
    QString first = aMessage.section( '|', 0, 0 );
    int pipePos = aMessage.indexOf( '|' );
    if( pipePos != -1 )
        rest = aMessage.mid( pipePos );

    if( m_reporter )
    {
        QString msg = QString::asprintf( _( "ERROR: <a href='%d:%d'>%s</a>%s" ).toStdString().c_str(), CurLineNumber(),
                                         CurOffset(), first.toStdString().c_str(), rest.toStdString().c_str() );

        m_reporter->Report( msg, RPT_SEVERITY_ERROR );
    }
    else
    {
        QString msg = QString::asprintf( _( "ERROR: %s%s" ).toStdString().c_str(), first.toStdString().c_str(), rest.toStdString().c_str() );

        THROW_PARSE_ERROR( msg, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
    }
}


void DRC_RULES_PARSER::reportDeprecation( const QString& oldToken, const QString newToken )
{
    if( m_reporter )
    {
        QString msg = QString::asprintf( _( "The '%s' keyword has been deprecated.  "
                                            "Please use '%s' instead." ).toStdString().c_str(),
                                         oldToken.toStdString().c_str(),
                                         newToken.toStdString().c_str());

        m_reporter->Report( msg, RPT_SEVERITY_WARNING );
    }
}


void DRC_RULES_PARSER::parseUnknown()
{
    int depth = 1;

    for( T token = NextTok();  token != T_EOF;  token = NextTok() )
    {
        if( token == T_LEFT )
            depth++;

        if( token == T_RIGHT )
        {
            if( --depth == 0 )
                break;
        }
    }
}


void DRC_RULES_PARSER::Parse( std::vector<std::shared_ptr<DRC_RULE>>& aRules, REPORTER* aReporter )
{
    bool     haveVersion = false;
    QString msg;

    m_reporter = aReporter;

    for( T token = NextTok(); token != T_EOF; token = NextTok() )
    {
        if( token != T_LEFT )
            reportError( _( "Missing '('." ) );

        token = NextTok();

        if( !haveVersion && token != T_version )
        {
            reportError( _( "Missing version statement." ) );
            haveVersion = true;     // don't keep on reporting it
        }

        switch( token )
        {
        case T_version:
            haveVersion = true;
            token = NextTok();

            if( (int) token == DSN_RIGHT )
            {
                reportError( _( "Missing version number." ) );
                break;
            }

            if( (int) token == DSN_NUMBER )
            {
                m_requiredVersion = (int)strtol( CurText(), nullptr, 10 );
                m_tooRecent = ( m_requiredVersion > DRC_RULE_FILE_VERSION );
                token = NextTok();
            }
            else
            {
                msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected version number." ).toStdString().c_str(),
                            FromUTF8().toStdString().c_str() );
                reportError( msg );
            }

            if( (int) token != DSN_RIGHT )
            {
                msg = QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(),
                            FromUTF8().toStdString().c_str() );
                reportError( msg );
                parseUnknown();
            }

            break;

        case T_rule:
            aRules.emplace_back( parseDRC_RULE() );
            break;

        case T_EOF:
            reportError( _( "Incomplete statement." ) );
            break;

        default:
            msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected %s." ).toStdString().c_str(), FromUTF8().toStdString().c_str(),
                        "rule or version" );
            reportError( msg );
            parseUnknown();
        }
    }

    if( m_reporter && !m_reporter->HasMessage() )
        m_reporter->Report( _( "No errors found." ), RPT_SEVERITY_INFO );

    m_reporter = nullptr;
}


std::shared_ptr<DRC_RULE> DRC_RULES_PARSER::parseDRC_RULE()
{
    std::shared_ptr<DRC_RULE> rule = std::make_shared<DRC_RULE>();

    T        token = NextTok();
    QString msg;

    if( !IsSymbol( token ) )
        reportError( _( "Missing rule name." ) );

    rule->m_Name = FromUTF8();

    for( token = NextTok(); token != T_RIGHT && token != T_EOF; token = NextTok() )
    {
        if( token != T_LEFT )
            reportError( _( "Missing '('." ) );

        token = NextTok();

        switch( token )
        {
        case T_constraint:
            parseConstraint( rule.get() );
            break;

        case T_condition:
            token = NextTok();

            if( (int) token == DSN_RIGHT )
            {
                reportError( _( "Missing condition expression." ) );
                break;
            }

            if( IsSymbol( token ) )
            {
                rule->m_Condition = new DRC_RULE_CONDITION( FromUTF8() );
                rule->m_Condition->Compile( m_reporter, CurLineNumber(), CurOffset() );
            }
            else
            {
                msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected quoted expression." ).toStdString().c_str(),
                            FromUTF8().toStdString().c_str() );
                reportError( msg );
            }

            if( (int) NextTok() != DSN_RIGHT )
            {
                reportError( QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(), FromUTF8().toStdString().c_str() ) );
                parseUnknown();
            }

            break;

        case T_layer:
            if( rule->m_LayerCondition != LSET::AllLayersMask() )
                reportError( _( "'layer' keyword already present." ) );

            rule->m_LayerCondition = parseLayer( &rule->m_LayerSource );
            break;

        case T_severity:
            rule->m_Severity = parseSeverity();
            break;

        case T_EOF:
            reportError( _( "Incomplete statement." ) );
            return rule;

        default:
            msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected %s." ).toStdString().c_str(), FromUTF8().toStdString().c_str(),
                        "constraint, condition, or disallow" );
            reportError( msg );
            parseUnknown();
        }
    }

    if( (int) CurTok() != DSN_RIGHT )
        reportError( _( "Missing ')'." ) );

    return rule;
}


void DRC_RULES_PARSER::parseConstraint( DRC_RULE* aRule )
{
    DRC_CONSTRAINT c;
    int            value;
    QString       msg;

    T token = NextTok();

    if( token == T_mechanical_clearance )
    {
        reportDeprecation( "mechanical_clearance", "physical_clearance" );
        token = T_physical_clearance;
    }
    else if( token == T_mechanical_hole_clearance )
    {
        reportDeprecation( "mechanical_hole_clearance", "physical_hole_clearance" );
        token = T_physical_hole_clearance;
    }
    else if( token == T_hole )
    {
        reportDeprecation( "hole", "hole_size" );
        token = T_hole_size;
    }
    else if( (int) token == DSN_RIGHT || token == T_EOF )
    {
        msg = QString::asprintf( _( "Missing constraint type.|  Expected %s." ).toStdString().c_str(),
                    "assertion, clearance, hole_clearance, edge_clearance, "
                         "physical_clearance, physical_hole_clearance, courtyard_clearance, "
                         "silk_clearance, hole_size, hole_to_hole, track_width, annular_width, "
                         "via_diameter, disallow, zone_connection, thermal_relief_gap, "
                         "thermal_spoke_width, min_resolved_spokes, length, skew, via_count, "
                         "diff_pair_gap or diff_pair_uncoupled" );
        reportError( msg );
        return;
    }

    switch( token )
    {
    case T_assertion:                 c.m_Type = ASSERTION_CONSTRAINT;                 break;
    case T_clearance:                 c.m_Type = CLEARANCE_CONSTRAINT;                 break;
    case T_creepage:                  c.m_Type = CREEPAGE_CONSTRAINT;                  break;
    case T_hole_clearance:            c.m_Type = HOLE_CLEARANCE_CONSTRAINT;            break;
    case T_edge_clearance:            c.m_Type = EDGE_CLEARANCE_CONSTRAINT;            break;
    case T_hole_size:                 c.m_Type = HOLE_SIZE_CONSTRAINT;                 break;
    case T_hole_to_hole:              c.m_Type = HOLE_TO_HOLE_CONSTRAINT;              break;
    case T_courtyard_clearance:       c.m_Type = COURTYARD_CLEARANCE_CONSTRAINT;       break;
    case T_silk_clearance:            c.m_Type = SILK_CLEARANCE_CONSTRAINT;            break;
    case T_text_height:               c.m_Type = TEXT_HEIGHT_CONSTRAINT;               break;
    case T_text_thickness:            c.m_Type = TEXT_THICKNESS_CONSTRAINT;            break;
    case T_track_width:               c.m_Type = TRACK_WIDTH_CONSTRAINT;               break;
    case T_track_angle:               c.m_Type = TRACK_ANGLE_CONSTRAINT;               break;
    case T_track_segment_length:      c.m_Type = TRACK_SEGMENT_LENGTH_CONSTRAINT;      break;
    case T_connection_width:          c.m_Type = CONNECTION_WIDTH_CONSTRAINT;          break;
    case T_annular_width:             c.m_Type = ANNULAR_WIDTH_CONSTRAINT;             break;
    case T_via_diameter:              c.m_Type = VIA_DIAMETER_CONSTRAINT;              break;
    case T_zone_connection:           c.m_Type = ZONE_CONNECTION_CONSTRAINT;           break;
    case T_thermal_relief_gap:        c.m_Type = THERMAL_RELIEF_GAP_CONSTRAINT;        break;
    case T_thermal_spoke_width:       c.m_Type = THERMAL_SPOKE_WIDTH_CONSTRAINT;       break;
    case T_min_resolved_spokes:       c.m_Type = MIN_RESOLVED_SPOKES_CONSTRAINT;       break;
    case T_disallow:                  c.m_Type = DISALLOW_CONSTRAINT;                  break;
    case T_length:                    c.m_Type = LENGTH_CONSTRAINT;                    break;
    case T_skew:                      c.m_Type = SKEW_CONSTRAINT;                      break;
    case T_via_count:                 c.m_Type = VIA_COUNT_CONSTRAINT;                 break;
    case T_diff_pair_gap:             c.m_Type = DIFF_PAIR_GAP_CONSTRAINT;             break;
    case T_diff_pair_uncoupled:       c.m_Type = MAX_UNCOUPLED_CONSTRAINT;             break;
    case T_physical_clearance:        c.m_Type = PHYSICAL_CLEARANCE_CONSTRAINT;        break;
    case T_physical_hole_clearance:   c.m_Type = PHYSICAL_HOLE_CLEARANCE_CONSTRAINT;   break;
    default:
        msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected %s." ).toStdString().c_str(), FromUTF8().toStdString().c_str(),
                    "assertion, clearance, hole_clearance, edge_clearance, "
                         "physical_clearance, physical_hole_clearance, courtyard_clearance, "
                         "silk_clearance, hole_size, hole_to_hole, track_width, track_angle, track_segment_length, annular_width, "
                         "disallow, zone_connection, thermal_relief_gap, thermal_spoke_width, "
                         "min_resolved_spokes, length, skew, via_count, via_diameter, "
                         "diff_pair_gap or diff_pair_uncoupled" );
        reportError( msg );
    }

    if( aRule->FindConstraint( c.m_Type ) )
    {
        msg = QString::asprintf( _( "Rule already has a '%s' constraint." ).toStdString().c_str(), FromUTF8().toStdString().c_str() );
        reportError( msg );
    }

    bool unitless = c.m_Type == VIA_COUNT_CONSTRAINT
                    || c.m_Type == MIN_RESOLVED_SPOKES_CONSTRAINT
                    || c.m_Type == TRACK_ANGLE_CONSTRAINT;

    if( c.m_Type == DISALLOW_CONSTRAINT )
    {
        for( token = NextTok();  token != T_RIGHT;  token = NextTok() )
        {
            if( (int) token == DSN_STRING )
                token = GetCurStrAsToken();

            switch( token )
            {
            case T_track:      c.m_DisallowFlags |= DRC_DISALLOW_TRACKS;     break;
            case T_via:        c.m_DisallowFlags |= DRC_DISALLOW_VIAS;       break;
            case T_micro_via:  c.m_DisallowFlags |= DRC_DISALLOW_MICRO_VIAS; break;
            case T_buried_via: c.m_DisallowFlags |= DRC_DISALLOW_BB_VIAS;    break;
            case T_pad:        c.m_DisallowFlags |= DRC_DISALLOW_PADS;       break;
            case T_zone:       c.m_DisallowFlags |= DRC_DISALLOW_ZONES;      break;
            case T_text:       c.m_DisallowFlags |= DRC_DISALLOW_TEXTS;      break;
            case T_graphic:    c.m_DisallowFlags |= DRC_DISALLOW_GRAPHICS;   break;
            case T_hole:       c.m_DisallowFlags |= DRC_DISALLOW_HOLES;      break;
            case T_footprint:  c.m_DisallowFlags |= DRC_DISALLOW_FOOTPRINTS; break;

            case T_EOF:
                reportError( _( "Missing ')'." ) );
                return;

            default:
                msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected %s." ).toStdString().c_str(), FromUTF8().toStdString().c_str(),
                            "track, via, micro_via, buried_via, pad, zone, text, graphic, "
                                 "hole, or footprint." );
                reportError( msg );
                break;
            }
        }

        if( (int) CurTok() != DSN_RIGHT )
            reportError( _( "Missing ')'." ) );

        aRule->AddConstraint( c );
        return;
    }
    else if( c.m_Type == ZONE_CONNECTION_CONSTRAINT )
    {
        token = NextTok();

        if( (int) token == DSN_STRING )
            token = GetCurStrAsToken();

        switch( token )
        {
        case T_solid:           c.m_ZoneConnection = ZONE_CONNECTION::FULL;    break;
        case T_thermal_reliefs: c.m_ZoneConnection = ZONE_CONNECTION::THERMAL; break;
        case T_none:            c.m_ZoneConnection = ZONE_CONNECTION::NONE;    break;

        case T_EOF:
            reportError( _( "Missing ')'." ) );
            return;

        default:
            msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected %s." ).toStdString().c_str(), FromUTF8().toStdString().c_str(),
                        "solid, thermal_reliefs or none." );
            reportError( msg );
            break;
        }

        if( (int) NextTok() != DSN_RIGHT )
            reportError( _( "Missing ')'." ) );

        aRule->AddConstraint( c );
        return;
    }
    else if( c.m_Type == MIN_RESOLVED_SPOKES_CONSTRAINT )
    {
        // We don't use a min/max/opt structure here because it would give a strong implication
        // that you could specify the optimal number of spokes.  We don't want to open that door
        // because the spoke generator is highly optimized around being able to "cheat" off of a
        // cartesian coordinate system.

        token = NextTok();

        if( (int) token == DSN_NUMBER )
        {
            value = (int) strtol( CurText(), nullptr, 10 );
            c.m_Value.SetMin( value );
        }
        else
        {
            reportError( _( "Expecting number." ) );
            parseUnknown();
        }

        if( (int) NextTok() != DSN_RIGHT )
            reportError( _( "Missing ')'." ) );

        aRule->AddConstraint( c );
        return;
    }
    else if( c.m_Type == ASSERTION_CONSTRAINT )
    {
        token = NextTok();

        if( (int) token == DSN_RIGHT )
            reportError( _( "Missing assertion expression." ) );

        if( IsSymbol( token ) )
        {
            c.m_Test = new DRC_RULE_CONDITION( FromUTF8() );
            c.m_Test->Compile( m_reporter, CurLineNumber(), CurOffset() );
        }
        else
        {
            msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected quoted expression." ).toStdString().c_str(), FromUTF8().toStdString().c_str() );
            reportError( msg );
        }

        if( (int) NextTok() != DSN_RIGHT )
        {
            reportError( QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(), FromUTF8().toStdString().c_str() ) );
            parseUnknown();
        }

        aRule->AddConstraint( c );
        return;
    }

    for( token = NextTok(); token != T_RIGHT && token != T_EOF; token = NextTok() )
    {
        if( token != T_LEFT )
            reportError( _( "Missing '('." ) );

        token = NextTok();

        switch( token )
        {
        case T_within_diff_pairs:
            if( c.m_Type != SKEW_CONSTRAINT )
            {
                reportError( _( "within_diff_pairs option invalid for constraint type." ) );
                break;
            }

            c.SetOption( DRC_CONSTRAINT::OPTIONS::SKEW_WITHIN_DIFF_PAIRS );

            if( (int) NextTok() != DSN_RIGHT )
            {
                reportError( QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(), FromUTF8().toStdString().c_str() ) );
                parseUnknown();
            }

            break;

        case T_min:
            token = NextTok();

            if( (int) token == DSN_RIGHT )
            {
                reportError( _( "Missing min value." ) );
                break;
            }

            parseValueWithUnits( FromUTF8(), value, unitless );
            c.m_Value.SetMin( value );

            if( (int) NextTok() != DSN_RIGHT )
            {
                reportError( QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(), FromUTF8().toStdString().c_str() ) );
                parseUnknown();
            }

            break;

        case T_max:
            token = NextTok();

            if( (int) token == DSN_RIGHT )
            {
                reportError( _( "Missing max value." ) );
                break;
            }

            parseValueWithUnits( FromUTF8(), value, unitless );

            c.m_Value.SetMax( value );

            if( (int) NextTok() != DSN_RIGHT )
            {
                reportError( QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(), FromUTF8().toStdString().c_str() ) );
                parseUnknown();
            }

            break;

        case T_opt:
            token = NextTok();

            if( (int) token == DSN_RIGHT )
            {
                reportError( _( "Missing opt value." ) );
                break;
            }

            parseValueWithUnits( FromUTF8(), value, unitless );
            c.m_Value.SetOpt( value );

            if( (int) NextTok() != DSN_RIGHT )
            {
                reportError( QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(), FromUTF8().toStdString().c_str() ) );
                parseUnknown();
            }

            break;

        case T_EOF:
            reportError( _( "Incomplete statement." ) );
            return;

        default:
            msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected %s." ).toStdString().c_str(),
                        FromUTF8().toStdString().c_str(),
                        "min, max, or opt" );
            reportError( msg );
            parseUnknown();
        }
    }

    if( (int) CurTok() != DSN_RIGHT )
        reportError( _( "Missing ')'." ) );

    aRule->AddConstraint( c );
}


void DRC_RULES_PARSER::parseValueWithUnits( const QString& aExpr, int& aResult, bool aUnitless )
{
    auto errorHandler =
            [&]( const QString& aMessage, int aOffset )
            {
                QString rest;
                QString first = aMessage.section( '|', 0, 0 );
                int pipePos = aMessage.indexOf( '|' );
                if( pipePos != -1 )
                    rest = aMessage.mid( pipePos );

                if( m_reporter )
                {
                    QString msg = QString::asprintf( _( "ERROR: <a href='%d:%d'>%s</a>%s" ).toStdString().c_str(),
                                                     CurLineNumber(), CurOffset() + aOffset, first.toStdString().c_str(), rest.toStdString().c_str() );

                    m_reporter->Report( msg, RPT_SEVERITY_ERROR );
                }
                else
                {
                    QString msg = QString::asprintf( _( "ERROR: %s%s" ).toStdString().c_str(), first.toStdString().c_str(), rest.toStdString().c_str() );

                    THROW_PARSE_ERROR( msg, CurSource(), CurLine(), CurLineNumber(),
                                       CurOffset() + aOffset );
                }
            };

    PCBEXPR_EVALUATOR evaluator( aUnitless ? (LIBEVAL::UNIT_RESOLVER*) new PCBEXPR_UNITLESS_RESOLVER()
                                            : (LIBEVAL::UNIT_RESOLVER*) new PCBEXPR_UNIT_RESOLVER() );
    evaluator.SetErrorCallback( errorHandler );

    evaluator.Evaluate( aExpr );
    aResult = evaluator.Result();
}


LSET DRC_RULES_PARSER::parseLayer( QString* aSource )
{
    LSET retVal;
    int  token = NextTok();

    if( (int) token == DSN_RIGHT )
    {
        reportError( _( "Missing layer name or type." ) );
        return LSET::AllCuMask();
    }
    else if( token == T_outer )
    {
        *aSource = GetTokenString( token );
        retVal = LSET::ExternalCuMask();
    }
    else if( token == T_inner )
    {
        *aSource = GetTokenString( token );
        retVal = LSET::InternalCuMask();
    }
    else
    {
        QString     layerName = FromUTF8();
        QStringList& layerMap = ENUM_MAP<PCB_LAYER_ID>::Instance().Choices();

        for( unsigned ii = 0; ii < layerMap.count(); ++ii )
        {
            const QString& entry = layerMap[ii];

            if( QRegExp( layerName, Qt::CaseInsensitive, QRegExp::Wildcard ).exactMatch( entry ) )
            {
                *aSource = layerName;
                retVal.set( ToLAYER_ID( ENUM_MAP<PCB_LAYER_ID>::Instance().ToEnum( entry ) ) );
            }
        }

        if( !retVal.any() )
        {
            reportError( QString::asprintf( _( "Unrecognized layer '%s'." ).toStdString().c_str(), layerName.toStdString().c_str() ) );
            retVal.set( Rescue );
        }
    }

    if( (int) NextTok() != DSN_RIGHT )
    {
        reportError( QString::asprintf( _( "Unrecognized item '%s'." ).toStdString().c_str(), FromUTF8().toStdString().c_str() ) );
        parseUnknown();
    }

    return retVal;
}


SEVERITY DRC_RULES_PARSER::parseSeverity()
{
    SEVERITY retVal = RPT_SEVERITY_UNDEFINED;
    QString msg;

    T token = NextTok();

    if( (int) token == DSN_RIGHT || token == T_EOF )
    {
        reportError( _( "Missing severity name." ) );
        return RPT_SEVERITY_UNDEFINED;
    }

    switch( token )
    {
    case T_ignore:    retVal = RPT_SEVERITY_IGNORE;    break;
    case T_warning:   retVal = RPT_SEVERITY_WARNING;   break;
    case T_error:     retVal = RPT_SEVERITY_ERROR;     break;
    case T_exclusion: retVal = RPT_SEVERITY_EXCLUSION; break;

    default:
        msg = QString::asprintf( _( "Unrecognized item '%s'.| Expected %s." ).toStdString().c_str(),
                    FromUTF8().toStdString().c_str(),
                    "ignore, warning, error, or exclusion" );
        reportError( msg );
        parseUnknown();
    }

    if( (int) NextTok() != DSN_RIGHT )
        reportError( _( "Missing ')'." ) );

    return retVal;
}