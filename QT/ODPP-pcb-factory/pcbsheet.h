#ifndef PCBSHEET_H
#define PCBSHEET_H

#define PCB_SHEET_SIZE 450
#define TOP_LEFT_CORNER_OFFSET 0

#include <QList>

#include "pcbboard.h"

class PCBSheet
{
public:
    PCBSheet();
    PCBSheet(uint arg_id);
    bool addPcbBoardSimple(PCBBoard arg_board);
    QRect getRect();
    uint getPcbBoardListSize();
    QList <PCBBoard> getPcbboardList();
    QPoint getLastCorner();
    int getMaxHeight();
    int getMaxY();

private:
    uint id;
    QList<PCBBoard> board_list;
    QSize size;
    QRect mainRect;
};

#endif // PCBSHEET_H
