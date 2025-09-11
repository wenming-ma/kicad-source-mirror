/*
 * PCB Renderer Fixed Version
 * Fixed coordinate transformation and scaling issues
 */

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sstream>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
#include <QTransform>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QFileInfo>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QDockWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QTimer>

// KiCad headers
#include <board.h>
#include <footprint.h>
#include <pad.h>
#include <pcb_track.h>
#include <pcb_shape.h>
#include <pcb_text.h>
#include <zone.h>
#include <layer_ids.h>
#include <pcb_io/kicad_sexpr/pcb_io_kicad_sexpr.h>
#include <geometry/shape.h>
#include <geometry/shape_poly_set.h>
#include <geometry/shape_line_chain.h>
#include <geometry/shape_circle.h>
#include <geometry/shape_rect.h>
#include <geometry/shape_segment.h>
#include <geometry/shape_simple.h>
#include <geometry/shape_compound.h>
#include <convert_basic_shapes_to_polygon.h>

// Fixed scale factor - KiCad uses nanometers, we convert to millimeters then to pixels
// 1mm = 1,000,000 nm
// We want 1mm = approximately 10 pixels for good visibility
const double NM_TO_MM = 1.0 / 1000000.0;
const double MM_TO_PIXEL = 10.0;
const double NM_TO_PIXEL = NM_TO_MM * MM_TO_PIXEL;

// Debug output class
class DebugLogger {
public:
    static DebugLogger& instance() {
        static DebugLogger inst;
        return inst;
    }
    
    void setTextEdit(QTextEdit* edit) { m_textEdit = edit; }
    
    void log(const QString& message) {
        if (m_textEdit) {
            m_textEdit->append(message);
            // Keep only last 500 lines
            if (m_textEdit->document()->lineCount() > 500) {
                QTextCursor cursor = m_textEdit->textCursor();
                cursor.movePosition(QTextCursor::Start);
                cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 50);
                cursor.removeSelectedText();
            }
        }
        std::cout << message.toStdString() << std::endl;
    }
    
    void clear() {
        if (m_textEdit) {
            m_textEdit->clear();
        }
    }
    
private:
    QTextEdit* m_textEdit = nullptr;
};

#define DEBUG_LOG(msg) DebugLogger::instance().log(msg)

// Render settings
class RenderSettings {
public:
    RenderSettings() {
        // Layer colors with better visibility
        m_layerColors[F_Cu] = QColor(255, 0, 0, 255);      // Bright red
        m_layerColors[B_Cu] = QColor(0, 0, 255, 255);      // Bright blue
        m_layerColors[In1_Cu] = QColor(0, 255, 0, 255);    // Bright green
        m_layerColors[In2_Cu] = QColor(255, 255, 0, 255);  // Yellow
        m_layerColors[F_SilkS] = QColor(255, 255, 255, 255); // White
        m_layerColors[B_SilkS] = QColor(200, 200, 200, 255); // Light gray
        m_layerColors[F_Mask] = QColor(0, 180, 0, 150);    // Semi-transparent green
        m_layerColors[B_Mask] = QColor(180, 0, 0, 150);    // Semi-transparent red
        m_layerColors[Edge_Cuts] = QColor(255, 255, 0, 255); // Yellow
        m_layerColors[F_Paste] = QColor(200, 200, 200, 200);
        m_layerColors[B_Paste] = QColor(150, 150, 150, 200);
        
        // Element visibility - all on by default
        m_drawPads = true;
        m_drawTracks = true;
        m_drawVias = true;
        m_drawZones = true;
        m_drawShapes = true;
        m_drawFootprints = true;
        m_drawDrills = true;
        m_drawPadNumbers = false;
        m_drawNetNames = false;
        
        // Rendering options
        m_fillMode = true;
        m_outlineWidth = 2.0;  // Thicker outlines for visibility
        m_debugMode = false;   // Start with debug off
        
        // Layer visibility - all visible
        for (int i = 0; i < PCB_LAYER_ID_COUNT; ++i) {
            m_layerVisible[i] = true;
        }
    }
    
