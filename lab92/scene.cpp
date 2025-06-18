#include "scene.h"
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QPolygonF>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>
#include <QRandomGenerator>
#include "customgraphicsitem.h"

Scene::Scene(QObject *parent)
    : QGraphicsScene(parent), connectionMode(false), shapeCounter(0) {}

void Scene::addRectangle() {
    CustomGraphicsItem *rect = new CustomGraphicsItem();
    QGraphicsRectItem *shape = new QGraphicsRectItem(0, 0, 100, 50, rect);
    shape->setPen(QPen(Qt::blue, 2));

    rect->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    int x = QRandomGenerator::global()->bounded(0, width());
    int y = QRandomGenerator::global()->bounded(0, height());
    rect->setPos(x, y);

    addItem(rect);
    itemIds[rect] = shapeCounter++;
    itemTypes[rect] = "Rectangle";
    connections[rect] = {};
}

void Scene::addEllipse() {
    CustomGraphicsItem *ellipse = new CustomGraphicsItem();
    QGraphicsEllipseItem *shape = new QGraphicsEllipseItem(0, 0, 80, 50, ellipse);
    shape->setPen(QPen(Qt::red, 2));

    ellipse->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    int x = QRandomGenerator::global()->bounded(0, width());
    int y = QRandomGenerator::global()->bounded(0, height());
    ellipse->setPos(x, y);

    addItem(ellipse);
    itemIds[ellipse] = shapeCounter++;
    itemTypes[ellipse] = "Ellipse";
    connections[ellipse] = {};
}

void Scene::addPolygon(int sides) {
    if (sides < 3) return;

    CustomGraphicsItem *polygonItem = new CustomGraphicsItem();
    QPolygonF polygon;
    qreal angleStep = 360.0 / sides;

    for (int i = 0; i < sides; ++i) {
        qreal angle = qDegreesToRadians(angleStep * i);
        polygon << QPointF(50 * cos(angle), 50 * sin(angle));
    }

    QGraphicsPolygonItem *shape = new QGraphicsPolygonItem(polygon, polygonItem);
    shape->setPen(QPen(Qt::green, 2));

    polygonItem->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    int x = QRandomGenerator::global()->bounded(0, width());
    int y = QRandomGenerator::global()->bounded(0, height());
    polygonItem->setPos(x, y);

    addItem(polygonItem);
    itemIds[polygonItem] = shapeCounter++;
    itemTypes[polygonItem] = "Polygon";
    connections[polygonItem] = {};
}

void Scene::startConnectionMode() {
    connectionMode = true;
    clearSelectedItems();
}

void Scene::addConnection(CustomGraphicsItem *item1, CustomGraphicsItem *item2) {
    if (!item1 || !item2 || item1 == item2) return;

    QPointF point1 = item1->mapToScene(item1->boundingRect().center());
    QPointF point2 = item2->mapToScene(item2->boundingRect().center());

    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(point1, point2));
    line->setPen(QPen(Qt::black, 2));
    addItem(line);

    item1->addConnection(item2, line);
    item2->addConnection(item1, line);
}

void Scene::clearSelectedItems() {
    for (auto item : selectedItemsForConnection) {
        item->setSelected(false);
    }
    selectedItemsForConnection.clear();
}

void Scene::deleteSelected() {
    for (auto item : selectedItems()) {
        auto customItem = dynamic_cast<CustomGraphicsItem *>(item);
        if (customItem) {
            for (auto &conn : customItem->connections) {
                CustomGraphicsItem *other = conn.first;
                QGraphicsLineItem *lineItem = conn.second;

                if (lineItem) {
                    removeItem(lineItem);
                    delete lineItem;
                }

                other->removeConnection(customItem);
            }
            customItem->connections.clear();
        }

        removeItem(item);
        delete item;
    }
}


void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    auto item = itemAt(event->scenePos(), QTransform());

    if (connectionMode) {
        if (item && !selectedItemsForConnection.contains(item)) {
            item->setSelected(true);
            selectedItemsForConnection.append(item);
            if (selectedItemsForConnection.size() == 2) {

                QGraphicsItem *item1 = selectedItemsForConnection[0];
                QGraphicsItem *item2 = selectedItemsForConnection[1];
                QLineF line(item1->sceneBoundingRect().center(), item2->sceneBoundingRect().center());
                QGraphicsLineItem *connection = new QGraphicsLineItem(line);
                connection->setPen(QPen(Qt::black, 2));
                addItem(connection);
                connections[item1].append(connection);
                connections[item2].append(connection);
                clearSelectedItems();
                connectionMode = false;
            }
        }
    } else {
        if (item) {
            item->setSelected(!item->isSelected());
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mouseMoveEvent(event);
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
    if (item && item->isSelected() && event->buttons() & Qt::LeftButton) {
        item->setPos(event->scenePos());

        auto customItem = dynamic_cast<CustomGraphicsItem *>(item);
        if (customItem) {
            for (auto &pair : customItem->connections) {
                QPointF point1 = customItem->mapToScene(customItem->boundingRect().center());
                QPointF point2 = pair.first->mapToScene(pair.first->boundingRect().center());
                pair.second->setLine(QLineF(point1, point2));
            }
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
    updateConnections();
}
void Scene::updateConnections() {
    for (auto item : items()) {
        auto customItem = dynamic_cast<CustomGraphicsItem *>(item);
        if (customItem) {
            customItem->updateConnections();
        }
    }
}

/*
void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (auto item = itemAt(event->scenePos(), QTransform())) {
        if (item->isSelected() && event->buttons() & Qt::LeftButton) {
            item->setPos(event->scenePos());
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}
*/

void Scene::filterShapes(const QString &filterType, const QString &filterValue) {
    for (auto item : items()) {
        bool visible = true;
        if (filterType == "type") {
            if (itemTypes[item] != filterValue) visible = false;
        } else if (filterType == "id") {
            if (QString::number(itemIds[item]) != filterValue) visible = false;
        }

        item->setVisible(visible);
    }
}
