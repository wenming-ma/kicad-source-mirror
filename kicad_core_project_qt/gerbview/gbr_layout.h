
/**
 * @file gbr_layout.h
 * @brief Class CLASS_GBR_LAYOUT to handle info to draw loaded Gerber images
 * and page frame reference
 */

#ifndef GBR_LAYOUT_H
#define GBR_LAYOUT_H


#include <gerbview.h>                       // GERBER_DRAWLAYERS_COUNT
#include <title_block.h>
#include <gerber_draw_item.h>
#include <math/box2.h>
#include <QString>

class GERBER_FILE_IMAGE_LIST;

/**
 * A list of #GERBER_DRAW_ITEM objects currently loaded.
 */
class GBR_LAYOUT : public EDA_ITEM
{
public:
    GBR_LAYOUT();
    ~GBR_LAYOUT();

    QString GetClass() const override
    {
        return "GBR_LAYOUT";
    }

    // Accessor to the GERBER_FILE_IMAGE_LIST,
    // which handles the list of gerber files (and drill files) images loaded
    GERBER_FILE_IMAGE_LIST* GetImagesList() const;

    const VECTOR2I& GetAuxOrigin() const { return m_originAxisPosition; }
    void            SetAuxOrigin( const VECTOR2I& aPosition ) { m_originAxisPosition = aPosition; }

    TITLE_BLOCK& GetTitleBlock() { return m_titles; }
    void SetTitleBlock( const TITLE_BLOCK& aTitleBlock ) { m_titles = aTitleBlock; }

    /**
     * Calculate the bounding box containing all Gerber items.
     *
     * @return the full item list bounding box.
     */
    BOX2I ComputeBoundingBox() const;

    const BOX2I GetBoundingBox() const override
    {
        return ComputeBoundingBox();
    }

    void SetBoundingBox( const BOX2I& aBox ) { m_BoundingBox = aBox; }

    ///< @copydoc EDA_ITEM::Visit()
    INSPECT_RESULT Visit( INSPECTOR inspector, void* testData,
                          const std::vector<KICAD_T>& aScanTypes ) override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override { ShowDummy( os ); }
#endif

private:
    mutable BOX2I    m_BoundingBox;
    TITLE_BLOCK      m_titles;
    VECTOR2I         m_originAxisPosition;
};

#endif      // #ifndef GBR_LAYOUT_H