    QColor getLayerColor(PCB_LAYER_ID layer) const {
        auto it = m_layerColors.find(layer);
        return it != m_layerColors.end() ? it->second : QColor(128, 128, 128, 255);
    }
    
    bool isLayerVisible(PCB_LAYER_ID layer) const {
        return m_layerVisible[layer];
    }
    
    void setLayerVisible(PCB_LAYER_ID layer, bool visible) {
        m_layerVisible[layer] = visible;
    }
    
    bool m_drawPads;
    bool m_drawTracks;
    bool m_drawVias;
    bool m_drawZones;
    bool m_drawShapes;
    bool m_drawFootprints;
    bool m_drawDrills;
    bool m_drawPadNumbers;
    bool m_drawNetNames;
    bool m_fillMode;
    double m_outlineWidth;
    bool m_debugMode;
    
private:
    std::map<PCB_LAYER_ID, QColor> m_layerColors;
    bool m_layerVisible[PCB_LAYER_ID_COUNT];
};

// PCB Renderer
class PCBRenderer {
public:
    PCBRenderer(QPainter* painter, RenderSettings* settings)
        : m_painter(painter), m_settings(settings) {
        m_drawnItems = 0;
    }
    
    int getDrawnItemsCount() const { return m_drawnItems; }
    
    void renderBoard(BOARD* board, PCB_LAYER_ID layer) {
        if (!board || !m_settings->isLayerVisible(layer)) {
            return;
        }
        
        // Render in proper order for visibility
        if (m_settings->m_drawZones) {
            renderZones(board, layer);
        }
        
        if (m_settings->m_drawTracks) {
            renderTracks(board, layer);
        }
        
        if (m_settings->m_drawVias) {
            renderVias(board, layer);
        }
        
        if (m_settings->m_drawShapes) {
            renderShapes(board, layer);
        }
        
        if (m_settings->m_drawFootprints) {
            renderFootprints(board, layer);
        }
    }
    
private:
    // Fixed coordinate transformation - no additional scaling here
    QPointF toScene(const VECTOR2I& pos) {
        // Convert nanometers to pixels with proper scale
        // Also flip Y axis because Qt's Y increases downward
        return QPointF(pos.x * NM_TO_PIXEL, -pos.y * NM_TO_PIXEL);
    }
    
    double toScene(int nm) {
        return nm * NM_TO_PIXEL;
    }
    
