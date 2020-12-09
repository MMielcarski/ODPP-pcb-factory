#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pcbboard.h"

// ## CONSTRUCTORS
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initOrderTable();
    initBoardTable();
    initScene();
}

MainWindow::~MainWindow()
{
    delete ui;
}
// ## CONSTRUCTORS

// ## BUTTON TRIGGERS
void MainWindow::on_actionImport_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Import danych", "Import danych spowoduje usunięcie istniejących danych. Kontynuować?",
                                QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QMessageBox msgBox;
        QString filePath = QFileDialog::getOpenFileName(this, tr("Import danych wejściowych"));
        const QFileInfo fileDir(filePath);
        if(!fileDir.exists()) {
            msgBox.setText("Ten plik nie istnieje!");
            msgBox.exec();
        } else {
            clearData();
            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly)) {
                int lineindex = 0;
                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString fileLine = in.readLine();
                    QStringList lineToken = fileLine.split(";", QString::SkipEmptyParts);
                    QString board_id = lineToken.at(0);
                    QSize board_size = QSize(lineToken.at(1).toInt(), lineToken.at(2).toInt());
                    int board_quantity = lineToken.at(3).toInt();

                    PCBBoard pcbCSV(board_id, board_size, board_quantity);
                    addPcbOrderToList(pcbCSV);
                    addOrderTableRow(pcbCSV);
                    for(int j=0; j<lineToken.at(3).toInt(); j++){
                        PCBBoard pcbboard(board_id + "-" + QString::number(j), board_size, board_quantity);
                        pcbboard.setColor(pcbCSV.getColor());
                        addPcbBoardToList(pcbboard);
                        addBoardTableRow(pcbboard);
                    }
                    lineindex++;
                }
                file.close();
            }
            ui->tabWidget->setCurrentIndex(ORDERS_TABLE_TAB);
            msgBox.setText("Plik zaimportowany");
            msgBox.exec();
        }
    }
}

void MainWindow::on_actionAlgorytm_Prosty_triggered()
{
    optimizeSimple();
    if(pcbsheetsList.size() > 0) {
        displaySheet(currentSheet);
        updateSheetNavigation();
    }
    ui->tabWidget->setCurrentIndex(GRAPHICS_SCENE_TAB);
}

void MainWindow::on_actionAlgorytm_Best_Fit_triggered()
{
    optimiseBestFit();
    if(pcbsheetsList.size() > 0)
    {
        displaySheet(currentSheet);
        updateSheetNavigation();
    }
    ui->tabWidget->setCurrentIndex(GRAPHICS_SCENE_TAB);
}


void MainWindow::on_buttonPrevSheet_clicked()
{
    if(currentSheet > 0) {
        currentSheet--;
        updateSheetNavigation();
        displaySheet(currentSheet);
    }
}

void MainWindow::on_buttonNextSheet_clicked()
{
    if(currentSheet < getPcbsheetsCount()-1) {
        currentSheet++;
        updateSheetNavigation();
        displaySheet(currentSheet);
    }
}

void MainWindow::on_lineEditCurrentSheet_returnPressed()
{
    int usrInput = ui->lineEditCurrentSheet->text().toInt();
    if( usrInput != currentSheet+1 && usrInput > 0 && usrInput <= getPcbsheetsCount()) {
        currentSheet = usrInput - 1;
        updateSheetNavigation();
        displaySheet(currentSheet);
    }
}
// ## BUTTON TRIGGERS

// ## SOLVING ALGORITHMS

void MainWindow::optimizeSimple()
{
    int sheet_id = 0, pcbboards_left;
    pcbsheetsList.clear();
    if(pcbboardsList.size() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Brak wczytanych danych!");
        msgBox.exec();
    } else {
        pcbboards_left = pcbboardsList.size() - 1;
        while(pcbboards_left > 0) {
            pcbsheetsList.append(PCBSheet(sheet_id));
            while(pcbboards_left >= 0 && pcbsheetsList[sheet_id].addPcbBoardSimple(pcbboardsList[pcbboards_left])) {
                pcbboards_left--;
            }
            sheet_id++;
        }
    }
}

