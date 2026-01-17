// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCB_PLOT_PARAMS_PARSER_H_
#define PCB_PLOT_PARAMS_PARSER_H_

#include <pcb_plot_params_lexer.h>

class QString;
class PCB_PLOT_PARAMS;
class LINE_READER;


/**
 * The parser for PCB_PLOT_PARAMS.
 */
class PCB_PLOT_PARAMS_PARSER : public PCB_PLOT_PARAMS_LEXER
{
public:
    PCB_PLOT_PARAMS_PARSER( LINE_READER* aReader, int aBoardFileVersion );
    PCB_PLOT_PARAMS_PARSER( char* aLine, const QString& aSource );

    LINE_READER* GetReader() { return reader; };

    void Parse( PCB_PLOT_PARAMS* aPcbPlotParams );

private:
    bool parseBool();

    /**
     * Parse an integer and constrains it between two values.
     *
     * @param aMin is the smallest return value.
     * @param aMax is the largest return value.
     * @return the parsed integer.
     */
    int parseInt( int aMin, int aMax );

    /**
     * Parse a double precision floating point number.
     *
     * @return the parsed double.
     */
    double parseDouble();

    /**
     * Skip the current token level.
     *
     * Search for the RIGHT parenthesis which closes the current description.
     */
    void skipCurrent();

    int m_boardFileVersion;
};

#endif // PCB_PLOT_PARAMS_PARSER_H_
