#pragma once
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <unordered_map>
#include "NodeItem.h"
#include "EdgeItem.h"
#include "Graph.h"

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GraphScene(QObject* parent = nullptr);

    void setGraph(const transport::Graph* g);
    void setPositions(const std::unordered_map<int, QPointF>& pos);
    const std::unordered_map<int, QPointF>& positions() const { return pos_; }

    void drawGraph();
    void setBackgroundImage(const QString& path); // carga/coloca imagen (opcional)

    // resaltados
    void highlightPath(const std::vector<int>& path);
    void highlightMST(const std::vector<std::pair<int, int>>& edges);

signals:
    // señales para menus desde la ventana
    void contextOnEmpty(const QPointF& scenePos);
    void requestEdgeMenu(EdgeItem* edge, const QPointF& pos);
    void nodeMoved(int id, QPointF pos);
    void nodeContext(NodeItem* node, QPointF pos);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* e) override;

private:
    const transport::Graph* g_ = nullptr;
    std::unordered_map<int, NodeItem*> nodes_;
    std::unordered_map<long long, EdgeItem*> edges_;
    std::unordered_map<int, QPointF> pos_;
    QGraphicsPixmapItem* bg_ = nullptr;

    static long long key(int a, int b) { if (a > b) std::swap(a, b); return ((long long)a << 32) | (unsigned)b; }
    void ensureNodes();
    void ensureEdges();
    void repositionAll();
    void clearAll();
};