    void renderPad(PAD* pad, PCB_LAYER_ID layer) {
        if (!pad || !pad->IsOnLayer(layer) || !pad->FlashLayer(layer)) {
            return;
        }
        
        QColor color = m_settings->getLayerColor(layer);
        
        m_painter->save();
        
        // Get pad position and apply transformation
        VECTOR2I padPosNm = pad->GetPosition();
        QPointF padPos = toScene(padPosNm);
        m_painter->translate(padPos);
        
        // Apply rotation (KiCad uses tenths of degrees, convert to degrees)
        double rotation = -pad->GetOrientation().AsDegrees();
        m_painter->rotate(rotation);
        
        // Set drawing style
        if (m_settings->m_fillMode) {
            m_painter->setPen(Qt::NoPen);
            m_painter->setBrush(QBrush(color));
        } else {
            m_painter->setPen(QPen(color, m_settings->m_outlineWidth));
            m_painter->setBrush(Qt::NoBrush);
        }
        
        // Get pad shape and size
        PAD_SHAPE shape = pad->GetShape(layer);
        VECTOR2I sizeNm = pad->GetSize(layer);
        double width = toScene(sizeNm.x);
        double height = toScene(sizeNm.y);
        
        if (m_settings->m_debugMode) {
            DEBUG_LOG(QString("Pad %1: pos(%2,%3)nm -> (%4,%5)px, size(%6,%7)nm -> (%8,%9)px")
                     .arg(pad->GetNumber())
                     .arg(padPosNm.x).arg(padPosNm.y)
                     .arg(padPos.x()).arg(padPos.y())
                     .arg(sizeNm.x).arg(sizeNm.y)
                     .arg(width).arg(height));
        }
        
        // Draw based on shape type
        switch (shape) {
            case PAD_SHAPE::CIRCLE: {
                double radius = width / 2.0;
                m_painter->drawEllipse(QPointF(0, 0), radius, radius);
                break;
            }
            
            case PAD_SHAPE::RECTANGLE: {
                m_painter->drawRect(QRectF(-width/2, -height/2, width, height));
                break;
            }
            
            case PAD_SHAPE::OVAL: {
                m_painter->drawEllipse(QPointF(0, 0), width/2, height/2);
                break;
            }
            
            case PAD_SHAPE::ROUNDRECT: {
                double radius = toScene(pad->GetRoundRectCornerRadius(layer));
                QPainterPath path;
                path.addRoundedRect(-width/2, -height/2, width, height, radius, radius);
                m_painter->drawPath(path);
                break;
            }
            
            case PAD_SHAPE::TRAPEZOID: {
                VECTOR2I delta = pad->GetDelta(layer);
                double dx = toScene(delta.x) / 2.0;
                double dy = toScene(delta.y) / 2.0;
                
                QPolygonF trapezoid;
                trapezoid << QPointF(-width/2 - dx, -height/2 - dy)
                         << QPointF(width/2 + dx, -height/2 + dy)
                         << QPointF(width/2 - dx, height/2 + dy)
                         << QPointF(-width/2 + dx, height/2 - dy);
                m_painter->drawPolygon(trapezoid);
                break;
            }
            
            case PAD_SHAPE::CUSTOM: {
                // For custom shapes, use the effective shape
                std::shared_ptr<SHAPE> effectiveShape = pad->GetEffectiveShape(layer);
                if (effectiveShape) {
                    drawShape(effectiveShape.get());
                }
                break;
            }
            
            case PAD_SHAPE::CHAMFERED_RECT: {
                // Draw as rounded rect for now
                m_painter->drawRect(QRectF(-width/2, -height/2, width, height));
                break;
            }
            
            default:
                // Fallback to rectangle
                m_painter->drawRect(QRectF(-width/2, -height/2, width, height));
                break;
        }
        
        // Draw drill hole if present
        if (m_settings->m_drawDrills && pad->HasHole()) {
            VECTOR2I drillSize = pad->GetDrillSize();
            double drillWidth = toScene(drillSize.x);
            double drillHeight = toScene(drillSize.y);
            
            m_painter->setPen(QPen(Qt::black, 1));
            m_painter->setBrush(QBrush(QColor(40, 40, 40)));  // Dark gray for hole
            
            if (pad->GetDrillShape() == PAD_DRILL_SHAPE::OBLONG) {
                m_painter->drawEllipse(QPointF(0, 0), drillWidth/2, drillHeight/2);
            } else {
                m_painter->drawEllipse(QPointF(0, 0), drillWidth/2, drillWidth/2);
            }
        }
        
        // Draw pad number if enabled
        if (m_settings->m_drawPadNumbers && !pad->GetNumber().isEmpty()) {
            m_painter->setPen(QPen(Qt::white));
            m_painter->setBrush(Qt::NoBrush);
            QFont font("Arial", 8);
            m_painter->setFont(font);
            
            QRectF textRect(-width/2, -height/2, width, height);
            m_painter->drawText(textRect, Qt::AlignCenter, pad->GetNumber());
        }
        
        m_painter->restore();
        m_drawnItems++;
    }
    
