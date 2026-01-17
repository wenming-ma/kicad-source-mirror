
#include <board.h>
#include <footprint.h>
#include <pad.h>
#include <pcb_track.h>
#include <collectors.h>
#include <reporter.h>
#include <richio.h>
#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <gendrill_file_writer_base.h>


/* Helper function for sorting hole list.
 * Compare function used for sorting holes type type:
 * plated then not plated
 * then by increasing diameter value
 * then by attribute type (vias, pad, mechanical)
 * then by X then Y position
 */
static bool cmpHoleSorting( const HOLE_INFO& a, const HOLE_INFO& b )
{
    if( a.m_Hole_NotPlated != b.m_Hole_NotPlated )
        return b.m_Hole_NotPlated;

    if( a.m_Hole_Diameter != b.m_Hole_Diameter )
        return a.m_Hole_Diameter < b.m_Hole_Diameter;

    // At this point (same diameter, same plated type), group by attribute
    // type (via, pad, mechanical, although currently only not plated pads are mechanical)
    if( a.m_HoleAttribute != b.m_HoleAttribute )
        return a.m_HoleAttribute < b.m_HoleAttribute;

    // At this point (same diameter, same type), sort by X then Y position.
    // This is optimal for drilling and make the file reproducible as long as holes
    // have not changed, even if the data order has changed.
    if( a.m_Hole_Pos.x != b.m_Hole_Pos.x )
        return a.m_Hole_Pos.x < b.m_Hole_Pos.x;

    return a.m_Hole_Pos.y < b.m_Hole_Pos.y;
}


