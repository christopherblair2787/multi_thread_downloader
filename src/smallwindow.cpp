#include <QMimeData>
#include <QUrl>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include <QDebug>
#include "smallwindow.h"

SmallWindow::SmallWindow(QWidget *parent, Qt::WFlags f)
{
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Tool);
    setFixedSize(50,50);
    //	setBackgroundRole(QPalette::Shadow);
    setAcceptDrops(true);

    maxSpeed = 250*1024;
    speed = 0;
    maxHeight = 50;
    pointWidth = 2;
}
void SmallWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        last = event->globalPos();
        pos0 = event->globalPos() - event->pos();
    }
}
void SmallWindow::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons()!=Qt::LeftButton)) return;

    QPoint newPos = event->globalPos();
    QPoint upLeft = pos0 + newPos - last;
    move(upLeft);
}
void SmallWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}
void SmallWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        QString text;
        if (urlList.size()>0)
        {
            QString urlString = urlList.at(0).toString();

            emit addNewTask(urlString);
            qDebug()<<qPrintable(QString("urlString:%1").arg(urlString));
        }
    }
    event->acceptProposedAction();
}
void SmallWindow::setData( qint64 maxSpeed, int pointWidth )
{
    this->maxSpeed = maxSpeed;
    this->pointWidth = pointWidth;
}
void SmallWindow::setString(QString showString)
{
    this->showString = showString;
}
void SmallWindow::setSpeedPoint(QList <qint64> speedPointList)
{
    this->speedPointList = speedPointList;
    while ((speedPointList.size()-2)*pointWidth > maxWidth)
        speedPointList.removeAt(0);
    refreshPixmap();
}
void SmallWindow::paintEvent(QPaintEvent *event)
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);
}
void SmallWindow::refreshPixmap()
{
    pixmap = QPixmap(size());
    pixmap.fill(this, 0, 0);

    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawCurves(&painter);
    drawWindow(&painter);
    drawText(&painter);
    update();
}
void SmallWindow::refreshIcon()
{
    pixmap = QPixmap(size());
    pixmap.fill(this, 0, 0);

    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawIcon(&painter);
    drawWindow(&painter);
    update();
}
void SmallWindow::drawGrid(QPainter *painter)
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
void SmallWindow::drawIcon(QPainter *painter)
{
    QRect TaskGraphRect = geometry();
    maxWidth = TaskGraphRect.width();
    QBrush brush(QColor(Qt::lightGray));
    painter->fillRect(0, 0, maxWidth, maxHeight, brush);

    QPixmap iconPixmap(":images/icon.png");
    QRectF source(0.0, 0.0, 64.0, 64.0);
    QRectF target(5, 5, 40.0, 40.0);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->drawPixmap(target, iconPixmap, source);
}
void SmallWindow::drawCurves(QPainter *painter)
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
    }
    path.lineTo(pointx , maxHeight);
    path.lineTo(maxWidth , maxHeight);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawPath(path);
    QBrush background;
    background = QBrush(QColor(150, 150, 150, 230));
    painter->fillPath(path, background);
}
void SmallWindow::drawWindow(QPainter *painter)
{
    QPen pen(Qt::lightGray);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawLine(0, 0, maxWidth, 0);
    painter->drawLine(0, 0, 0, maxHeight);
    painter->drawLine(maxWidth, 0, maxWidth, maxHeight);
    painter->drawLine(0, maxHeight, maxWidth, maxHeight);
    pen.setColor(Qt::darkGray);
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawLine(1, 1, maxWidth-1, 1);
    painter->drawLine(1, 1, 1, maxHeight-1);
    painter->drawLine(maxWidth-1, 1, maxWidth-1, maxHeight-1);
    painter->drawLine(1, maxHeight-1, maxWidth-1, maxHeight-1);
}
void SmallWindow::drawText(QPainter *painter)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(Qt::green);
    painter->setPen(pen);
    painter->drawText(QPoint(3, 13), showString);
}
