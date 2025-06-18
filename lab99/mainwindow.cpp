#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "icondelegate.h"
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QDebug>
#include <QtMath>
#include <QFile>
#include <QComboBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    initializeDatabase();


    model = new QSqlTableModel(this);
    model->setTable("figures");
    model->select();
    ui->tableView->setModel(model);


    scene = new CustomScene(this);
    ui->graphicsView->setScene(scene);


    setupConnections();



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeDatabase()
{

    QFile dbFile("figures.db");
    if (dbFile.exists()) {
        dbFile.remove();
    }


    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("figures.db");

    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Failed to open the database: " + db.lastError().text());
        return;
    }


    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS figures ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "type TEXT, "
               "related_ids TEXT, type_count INTEGER)");
}

void MainWindow::updateDelegate()
{

    ui->tableView->setItemDelegateForColumn(3, new IconDelegate(this));
}

void MainWindow::setupConnections() {
    connect(ui->addPolygonButton, &QPushButton::clicked, this, &MainWindow::addPolygon);
    connect(ui->addEllipseButton, &QPushButton::clicked, this, &MainWindow::addEllipse);
    connect(ui->addRectangleButton, &QPushButton::clicked, this, &MainWindow::addRectangle);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedItem);
    connect(scene, &CustomScene::itemSelected, this, &MainWindow::onSceneItemSelected);
    connect(ui->filterButton, &QPushButton::clicked, this, &MainWindow::onFilterButtonClicked);
    connect(ui->deletePairButton, &QPushButton::clicked, this, &MainWindow::deletePair);
    connect(ui->hideConnectionsButton, &QPushButton::clicked, this, &MainWindow::hideConnections);

    connect(ui->createPairButton, &QPushButton::clicked, this, [this]() {
        bool ok1, ok2;
        int id1 = QInputDialog::getInt(this, "Create Pair", "Enter ID of the first figure:", 0, 0, 100000, 1, &ok1);
        int id2 = QInputDialog::getInt(this, "Create Pair", "Enter ID of the second figure:", 0, 0, 100000, 1, &ok2);

        if (!ok1 || !ok2) {
            QMessageBox::warning(this, "Error", "Invalid IDs entered.");
            return;
        }

        if (id1 == id2) {
            QMessageBox::warning(this, "Error", "Cannot create a pair between the same figure.");
            return;
        }



        scene->createPair(id1, id2);
        model->select();
    });
}

int MainWindow::getNextAvailableId()
{
    QSqlQuery query("SELECT MAX(id) FROM figures");
    if (query.exec()) {
        if (query.next()) {
            int maxId = query.value(0).toInt();
            return maxId + 1;
        }
    }
    return 1;
}


int MainWindow::countFiguresByType(const QString& type)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM figures WHERE type = ?");
    query.addBindValue(type);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to count figures: " + query.lastError().text());
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt() + 1;
    }

    return 1;
}

void MainWindow::addPolygon()
{
    bool ok;
    int sides = ui->polygonSidesEdit->text().toInt(&ok);

    if (!ok || sides < 3) {
        QMessageBox::warning(this, "Error", "Enter a valid number of sides (3 or more) for the polygon.");
        return;
    }

    double radius = QInputDialog::getDouble(this, "Enter Radius", "Radius:", 50.0, 1.0, 1000.0, 1, &ok);
    if (!ok || radius <= 0) {
        QMessageBox::warning(this, "Error", "Enter a valid radius greater than 0.");
        return;
    }

    QPolygonF polygon;
    for (int i = 0; i < sides; ++i) {
        qreal angle = (2 * M_PI * i) / sides;
        qreal x = radius * qCos(angle);
        qreal y = radius * qSin(angle);
        polygon << QPointF(x, y);
    }

    QGraphicsPolygonItem *polygonItem = scene->addPolygon(polygon, QPen(Qt::black), QBrush(Qt::blue));


    int itemId = getNextAvailableId();
    polygonItem->setData(0, itemId);
    polygonItem->setData(1, "polygon");
    int count = countFiguresByType("polygon");


    QSqlQuery query;
    query.prepare("INSERT INTO figures (id, type, related_ids, type_count) VALUES (?, ?, ?, ?)");
    query.addBindValue(itemId);
    query.addBindValue("polygon");
    query.addBindValue("");
    query.addBindValue(count);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to add polygon: " + query.lastError().text());
    } else {
        updateTypeCount("polygon");
        updateDelegate();
        model->select();
    }
}