void GENDRILL_WRITER_BASE::buildHolesList( DRILL_LAYER_PAIR aLayerPair,
                                           bool aGenerateNPTH_list )
{
    HOLE_INFO new_hole;

    m_holeListBuffer.clear();
    m_toolListBuffer.clear();

    Q_ASSERT( aLayerPair.first < aLayerPair.second );  // fix the caller

    // build hole list for vias
    if( ! aGenerateNPTH_list )  // vias are always plated !
    {
        for( auto track : m_pcb->Tracks() )
        {
            if( track->Type() != PCB_VIA_T )
                continue;

            PCB_VIA* via = static_cast<PCB_VIA*>( track );
            int      hole_sz = via->GetDrillValue();

            if( hole_sz == 0 )   // Should not occur.
                continue;

            new_hole.m_ItemParent = via;

            if( aLayerPair == DRILL_LAYER_PAIR( F_Cu, B_Cu ) )
                new_hole.m_HoleAttribute = HOLE_ATTRIBUTE::HOLE_VIA_THROUGH;
            else
                new_hole.m_HoleAttribute = HOLE_ATTRIBUTE::HOLE_VIA_BURIED;

            new_hole.m_Tool_Reference = -1;         // Flag value for Not initialized
            new_hole.m_Hole_Orient    = ANGLE_0;
            new_hole.m_Hole_Diameter  = hole_sz;
            new_hole.m_Hole_NotPlated = false;
            new_hole.m_Hole_Size.x = new_hole.m_Hole_Size.y = new_hole.m_Hole_Diameter;

            new_hole.m_Hole_Shape = 0;              // hole shape: round
            new_hole.m_Hole_Pos = via->GetStart();

            via->LayerPair( &new_hole.m_Hole_Top_Layer, &new_hole.m_Hole_Bottom_Layer );

            // LayerPair() returns params with m_Hole_Bottom_Layer > m_Hole_Top_Layer
            // Remember: top layer = 0 and bottom layer = 31 for through hole vias
            // Any captured via should be from aLayerPair.first to aLayerPair.second exactly.
            if( new_hole.m_Hole_Top_Layer    != aLayerPair.first ||
                new_hole.m_Hole_Bottom_Layer != aLayerPair.second )
                continue;

            m_holeListBuffer.push_back( new_hole );
        }
    }

    if( aLayerPair == DRILL_LAYER_PAIR( F_Cu, B_Cu ) )
    {
        // add holes for thru hole pads
        for( FOOTPRINT* footprint : m_pcb->Footprints() )
        {
            for( PAD* pad : footprint->Pads() )
            {
                if( !m_merge_PTH_NPTH )
                {
                    if( !aGenerateNPTH_list && pad->GetAttribute() == PAD_ATTRIB::NPTH )
                        continue;

                    if( aGenerateNPTH_list && pad->GetAttribute() != PAD_ATTRIB::NPTH )
                        continue;
                }

                if( pad->GetDrillSize().x == 0 )
                    continue;

                new_hole.m_ItemParent     = pad;
                new_hole.m_Hole_NotPlated = (pad->GetAttribute() == PAD_ATTRIB::NPTH);
                new_hole.m_HoleAttribute  = new_hole.m_Hole_NotPlated
                                                ? HOLE_ATTRIBUTE::HOLE_MECHANICAL
                                                : HOLE_ATTRIBUTE::HOLE_PAD;
                new_hole.m_Tool_Reference = -1;         // Flag is: Not initialized
                new_hole.m_Hole_Orient    = pad->GetOrientation();
                new_hole.m_Hole_Shape     = 0;           // hole shape: round
                new_hole.m_Hole_Diameter  = std::min( pad->GetDrillSize().x, pad->GetDrillSize().y );
                new_hole.m_Hole_Size.x    = new_hole.m_Hole_Size.y = new_hole.m_Hole_Diameter;

                // Convert oblong holes that are actually circular into drill hits
                if( pad->GetDrillShape() != PAD_DRILL_SHAPE::CIRCLE &&
                        pad->GetDrillSizeX() != pad->GetDrillSizeY() )
                {
                    new_hole.m_Hole_Shape = 1; // oval flag set
                }

                new_hole.m_Hole_Size         = pad->GetDrillSize();
                new_hole.m_Hole_Pos          = pad->GetPosition();  // hole position
                new_hole.m_Hole_Bottom_Layer = B_Cu;
                new_hole.m_Hole_Top_Layer    = F_Cu;    // pad holes are through holes
                m_holeListBuffer.push_back( new_hole );
            }
        }
    }

    // Sort holes per increasing diameter value (and for each dimater, by position)
    sort( m_holeListBuffer.begin(), m_holeListBuffer.end(), cmpHoleSorting );

    // build the tool list
    int last_hole = -1;     // Set to not initialized (this is a value not used
                            // for m_holeListBuffer[ii].m_Hole_Diameter)
    bool last_notplated_opt = false;
    HOLE_ATTRIBUTE last_attribute = HOLE_ATTRIBUTE::HOLE_UNKNOWN;

    DRILL_TOOL new_tool( 0, false );
    unsigned   jj;

    for( unsigned ii = 0; ii < m_holeListBuffer.size(); ii++ )
    {
        if( m_holeListBuffer[ii].m_Hole_Diameter != last_hole
            || m_holeListBuffer[ii].m_Hole_NotPlated != last_notplated_opt
#if USE_ATTRIB_FOR_HOLES
            || m_holeListBuffer[ii].m_HoleAttribute != last_attribute
#endif
            )
        {
            new_tool.m_Diameter = m_holeListBuffer[ii].m_Hole_Diameter;
            new_tool.m_Hole_NotPlated = m_holeListBuffer[ii].m_Hole_NotPlated;
            new_tool.m_HoleAttribute = m_holeListBuffer[ii].m_HoleAttribute;
            m_toolListBuffer.push_back( new_tool );
            last_hole = new_tool.m_Diameter;
            last_notplated_opt = new_tool.m_Hole_NotPlated;
            last_attribute = new_tool.m_HoleAttribute;
        }

        jj = m_toolListBuffer.size();

        if( jj == 0 )
            continue;                                        // Should not occurs

        m_holeListBuffer[ii].m_Tool_Reference = jj;          // Tool value Initialized (value >= 1)

        m_toolListBuffer.back().m_TotalCount++;

        if( m_holeListBuffer[ii].m_Hole_Shape )
            m_toolListBuffer.back().m_OvalCount++;
    }
}


