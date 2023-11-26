#ifndef __SMALLWINDOW_H__
#define __SMALLWINDOW_H__

#include <QWidget>
#include <QPainter>
#include <QBitmap>
#include <QMouseEvent>

class SmallWindow : public QWidget
{
    Q_OBJECT

signals:
    void addNewTask(QString urlString);

public:
    SmallWindow(QWidget * parent = 0, Qt::WFlags f = 0);
    void setData(qint64 maxSpeed, int pointWidth);
    void setString(QString showString);
    void setSpeedPoint(QList <qint64> speedPointList);
    void refreshPixmap();
    void refreshIcon();

    qint64 maxSpeed;
    qint64 speed;
    int pointWidth;
    QList <qint64> speedPointList;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void drawGrid(QPainter *painter);
    void drawIcon(QPainter *painter);
    void drawCurves(QPainter *painter);
    void drawWindow(QPainter *painter);
    void drawText(QPainter *painter);

    QPoint last;
    QPoint pos0;

    int maxWidth;
    int maxHeight;
    QPixmap pixmap;
    QString showString;
};

#endif // __SMALLWINDOW_H__
