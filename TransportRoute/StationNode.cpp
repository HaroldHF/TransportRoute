#include "StationNode.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QRadialGradient>
#include <QFont>

StationNode::StationNode(int id, const QString& name, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , stationId(id)
    , stationName(name)
    , isSelected(false)
    , isHighlighted(false)
    , isHovered(false)
    , visitOrder(-1)
    , highlightColor(0, 212, 255)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setZValue(10); // Nodos siempre encima de aristas
}

QRectF StationNode::boundingRect() const {
    qreal r = selectionRadius + 5; // Margen para sombra
    return QRectF(-r, -r, 2 * r, 2 * r);
}

QPainterPath StationNode::shape() const {
    QPainterPath path;
    path.addEllipse(-radius, -radius, 2 * radius, 2 * radius);
    return path;
}

void StationNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    qreal r = getNodeRadius();
    QColor nodeColor = getNodeColor();

    // Sombra
    if (!isHighlighted) {
        QRadialGradient shadowGradient(0, 2, r + 3);
        shadowGradient.setColorAt(0, QColor(0, 0, 0, 80));
        shadowGradient.setColorAt(1, QColor(0, 0, 0, 0));
        painter->setBrush(shadowGradient);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(0, 2), r + 3, r + 3);
    }

    // Glow effect si está resaltado o hover
    if (isHighlighted || isHovered) {
        QRadialGradient glowGradient(0, 0, r + 8);
        QColor glowColor = isHighlighted ? highlightColor : QColor(255, 255, 255);
        glowColor.setAlpha(isHighlighted ? 100 : 50);
        glowGradient.setColorAt(0, glowColor);
        glowColor.setAlpha(0);
        glowGradient.setColorAt(1, glowColor);
        painter->setBrush(glowGradient);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(0, 0), r + 8, r + 8);
    }

    // Círculo principal con gradiente
    QRadialGradient gradient(0, -r * 0.3, r * 1.5);
    gradient.setColorAt(0, nodeColor.lighter(130));
    gradient.setColorAt(1, nodeColor);
    painter->setBrush(gradient);

    // Borde
    QPen pen;
    if (isSelected) {
        pen = QPen(QColor(255, 107, 157), 3); // Rosa neón
    }
    else if (isHighlighted) {
        pen = QPen(highlightColor, 3);
    }
    else {
        pen = QPen(QColor(60, 60, 80), 2);
    }
    painter->setPen(pen);
    painter->drawEllipse(QPointF(0, 0), r, r);

    // Icono de estación (simple)
    painter->setPen(QPen(Qt::white, 2));
    painter->setBrush(Qt::NoBrush);
    qreal iconSize = r * 0.5;
    painter->drawRect(-iconSize / 2, -iconSize / 2, iconSize, iconSize);
    painter->drawLine(-iconSize / 2, 0, iconSize / 2, 0);
    painter->drawLine(0, -iconSize / 2, 0, iconSize / 2);

    // Orden de visita (para BFS/DFS)
    if (visitOrder >= 0) {
        QFont font("Arial", 10, QFont::Bold);
        painter->setFont(font);
        painter->setPen(Qt::white);
        painter->setBrush(QColor(255, 107, 157));
        painter->drawEllipse(QPointF(r * 0.7, -r * 0.7), 8, 8);
        painter->drawText(QRectF(r * 0.7 - 8, -r * 0.7 - 8, 16, 16),
            Qt::AlignCenter, QString::number(visitOrder));
    }

    // Etiqueta de nombre
    QFont labelFont("Segoe UI", 9, QFont::Bold);
    painter->setFont(labelFont);

    // Fondo de la etiqueta
    QFontMetrics fm(labelFont);
    QString displayName = stationName;
    if (fm.horizontalAdvance(displayName) > 120) {
        displayName = fm.elidedText(displayName, Qt::ElideRight, 120);
    }
    QRectF textRect = fm.boundingRect(displayName);
    textRect.moveCenter(QPointF(0, r + 18));
    textRect.adjust(-5, -2, 5, 2);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(42, 42, 62, 200));
    painter->drawRoundedRect(textRect, 4, 4);

    // Texto
    painter->setPen(QColor(224, 224, 224));
    painter->drawText(textRect, Qt::AlignCenter, displayName);
}

QColor StationNode::getNodeColor() const {
    if (isHighlighted) {
        return highlightColor;
    }
    if (isSelected) {
        return QColor(100, 100, 255); // Azul
    }
    return QColor(70, 130, 180); // Steel blue
}

qreal StationNode::getNodeRadius() const {
    if (isHovered || isSelected) return radius * 1.1;
    return radius;
}

void StationNode::setSelected(bool selected) {
    isSelected = selected;
    update();
}

void StationNode::setHighlighted(bool highlighted, const QColor& color) {
    isHighlighted = highlighted;
    highlightColor = color;
    update();
}

void StationNode::setVisitOrder(int order) {
    visitOrder = order;
    update();
}

void StationNode::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(stationId);
    }
    QGraphicsObject::mousePressEvent(event);
}

void StationNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(stationId);
    }
    QGraphicsObject::mouseDoubleClickEvent(event);
}

void StationNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsObject::mouseReleaseEvent(event);
}

QVariant StationNode::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        QPointF newPos = value.toPointF();
        emit positionChanged(stationId, newPos.x(), newPos.y());
    }
    return QGraphicsObject::itemChange(change, value);
}

void StationNode::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void StationNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}