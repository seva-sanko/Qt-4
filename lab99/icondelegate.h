#ifndef ICONDELEGATE_H
#define ICONDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class IconDelegate : public QStyledItemDelegate {
public:
    explicit IconDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {

        if (index.column() == 3) {

            int count = index.data(Qt::DisplayRole).toInt();


            QModelIndex typeIndex = index.model()->index(index.row(), 1);
            QString shapeType = typeIndex.data(Qt::DisplayRole).toString();

            painter->save();


            int iconCount = 1;
            if (count >= 4 && count <= 10) {
                iconCount = 2;
            } else if (count > 10) {
                iconCount = 3;
            }


            int iconSize = option.rect.height() * 0.7;
            int spacing = 5;


            int x = option.rect.x() + spacing;
            int y = option.rect.y() + (option.rect.height() - iconSize) / 2;


            for (int i = 0; i < iconCount; ++i) {
                QRect iconRect(x, y, iconSize, iconSize);


                if (shapeType == "rectangle") {
                    painter->setBrush(Qt::red);
                    painter->drawRect(iconRect);
                } else if (shapeType == "ellipse") {
                    painter->setBrush(Qt::green);
                    painter->drawEllipse(iconRect);
                } else if (shapeType == "polygon") {
                    painter->setBrush(Qt::blue);

                    QPointF point1(x + iconSize / 2, y);
                    QPointF point2(x, y + iconSize);
                    QPointF point3(x + iconSize, y + iconSize);

                    QPolygonF triangle;
                    triangle << point1 << point2 << point3;

                    painter->drawPolygon(triangle);
                }


                x += iconSize + spacing;
            }

            painter->restore();
        } else {

            QStyledItemDelegate::paint(painter, option, index);
        }
    }
};

#endif