std::vector<DRILL_LAYER_PAIR> GENDRILL_WRITER_BASE::getUniqueLayerPairs() const
{
    Q_ASSERT( m_pcb );

    PCB_TYPE_COLLECTOR  vias;

    vias.Collect( m_pcb, { PCB_VIA_T } );

    std::set<DRILL_LAYER_PAIR> unique;
    DRILL_LAYER_PAIR           layer_pair;

    for( int i = 0; i < vias.GetCount(); ++i )
    {
        PCB_VIA*  v = static_cast<PCB_VIA*>( vias[i] );

        v->LayerPair( &layer_pair.first, &layer_pair.second );

        // only make note of blind buried.
        // thru hole is placed unconditionally as first in fetched list.
        if( layer_pair != DRILL_LAYER_PAIR( F_Cu, B_Cu ) )
            unique.insert( layer_pair );
    }

    std::vector<DRILL_LAYER_PAIR> ret;

    ret.emplace_back( F_Cu, B_Cu );      // always first in returned list

    for( const DRILL_LAYER_PAIR& pair : unique )
        ret.push_back( pair );

    return ret;
}


const std::string GENDRILL_WRITER_BASE::layerName( PCB_LAYER_ID aLayer ) const
{
    // Generic names here.
    switch( aLayer )
    {
    case F_Cu:
        return "front";
    case B_Cu:
        return "back";
    default:
    {
        // aLayer use even values, and the first internal layer (In1) is B_Cu + 2.
        int ly_id = ( aLayer - B_Cu ) / 2;
        return StrPrintf( "in%d", ly_id );
    }
    }
}


const std::string GENDRILL_WRITER_BASE::layerPairName( DRILL_LAYER_PAIR aPair ) const
{
    std::string ret = layerName( aPair.first );
    ret += '-';
    ret += layerName( aPair.second );

    return ret;
}


const QString GENDRILL_WRITER_BASE::getDrillFileName( DRILL_LAYER_PAIR aPair, bool aNPTH,
                                                       bool aMerge_PTH_NPTH ) const
{
    Q_ASSERT( m_pcb );

    QString extend;

    if( aNPTH )
        extend = "-NPTH";
    else if( aPair == DRILL_LAYER_PAIR( F_Cu, B_Cu ) )
    {
        if( !aMerge_PTH_NPTH )
            extend = "-PTH";
        // if merged, extend with nothing
    }
    else
    {
        extend += '-';
        extend += QString::fromStdString( layerPairName( aPair ) );
    }

    QFileInfo fn( m_pcb->GetFileName() );

    QString baseName = fn.baseName() + extend;
    QString ret = baseName + "." + m_drillFileExtension;

    return ret;
}

bool GENDRILL_WRITER_BASE::CreateMapFilesSet( const QString& aPlotDirectory,
                                              REPORTER * aReporter )
{
    QFileInfo fn;
    QString msg;

    std::vector<DRILL_LAYER_PAIR> hole_sets = getUniqueLayerPairs();

    // append a pair representing the NPTH set of holes, for separate drill files.
    if( !m_merge_PTH_NPTH )
        hole_sets.emplace_back( F_Cu, B_Cu );

    for( std::vector<DRILL_LAYER_PAIR>::const_iterator it = hole_sets.begin();
         it != hole_sets.end();  ++it )
    {
        DRILL_LAYER_PAIR  pair = *it;
        // For separate drill files, the last layer pair is the NPTH drill file.
        bool doing_npth = m_merge_PTH_NPTH ? false : ( it == hole_sets.end() - 1 );

        buildHolesList( pair, doing_npth );

        // The file is created if it has holes, or if it is the non plated drill file
        // to be sure the NPTH file is up to date in separate files mode.
        // Also a PTH drill file is always created, to be sure at least one plated hole drill file
        // is created (do not create any PTH drill file can be seen as not working drill generator).
        if( getHolesCount() > 0 || doing_npth || pair == DRILL_LAYER_PAIR( F_Cu, B_Cu ) )
        {
            QString drillFileName = getDrillFileName( pair, doing_npth, m_merge_PTH_NPTH );
            QFileInfo fileInfo( drillFileName );
            QString baseName = fileInfo.baseName();

            QString fullfilename = QDir( aPlotDirectory ).filePath( baseName + "-drl_map" );
            fullfilename += "." + GetDefaultPlotExtension( m_mapFileFmt );

            bool success = genDrillMapFile( fullfilename, m_mapFileFmt );

            if( ! success )
            {
                if( aReporter )
                {
                    msg = QString( "Failed to create file '%1'." ).arg( fullfilename );
                    aReporter->Report( msg, RPT_SEVERITY_ERROR );
                }

                return false;
            }
            else
            {
                if( aReporter )
                {
                    msg = QString( "Created file '%1'." ).arg( fullfilename );
                    aReporter->Report( msg, RPT_SEVERITY_ACTION );
                }
            }
        }
    }

    return true;
}


