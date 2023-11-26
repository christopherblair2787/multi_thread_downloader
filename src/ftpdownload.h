#ifndef __FTPDOWNLOAD_H__
#define __FTPDOWNLOAD_H__

#include <QtNetwork>
#include <QTimer>
#include <QFile>
#include "downloadthread.h"

class DownloadThread;

class FtpDownload : public QObject
{
    Q_OBJECT

signals:
    void returnThreadMessages(int taskInfoListId, int taskThreadListId, int messagesType, QString strResponseHeader);
    void fileDownloadFinished(int taskInfoListId, int taskThreadListId);
    void returnFileSize(int taskInfoListId, qint64 fileSize);
    void finished();

public:
    FtpDownload(DownloadThread *downloadThread, QObject *parent = 0);
    ~FtpDownload();
    void downloadFile();

    QFtp *ftp;
    QFile *file;
    QReadWriteLock lock;


private slots:
    void ftpDone(bool error);
    void connectStateChanged(int state);
    void writeData();
    void ftpCommandStarted(int id);
    void ftpCommandFinished(int id, bool error);
    void ftpRawCommandReply(int replyCode, const QString & detail);
    void connectTimeout();

private:
    DownloadThread *downloadThread;

    QString errorString;
    QString unconnectedString;
    QString retryString;
    int connectTimeOutInt;
    int retryDelayInt;
    QTimer *connectTimeoutTimer;
    int rawCommandNumber;
};

#endif // __FTPDOWNLOAD_H__
