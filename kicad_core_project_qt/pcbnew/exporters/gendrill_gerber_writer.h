// Classes used in drill files, map files and report files generation.

#ifndef _GENDRILL_GERBER_WRITER_
#define _GENDRILL_GERBER_WRITER_

#include <QString>
#include <gendrill_file_writer_base.h>

class BOARD;

// Used to create Gerber drill files.
class GERBER_WRITER: public GENDRILL_WRITER_BASE
{
public:
    GERBER_WRITER( BOARD* aPcb );

    virtual ~GERBER_WRITER()
    {
    }

    /**
     * Initialize internal parameters to match the given format.
     *
     * @param aRightDigits is the number of digits for mantissa part of coordinates (5 or 6).
     */
    void SetFormat( int aRightDigits = 6 );

    /**
     * Initialize internal parameters to match drill options.
     *
     * @note PTH and NPTH are always separate files in Gerber format.
     *
     * @param aOffset is the drill coordinates offset.
     */
    void SetOptions( const VECTOR2I& aOffset )
    {
        m_offset = aOffset;
        m_merge_PTH_NPTH = false;
    }

    /**
     * Create the full set of Excellon drill file for the board filenames are computed from
     * the board name, and layers id.
     *
     * @param aPlotDirectory is the output folder.
     * @param aGenDrill set to true to generate the EXCELLON drill file.
     * @param aGenMap set to true to generate a drill map file.
     * @param aReporter is a #REPORTER to return activity or any message (can be NULL).
     *
     * @return True if successful, false if any error occurred
     */
    bool CreateDrillandMapFilesSet( const QString& aPlotDirectory,
                                    bool aGenDrill, bool aGenMap,
                                    REPORTER * aReporter = nullptr );

private:
    /**
     * Create an Excellon drill file.
     *
     * @param aFullFilename is the full file name.
     * @param aIsNpth set to true for a NPTH file or false for a PTH file.
     * @param aLayerPair is the first board layer and the last board layer for this drill file
     *                   for blind buried vias, they are not always top and bottom layers/
     * @return hole count or -1 if the file cannot be created.
     */
    int createDrillFile( QString& aFullFilename, bool aIsNpth, DRILL_LAYER_PAIR aLayerPair );

    /**
     * @param aPair is the layer pair.
     * @param aNPTH set to true to generate the filename of NPTH holes.
     * @param aMerge_PTH_NPTH set to true to generate the filename of a file which contains both
     *                        NPH and NPTH holes
     * @return a filename which identify the drill file function.  It is the board name with the
     *         layer pair names added, and for separate (PTH and NPTH) files, "-NPH" or "-NPTH"
     *         added.
     */
    virtual const QString getDrillFileName( DRILL_LAYER_PAIR aPair, bool aNPTH,
                                             bool aMerge_PTH_NPTH ) const override;
};

#endif  //  #ifndef _GENDRILL_GERBER_WRITER_
