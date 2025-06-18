#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSqlDatabase>
#include <QPushButton>
#include <QTableView>
#include "customscene.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addPolygon();
    void addEllipse();
    void addRectangle();
    void deleteSelectedItem();
    void onFilterButtonClicked();
    void filterSceneItems(const QString &typeFilter);
    void updateTypeCount(const QString &type);
    void updateTypeCountDel(const QString &type);
    void deletePair();
    void hideConnections();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
    CustomScene *scene;
    int selectedSceneItemId = -1;
    QGraphicsItem* findItemById(int itemId);

    void initializeDatabase();
    void updateDelegate();
    void setupConnections();
    void onSceneItemSelected(int itemId);
    int getNextAvailableId();
    int countFiguresByType(const QString& type);
};

#endif // MAINWINDOW_H
