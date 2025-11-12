#pragma once
#include <QGraphicsObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QColor>

class StationNode : public QGraphicsObject {
    Q_OBJECT

public:
    explicit StationNode(int id, const QString& name, QGraphicsItem* parent = nullptr);

    int getId() const { return stationId; }
    QString getName() const { return stationName; }

    // Estado visual
    void setSelected(bool selected);
    void setHighlighted(bool highlighted, const QColor& color = QColor(0, 212, 255));
    void setVisitOrder(int order); // Para BFS/DFS

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    QPainterPath shape() const override;

signals:
    void clicked(int id);
    void doubleClicked(int id);
    void positionChanged(int id, double x, double y);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    int stationId;
    QString stationName;

    // Estado
    bool isSelected;
    bool isHighlighted;
    bool isHovered;
    int visitOrder; // -1 si no aplica
    QColor highlightColor;

    // Geometría
    static constexpr qreal radius = 20.0;
    static constexpr qreal selectionRadius = 25.0;

    // Helpers
    QColor getNodeColor() const;
    qreal getNodeRadius() const;
};