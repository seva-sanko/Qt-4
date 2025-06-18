#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTableView>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Scene.h"
#include "ShapeModel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void addRectangle();
    void addEllipse();
    void addPolygon();
    void addConnection();
    void deleteSelected();
    void filterShapes();

private:
    Scene *scene;
    QGraphicsView *view;
    QTableView *tableView;
    ShapeModel *model;
    QPushButton *addRectButton;
    QPushButton *addEllipseButton;
    QPushButton *addPolygonButton;
    QPushButton *addConnectionButton;
    QPushButton *deleteButton;
    QPushButton *filterButton;
    QLineEdit *filterValueLineEdit;
    QComboBox *filterTypeComboBox;
    QLineEdit *polygonSidesLineEdit;
};

#endif // MAINWINDOW_H
