#include "customscene.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

CustomScene::CustomScene(QObject *parent)
    : QGraphicsScene(parent) {}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
    if (item) {
        selectedItem = item;
        selectedItemId = item->data(0).toInt();
        qDebug() << "Item selected: ID =" << selectedItemId;
        emit itemSelected(selectedItemId);
        maxZValue += 1;
        item->setZValue(maxZValue);
    } else {
        selectedItem = nullptr;
        selectedItemId = -1;
    }
    QGraphicsScene::mousePressEvent(event);
}

void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (selectedItem && event->buttons() & Qt::LeftButton) {
        selectedItem->setPos(event->scenePos());


        for (CustomLine *line : lines) {
            if (line->startItem() == selectedItem || line->endItem() == selectedItem) {
                line->updateLine();
            }
        }

        emit itemMoved(selectedItemId, event->scenePos());
    }

    QGraphicsScene::mouseMoveEvent(event);

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item && item->isSelected()) {
        item->setZValue(maxZValue + 1);

        maxZValue = item->zValue();
    }
}

void CustomScene::createPair(int id1, int id2) {
    if (id1 == id2) {
        qWarning() << "Cannot create a pair with the same figure.";
        return;
    }

    QGraphicsItem *item1 = nullptr;
    QGraphicsItem *item2 = nullptr;

    for (QGraphicsItem *item : items()) {
        int itemId = item->data(0).toInt();
        if (itemId == id1) item1 = item;
        if (itemId == id2) item2 = item;
        if (item1 && item2) break;
    }

    if (!item1 || !item2) {
        qWarning() << "One or both items not found for IDs:" << id1 << id2;
        return;
    }

    CustomLine *line = new CustomLine(item1, item2, this);
    lines.append(line);

    QSqlQuery query;
    QString relatedIds1, relatedIds2;

    query.prepare("SELECT related_ids FROM figures WHERE id = ?");
    query.addBindValue(id1);
    if (query.exec() && query.next()) {
        relatedIds1 = query.value(0).toString();
    } else {
        qWarning() << "Failed to fetch related IDs for figure" << id1 << ":" << query.lastError().text();
    }

    query.prepare("SELECT related_ids FROM figures WHERE id = ?");
    query.addBindValue(id2);
    if (query.exec() && query.next()) {
        relatedIds2 = query.value(0).toString();
    } else {
        qWarning() << "Failed to fetch related IDs for figure" << id2 << ":" << query.lastError().text();
    }

    if (!relatedIds1.split(",").contains(QString::number(id2))) {
        relatedIds1 += (relatedIds1.isEmpty() ? "" : ",") + QString::number(id2);
    }

    if (!relatedIds2.split(",").contains(QString::number(id1))) {
        relatedIds2 += (relatedIds2.isEmpty() ? "" : ",") + QString::number(id1);
    }

    query.prepare("UPDATE figures SET related_ids = ? WHERE id = ?");
    query.addBindValue(relatedIds1);
    query.addBindValue(id1);
    if (!query.exec()) {
        qWarning() << "Failed to update related IDs for figure" << id1 << ":" << query.lastError().text();
    }

    query.prepare("UPDATE figures SET related_ids = ? WHERE id = ?");
    query.addBindValue(relatedIds2);
    query.addBindValue(id2);
    if (!query.exec()) {
        qWarning() << "Failed to update related IDs for figure" << id2 << ":" << query.lastError().text();
    }

    qDebug() << "Pair created between figures" << id1 << "and" << id2 << "and updated in the database.";
}

void CustomScene::deletePair(int id1, int id2) {
    if (id1 == id2) {
        qWarning() << "Cannot delete a pair with the same figure.";
        return;
    }

    QGraphicsItem *item1 = nullptr;
    QGraphicsItem *item2 = nullptr;


    for (QGraphicsItem *item : items()) {
        int itemId = item->data(0).toInt();
        if (itemId == id1) item1 = item;
        if (itemId == id2) item2 = item;
        if (item1 && item2) break;
    }

    if (!item1 || !item2) {
        qWarning() << "One or both items not found for IDs:" << id1 << id2;
        return;
    }


    auto it = lines.begin();
    while (it != lines.end()) {
        CustomLine *line = *it;
        if ((line->startItem() == item1 && line->endItem() == item2) ||
            (line->startItem() == item2 && line->endItem() == item1)) {
            line->removeFromScene();
            it = lines.erase(it);
        } else {
            ++it;
        }
    }


    QSqlQuery query;


    QString relatedIds1;
    query.prepare("SELECT related_ids FROM figures WHERE id = ?");
    query.addBindValue(id1);
    if (query.exec() && query.next()) {
        relatedIds1 = query.value(0).toString();
    }
    QStringList updatedRelatedIds1 = relatedIds1.split(",", QString::SkipEmptyParts);
    updatedRelatedIds1.removeAll(QString::number(id2));

    query.prepare("UPDATE figures SET related_ids = ? WHERE id = ?");
    query.addBindValue(updatedRelatedIds1.join(","));
    query.addBindValue(id1);
    if (!query.exec()) {
        qWarning() << "Failed to update related IDs for figure" << id1 << ":" << query.lastError().text();
    }


    QString relatedIds2;
    query.prepare("SELECT related_ids FROM figures WHERE id = ?");
    query.addBindValue(id2);
    if (query.exec() && query.next()) {
        relatedIds2 = query.value(0).toString();
    }
    QStringList updatedRelatedIds2 = relatedIds2.split(",", QString::SkipEmptyParts);
    updatedRelatedIds2.removeAll(QString::number(id1));

    query.prepare("UPDATE figures SET related_ids = ? WHERE id = ?");
    query.addBindValue(updatedRelatedIds2.join(","));
    query.addBindValue(id2);
    if (!query.exec()) {
        qWarning() << "Failed to update related IDs for figure" << id2 << ":" << query.lastError().text();
    }

    qDebug() << "Pair deleted between figures" << id1 << "and" << id2 << "and updated in the database.";
}

void CustomScene::deleteRelatedLines(int id) {
    auto it = lines.begin();
    while (it != lines.end()) {
        CustomLine *line = *it;
        if (line->startItem()->data(0).toInt() == id || line->endItem()->data(0).toInt() == id) {
            line->removeFromScene();
            it = lines.erase(it);
        } else {
            ++it;
        }
    }
}

void CustomScene::hideConnections(int itemId) {
    QGraphicsItem *selectedItem = nullptr;


    for (QGraphicsItem *item : items()) {
        if (item->data(0).toInt() == itemId) {
            selectedItem = item;
            break;
        }
    }

    if (!selectedItem) {
        qWarning() << "Item with ID" << itemId << "not found.";
        return;
    }


    selectedItem->setVisible(false);


    for (CustomLine *line : lines) {
        if (line->startItem() == selectedItem || line->endItem() == selectedItem) {
            line->setVisible(false);


            if (line->startItem() != selectedItem) {
                line->startItem()->setVisible(false);
            }
            if (line->endItem() != selectedItem) {
                line->endItem()->setVisible(false);
            }
        }
    }

    qDebug() << "Connections hidden for item with ID:" << itemId;
}