void MainWindow::addEllipse()
{
    bool ok;


    int width = QInputDialog::getInt(this, "Enter Width", "Width:", 100, 1, 1000, 1, &ok);
    if (!ok || width <= 0) {
        QMessageBox::warning(this, "Error", "Enter a valid width greater than 0.");
        return;
    }


    int height = QInputDialog::getInt(this, "Enter Height", "Height:", 50, 1, 1000, 1, &ok);
    if (!ok || height <= 0) {
        QMessageBox::warning(this, "Error", "Enter a valid height greater than 0.");
        return;
    }

    QGraphicsEllipseItem *ellipseItem = scene->addEllipse(-width/2, -height/2, width, height, QPen(Qt::black), QBrush(Qt::green));

    int itemId = getNextAvailableId();
    ellipseItem->setData(0, itemId);
    ellipseItem->setData(1, "ellipse");
    int count = countFiguresByType("ellipse");

    QSqlQuery query;
    query.prepare("INSERT INTO figures (id, type, related_ids, type_count) VALUES (?, ?, ?, ?)");
    query.addBindValue(itemId);
    query.addBindValue("ellipse");
    query.addBindValue("");
    query.addBindValue(count);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to add ellipse: " + query.lastError().text());
    } else {
        updateTypeCount("ellipse");
        updateDelegate();
        model->select();
    }
}

void MainWindow::addRectangle()
{
    bool ok;


    int width = QInputDialog::getInt(this, "Enter Width", "Width:", 100, 1, 1000, 1, &ok);
    if (!ok || width <= 0) {
        QMessageBox::warning(this, "Error", "Enter a valid width greater than 0.");
        return;
    }


    int height = QInputDialog::getInt(this, "Enter Height", "Height:", 50, 1, 1000, 1, &ok);
    if (!ok || height <= 0) {
        QMessageBox::warning(this, "Error", "Enter a valid height greater than 0.");
        return;
    }

    QGraphicsRectItem *rectItem = scene->addRect(-width, -height, width, height, QPen(Qt::black), QBrush(Qt::red));
    int itemId = getNextAvailableId();
    rectItem->setData(0, itemId);
    rectItem->setData(1, "rectangle");
    int count = countFiguresByType("rectangle");


    QSqlQuery query;
    query.prepare("INSERT INTO figures (id, type, related_ids, type_count) VALUES (?, ?, ?, ?)");
    query.addBindValue(itemId);
    query.addBindValue("rectangle");
    query.addBindValue("");
    query.addBindValue(count);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error", "Failed to add rectangle: " + query.lastError().text());
    } else {
        updateTypeCount("rectangle");
        updateDelegate();
        model->select();
    }
}