    void drawShape(const SHAPE* shape) {
        if (!shape) return;
        
        switch (shape->Type()) {
            case SH_RECT: {
                const SHAPE_RECT* rect = static_cast<const SHAPE_RECT*>(shape);
                VECTOR2I pos = rect->GetPosition();
                int w = rect->GetWidth();
                int h = rect->GetHeight();
                // Position is corner, not center
                m_painter->drawRect(QRectF(toScene(pos.x), toScene(-pos.y - h), 
                                          toScene(w), toScene(h)));
                break;
            }
            
            case SH_CIRCLE: {
                const SHAPE_CIRCLE* circle = static_cast<const SHAPE_CIRCLE*>(shape);
                QPointF center = toScene(circle->GetCenter());
                double radius = toScene(circle->GetRadius());
                m_painter->drawEllipse(center, radius, radius);
                break;
            }
            
            case SH_SEGMENT: {
                const SHAPE_SEGMENT* seg = static_cast<const SHAPE_SEGMENT*>(shape);
                QPointF start = toScene(seg->GetSeg().A);
                QPointF end = toScene(seg->GetSeg().B);
                double width = toScene(seg->GetWidth());
                
                if (width > 0) {
                    QPen oldPen = m_painter->pen();
                    QPen pen = m_painter->pen();
                    pen.setWidthF(width);
                    pen.setCapStyle(Qt::RoundCap);
                    m_painter->setPen(pen);
                }
                m_painter->drawLine(start, end);
                break;
            }
            
            case SH_SIMPLE: {
                const SHAPE_SIMPLE* simple = static_cast<const SHAPE_SIMPLE*>(shape);
                if (simple->PointCount() >= 2) {
                    QPolygonF polygon;
                    for (int i = 0; i < simple->PointCount(); ++i) {
                        polygon << toScene(simple->CPoint(i));
                    }
                    m_painter->drawPolygon(polygon);
                }
                break;
            }
            
            case SH_POLY_SET: {
                const SHAPE_POLY_SET* polySet = static_cast<const SHAPE_POLY_SET*>(shape);
                for (int i = 0; i < polySet->OutlineCount(); ++i) {
                    const SHAPE_LINE_CHAIN& outline = polySet->Outline(i);
                    QPolygonF polygon;
                    for (int j = 0; j < outline.PointCount(); ++j) {
                        polygon << toScene(outline.CPoint(j));
                    }
                    m_painter->drawPolygon(polygon);
                }
                break;
            }
            
            case SH_COMPOUND: {
                const SHAPE_COMPOUND* compound = static_cast<const SHAPE_COMPOUND*>(shape);
                for (const SHAPE* subshape : compound->Shapes()) {
                    drawShape(subshape);
                }
                break;
            }
            
            default:
                break;
        }
    }
    
