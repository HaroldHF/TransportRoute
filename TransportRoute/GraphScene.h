#pragma once
#include <QGraphicsScene>
#include <QPen>
#include <QBrush>
#include <unordered_map>
#include <vector>
#include "Graph.h"   

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GraphScene(QObject* parent = nullptr);

    // asignar y redibujar grafo
    void setGraph(const transport::Graph* g);
    void drawGraph();

    // resaltar resultados
    void highlightPath(const std::vector<int>& path);
    void highlightMST(const std::vector<std::pair<int, int>>& edges);

private:
    const transport::Graph* g_ = nullptr;

    // layout circular simple: id -> pos
    std::unordered_map<int, QPointF> pos_;

    // helpers
    void computeCircularLayout();
    void clearHighlights();

    // estilos
    QPen penEdgeOpen_;
    QPen penEdgeClosed_;
    QPen penPath_;
    QPen penMst_;
    QBrush brushNode_;
};
