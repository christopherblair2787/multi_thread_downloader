#ifndef __TASKGRAPHPAINTERWIDGET_H__
#define __TASKGRAPHPAINTERWIDGET_H__

#include <QWidget>
#include <QPainter>
#include <QBitmap>

class TaskGraphPainterWidget : public QWidget
{
    Q_OBJECT

public:
    TaskGraphPainterWidget(QWidget * parent = 0, Qt::WFlags f = 0 );

    void setData( qint64 fileSize, qint64 blockSize );
    void setBlockSizeData( qint64 blockSize );
    void setNotDownloadListClear();
    void setNotDownloadList( int taskThreadListId, qint64 startPosition, qint64 endPosition);
    void newReceivedListClear();
    void setNewReceived(int taskThreadListId, QList <qint64> newReceivedList);
    void refreshAll();
    void refreshPixmap();
    void refreshThreadLastBlock(int newThreadReceivedListId);

    qint64 fileSize;
    qint64 blockSize;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *event);

private:
    void drawNotDownload(QPainter *painter);
    void drawDownloaded(QPainter *painter);
    void drawNoSizeFile(QPainter *painter);
    void drawNewReceivedData(QPainter *painter);
    void drawThreadLastBlock(QPainter *painter, int newThreadReceivedListId);


    int x;
    int y;
    int maxWidth;
    int maxHeight;
    QPixmap pixmap;
    QPixmap pixmap2;
    QPixmap downloadedPixmap;
    QPixmap downloadingPixmap;
    QPixmap notDownloadPixmap;
    int taskThreadListId;
    struct _NotDownload
    {
        int taskThreadListId;
        qint64 startPosition;
        qint64 endPosition;
    };
    typedef struct _NotDownload NotDownload;
    QList <NotDownload> notDownloadList;
    struct _NewReceived
    {
        int taskThreadListId;
        QList <qint64> newThreadReceivedList;
    };
    typedef struct _NewReceived NewReceived;
    QList <NewReceived> newTaskReceivedList;
};
#endif // __TASKGRAPHPAINTERWIDGET_H__