const QString GENDRILL_WRITER_BASE::BuildFileFunctionAttributeString(
                        DRILL_LAYER_PAIR aLayerPair, TYPE_FILE aHoleType,
                        bool aCompatNCdrill ) const
{
// Build a QString containing the .FileFunction attribute for drill files.
// %TF.FileFunction,Plated[NonPlated],layer1num,layer2num,PTH[NPTH][Blind][Buried],Drill[Route][Mixed]*%
    QString text;

    if( aCompatNCdrill )
        text = "; #@! ";
    else
        text = "%";

    text += "TF.FileFunction,";

    if( aHoleType == NPTH_FILE )
        text += "NonPlated,";
    else if( aHoleType == MIXED_FILE )  // only for Excellon format
        text += "MixedPlating,";
    else
        text += "Plated,";

    int layer1 = aLayerPair.first;
    int layer2 = aLayerPair.second;
    // In Gerber files, layers num are 1 to copper layer count instead of F_Cu to B_Cu
    // (0 to copper layer count-1)
    // Note also for a n copper layers board, gerber layers num are 1 ... n
    //
    // Copper layers use even values, so the layer id in file is
    // (Copper layer id) /2 + 1 if layer is not B_Cu
    if( layer1 == F_Cu )
        layer1 = 1;
    else
        layer1 = ( ( layer1 - B_Cu ) / 2 ) + 1;

    if( layer2 == B_Cu )
        layer2 = m_pcb->GetCopperLayerCount();
    else
        layer2 = ( ( layer2 - B_Cu ) / 2) + 1;

    text += QString::number( layer1 ) + "," + QString::number( layer2 );

    // Now add PTH or NPTH or Blind or Buried attribute
    int toplayer = 1;
    int bottomlayer = m_pcb->GetCopperLayerCount();

    if( aHoleType == NPTH_FILE )
        text += ",NPTH";
    else if( aHoleType == MIXED_FILE )      // only for Excellon format
    {
        // write nothing
    }
    else if( layer1 == toplayer && layer2 == bottomlayer )
        text += ",PTH";
    else if( layer1 == toplayer || layer2 == bottomlayer )
        text += ",Blind";
    else
        text += ",Buried";

    // In NC drill file, these previous parameters should be enough:
    if( aCompatNCdrill )
        return text;


    // Now add Drill or Route or Mixed:
    // file containing only round holes have Drill attribute
    // file containing only oblong holes have Routed attribute
    // file containing both holes have Mixed attribute
    bool hasOblong = false;
    bool hasDrill = false;

    for( unsigned ii = 0; ii < m_holeListBuffer.size(); ii++ )
    {
        const HOLE_INFO& hole_descr = m_holeListBuffer[ii];

        if( hole_descr.m_Hole_Shape )   // m_Hole_Shape not 0 is an oblong hole)
            hasOblong = true;
        else
            hasDrill = true;
    }

    if( hasOblong && hasDrill )
        text += ",Mixed";
    else if( hasDrill )
        text += ",Drill";
    else if( hasOblong )
        text += ",Rout";

    // else: empty file.

    // End of .FileFunction attribute:
    text += "*%";

    return text;
}
