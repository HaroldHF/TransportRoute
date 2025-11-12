#include "MapCanvas.h"
#include <QPainter>
#include <QGraphicsRectItem>
#include <cmath>
#include <qscrollbar.h>
MapCanvas::MapCanvas(QWidget* parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , controller(nullptr)
    , hasBackground(false)
    , currentMode(MODE_SELECT)
    , firstStationForRoute(-1)
    , isPanning(false)
    , currentScale(1.0)
{
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setDragMode(QGraphicsView::NoDrag);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Tamaño de la escena
    scene->setSceneRect(0, 0, 1200, 800);

    // Estilo visual
    setStyleSheet(R"(
        QGraphicsView {
            background-color: #1e1e2e;
            border: none;
        }
    )");
}

MapCanvas::~MapCanvas() {
    clearGraph();
}

void MapCanvas::setController(transport::TransportController* ctrl) {
    controller = ctrl;
}

void MapCanvas::setBackgroundMap(const QString& imagePath) {
    backgroundMap = QPixmap(imagePath);
    hasBackground = !backgroundMap.isNull();
    if (hasBackground) {
        scene->setSceneRect(backgroundMap.rect());
    }
    viewport()->update();
}

void MapCanvas::refreshGraph() {
    if (!controller) return;

    clearGraph();

    // Crear aristas primero (para que estén detrás de los nodos)
    const auto& graphData = controller->graph.data();
    std::set<std::pair<int, int>> processedEdges;

    for (const auto& [u, edges] : graphData) {
        for (const auto& edge : edges) {
            int v = edge.to;
            if (u < v) { // Evitar duplicados
                auto key = std::make_pair(u, v);
                if (processedEdges.find(key) == processedEdges.end()) {
                    createRouteEdge(u, v, edge.w, edge.closed);
                    processedEdges.insert(key);
                }
            }
        }
    }

    // Crear nodos de estaciones
    auto stations = controller->stationsInOrder();
    for (const auto& station : stations) {
        createStationNode(station);
    }
}

void MapCanvas::clearGraph() {
    for (auto edge : routeEdges) {
        scene->removeItem(edge);
        delete edge;
    }
    routeEdges.clear();

    for (auto& [id, node] : stationNodes) {
        scene->removeItem(node);
        delete node;
    }
    stationNodes.clear();
    selectedStations.clear();
}

void MapCanvas::createStationNode(const transport::Station& station) {
    auto node = new StationNode(station.id, QString::fromStdString(station.name));
    node->setPos(station.x, station.y);
    scene->addItem(node);
    stationNodes[station.id] = node;

    // Conectar señales
    connect(node, &StationNode::clicked, this, [this](int id) {
        emit stationClicked(id);
        });
    connect(node, &StationNode::doubleClicked, this, &MapCanvas::stationDoubleClicked);
    connect(node, &StationNode::positionChanged, this, &MapCanvas::stationMoved);
}

void MapCanvas::createRouteEdge(int u, int v, double weight, bool closed) {
    auto nodeU = stationNodes.find(u);
    auto nodeV = stationNodes.find(v);
    if (nodeU == stationNodes.end() || nodeV == stationNodes.end()) return;

    auto edge = new RouteEdge(nodeU->second, nodeV->second, weight, closed);
    scene->addItem(edge);
    routeEdges.push_back(edge);

    connect(edge, &RouteEdge::clicked, this, [this, u, v]() {
        emit edgeClicked(u, v);
        });
}

void MapCanvas::selectStation(int id) {
    selectedStations.insert(id);
    if (auto it = stationNodes.find(id); it != stationNodes.end()) {
        it->second->setSelected(true);
    }
}

void MapCanvas::deselectAll() {
    for (int id : selectedStations) {
        if (auto it = stationNodes.find(id); it != stationNodes.end()) {
            it->second->setSelected(false);
        }
    }
    selectedStations.clear();
}

std::vector<int> MapCanvas::getSelectedStations() const {
    return std::vector<int>(selectedStations.begin(), selectedStations.end());
}

void MapCanvas::highlightPath(const std::vector<int>& path, const QColor& color) {
    clearHighlights();

    // Resaltar aristas del camino
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        int u = path[i], v = path[i + 1];
        for (auto edge : routeEdges) {
            if (edge->connects(u, v)) {
                edge->setHighlighted(true, color);
            }
        }
    }

    // Resaltar nodos del camino
    for (int id : path) {
        if (auto it = stationNodes.find(id); it != stationNodes.end()) {
            it->second->setHighlighted(true, color);
        }
    }
}

