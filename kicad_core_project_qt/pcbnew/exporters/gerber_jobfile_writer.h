/**
 * @file gerber_jobfile_writer.h
 * @brief Classes used to generate a Gerber job file in JSON
 */


#ifndef GERBER_JOBFILE_WRITER_H
#define GERBER_JOBFILE_WRITER_H

#include <json_common.h>
#include <QStringList>


// A helper enum to handle sides of some layers (silk, mask)
enum ONSIDE
{
    SIDE_NONE = 0,      // layers not present
    SIDE_TOP = 1,       // top layer only
    SIDE_BOTTOM = 2,    // bottom layer only
    SIDE_BOTH = SIDE_TOP|SIDE_BOTTOM    // both layers
};

class BOARD;

/**
 * JOBFILE_PARAMS store the list of parameters written in Gerber job file
 * especially list of .gbr filenames and the corresponding layer id belonging the job
 */
class JOBFILE_PARAMS
{
public:
    QStringList m_GerberFileList;         // the list of gerber filenames (without path)
    std::vector<PCB_LAYER_ID> m_LayerId;    // the list of corresponding layer id
};


/**
 * GERBER_JOBFILE_WRITER is a class used to create Gerber job file
 * a Gerber job file stores info to make a board:
 * list of gerber files
 * info about the board itsel:
 *  size, number of copper layers
 *  thickness of the board, copper and dielectric
 *  and some other info (colors, finish type ...)
 *
 * note: dimensions are always in mm in Kicad job file (can be also in inches in a job file)
 * and they are in floating point notation
 */
class GERBER_JOBFILE_WRITER
{
public:
    GERBER_JOBFILE_WRITER( BOARD* aPcb, REPORTER* aReporter = nullptr );

    virtual ~GERBER_JOBFILE_WRITER()
    {
    }

    /**
     * add a gerber file name and type in job file list
     * @param aLayer is the PCB_LAYER_ID corresponding to the gerber file
     * @param aFilename is the filename (without path) of the gerber file
     */
    void AddGbrFile( PCB_LAYER_ID aLayer, QString& aFilename )
    {
        m_params.m_GerberFileList.append( aFilename );
        m_params.m_LayerId.push_back( aLayer );
    }

    /**
     * Creates a Gerber job file
     * @param aFullFilename = the full filename
     * @return true, or false if the file cannot be created
     */
    bool  CreateJobFile( const QString& aFullFilename );

    /**
     * Creates an Gerber job file in JSON format
     * @param aFullFilename = the full filename
     * @param aParams = true for a NPTH file, false for a PTH file
     * @return true, or false if the file cannot be created
     */
    bool  WriteJSONJobFile( const QString& aFullFilename );

private:
    /** @return SIDE_NONE if no silk screen layer is in list
     * SIDE_TOP if top silk screen layer is in list
     * SIDE_BOTTOM if bottom silk screen layer is in list
     * SIDE_BOTH if top and bottom silk screen layers are in list
     */
    enum ONSIDE hasSilkLayers();

    /** @return SIDE_NONE if no soldermask layer is in list
     * SIDE_TOP if top soldermask layer is in list
     * SIDE_BOTTOM if bottom soldermask layer is in list
     * SIDE_BOTH if top and bottom soldermask layers are in list
     */
    enum ONSIDE hasSolderMasks();

    /** @return the key associated to sides used for some layers
     * No TopOnly BotOnly Both
     */
    const char* sideKeyValue( enum ONSIDE aValue );

    /**
     * Add the job file header in JSON format to m_JSONbuffer
     */
    void addJSONHeader();

    /**
     * Add the General Specs in JSON format to m_JSONbuffer
     */
    void addJSONGeneralSpecs();

    /**
     * Add the Files Attributes section in JSON format to m_JSONbuffer
     */
    void addJSONFilesAttributes();

    /**
     * Add the Material Stackup section in JSON format to m_JSONbuffer
     * This is the ordered list of stackup layers (mask, paste, silk, copper, dielectric)
     * used to make the physical board. Therefore not all layers are listed here
     */
    void addJSONMaterialStackup();

    /**
     * Add the Design Rules section in JSON format to m_JSONbuffer
     */
    void addJSONDesignRules();

    /** A helper function to convert a QString ( therefore a Unicode text ) to
     * a JSON compatible string (a escaped unicode sequence of 4 hexa).
     */
    std::string formatStringFromUTF32( const QString& aText );

    /** A helper function to convert a double in Pcbnew internal units to
     * a JSON double value (in mm), with only 4 digits in mantissa for a better readability
     * when printed using %g or equivalent format.
     */
    double mapValue( double aUiValue );

private:
    BOARD* m_pcb;                  // The board
    REPORTER* m_reporter;          // a reporter for messages (can be null)
    JOBFILE_PARAMS m_params;       // the list of various prms and data to write in a job file
    double m_conversionUnits;      // scaling factor to convert brd units to gerber units (mm)
    nlohmann::ordered_json m_json; // json document built by this class
};

#endif  //  #ifndef GERBER_JOBFILE_WRITER_H