    void renderTracks(BOARD* board, PCB_LAYER_ID layer) {
        for (PCB_TRACK* track : board->Tracks()) {
            if (track && track->IsOnLayer(layer) && track->Type() == PCB_TRACE_T) {
                QColor color = m_settings->getLayerColor(layer);
                double width = toScene(track->GetWidth());
                
                QPointF start = toScene(track->GetStart());
                QPointF end = toScene(track->GetEnd());
                
                if (m_settings->m_fillMode) {
                    m_painter->setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap));
                } else {
                    m_painter->setPen(QPen(color, m_settings->m_outlineWidth));
                }
                m_painter->drawLine(start, end);
                m_drawnItems++;
            }
        }
    }
    
    void renderVias(BOARD* board, PCB_LAYER_ID layer) {
        for (PCB_TRACK* track : board->Tracks()) {
            if (track && track->Type() == PCB_VIA_T) {
                PCB_VIA* via = static_cast<PCB_VIA*>(track);
                if (via->IsOnLayer(layer)) {
                    QColor color = m_settings->getLayerColor(layer);
                    QPointF center = toScene(via->GetPosition());
                    double radius = toScene(via->GetWidth() / 2);
                    
                    if (m_settings->m_fillMode) {
                        m_painter->setPen(Qt::NoPen);
                        m_painter->setBrush(QBrush(color));
                    } else {
                        m_painter->setPen(QPen(color, m_settings->m_outlineWidth));
                        m_painter->setBrush(Qt::NoBrush);
                    }
                    
                    m_painter->drawEllipse(center, radius, radius);
                    
                    // Draw drill
                    if (m_settings->m_drawDrills) {
                        int drillSize = via->GetDrillValue();
                        if (drillSize > 0) {
                            double drillRadius = toScene(drillSize / 2);
                            m_painter->setPen(QPen(Qt::black, 1));
                            m_painter->setBrush(QBrush(QColor(40, 40, 40)));
                            m_painter->drawEllipse(center, drillRadius, drillRadius);
                        }
                    }
                    m_drawnItems++;
                }
            }
        }
    }
    
    void renderZones(BOARD* board, PCB_LAYER_ID layer) {
        for (ZONE* zone : board->Zones()) {
            if (zone && zone->IsOnLayer(layer)) {
                const std::shared_ptr<SHAPE_POLY_SET>& filledPoly = zone->GetFilledPolysList(layer);
                if (filledPoly && filledPoly->OutlineCount() > 0) {
                    QColor color = m_settings->getLayerColor(layer);
                    color.setAlphaF(0.5);  // Semi-transparent
                    
                    m_painter->setPen(Qt::NoPen);
                    m_painter->setBrush(QBrush(color));
                    
                    for (int i = 0; i < filledPoly->OutlineCount(); ++i) {
                        const SHAPE_LINE_CHAIN& outline = filledPoly->Outline(i);
                        QPolygonF polygon;
                        for (int j = 0; j < outline.PointCount(); ++j) {
                            polygon << toScene(outline.CPoint(j));
                        }
                        m_painter->drawPolygon(polygon);
                    }
                    m_drawnItems++;
                }
            }
        }
    }
    
    void renderShapes(BOARD* board, PCB_LAYER_ID layer) {
        for (BOARD_ITEM* item : board->Drawings()) {
            if (item && item->Type() == PCB_SHAPE_T && item->IsOnLayer(layer)) {
                PCB_SHAPE* shape = static_cast<PCB_SHAPE*>(item);
                renderPCBShape(shape, layer);
                m_drawnItems++;
            }
        }
    }
    
    void renderPCBShape(PCB_SHAPE* shape, PCB_LAYER_ID layer) {
        QColor color = m_settings->getLayerColor(layer);
        double lineWidth = toScene(shape->GetWidth());
        
        if (shape->IsFilled() && m_settings->m_fillMode) {
            m_painter->setPen(Qt::NoPen);
            m_painter->setBrush(QBrush(color));
        } else {
            m_painter->setPen(QPen(color, lineWidth > 0 ? lineWidth : m_settings->m_outlineWidth));
            m_painter->setBrush(Qt::NoBrush);
        }
        
        switch (shape->GetShape()) {
            case SHAPE_T::SEGMENT: {
                QPointF start = toScene(shape->GetStart());
                QPointF end = toScene(shape->GetEnd());
                m_painter->drawLine(start, end);
                break;
            }
            
            case SHAPE_T::RECTANGLE: {
                QPointF start = toScene(shape->GetStart());
                QPointF end = toScene(shape->GetEnd());
                m_painter->drawRect(QRectF(start, end).normalized());
                break;
            }
            
            case SHAPE_T::CIRCLE: {
                QPointF center = toScene(shape->GetCenter());
                double radius = toScene(shape->GetRadius());
                m_painter->drawEllipse(center, radius, radius);
                break;
            }
            
            case SHAPE_T::POLY: {
                const SHAPE_POLY_SET& polySet = shape->GetPolyShape();
                if (polySet.OutlineCount() > 0) {
                    const SHAPE_LINE_CHAIN& outline = polySet.Outline(0);
                    QPolygonF polygon;
                    for (int i = 0; i < outline.PointCount(); ++i) {
                        polygon << toScene(outline.CPoint(i));
                    }
                    m_painter->drawPolygon(polygon);
                }
                break;
            }
            
            default:
                break;
        }
    }
    
    void renderFootprints(BOARD* board, PCB_LAYER_ID layer) {
        for (FOOTPRINT* footprint : board->Footprints()) {
            if (!footprint) continue;
            
            // Render footprint graphics
            for (BOARD_ITEM* item : footprint->GraphicalItems()) {
                if (item && item->Type() == PCB_SHAPE_T && item->IsOnLayer(layer)) {
                    renderPCBShape(static_cast<PCB_SHAPE*>(item), layer);
                }
            }
            
            // Render pads
            if (m_settings->m_drawPads) {
                for (PAD* pad : footprint->Pads()) {
                    renderPad(pad, layer);
                }
            }
        }
    }
    
