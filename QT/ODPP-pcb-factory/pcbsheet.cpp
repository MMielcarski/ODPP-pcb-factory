#include "pcbsheet.h"

PCBSheet::PCBSheet()
{

}

PCBSheet::PCBSheet(uint arg_id)
{
    id = arg_id;
    size.setWidth(PCB_SHEET_SIZE);
    size.setHeight(PCB_SHEET_SIZE);
    mainRect.setSize(size);
}

bool PCBSheet::addPcbBoardSimple(PCBBoard arg_board)
{
    int new_w = arg_board.getSize().width();
    int new_h = arg_board.getSize().height();
    QPoint last_corner = getLastCorner();

    if((new_w < (PCB_SHEET_SIZE - last_corner.x()))) {
        if((new_h < (PCB_SHEET_SIZE - last_corner.y()))) {
            arg_board.setCoords(last_corner + QPoint(1,0));
        } else {
            return false;
        }
    } else {
        int max_h = getMaxY();
        if(new_h < PCB_SHEET_SIZE - max_h) {
            arg_board.setCoords(QPoint(TOP_LEFT_CORNER_OFFSET, max_h) + QPoint(1,0));
        } else {
            return false;
        }
    }
    board_list.append(arg_board);
    return true;
}

QPoint PCBSheet::getLastCorner()
{
    if(getPcbBoardListSize() == 0) {
        return QPoint(TOP_LEFT_CORNER_OFFSET,TOP_LEFT_CORNER_OFFSET);
    } else {
        return QPoint(board_list.last().getTopRightCorner());
    }
}

QRect PCBSheet::getRect()
{
    return mainRect;
}

int PCBSheet::getMaxHeight()
{
    int max_h = 0;
    for(int i=0; i<getPcbBoardListSize(); i++) {
        if(board_list[i].getSize().height() > max_h) {
            max_h = board_list[i].getSize().height();
        }
    }
    return max_h;
}

int PCBSheet::getMaxY()
{
    int max_y = 0;
    for(int i=0; i<getPcbBoardListSize(); i++) {
        if(board_list[i].getRect().bottom() > max_y) {
            max_y = board_list[i].getRect().bottom();
        }
    }
    return max_y;
}



uint PCBSheet::getPcbBoardListSize()
{
    return board_list.size();
}

QList <PCBBoard> PCBSheet::getPcbboardList()
{
    return board_list;
}
