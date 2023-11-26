#ifndef __DOWNLOADTHREAD_H__
#define __DOWNLOADTHREAD_H__
#include <QThread>
#include <QtNetwork>
#include <QIcon>

#include "httpdownload.h"
#include "ftpdownload.h"

class HttpDownload;
class FtpDownload;

class DownloadThread : public QThread
{
    Q_OBJECT

signals:
    void returnThreadMessages(int taskInfoListId, int taskThreadListId, int messagesType, QString strResponseHeader);
    void fileDownloadFinished(int taskInfoListId, int taskThreadListId);
    void returnFileSize(int taskInfoListId, qint64 fileSize);

public:
    DownloadThread(QObject *parent = 0);
    ~DownloadThread();

    QTimer *waitTimer;
    bool threadStopped;
    bool threadError;


    enum ThreadMessagesType
    {
        uploadMessages = 0,
        downloadMessages,
        infomationMessages,
        errorMessages
    };

    struct _ThreadTaskInfomation
    {
        int taskInfoListId;
        int taskThreadListId;
        QUrl url;
        QString proxyHost;
        int proxyPort;
        QString proxyUser;
        QString proxyPassword;
        QString referrer;
        QString fileName;
        qint64 size;
        qint64 completed;
        qint64 startPosition;
        qint64 speed;
        bool httpRequestAborted;
        bool httpRequestFinished;
        QList<QIcon> stateDetailList;
        QList<QString> timeDetailList;
        QList<QString> messageDetailList;
        QList <qint64> newReceivedList;
    };
    typedef struct _ThreadTaskInfomation ThreadTaskInfomation;
    ThreadTaskInfomation threadTaskInfomation;

public slots:
    void setThreadStopped();

protected:
    void run();

private slots:
    void getThreadMessages(int taskInfoListId, int taskThreadListId, int messagesType, QString strResponseHeader);
    void getFileDownloadFinished(int taskInfoListId, int taskThreadListId);
    void getFileSize(int taskInfoListId, qint64 fileSize);

private:
    HttpDownload *httpDownload;
    FtpDownload *ftpDownload;

};

#endif // __DOWNLOADTHREAD_H__