private:
    QPainter* m_painter;
    RenderSettings* m_settings;
    int m_drawnItems;
};

// Render widget
class PCBRenderWidget : public QWidget {
public:
    PCBRenderWidget(BOARD* board, QWidget* parent = nullptr)
        : QWidget(parent), m_board(board), m_zoom(1.0), m_panOffset(0, 0) {
        setFocusPolicy(Qt::StrongFocus);
        setMouseTracking(true);
        setMinimumSize(800, 600);
        
        if (m_board) {
            BOX2I bbox = m_board->GetBoundingBox();
            // Convert board center from nanometers to pixels
            m_boardCenter = QPointF(bbox.GetCenter().x * NM_TO_PIXEL,
                                  -bbox.GetCenter().y * NM_TO_PIXEL);
            
            DEBUG_LOG(QString("Board bounding box: (%1, %2) to (%3, %4) nm")
                     .arg(bbox.GetX()).arg(bbox.GetY())
                     .arg(bbox.GetRight()).arg(bbox.GetBottom()));
            DEBUG_LOG(QString("Board center: (%1, %2) pixels")
                     .arg(m_boardCenter.x()).arg(m_boardCenter.y()));
            DEBUG_LOG(QString("Board size: %1 x %2 mm")
                     .arg(bbox.GetWidth() * NM_TO_MM)
                     .arg(bbox.GetHeight() * NM_TO_MM));
        }
    }
    
    RenderSettings& getSettings() { return m_settings; }
    
    void refresh() { update(); }
    
    void fitToWindow() {
        if (!m_board) return;
        
        BOX2I bbox = m_board->GetBoundingBox();
        double boardWidth = bbox.GetWidth() * NM_TO_PIXEL;
        double boardHeight = bbox.GetHeight() * NM_TO_PIXEL;
        
        if (boardWidth <= 0 || boardHeight <= 0) {
            DEBUG_LOG("Warning: Invalid board dimensions");
            return;
        }
        
        double scaleX = width() / boardWidth;
        double scaleY = height() / boardHeight;
        
        m_zoom = std::min(scaleX, scaleY) * 0.8;  // 80% to leave margin
        m_panOffset = QPointF(0, 0);
        
        DEBUG_LOG(QString("Fit to window: board %1x%2 pixels, widget %3x%4, zoom %5")
                 .arg(boardWidth).arg(boardHeight)
                 .arg(width()).arg(height())
                 .arg(m_zoom));
        
        update();
    }
    
protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        
        // Clear background
        painter.fillRect(rect(), QColor(20, 20, 20));
        
        if (!m_board) {
            painter.setPen(Qt::white);
            painter.drawText(rect(), Qt::AlignCenter, "No board loaded");
            return;
        }
        
        // Save painter state
        painter.save();
        
        // Apply transformations
        // Move origin to center of widget
        painter.translate(width() / 2.0 + m_panOffset.x(), 
                         height() / 2.0 + m_panOffset.y());
        
        // Apply zoom
        painter.scale(m_zoom, m_zoom);
        
        // Move to board center
        painter.translate(-m_boardCenter.x(), -m_boardCenter.y());
        
        // Draw origin marker for debugging
        if (m_settings.m_debugMode) {
            painter.setPen(QPen(Qt::yellow, 2));
            painter.drawLine(-20, 0, 20, 0);
            painter.drawLine(0, -20, 0, 20);
        }
        
        // Create renderer
        PCBRenderer renderer(&painter, &m_settings);
        
        // Render layers in proper order (back to front)
        std::vector<PCB_LAYER_ID> layers = {
            B_Cu,      // Back copper
            In2_Cu,    // Inner layers
            In1_Cu,
            F_Cu,      // Front copper
            B_Mask,    // Solder mask
            F_Mask,
            B_Paste,   // Solder paste
            F_Paste,
            B_SilkS,   // Silkscreen
            F_SilkS,
            Edge_Cuts  // Board outline
        };
        
        for (PCB_LAYER_ID layer : layers) {
            renderer.renderBoard(m_board, layer);
        }
        
