#include "customgraphicsitem.h"
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QList>
#include <QPair>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

void CustomGraphicsItem::addConnection(CustomGraphicsItem *other, QGraphicsLineItem *line) {
    connections.append({other, line});
}

void CustomGraphicsItem::removeConnection(CustomGraphicsItem *other) {

    for (auto it = connections.begin(); it != connections.end(); ++it) {
        if (it->first == other) {

            if (scene()) {
                scene()->removeItem(it->second);
            }
            connections.erase(it);
            break;
        }
    }
}

QVariant CustomGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange) {
        for (auto &conn : connections) {
            CustomGraphicsItem *other = conn.first;
            QGraphicsLineItem *line = conn.second;

            QPointF point1 = this->scenePos();
            QPointF point2 = other->scenePos();
            line->setLine(QLineF(point1, point2));
        }
    }
    return QGraphicsItemGroup::itemChange(change, value);
}

void CustomGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    setPos(mapToScene(event->pos()));

    updateConnections();

    QGraphicsItem::mouseMoveEvent(event);
}

void CustomGraphicsItem::updateConnections() {
    for (auto &conn : connections) {
        CustomGraphicsItem *otherItem = conn.first;
        QGraphicsLineItem *line = conn.second;

        QPointF point1 = mapToScene(boundingRect().center());
        QPointF point2 = otherItem->mapToScene(otherItem->boundingRect().center());

        line->setLine(QLineF(point1, point2));
    }
}