void MapCanvas::highlightVisitOrder(const std::vector<int>& order) {
    clearHighlights();

    // Colorear nodos según orden de visita (gradiente)
    for (size_t i = 0; i < order.size(); ++i) {
        int id = order[i];
        if (auto it = stationNodes.find(id); it != stationNodes.end()) {
            // Gradiente de azul a verde
            int hue = 200 - (i * 100 / order.size());
            QColor color = QColor::fromHsv(hue, 200, 255);
            it->second->setHighlighted(true, color);
            it->second->setVisitOrder(i + 1);
        }
    }
}

void MapCanvas::highlightMST(const std::vector<std::pair<int, int>>& edges) {
    clearHighlights();

    QColor mstColor(0, 255, 136); // Verde neón
    for (const auto& [u, v] : edges) {
        for (auto edge : routeEdges) {
            if (edge->connects(u, v)) {
                edge->setHighlighted(true, mstColor);
            }
        }

        if (auto it = stationNodes.find(u); it != stationNodes.end()) {
            it->second->setHighlighted(true, mstColor);
        }
        if (auto it = stationNodes.find(v); it != stationNodes.end()) {
            it->second->setHighlighted(true, mstColor);
        }
    }
}

void MapCanvas::clearHighlights() {
    for (auto edge : routeEdges) {
        edge->setHighlighted(false, Qt::white);
    }
    for (auto& [id, node] : stationNodes) {
        node->setHighlighted(false, Qt::white);
        node->setVisitOrder(-1);
    }
}

void MapCanvas::setInteractionMode(InteractionMode mode) {
    currentMode = mode;
    firstStationForRoute = -1;

    // Cambiar cursor según modo
    switch (mode) {
    case MODE_SELECT:
        setCursor(Qt::ArrowCursor);
        break;
    case MODE_ADD_STATION:
        setCursor(Qt::CrossCursor);
        break;
    case MODE_ADD_ROUTE:
        setCursor(Qt::PointingHandCursor);
        break;
    }
}

void MapCanvas::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom con Ctrl + Wheel
        qreal factor = std::pow(1.2, event->angleDelta().y() / 240.0);
        scaleView(factor);
        event->accept();
    }
    else {
        QGraphicsView::wheelEvent(event);
    }
}

void MapCanvas::scaleView(qreal scaleFactor) {
    qreal newScale = currentScale * scaleFactor;
    if (newScale < 0.1 || newScale > 10.0) return;

    scale(scaleFactor, scaleFactor);
    currentScale = newScale;
}

void MapCanvas::mousePressEvent(QMouseEvent* event) {
    QPointF scenePos = mapToScene(event->pos());

    if (event->button() == Qt::MiddleButton) {
        isPanning = true;
        lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    if (currentMode == MODE_ADD_STATION && event->button() == Qt::LeftButton) {
        emit stationAdded(scenePos.x(), scenePos.y());
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void MapCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MiddleButton) {
        isPanning = false;
        setCursor(currentMode == MODE_SELECT ? Qt::ArrowCursor : Qt::CrossCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MapCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (isPanning) {
        QPoint delta = event->pos() - lastPanPoint;
        lastPanPoint = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MapCanvas::drawBackground(QPainter* painter, const QRectF& rect) {
    painter->fillRect(rect, QColor(30, 30, 46)); // #1e1e2e

    if (hasBackground) {
        painter->setOpacity(0.3); // Mapa semi-transparente
        painter->drawPixmap(scene->sceneRect().toRect(), backgroundMap);
        painter->setOpacity(1.0);
    }

    // Grid sutil
    painter->setPen(QPen(QColor(60, 60, 80), 1, Qt::DotLine));
    for (int x = 0; x < scene->width(); x += 50) {
        painter->drawLine(x, 0, x, scene->height());
    }
    for (int y = 0; y < scene->height(); y += 50) {
        painter->drawLine(0, y, scene->width(), y);
    }
}