
#ifndef DS_PROXY_VIEW_ITEM_H
#define DS_PROXY_VIEW_ITEM_H

#include <eda_item.h>

class BOARD;
class PAGE_INFO;
class PROJECT;
class TITLE_BLOCK;
class DS_DRAW_ITEM_LINE;
class DS_DRAW_ITEM_RECT;
class DS_DRAW_ITEM_TEXT;
class DS_DRAW_ITEM_BITMAP;
class DS_DRAW_ITEM_LIST;

namespace KIGFX
{
class VIEW;
class GAL;
}

class DS_PROXY_VIEW_ITEM : public EDA_ITEM
{
public:
    DS_PROXY_VIEW_ITEM( const EDA_IU_SCALE& aIuScale, const PAGE_INFO* aPageInfo,
                        const PROJECT* aProject, const TITLE_BLOCK* aTitleBlock,
                        const std::map<QString, QString>* aProperties );

    /**
     * Set the file name displayed in the title block.
     */
    void SetFileName( const std::string& aFileName ) { m_fileName = aFileName; }

    /**
     * Set the sheet name displayed in the title block.
     */
    void SetSheetName( const std::string& aSheetName ) { m_sheetName = aSheetName; }

    /**
     * Set the sheet path displayed in the title block.
     */
    void SetSheetPath( const std::string& aSheetPath ) { m_sheetPath = aSheetPath; }

    /**
     * Change the page number displayed in the title block.
     */
    void SetPageNumber( const std::string& aPageNumber ) { m_pageNumber = aPageNumber; }

    /**
     * Change the sheet-count number displayed in the title block.
     */
    void SetSheetCount( int aSheetCount ) { m_sheetCount = aSheetCount; }

    /**
     * Change if this is first page.
     *
     * Title blocks have an option to allow all subsequent pages to not display a title
     * block.  This needs to be set to false when displaying any page but the first page.
     */
    void SetIsFirstPage( bool aIsFirstPage ) { m_isFirstPage = aIsFirstPage; }

    /**
     * Can be used to override which layer ID is used for drawing sheet item colors
     * @param aLayerId is the color to use (defaults to LAYER_DRAWINGSHEET if this is not called)
     */
    void SetColorLayer( int aLayerId ) { m_colorLayer = aLayerId; }

    /**
     * Override the layer used to pick the color of the page border (normally LAYER_GRID)
     *
     * @param aLayerId is the layer to use
     */
    void SetPageBorderColorLayer( int aLayerId ) { m_pageBorderColorLayer = aLayerId; }

    const PAGE_INFO& GetPageInfo() { return *m_pageInfo; }
    const TITLE_BLOCK& GetTitleBlock() { return *m_titleBlock; }

    /// @copydoc VIEW_ITEM::ViewBBox()
    const BOX2I ViewBBox() const override;

    /// @copydoc VIEW_ITEM::ViewDraw()
    void ViewDraw( int aLayer, KIGFX::VIEW* aView ) const override;

    /// @copydoc VIEW_ITEM::ViewGetLayers()
    std::vector<int> ViewGetLayers() const override;

#if defined(DEBUG)
    /// @copydoc EDA_ITEM::Show()
    void Show( int x, std::ostream& st ) const override { }
#endif

    /**
     * Get class name.
     *
     * @return  string "DS_PROXY_VIEW_ITEM"
     */
    virtual QString GetClass() const override
    {
        return "DS_PROXY_VIEW_ITEM";
    }

    bool HitTestDrawingSheetItems( KIGFX::VIEW* aView, const VECTOR2I& aPosition );

protected:
    void buildDrawList( KIGFX::VIEW* aView, const std::map<QString, QString>* aProperties,
                        DS_DRAW_ITEM_LIST* aDrawList ) const;

protected:
    const EDA_IU_SCALE& m_iuScale;

    std::string         m_fileName;
    std::string         m_sheetName;
    std::string         m_sheetPath;
    const TITLE_BLOCK*  m_titleBlock;
    const PAGE_INFO*    m_pageInfo;
    std::string         m_pageNumber;
    int                 m_sheetCount;
    bool                m_isFirstPage;
    const PROJECT*      m_project;

    const std::map<QString, QString>* m_properties;

    /**
     * Layer that is used for drawing sheet color (LAYER_DRAWINGSHEET is always used
     * for visibility).
     */
    int                 m_colorLayer;

    /// Layer that is used for page border color
    int                 m_pageBorderColorLayer;
};

#endif /* DS_PROXY_VIEW_ITEM_H */
