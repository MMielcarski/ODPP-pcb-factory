#ifndef PCBBOARD_H
#define PCBBOARD_H

#include <QSize>
#include <QString>
#include <QColor>
#include <QRectF>
#include <QDebug>
#include <QRandomGenerator>

class PCBBoard
{
public:
    PCBBoard();
    PCBBoard(QString arg_id, QSize arg_size, uint arg_quantity);
    QString getId();
    QSize getSize();
    uint getQuantity();
    QRect getRect();
    QColor getColor();
    QPoint getTopRightCorner();
    void setColor(QColor arg_color);
    void setCoords(QPoint arg_coord);
    void D_printBoardInfo();

private:
    QString id;
    QSize size;
    uint quantity;
    QColor color;
    QRect rect;
    QPoint pos;

};

#endif // PCBBOARD_H
