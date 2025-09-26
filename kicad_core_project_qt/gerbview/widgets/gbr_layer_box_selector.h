// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef GBR_LAYER_BOX_SELECTOR_H
#define GBR_LAYER_BOX_SELECTOR_H

#include <memory>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QString>

#include <widgets/layer_box_selector.h>

class LAYER_PRESENTATION;

// class to display a layer list in GerbView.
class GBR_LAYER_BOX_SELECTOR : public LAYER_BOX_SELECTOR
{
public:
    GBR_LAYER_BOX_SELECTOR( QWidget* parent, int id, const QPoint& pos = QPoint(),
                            const QSize& size = QSize(), int n = 0,
                            const QString choices[] = nullptr );

    // Reload the Layers names and bitmaps
    void Resync() override;

    // Return true if the layer id is enabled (i.e. is it should be displayed)
    bool isLayerEnabled( int aLayer ) const override { return true; }

private:
    std::unique_ptr<LAYER_PRESENTATION> m_layerPresentation;
};

#endif //GBR_LAYER_BOX_SELECTOR_H