void MainWindow::optimiseBestFit()
{   
    int sheet_id = 0, pcbboards_left;
    pcbsheetsList.clear();
    if(pcbboardsList.size() == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Brak wczytanych danych!");
        msgBox.exec();
    }
    else //algorytm
    {
        pcbboards_left = pcbboardsList.size() - 1;
        while(pcbboards_left > 0)
        {
            pcbsheetsList.append(PCBSheet(sheet_id));
            QList<QRect> rectangleList;
            rectangleList.append(pcbsheetsList[sheet_id].getRect());

            while(pcbboards_left >= 0)
            {
                //Decide the free rectangle to pack the board into
                int minArea = 2000000000;
                int bestRect = -1;
                for (int i=0;i<rectangleList.size();i++)
                {
                    int area = rectangleList[i].width()*rectangleList[i].height(); //BEST AREA FIT
                    if ( ( (rectangleList[i].height()>=pcbboardsList[pcbboards_left].getSize().height() &&
                            rectangleList[i].width()>=pcbboardsList[pcbboards_left].getSize().width() ) ||
                            (rectangleList[i].height()>=pcbboardsList[pcbboards_left].getSize().width() &&
                             rectangleList[i].width()>=pcbboardsList[pcbboards_left].getSize().height() ) ) &&
                         area < minArea )
                    {
                        minArea = area;
                        bestRect = i;
                    }
                }
                //If no such rectangle is found restart with new sheet
                if (bestRect == -1) break;
                //Decide the orientation for the board
                if ( rectangleList[bestRect].height()<pcbboardsList[pcbboards_left].getSize().height() ||
                        rectangleList[bestRect].width()<pcbboardsList[pcbboards_left].getSize().width() )
                {
                    pcbboardsList[pcbboards_left].spin();
                }
                //  and place it at the top left of the rectangle
                pcbsheetsList[sheet_id].addPcbBoardBestFit(pcbboardsList[pcbboards_left],rectangleList[bestRect].x(),rectangleList[bestRect].y());
                //Use the guillotine split scheme to subdivide the rectangle
                int newW1,newW2,newH1,newH2;
                int newX1,newX2,newY1,newY2;
                if (pcbboardsList[pcbboards_left].getSize().height()<pcbboardsList[pcbboards_left].getSize().width()) //SHORTER AXIS SPLIT
                {
                    newW1 = pcbboardsList[pcbboards_left].getSize().width();
                    newH2 = rectangleList[bestRect].height();
                }
                else
                {
                    newW1 = rectangleList[bestRect].width();
                    newH2 = pcbboardsList[pcbboards_left].getSize().height();
                }
                newX1 = rectangleList[bestRect].x();
                newY1 = rectangleList[bestRect].y() + pcbboardsList[pcbboards_left].getSize().height();
                newH1 = rectangleList[bestRect].height() - pcbboardsList[pcbboards_left].getSize().height();
                newX2 = rectangleList[bestRect].x() + pcbboardsList[pcbboards_left].getSize().width();
                newY2 = rectangleList[bestRect].y();
                newW2 = rectangleList[bestRect].width() - pcbboardsList[pcbboards_left].getSize().width();
                //Remove used rectangle and set the new ones
                rectangleList.removeAt(bestRect);
                if (newW1 > 0 && newH1 > 0) rectangleList.append(QRect(newX1,newY1,newW1,newH1));
                if (newW2 > 0 && newH2 > 0) rectangleList.append(QRect(newX2,newY2,newW2,newH2));

                pcbboards_left--;
            }
            sheet_id++;
        }
    }
}
// ## SOLVING ALGORITHMS


// ## GRAPHICS SCENE HANDLING
void MainWindow::initScene()
{
    sheetScene = new QGraphicsScene();
    currentSheet = 0;
}

