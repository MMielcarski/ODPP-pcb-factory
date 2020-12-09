#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QRandomGenerator>

#include <pcbboard.h>
#include "pcbsheet.h"

#define ORDERS_TABLE_TAB 0
#define GRAPHICS_SCENE_TAB 2

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionImport_triggered();
    void on_actionAlgorytm_Prosty_triggered();
    void on_actionAlgorytm_Best_Fit_triggered();
    void on_buttonPrevSheet_clicked();
    void on_buttonNextSheet_clicked();
    void on_lineEditCurrentSheet_returnPressed();

private:
    Ui::MainWindow *ui;

    // MAIN
    void clearData();

    // PCBBoards
    QList <PCBBoard> pcbboardOrdersList;
    QList <PCBBoard> pcbboardsList;
    void addPcbOrderToList(PCBBoard arg_pcb);
    void addPcbBoardToList(PCBBoard arg_pcb);

    //PCBSheets
    QList <PCBSheet> pcbsheetsList;
    void optimizeSimple();
    void optimiseBestFit();
    int getPcbsheetsCount();

    // ORDER TABLE
    void initOrderTable();
    void addOrderTableRow(PCBBoard board);

    // BOARDS TABLE
    void initBoardTable();
    void addBoardTableRow(PCBBoard board);

    // Graphics Scene
    QGraphicsScene* sheetScene;
    QGraphicsRectItem* currentSheetPtr;
    int currentSheet;
    void initScene();
    void displaySheet(int sheet_id);
    void updateSheetNavigation();
};
#endif // MAINWINDOW_H
