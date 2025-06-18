#include "MainWindow.h"
#include "Scene.h"
#include "ShapeModel.h"
#include <QSplitter>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), scene(new Scene(this)), model(new ShapeModel(this)) {

    view = new QGraphicsView(scene, this);
    tableView = new QTableView(this);
    tableView->setModel(model);

    addRectButton = new QPushButton("Добавить прямоугольник", this);
    addEllipseButton = new QPushButton("Добавить эллипс", this);
    addPolygonButton = new QPushButton("Добавить многоугольник", this);
    addConnectionButton = new QPushButton("Создать связь", this);
    deleteButton = new QPushButton("Удалить выбранное", this);
    filterButton = new QPushButton("Фильтровать", this);

    filterValueLineEdit = new QLineEdit(this);
    filterTypeComboBox = new QComboBox(this);
    filterTypeComboBox->addItem("Тип", "type");
    filterTypeComboBox->addItem("ID", "id");

    polygonSidesLineEdit = new QLineEdit(this);

    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    splitter->addWidget(tableView);
    splitter->addWidget(view);

    tableView->setMinimumWidth(200);
    view->setMinimumWidth(600);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(splitter);

    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Фильтровать:", this));
    filterLayout->addWidget(filterTypeComboBox);
    filterLayout->addWidget(filterValueLineEdit);
    filterLayout->addWidget(filterButton);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Количество сторон многоугольника:", polygonSidesLineEdit);

    layout->addLayout(filterLayout);
    layout->addLayout(formLayout);
    layout->addWidget(addRectButton);
    layout->addWidget(addEllipseButton);
    layout->addWidget(addPolygonButton);
    layout->addWidget(addConnectionButton);
    layout->addWidget(deleteButton);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    connect(addRectButton, &QPushButton::clicked, this, &MainWindow::addRectangle);
    connect(addEllipseButton, &QPushButton::clicked, this, &MainWindow::addEllipse);
    connect(addPolygonButton, &QPushButton::clicked, this, &MainWindow::addPolygon);
    connect(addConnectionButton, &QPushButton::clicked, scene, &Scene::startConnectionMode);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelected);
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::filterShapes);
}

void MainWindow::addRectangle() {
    scene->addRectangle();
}

void MainWindow::addEllipse() {
    scene->addEllipse();
}

void MainWindow::addPolygon() {
    bool ok;
    int sides = polygonSidesLineEdit->text().toInt(&ok);
    if (ok && sides >= 3) {
        scene->addPolygon(sides);
    }
}

void MainWindow::addConnection() {
    auto selectedItems = scene->selectedItems();

    if (selectedItems.size() != 2) {
        //QMessageBox::warning(this, "Ошибка", "Выберите ровно два примитива для создания связи.");
        return;
    }

    auto *item1 = dynamic_cast<CustomGraphicsItem *>(selectedItems[0]);
    auto *item2 = dynamic_cast<CustomGraphicsItem *>(selectedItems[1]);

    if (!item1 || !item2) {
        //QMessageBox::warning(this, "Ошибка", "Выбранные элементы не поддерживают создание связи.");
        return;
    }

    scene->addConnection(item1, item2);
}

void MainWindow::deleteSelected() {
    scene->deleteSelected();
}

void MainWindow::filterShapes() {
    QString filterType = filterTypeComboBox->currentData().toString();
    QString filterValue = filterValueLineEdit->text();
    scene->filterShapes(filterType, filterValue);
}
