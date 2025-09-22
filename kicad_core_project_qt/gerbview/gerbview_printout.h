
#ifndef GERBVIEW_PRINTOUT_H
#define GERBVIEW_PRINTOUT_H

#include <board_printout.h>
#include <QString>

class GBR_LAYOUT;
class LSET;

class GERBVIEW_PRINTOUT : public BOARD_PRINTOUT
{
public:
    GERBVIEW_PRINTOUT( GBR_LAYOUT* aLayout, const BOARD_PRINTOUT_SETTINGS& aParams,
            const KIGFX::VIEW* aView, const QString& aTitle );

    bool OnPrintPage( int aPage ) override;

protected:
    int milsToIU( double aMils ) const override;

    void setupViewLayers( KIGFX::VIEW& aView, const LSET& aLayerSet ) override;

    void setupGal( KIGFX::GAL* aGal ) override;

     BOX2I getBoundingBox() override;

    std::unique_ptr<KIGFX::PAINTER> getPainter( KIGFX::GAL* aGal ) override;

private:
    GBR_LAYOUT* m_layout;
};

#endif /* GERBVIEW_PRINTOUT_H */
