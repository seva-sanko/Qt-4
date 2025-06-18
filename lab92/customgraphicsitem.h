#ifndef CUSTOMGRAPHICSITEM_H
#define CUSTOMGRAPHICSITEM_H

#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include <QList>
#include <QPair>

class CustomGraphicsItem : public QGraphicsItemGroup {
public:
    explicit CustomGraphicsItem(QGraphicsItem *parent = nullptr) : QGraphicsItemGroup(parent) {}

    QList<QPair<CustomGraphicsItem *, QGraphicsLineItem *>> connections;

    void addConnection(CustomGraphicsItem *other, QGraphicsLineItem *line);

    void removeConnection(CustomGraphicsItem *other);

    void updateConnections();

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

#endif
