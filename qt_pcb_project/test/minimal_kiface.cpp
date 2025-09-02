/*
 * Minimal implementation of Kiface() function for qt_pcb_project testing
 */

#include <kiface_base.h>

class MINIMAL_KIFACE : public KIFACE_BASE
{
public:
    MINIMAL_KIFACE() : KIFACE_BASE( "minimal_test", KIWAY::KIWAY_FACE_COUNT ) {}
    virtual ~MINIMAL_KIFACE() {}

    virtual bool OnKifaceStart( PGM_BASE* aProgram, int aCtlBits, KIWAY* aKiway ) override
    {
        // Minimal implementation - just return success
        return true;
    }

    virtual void OnKifaceEnd() override
    {
        // Minimal implementation - do nothing
    }

    virtual wxWindow* CreateKiWindow( wxWindow* aParent, int aClassId, KIWAY* aKIWAY,
                                      int aCtlBits = 0 ) override
    {
        // Minimal implementation - return null
        return nullptr;
    }

    virtual void* IfaceOrAddress( int aDataId ) override
    {
        // Minimal implementation - return null
        return nullptr;
    }
};


KIFACE_BASE& Kiface()
{
    static MINIMAL_KIFACE kiface;
    return kiface;
}
