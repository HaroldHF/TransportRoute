#pragma once
#include <QObject>
#include <QGraphicsLineItem>
#include <QPen>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QPointF>
#include <QLineF>

class EdgeItem : public QObject, public QGraphicsLineItem {
    Q_OBJECT
public:
    EdgeItem(int u, int v, double w, bool closed, QGraphicsItem* parent = nullptr)
        : QObject(), QGraphicsLineItem(parent), u_(u), v_(v), w_(w), closed_(closed) {
        setZValue(5);
        setPen(closed_ ? penClosed() : penOpen());
        setAcceptHoverEvents(true);
    }
    int u() const { return u_; }
    int v() const { return v_; }
    double w() const { return w_; }
    bool closed() const { return closed_; }

    void updateGeom(const QPointF& pu, const QPointF& pv) {
        setLine(QLineF(pu, pv));
    }
    void setWeight(double w) { w_ = w; }
    void setClosed(bool c) { closed_ = c; setPen(closed_ ? penClosed() : penOpen()); }

    QPainterPath shape() const override {
        QPainterPathStroker s;
        s.setWidth(12);
        return s.createStroke(QGraphicsLineItem::shape());
    }

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* e) override {
        emit contextOnEdge(this, e->scenePos());
    }
    void hoverEnterEvent(QGraphicsSceneHoverEvent*) override { setZValue(20); }
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override { setZValue(5); }

signals:
    void contextOnEdge(EdgeItem* self, const QPointF& pos);

private:
    static QPen penOpen() { QPen p; p.setWidthF(1.5); return p; }
    static QPen penClosed() { QPen p; p.setWidthF(1.5); p.setColor(Qt::red); p.setStyle(Qt::DashLine); return p; }

    int u_, v_;
    double w_;
    bool closed_;
};