        // Restore painter state
        painter.restore();
        
        // Draw HUD
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 10));
        painter.drawText(10, 20, QString("Zoom: %1x").arg(m_zoom, 0, 'f', 2));
        painter.drawText(10, 40, QString("Center: (%1, %2)")
                        .arg(m_boardCenter.x(), 0, 'f', 1)
                        .arg(m_boardCenter.y(), 0, 'f', 1));
        painter.drawText(10, 60, QString("Items drawn: %1").arg(renderer.getDrawnItemsCount()));
        
        // Draw help text
        painter.setPen(Qt::gray);
        painter.drawText(10, height() - 40, "Controls: Mouse wheel = Zoom, Left drag = Pan");
        painter.drawText(10, height() - 20, "Keys: F = Fit to window, R = Reset view");
    }
    
    void wheelEvent(QWheelEvent* event) override {
        double scaleFactor = 1.15;
        
        // Get mouse position before zoom
        QPointF mousePos = event->position();
        
        // Calculate zoom
        if (event->angleDelta().y() > 0) {
            m_zoom = std::min(m_zoom * scaleFactor, 100.0);  // Max zoom 100x
        } else {
            m_zoom = std::max(m_zoom / scaleFactor, 0.01);   // Min zoom 0.01x
        }
        
        update();
    }
    
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            m_lastMousePos = event->pos();
            setCursor(Qt::ClosedHandCursor);
        }
    }
    
    void mouseReleaseEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            setCursor(Qt::ArrowCursor);
        }
    }
    
    void mouseMoveEvent(QMouseEvent* event) override {
        if (event->buttons() & Qt::LeftButton) {
            QPointF delta = event->pos() - m_lastMousePos;
            m_panOffset += delta;
            m_lastMousePos = event->pos();
            update();
        }
    }
    
    void keyPressEvent(QKeyEvent* event) override {
        switch (event->key()) {
            case Qt::Key_F:
                fitToWindow();
                break;
            case Qt::Key_R:
                m_zoom = 1.0;
                m_panOffset = QPointF(0, 0);
                update();
                break;
            case Qt::Key_Plus:
            case Qt::Key_Equal:
                m_zoom = std::min(m_zoom * 1.2, 100.0);
                update();
                break;
            case Qt::Key_Minus:
                m_zoom = std::max(m_zoom / 1.2, 0.01);
                update();
                break;
            default:
                QWidget::keyPressEvent(event);
        }
    }
    
private:
    BOARD* m_board;
    RenderSettings m_settings;
    double m_zoom;
    QPointF m_panOffset;
    QPointF m_boardCenter;
    QPoint m_lastMousePos;
};

