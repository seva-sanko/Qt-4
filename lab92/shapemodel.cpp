#include "shapemodel.h"

ShapeModel::ShapeModel(QObject *parent) : QAbstractTableModel(parent) {}

int ShapeModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return shapes.size();
}

int ShapeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 2;
}

QVariant ShapeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) return QVariant();

    const Shape &shape = shapes[index.row()];
    if (index.column() == 0) return shape.type;
    if (index.column() == 1) return shape.id;

    return QVariant();
}

void ShapeModel::addShape(const Shape &shape) {
    beginInsertRows(QModelIndex(), shapes.size(), shapes.size());
    shapes.append(shape);
    endInsertRows();
}

void ShapeModel::removeShape(int id) {
    for (int i = 0; i < shapes.size(); ++i) {
        if (shapes[i].id == id) {
            beginRemoveRows(QModelIndex(), i, i);
            shapes.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}
