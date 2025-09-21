
#ifndef GAL_PRINT_H
#define GAL_PRINT_H

#include <gal/gal.h>

class QPaintDevice;

namespace KIGFX {
class GAL;
class GAL_DISPLAY_OPTIONS;


class GAL_API PRINT_CONTEXT
{
public:
    virtual ~PRINT_CONTEXT() {}
    virtual double GetNativeDPI() const = 0;
    virtual bool HasNativeLandscapeRotation() const = 0;
};


/**
 * Wrapper around GAL to provide information needed for printing.
 */
class GAL_API GAL_PRINT
{
public:
    static std::unique_ptr<GAL_PRINT> Create( GAL_DISPLAY_OPTIONS& aOptions, QPaintDevice* aDC );

    virtual ~GAL_PRINT() {}

    virtual GAL* GetGAL() = 0;

    virtual PRINT_CONTEXT* GetPrintCtx() const = 0;

    /**
     * @param aSize is the printing sheet size expressed in inches.
     * @param aRotateIfLandscape true if the platform requires 90 degrees
     * rotation in order to print in landscape format.
     */
    virtual void SetNativePaperSize( const VECTOR2D& aSize, bool aRotateIfLandscape ) = 0;

    /**
     * @param aSize is the schematics sheet size expressed in inches.
     */
    virtual void SetSheetSize( const VECTOR2D& aSize ) = 0;
};

}; // end namespace KIGFX

#endif /* GAL_PRINT_H */
