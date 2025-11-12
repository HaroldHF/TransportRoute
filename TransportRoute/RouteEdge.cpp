#include "RouteEdge.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QtMath>

RouteEdge::RouteEdge(StationNode* from, StationNode* to, double weight, bool closed)
    : fromNode(from)
    , toNode(to)
    , edgeWeight(weight)
    , isClosed_(closed)
    , isHighlighted_(false)
    , isHovered_(false)
    , highlightColor(0, 212, 255)
{
    setAcceptHoverEvents(true);
    setZValue(-1); // Detrás de los nodos

    connect(fromNode, &StationNode::positionChanged, this, &RouteEdge::updatePosition);
    connect(toNode, &StationNode::positionChanged, this, &RouteEdge::updatePosition);

    updateGeometry();
}

bool RouteEdge::connects(int u, int v) const {
    int id1 = fromNode->getId();
    int id2 = toNode->getId();
    return (id1 == u && id2 == v) || (id1 == v && id2 == u);
}

void RouteEdge::setHighlighted(bool highlighted, const QColor& color) {
    isHighlighted_ = highlighted;
    highlightColor = color;
    update();
}

void RouteEdge::setClosed(bool closed) {
    isClosed_ = closed;
    update();
}

void RouteEdge::setWeight(double weight) {
    edgeWeight = weight;
    update();
}

QRectF RouteEdge::boundingRect() const {
    qreal extra = 20;
    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
        destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

QPainterPath RouteEdge::shape() const {
    QPainterPath path;
    path.moveTo(sourcePoint);
    path.lineTo(destPoint);
    QPainterPathStroker stroker;
    stroker.setWidth(10);
    return stroker.createStroke(path);
}

void RouteEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!fromNode || !toNode) return;

    painter->setRenderHint(QPainter::Antialiasing);

    // Color de la línea
    QColor lineColor;
    qreal lineWidth = 3.0;
    Qt::PenStyle penStyle = Qt::SolidLine;

    if (isClosed_) {
        lineColor = QColor(255, 71, 87); // Rojo para cerrada
        penStyle = Qt::DashLine;
        lineWidth = 4.0;
    }
    else if (isHighlighted_) {
        lineColor = highlightColor;
        lineWidth = 5.0;
    }
    else if (isHovered_) {
        lineColor = QColor(255, 255, 255, 150);
        lineWidth = 4.0;
    }
    else {
        lineColor = QColor(100, 149, 237); // Cornflower blue
    }

    // Sombra sutil
    if (!isClosed_) {
        painter->setPen(QPen(QColor(0, 0, 0, 50), lineWidth + 2, penStyle));
        painter->drawLine(sourcePoint.x() + 2, sourcePoint.y() + 2,
            destPoint.x() + 2, destPoint.y() + 2);
    }

    // Línea principal
    QPen pen(lineColor, lineWidth, penStyle, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawLine(sourcePoint, destPoint);

    // Etiqueta de peso (en el centro)
    QPointF midPoint = (sourcePoint + destPoint) / 2.0;

    QString weightText = QString::number(edgeWeight, 'f', 1) + " min";
    if (isClosed_) {
        weightText += " ⊗"; // Símbolo de cerrado
    }

    QFont font("Segoe UI", 8, QFont::Bold);
    painter->setFont(font);
    QFontMetrics fm(font);
    QRectF textRect = fm.boundingRect(weightText);
    textRect.moveCenter(midPoint);
    textRect.adjust(-4, -2, 4, 2);

    // Fondo de la etiqueta
    QColor bgColor = isClosed_ ? QColor(255, 71, 87, 220) : QColor(42, 42, 62, 220);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawRoundedRect(textRect, 3, 3);

    // Texto
    painter->setPen(Qt::white);
    painter->drawText(textRect, Qt::AlignCenter, weightText);

    // Indicador de dirección (flecha pequeña en el medio)
    if (!isClosed_ && isHighlighted_) {
        QLineF line(sourcePoint, destPoint);
        qreal angle = std::atan2(-line.dy(), line.dx());

        QPointF arrowP1 = midPoint + QPointF(std::sin(angle + M_PI / 3) * 8,
            std::cos(angle + M_PI / 3) * 8);
        QPointF arrowP2 = midPoint + QPointF(std::sin(angle + M_PI - M_PI / 3) * 8,
            std::cos(angle + M_PI - M_PI / 3) * 8);

        painter->setBrush(lineColor);
        painter->setPen(QPen(lineColor, 2));
        QPolygonF arrowHead;
        arrowHead << midPoint << arrowP1 << arrowP2;
        painter->drawPolygon(arrowHead);
    }
}

void RouteEdge::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QGraphicsObject::mousePressEvent(event);
}

void RouteEdge::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    isHovered_ = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void RouteEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    isHovered_ = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

void RouteEdge::updatePosition() {
    updateGeometry();
}

void RouteEdge::updateGeometry() {
    prepareGeometryChange();
    sourcePoint = mapFromItem(fromNode, 0, 0);
    destPoint = mapFromItem(toNode, 0, 0);
}