void MainWindow::displaySheet(int sheet_id)
{
    sheetScene->clear();
    QBrush sheetBgBrush(Qt::gray);
    QPen outlinePen(Qt::black);
    outlinePen.setWidth(1);
    ui->graphicsView->setScene(sheetScene);
    sheetScene->addRect(pcbsheetsList[sheet_id].getRect(), outlinePen, sheetBgBrush);

    QList <PCBBoard> tmp_list = pcbsheetsList[sheet_id].getPcbboardList();
    for(uint i=0; i<pcbsheetsList[sheet_id].getPcbBoardListSize(); i++) {
        sheetScene->addRect(tmp_list[i].getRect(), outlinePen, tmp_list[i].getColor());
    }
}

void MainWindow::updateSheetNavigation()
{
    ui->lineEditMaxSheet->setText(QString::number(getPcbsheetsCount()));
    ui->lineEditCurrentSheet->setText(QString::number(currentSheet+1));
}
// ## GRAPHICS SCENE HANDLING

// ## TABLES HANDLING
void MainWindow::initOrderTable()
{
    ui->tableWidgetOrders->setColumnCount(5);
    QStringList m_TableHeader;
    m_TableHeader<<"Model płytki"<<"Szerokość"<<"Wysokość"<<"Ilość"<<"Kolor";
    ui->tableWidgetOrders->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidgetOrders->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::addOrderTableRow(PCBBoard board)
{
    int new_row = ui->tableWidgetOrders->rowCount();
    ui->tableWidgetOrders->setRowCount(new_row + 1);
    ui->tableWidgetOrders->setItem(new_row, 0, new QTableWidgetItem(board.getId()));
    ui->tableWidgetOrders->setItem(new_row, 1, new QTableWidgetItem(QString::number(board.getSize().width())));
    ui->tableWidgetOrders->setItem(new_row, 2, new QTableWidgetItem(QString::number(board.getSize().height())));
    ui->tableWidgetOrders->setItem(new_row, 3, new QTableWidgetItem(QString::number(board.getQuantity())));
    ui->tableWidgetOrders->setItem(new_row, 4, new QTableWidgetItem);
    ui->tableWidgetOrders->item(new_row, 4)->setBackground(board.getColor());
}

void MainWindow::initBoardTable()
{
    ui->tableWidgetBoards->setColumnCount(4);
    QStringList m_TableHeader;
    m_TableHeader<<"ID płytki"<<"Szerokość"<<"Wysokość"<<"Kolor";
    ui->tableWidgetBoards->setHorizontalHeaderLabels(m_TableHeader);
    ui->tableWidgetBoards->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::addBoardTableRow(PCBBoard board)
{
    int new_row = ui->tableWidgetBoards->rowCount() + 1;
    ui->tableWidgetBoards->setRowCount(new_row);
    ui->tableWidgetBoards->setItem(new_row-1, 0, new QTableWidgetItem(board.getId()));
    ui->tableWidgetBoards->setItem(new_row-1, 1, new QTableWidgetItem(QString::number(board.getSize().width())));
    ui->tableWidgetBoards->setItem(new_row-1, 2, new QTableWidgetItem(QString::number(board.getSize().height())));
    ui->tableWidgetBoards->setItem(new_row-1, 3, new QTableWidgetItem);
    ui->tableWidgetBoards->item(new_row-1, 3)->setBackground(board.getColor());
}
// ## TABLES HANDLING

// ## DATA HANDLING
void MainWindow::clearData()
{
    pcbsheetsList.clear();
    pcbsheetsList.clear();
    pcbboardOrdersList.clear();
    sheetScene->clear();
    ui->tableWidgetOrders->clear();
    ui->tableWidgetBoards->clear();
}

void MainWindow::addPcbOrderToList(PCBBoard arg_pcb)
{
    pcbboardOrdersList.append(arg_pcb);
}

void MainWindow::addPcbBoardToList(PCBBoard arg_pcb)
{
    pcbboardsList.append(arg_pcb);
}

int MainWindow::getPcbsheetsCount()
{
    return pcbsheetsList.size();
}
// ## DATA HANDLING