// Main window (simplified version)
class MainWindow : public QMainWindow {
public:
    MainWindow(BOARD* board, QWidget* parent = nullptr)
        : QMainWindow(parent), m_board(board) {
        
        setWindowTitle("PCB Renderer - Fixed Version");
        resize(1200, 800);
        
        // Create render widget
        m_renderWidget = new PCBRenderWidget(board, this);
        setCentralWidget(m_renderWidget);
        
        // Create control panel
        createControlPanel();
        
        // Create menu
        createMenu();
        
        // Create status bar
        createStatusBar();
        
        // Auto-fit on startup
        QTimer::singleShot(100, [this]() {
            m_renderWidget->fitToWindow();
        });
    }
    
private:
    void createControlPanel() {
        QDockWidget* dock = new QDockWidget("Controls", this);
        QWidget* widget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(widget);
        
        // Elements group
        QGroupBox* elemGroup = new QGroupBox("Elements");
        QVBoxLayout* elemLayout = new QVBoxLayout();
        
        auto addCheckbox = [this, elemLayout](const QString& text, bool& setting) {
            QCheckBox* cb = new QCheckBox(text);
            cb->setChecked(setting);
            connect(cb, &QCheckBox::toggled, [this, &setting](bool checked) {
                setting = checked;
                m_renderWidget->refresh();
            });
            elemLayout->addWidget(cb);
        };
        
        addCheckbox("Pads", m_renderWidget->getSettings().m_drawPads);
        addCheckbox("Tracks", m_renderWidget->getSettings().m_drawTracks);
        addCheckbox("Vias", m_renderWidget->getSettings().m_drawVias);
        addCheckbox("Zones", m_renderWidget->getSettings().m_drawZones);
        addCheckbox("Shapes", m_renderWidget->getSettings().m_drawShapes);
        addCheckbox("Drill Holes", m_renderWidget->getSettings().m_drawDrills);
        
        elemGroup->setLayout(elemLayout);
        layout->addWidget(elemGroup);
        
        // Options group
        QGroupBox* optGroup = new QGroupBox("Options");
        QVBoxLayout* optLayout = new QVBoxLayout();
        
        QCheckBox* fillCb = new QCheckBox("Fill Mode");
        fillCb->setChecked(m_renderWidget->getSettings().m_fillMode);
        connect(fillCb, &QCheckBox::toggled, [this](bool checked) {
            m_renderWidget->getSettings().m_fillMode = checked;
            m_renderWidget->refresh();
        });
        optLayout->addWidget(fillCb);
        
        QCheckBox* debugCb = new QCheckBox("Debug Mode");
        debugCb->setChecked(false);
        connect(debugCb, &QCheckBox::toggled, [this](bool checked) {
            m_renderWidget->getSettings().m_debugMode = checked;
            m_renderWidget->refresh();
        });
        optLayout->addWidget(debugCb);
        
        optGroup->setLayout(optLayout);
        layout->addWidget(optGroup);
        
        layout->addStretch();
        dock->setWidget(widget);
        addDockWidget(Qt::LeftDockWidgetArea, dock);
    }
    
    void createMenu() {
        QMenuBar* menuBar = this->menuBar();
        QMenu* viewMenu = menuBar->addMenu("View");
        
        QAction* fitAction = viewMenu->addAction("Fit to Window (F)");
        connect(fitAction, &QAction::triggered, [this]() {
            m_renderWidget->fitToWindow();
        });
        
        QAction* resetAction = viewMenu->addAction("Reset View (R)");
        connect(resetAction, &QAction::triggered, [this]() {
            m_renderWidget->refresh();
        });
    }
    
    void createStatusBar() {
        QStatusBar* status = statusBar();
        
        if (m_board) {
            int padCount = 0;
            for (const FOOTPRINT* fp : m_board->Footprints()) {
                padCount += fp->Pads().size();
            }
            
            status->showMessage(QString("Board: %1 footprints, %2 pads, %3 tracks, %4 zones")
                              .arg(m_board->Footprints().size())
                              .arg(padCount)
                              .arg(m_board->Tracks().size())
                              .arg(m_board->Zones().size()));
        }
    }
    
private:
    BOARD* m_board;
    PCBRenderWidget* m_renderWidget;
};

// Simple PGM
class PCB_TEST_PGM {
public:
    PCB_TEST_PGM() {}
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    BOARD* board = nullptr;
    
    try {
        PCB_TEST_PGM pgm;
        
        QString pcbPath;
        if (argc > 1) {
            pcbPath = argv[1];
        } else {
            pcbPath = "test/complex_hierarchy.kicad_pcb";
            if (!QFileInfo::exists(pcbPath)) {
                pcbPath = "complex_hierarchy.kicad_pcb";
            }
        }
        
        std::cout << "Loading: " << pcbPath.toStdString() << std::endl;
        
        PCB_IO_KICAD_SEXPR pcbIO;
        board = pcbIO.LoadBoard(pcbPath, nullptr);
        
        if (!board) {
            std::cerr << "Failed to load PCB!" << std::endl;
            return 1;
        }
        
        std::cout << "Board loaded successfully" << std::endl;
        std::cout << "Footprints: " << board->Footprints().size() << std::endl;
        std::cout << "Tracks: " << board->Tracks().size() << std::endl;
        
        MainWindow window(board);
        window.show();
        
        int result = app.exec();
        delete board;
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        if (board) delete board;
        return 1;
    }
}