#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include <QGraphicsItem>

class CustomLine : public QGraphicsLineItem {
public:
    CustomLine(QGraphicsItem *startItem, QGraphicsItem *endItem, QGraphicsScene *scene)
        : QGraphicsLineItem(nullptr), m_startItem(startItem), m_endItem(endItem), m_scene(scene) {
        updateLine();
        scene->addItem(this);
    }

    void updateLine() {
        if (m_startItem && m_endItem) {
            QPointF startCenter = m_startItem->sceneBoundingRect().center();
            QPointF endCenter = m_endItem->sceneBoundingRect().center();
            setLine(QLineF(startCenter, endCenter));
        }
    }

    QGraphicsItem* startItem() const { return m_startItem; }
    QGraphicsItem* endItem() const { return m_endItem; }

    void removeFromScene() {
        if (m_scene) {
            m_scene->removeItem(this);
            delete this;
        }
    }

private:
    QGraphicsItem *m_startItem;
    QGraphicsItem *m_endItem;
    QGraphicsScene *m_scene;
};

class CustomScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit CustomScene(QObject *parent = nullptr);

signals:
    void itemSelected(int id);
    void itemMoved(int id, const QPointF &newPos);

public slots:
    void createPair(int id1, int id2);
    void deletePair(int id1, int id2);
    void deleteRelatedLines(int id);


    void hideConnections(int id);
    QGraphicsItem* getSelectedItem() const {
        return selectedItem;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QGraphicsItem *selectedItem = nullptr;
    int selectedItemId = -1;
    QList<CustomLine*> lines;
    qreal maxZValue;
};

#endif
