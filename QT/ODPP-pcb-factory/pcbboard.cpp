#include "pcbboard.h"

PCBBoard::PCBBoard()
{

}

PCBBoard::PCBBoard(QString arg_id, QSize arg_size, uint arg_quantity)
{
    id = arg_id;
    size = arg_size;
    quantity = arg_quantity;

    int color_r = QRandomGenerator::global()->bounded(0,255);
    int color_g = QRandomGenerator::global()->bounded(0,255);
    int color_b = QRandomGenerator::global()->bounded(0,255);
    color = QColor(color_r,color_g,color_b);
    rect.setSize(arg_size);
}

void PCBBoard::setCoords(QPoint arg_coord)
{
    rect.moveTopLeft(arg_coord);
}

QPoint PCBBoard::getTopRightCorner()
{
    return rect.topRight();
}

QString PCBBoard::getId()
{
    return id;
}

QSize PCBBoard::getSize()
{
    return size;
}

uint PCBBoard::getQuantity()
{
    return quantity;
}

QRect PCBBoard::getRect()
{
    return rect;
}

QColor PCBBoard::getColor()
{
    return color;
}

void PCBBoard::setColor(QColor arg_color)
{
    color = arg_color;
}

void PCBBoard::D_printBoardInfo()
{
    qDebug() <<"Id: "<<id<<" Size: "<<size<<" Quantity: "<<quantity;
}

void PCBBoard::spin()
{
    int w = rect.width();
    int h = rect.height();
    rect.setSize(QSize(h,w));
    size = rect.size();
}
