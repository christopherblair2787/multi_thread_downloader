#ifndef __SPEEDGRAPHWIDGET_H__
#define __SPEEDGRAPHWIDGET_H__

#include <QWidget>
#include <QPainter>
#include <QBitmap>

class SpeedGraphWidget : public QWidget
{
    Q_OBJECT

public:
    SpeedGraphWidget(QWidget * parent = 0, Qt::WFlags f = 0 );
    void setData( qint64 maxSpeed, int maxHeight, int pointWidth);
    void addSpeedPoint(qint64 speedPoint);
    void refreshPixmap();

    qint64 maxSpeed;
    qint64 speed;
    int pointWidth;
    QList <qint64> speedPointList;

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void drawGrid(QPainter *painter);
    void drawCurves(QPainter *painter);

    int maxWidth;
    int maxHeight;
    QPixmap pixmap;
};

#endif // __SPEEDGRAPHWIDGET_H__
