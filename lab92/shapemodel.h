#ifndef SHAPEMODEL_H
#define SHAPEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QString>

struct Shape {
    QString type;
    int id;
};

class ShapeModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ShapeModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addShape(const Shape &shape);
    void removeShape(int id);

private:
    QList<Shape> shapes;
};

#endif // SHAPEMODEL_H
