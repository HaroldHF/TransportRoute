#include "GraphScene.h"
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSimpleTextItem>
#include <QtMath>

GraphScene::GraphScene(QObject* p) :QGraphicsScene(p) {}

void GraphScene::setGraph(const transport::Graph* g) {
    g_ = g;
    drawGraph();
}

void GraphScene::setPositions(const std::unordered_map<int, QPointF>& pos) {
    pos_ = pos;
}

void GraphScene::setBackgroundImage(const QString& path) {
    if (bg_) { removeItem(bg_); delete bg_; bg_ = nullptr; }
    QPixmap pm(path);
    if (!pm.isNull()) {
        bg_ = addPixmap(pm);
        bg_->setZValue(-100);
    }
}

void GraphScene::drawGraph() {
    clearAll();
    if (!g_) return;

    // crear nodos (usa posiciones guardadas o layout circular si faltan)
    ensureNodes();
    // crear aristas
    ensureEdges();
    // ajustar rect
    setSceneRect(itemsBoundingRect().adjusted(-60, -60, 60, 60));
}

void GraphScene::ensureNodes() {
    // si faltan posiciones, layout circular
    if (pos_.empty()) {
        const auto& data = g_->data();
        int n = (int)data.size(); int i = 0; double R = 160 + 6.0 * n;
        for (const auto& kv : data) {
            double ang = (2 * M_PI * i) / std::max(1, n);
            pos_[kv.first] = QPointF(R * std::cos(ang), R * std::sin(ang));
            ++i;
        }
    }
    for (const auto& kv : g_->data()) {
        int id = kv.first;
        QPointF p = pos_.count(id) ? pos_[id] : QPointF(0, 0);
        auto* node = new NodeItem(id, QRectF(-14, -14, 28, 28));
        addItem(node);
        node->setPos(p);
        addSimpleText(QString::number(id))->setPos(p + QPointF(-6, -28));
        nodes_[id] = node;

        connect(node, &NodeItem::moved, this, [this](int id, QPointF pos) {
            pos_[id] = pos; emit nodeMoved(id, pos); repositionAll();
            });
        connect(node, &NodeItem::contextOnNode, this, [this](NodeItem* n, const QPointF& sp) {
            emit nodeContext(n, sp);
            });
    }
}

void GraphScene::ensureEdges() {
    for (const auto& [u, vec] : g_->data()) {
        for (const auto& e : vec) {
            if (u >= e.to) continue;
            auto k = key(u, e.to);
            auto* edge = new EdgeItem(u, e.to, e.w, e.closed);
            addItem(edge);
            edge->updateGeom(pos_[u], pos_[e.to]);
            edges_[k] = edge;

            connect(edge, &EdgeItem::contextOnEdge, this, [this](EdgeItem* ed, const QPointF& sp) {
                emit requestEdgeMenu(ed, sp);
                });
        }
    }
}

void GraphScene::repositionAll() {
    for (auto& [k, ed] : edges_) {
        ed->updateGeom(pos_[ed->u()], pos_[ed->v()]);
    }
}

void GraphScene::clearAll() {
    clear();
    nodes_.clear(); edges_.clear();
    if (bg_) { addItem(bg_); }
}

void GraphScene::highlightPath(const std::vector<int>& path) {
    if (path.size() < 2) return;
    QPen pen; pen.setWidthF(3.0); pen.setColor(Qt::blue);
    for (size_t i = 1; i < path.size(); ++i) {
        addLine(QLineF(pos_[path[i - 1]], pos_[path[i]]), pen)->setZValue(50);
    }
}

void GraphScene::highlightMST(const std::vector<std::pair<int, int>>& edges) {
    QPen pen; pen.setWidthF(3.0); pen.setColor(Qt::darkGreen);
    for (auto& e : edges) {
        addLine(QLineF(pos_[e.first], pos_[e.second]), pen)->setZValue(50);
    }
}

void GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* e) {
    // solo si click en vacio (los items emiten su propia señal)
    if (itemAt(e->scenePos(), QTransform()) == nullptr) emit contextOnEmpty(e->scenePos());
}
