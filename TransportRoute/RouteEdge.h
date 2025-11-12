#pragma once
#include <QGraphicsObject>
#include <QPen>
#include "StationNode.h"

class RouteEdge : public QGraphicsObject {
    Q_OBJECT

public:
    RouteEdge(StationNode* from, StationNode* to, double weight, bool closed = false);

    bool connects(int u, int v) const;
    void setHighlighted(bool highlighted, const QColor& color = QColor(0, 212, 255));
    void setClosed(bool closed);
    void setWeight(double weight);

    double getWeight() const { return edgeWeight; }
    bool isClosed() const { return isClosed_; }

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPainterPath shape() const override;

signals:
    void clicked();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private slots:
    void updatePosition();

private:
    StationNode* fromNode;
    StationNode* toNode;
    double edgeWeight;
    bool isClosed_;
    bool isHighlighted_;
    bool isHovered_;
    QColor highlightColor;

    QPointF sourcePoint;
    QPointF destPoint;

    void updateGeometry();
};