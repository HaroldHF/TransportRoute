#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QObject>

class NodeItem : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT
public:
    explicit NodeItem(int id, const QRectF& r, QGraphicsItem* parent = nullptr)
        : QGraphicsEllipseItem(r, parent), id_(id) {
        setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
        setZValue(10); // encima de aristas
    }
    int id() const { return id_; }

signals:
    void moved(int id, QPointF pos);
    void contextOnNode(NodeItem* self, const QPointF& scenePos);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override {
        if (change == ItemPositionHasChanged) emit moved(id_, scenePos());
        return QGraphicsEllipseItem::itemChange(change, value);
    }
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* e) override {
        emit contextOnNode(this, e->scenePos());
    }
private:
    int id_;
};
