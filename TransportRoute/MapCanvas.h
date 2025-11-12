#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <unordered_map>
#include <unordered_set>
#include "TransportController.h"
#include "StationNode.h"
#include "RouteEdge.h"

class MapCanvas : public QGraphicsView {
    Q_OBJECT

public:
    explicit MapCanvas(QWidget* parent = nullptr);
    ~MapCanvas();

    // Configuración
    void setController(transport::TransportController* ctrl);
    void setBackgroundMap(const QString& imagePath);

    // Renderizado del grafo
    void refreshGraph();
    void clearGraph();

    // Selección
    void selectStation(int id);
    void deselectAll();
    std::vector<int> getSelectedStations() const;

    // Visualización de resultados
    void highlightPath(const std::vector<int>& path, const QColor& color = QColor(0, 212, 255));
    void highlightVisitOrder(const std::vector<int>& order);
    void highlightMST(const std::vector<std::pair<int, int>>& edges);
    void clearHighlights();

    // Modos de interacción
    enum InteractionMode {
        MODE_SELECT,      // Seleccionar/mover estaciones
        MODE_ADD_STATION, // Agregar nueva estación
        MODE_ADD_ROUTE    // Agregar nueva ruta entre estaciones
    };
    void setInteractionMode(InteractionMode mode);
    InteractionMode getInteractionMode() const { return currentMode; }

signals:
    void stationClicked(int id);
    void stationDoubleClicked(int id);
    void stationMoved(int id, double x, double y);
    void stationAdded(double x, double y);
    void routeRequested(int fromId, int toId);
    void edgeClicked(int u, int v);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    QGraphicsScene* scene;
    transport::TransportController* controller;

    // Elementos visuales
    std::unordered_map<int, StationNode*> stationNodes;
    std::vector<RouteEdge*> routeEdges;
    std::unordered_set<int> selectedStations;

    // Fondo
    QPixmap backgroundMap;
    bool hasBackground;

    // Estado de interacción
    InteractionMode currentMode;
    int firstStationForRoute; // Para MODE_ADD_ROUTE
    bool isPanning;
    QPoint lastPanPoint;

    // Helpers
    void createStationNode(const transport::Station& station);
    void createRouteEdge(int u, int v, double weight, bool closed);
    void updateStationPositions();
    StationNode* findStationAt(const QPointF& pos);

    // Zoom
    void scaleView(qreal scaleFactor);
    qreal currentScale;
};