#include <QStylePainter>
#include <QStyleOptionFocusRect>

#include "speedGraphWidget.h"

SpeedGraphWidget::SpeedGraphWidget(QWidget * parent, Qt::WFlags f)
{
    maxSpeed = 250*1024;
    speed = 0;
    maxHeight = 60;
    pointWidth = 2;
    setFixedHeight(maxHeight);

    setBackgroundRole(QPalette::Shadow);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
void SpeedGraphWidget::setData( qint64 maxSpeed, int maxHeight, int pointWidth )
{
    this->maxSpeed = maxSpeed;
    this->maxHeight = maxHeight;
    this->pointWidth = pointWidth;

    setFixedHeight(maxHeight);
}
void SpeedGraphWidget::addSpeedPoint(qint64 speedPoint)
{
    speedPointList<<speedPoint;
    while ((speedPointList.size()-2)*pointWidth > maxWidth)
        speedPointList.removeAt(0);
    refreshPixmap();
}
void SpeedGraphWidget::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}
void SpeedGraphWidget::refreshPixmap()
{
    pixmap = QPixmap(size());
    pixmap.fill(this, 0, 0);

    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawCurves(&painter);
    update();
}
void SpeedGraphWidget::resizeEvent(QResizeEvent *event)
{
    QRect TaskGraphRect = geometry();
    maxWidth = TaskGraphRect.width();
    refreshPixmap();
}
void SpeedGraphWidget::drawGrid(QPainter *painter)
{
    QRect TaskGraphRect = geometry();
    maxWidth = TaskGraphRect.width();
    QBrush brush(QColor(0, 0, 0));
    painter->fillRect(0, 0, maxWidth, maxHeight, brush);
    QPen pen(Qt::white);
    pen.setWidth(1);
    painter->setPen(pen);

    painter->drawLine(0, maxHeight/3, maxWidth, maxHeight/3);
    painter->drawLine(0, maxHeight/3*2, maxWidth, maxHeight/3*2);
}
void SpeedGraphWidget::drawCurves(QPainter *painter)
{
    if (speedPointList.size()<2) return;

    QPen pen(Qt::white);
    pen.setWidth(3);
    painter->setPen(pen);

    QPainterPath path;
    int point = 0;
    int pointx = maxWidth-point*pointWidth;
    int pointy = maxHeight-maxHeight*(speedPointList.at(speedPointList.size()-1))/maxSpeed;
    path.moveTo(maxWidth, maxHeight);
    path.lineTo(pointx, pointy);
    for (int speedPointListID=speedPointList.size()-2;speedPointListID>=0;speedPointListID--)
    {
        point = speedPointList.size() - 1 - speedPointListID;
        pointx = maxWidth-point*pointWidth;
        pointy = maxHeight-maxHeight*(speedPointList.at(speedPointListID))/maxSpeed;
        path.lineTo(pointx , pointy);

        //		qDebug(qPrintable(QString("X:%1,y:%2").arg(pointx).arg(pointy)));
    }
    path.lineTo(pointx , maxHeight);
    path.lineTo(maxWidth , maxHeight);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPath(path);
    QBrush background;
    background = QBrush(QColor(150, 150, 150, 230));
    painter->fillPath(path, background);
}
