
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef LAYER_BOX_SELECTOR_H
#define LAYER_BOX_SELECTOR_H

#include <QComboBox>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QRect>


/**
 * Base class to build a layer list.
 */
class LAYER_SELECTOR
{
public:
    LAYER_SELECTOR();

    virtual ~LAYER_SELECTOR() { }

    bool SetLayersHotkeys( bool value );

protected:
    /// Return true if the layer id is enabled (i.e. is it should be displayed).
    virtual bool isLayerEnabled( int aLayer ) const = 0;

    bool m_layerhotkeys;
};


/**
 * Display a layer list in a QComboBox.
 */
class LAYER_BOX_SELECTOR : public QComboBox, public LAYER_SELECTOR
{
public:
    LAYER_BOX_SELECTOR( QWidget* parent, int id, const QPoint& pos = QPoint(),
                        const QSize& size = QSize(), int n = 0,
                        const QString choices[] = nullptr );

    ~LAYER_BOX_SELECTOR() override;

    int GetLayerSelection() const;

    int SetLayerSelection( int layer );

    // Reload the Layers
    // Virtual pure function because GerbView uses its own functions in a derived class
    virtual void Resync() = 0;

private:
#ifdef Q_OS_MAC
    void keyPressEvent( QKeyEvent* aEvent ) override;
    void paintEvent( QPaintEvent* event ) override;
#endif
};

#endif // LAYER_BOX_SELECTOR_H
