#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QMap>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QSet>
#include "customgraphicsitem.h"

class Scene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit Scene(QObject *parent = nullptr);

    void addRectangle();
    void addEllipse();
    void addPolygon(int sides);
    void startConnectionMode();
    void clearSelectedItems();
    void deleteSelected();
    void addConnection(CustomGraphicsItem *item1, CustomGraphicsItem *item2);
    void filterShapes(const QString &filterType, const QString &filterValue);
    void updateConnections();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QList<QGraphicsItem *> selectedItemsForConnection;
    int shapeCounter;
    QMap<QGraphicsItem *, int> itemIds;
    QMap<QGraphicsItem *, QString> itemTypes;
    QMap<QGraphicsItem *, QList<QGraphicsLineItem *>> connections;
    bool connectionMode = false;
    QList<CustomGraphicsItem *> connectionTargets;
};

#endif // SCENE_H
