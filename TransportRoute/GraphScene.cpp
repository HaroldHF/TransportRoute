#include "GraphScene.h"
#include <QtMath>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent)
{
    penEdgeOpen_.setWidthF(1.5);
    penEdgeClosed_.setWidthF(1.5);
    penEdgeClosed_.setStyle(Qt::DashLine);
    penEdgeClosed_.setColor(Qt::red);

    penPath_.setWidthF(3.0);
    penPath_.setColor(Qt::blue);

    penMst_.setWidthF(3.0);
    penMst_.setColor(Qt::darkGreen);

    brushNode_ = QBrush(Qt::white);
}

void GraphScene::setGraph(const transport::Graph* g) {
    g_ = g;
    drawGraph();
}

void GraphScene::computeCircularLayout() {
    pos_.clear();
    if (!g_) return;
    const auto& data = g_->data();
    int n = (int)data.size();
    if (n == 0) return;

    // radio segun cantidad
    const double R = 160 + 6.0 * n;
    const QPointF center(0, 0);

    int i = 0;
    for (const auto& kv : data) {
        int id = kv.first;
        double ang = (2.0 * M_PI * i) / std::max(1, n);
        QPointF p(center.x() + R * std::cos(ang),
            center.y() + R * std::sin(ang));
        pos_[id] = p;
        ++i;
    }
}

void GraphScene::drawGraph() {
    clear();
    clearHighlights();
    if (!g_) return;

    computeCircularLayout();

    // dibujar aristas (u<v para evitar duplicado visual)
    for (const auto& [u, vec] : g_->data()) {
        for (const auto& e : vec) {
            if (u >= e.to) continue; // dibujar una vez
            auto pu = pos_[u];
            auto pv = pos_[e.to];
            QPen pen = e.closed ? penEdgeClosed_ : penEdgeOpen_;
            addLine(QLineF(pu, pv), pen);
        }
    }

    // dibujar nodos
    const double r = 14.0;
    for (const auto& [id, p] : pos_) {
        auto* c = addEllipse(p.x() - r, p.y() - r, 2 * r, 2 * r, QPen(Qt::black), brushNode_);
        (void)c;
        auto* t = addSimpleText(QString::number(id));
        t->setPos(p.x() - 6, p.y() - 10);
    }

    // ajustar vista
    setSceneRect(itemsBoundingRect().adjusted(-40, -40, 40, 40));
}

void GraphScene::clearHighlights() {
    // nada persistente: todo es regenerado en drawGraph() y re-resaltado
}

void GraphScene::highlightPath(const std::vector<int>& path) {
    if (!g_ || path.size() < 2) return;
    for (size_t i = 1; i < path.size(); ++i) {
        int u = path[i - 1], v = path[i];
        if (!pos_.count(u) || !pos_.count(v)) continue;
        addLine(QLineF(pos_[u], pos_[v]), penPath_);
    }
}

void GraphScene::highlightMST(const std::vector<std::pair<int, int>>& edges) {
    if (!g_) return;
    for (const auto& e : edges) {
        int u = e.first, v = e.second;
        if (!pos_.count(u) || !pos_.count(v)) continue;
        addLine(QLineF(pos_[u], pos_[v]), penMst_);
    }
}
