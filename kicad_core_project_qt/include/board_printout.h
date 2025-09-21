

#ifndef BOARD_PRINTOUT_H
#define BOARD_PRINTOUT_H

#include <QPrinter>
#include <QString>
#include <layer_ids.h>
#include <lset.h>
#include <printout.h>
#include <math/box2.h>

#include <memory>

namespace KIGFX
{
class GAL;
class VIEW;
class PAINTER;
};


struct BOARD_PRINTOUT_SETTINGS : public PRINTOUT_SETTINGS
{
    BOARD_PRINTOUT_SETTINGS( const PAGE_INFO& aPageInfo );

    virtual ~BOARD_PRINTOUT_SETTINGS()
    {
    }

    LSET m_LayerSet;                   ///< Layers to print
    bool m_Mirror;                     ///< Print mirrored

    void Load( APP_SETTINGS_BASE* aConfig ) override;
    void Save( APP_SETTINGS_BASE* aConfig ) override;
};


/**
 * An object to handle the necessary information to control a printer
 * when printing a board.
 */
class BOARD_PRINTOUT
{
public:
    BOARD_PRINTOUT( const BOARD_PRINTOUT_SETTINGS& aParams, const KIGFX::VIEW* aView,
                    const QString& aTitle );

    virtual ~BOARD_PRINTOUT() {}

    virtual void GetPageInfo( int* minPage, int* maxPage, int* selPageFrom, int* selPageTo );

    virtual bool HasPage( int aPage )
    {
        return aPage <= m_settings.m_pageCount;
    }

    /**
     * Print a page (or a set of pages).
     *
     * @note This function prepares the print parameters for the function which actually prints
     *       the draw layers.
     *
     * @param aLayerName a text which can be printed as layer name.
     * @param aPageNum the number of the current page (only used to print this value).
     * @param aPageCount the number of pages to print (only used to print this value).
     */
    virtual void DrawPage( const QString& aLayerName = QString(),
                           int aPageNum = 1, int aPageCount = 1 );

protected:
    /// Convert mils to internal units.
    virtual int milsToIU( double aMils ) const = 0;

    /// Enable layers visibility for a printout.
    virtual void setupViewLayers( KIGFX::VIEW& aView, const LSET& aLayerSet );

    /// Configure #PAINTER object for a printout.
    virtual void setupPainter( KIGFX::PAINTER& aPainter );

    /// Configure #GAL object for a printout.
    virtual void setupGal( KIGFX::GAL* aGal );

    /// Return bounding box of the printed objects (excluding drawing-sheet frame).
    virtual BOX2I getBoundingBox() = 0;

    /// Return the #PAINTER instance used to draw the items.
    virtual std::unique_ptr<KIGFX::PAINTER> getPainter( KIGFX::GAL* aGal ) = 0;

    /// Source VIEW object (note that actual printing only refers to this object).
    const KIGFX::VIEW* m_view;

    /// Printout parameters.
    BOARD_PRINTOUT_SETTINGS m_settings;

    /// True if the caller is Gerbview, false for Pcbnew.
    bool  m_gerbviewPrint;
};

#endif      // BOARD_PRINTOUT_H