void MainWindow::updateTypeCountDel(const QString &type)
{

    QSqlQuery query;
    query.prepare("SELECT id FROM figures WHERE type = ?");
    query.addBindValue(type);

    if (query.exec()) {
        int count = 0;
        while (query.next()) {
            ++count;
        }


        query.prepare("UPDATE figures SET type_count = ? WHERE type = ?");
        query.addBindValue(count - 1);
        query.addBindValue(type);

        if (!query.exec()) {
            QMessageBox::critical(this, "Error", "Failed to update type_count: " + query.lastError().text());
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to fetch figures by type: " + query.lastError().text());
    }
}

void MainWindow::updateTypeCount(const QString &type)
{

    QSqlQuery query;
    query.prepare("SELECT id FROM figures WHERE type = ?");
    query.addBindValue(type);

    if (query.exec()) {
        int count = 0;
        while (query.next()) {
            ++count;
        }


        query.prepare("UPDATE figures SET type_count = ? WHERE type = ?");
        query.addBindValue(count);
        query.addBindValue(type);

        if (!query.exec()) {
            QMessageBox::critical(this, "Error", "Failed to update type_count: " + query.lastError().text());
        }
    } else {
        QMessageBox::critical(this, "Error", "Failed to fetch figures by type: " + query.lastError().text());
    }
}

void MainWindow::deleteSelectedItem() {
    int id = -1;


    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (!selectedIndexes.isEmpty()) {
        id = selectedIndexes.first().data().toInt();
    } else if (selectedSceneItemId != -1) {
        id = selectedSceneItemId;
    }

    if (id == -1) {
        QMessageBox::warning(this, "Warning", "No item selected for deletion.");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT related_ids FROM figures WHERE id = ?");
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        QStringList relatedIds = query.value(0).toString().split(",", QString::SkipEmptyParts);

        for (const QString &relatedId : relatedIds) {
            query.prepare("SELECT related_ids FROM figures WHERE id = ?");
            query.addBindValue(relatedId.toInt());
            if (query.exec() && query.next()) {
                QStringList idsToUpdate = query.value(0).toString().split(",", QString::SkipEmptyParts);
                idsToUpdate.removeAll(QString::number(id));

                query.prepare("UPDATE figures SET related_ids = ? WHERE id = ?");
                query.addBindValue(idsToUpdate.join(","));
                query.addBindValue(relatedId.toInt());
                query.exec();
            }
        }
    }

    query.prepare("SELECT type FROM figures WHERE id = ?");
    query.addBindValue(id);
    if (query.exec() && query.next()) {
        QString type = query.value(0).toString();
        updateTypeCountDel(type);
        updateDelegate();
    }


    query.prepare("DELETE FROM figures WHERE id = ?");
    query.addBindValue(id);


    scene->deleteRelatedLines(id);

    if (query.exec()) {

        for (QGraphicsItem *item : scene->items()) {
            if (item->data(0).toInt() == id) {
                scene->removeItem(item);
                delete item;
                break;
            }
        }

        model->select();
        selectedSceneItemId = -1;
    } else {
        QMessageBox::critical(this, "Error", "Failed to delete item: " + query.lastError().text());
    }
}

void MainWindow::onFilterButtonClicked()
{

    QString selectedType = ui->filterComboBox->currentText();

    if (selectedType == "Все") {

        model->setFilter("");
        model->select();
        filterSceneItems("");
    } else {
        QString sqlType;
        if (selectedType == "Полигон") {
            sqlType = "polygon";
        } else if (selectedType == "Эллипс") {
            sqlType = "ellipse";
        } else if (selectedType == "Прямоугольник") {
            sqlType = "rectangle";
        }


        model->setFilter(QString("type = '%1'").arg(sqlType));
        model->select();


        filterSceneItems(sqlType);
    }
}

void MainWindow::filterSceneItems(const QString &typeFilter)
{
    for (QGraphicsItem *item : scene->items()) {
        QString type = item->data(1).toString();

        if (typeFilter.isEmpty() || type == typeFilter) {
            item->setVisible(true);
        } else {
            item->setVisible(false);
        }
    }
}

void MainWindow::onSceneItemSelected(int itemId)
{

    for (int row = 0; row < model->rowCount(); ++row) {
        QModelIndex index = model->index(row, 0);
        if (index.data().toInt() == itemId) {
            ui->tableView->selectRow(row);
            selectedSceneItemId = itemId;
            return;
        }
    }
}

void MainWindow::deletePair() {
    bool ok1, ok2;


    int id1 = QInputDialog::getInt(this, "Delete Pair", "Enter the ID of the first figure:", 0, 0, 10000, 1, &ok1);
    if (!ok1) {
        QMessageBox::warning(this, "Cancelled", "Operation cancelled.");
        return;
    }


    int id2 = QInputDialog::getInt(this, "Delete Pair", "Enter the ID of the second figure:", 0, 0, 10000, 1, &ok2);
    if (!ok2) {
        QMessageBox::warning(this, "Cancelled", "Operation cancelled.");
        return;
    }


    scene->deletePair(id1, id2);


    model->select();
}

void MainWindow::hideConnections() {
    QGraphicsItem *item = scene->getSelectedItem();
    if (!item) {
        qDebug() << "No item selected!";
        return;
    }

    int selectedId = item->data(0).toInt();

    scene->hideConnections(selectedId);

    qDebug() << "Connections hidden for figure ID:" << selectedId;
}


