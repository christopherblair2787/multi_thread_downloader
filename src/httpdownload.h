#ifndef __HTTPDOWNLOAD_H__
#define __HTTPDOWNLOAD_H__

#include <QtNetwork>
#include <QIcon>
#include <QTimer>

#include "downloadthread.h"

class DownloadThread;

class HttpDownload : public QObject
{
    Q_OBJECT

signals:
    void returnThreadMessages(int taskInfoListId, int taskThreadListId, int messagesType, QString strResponseHeader);
    void fileDownloadFinished(int taskInfoListId, int taskThreadListId);
    void returnFileSize(int taskInfoListId, qint64 fileSize);
    void finished();

public:
    HttpDownload(DownloadThread *downloadThread, QObject *parent = 0);
    ~HttpDownload();
    void downloadFile();

    QHttp *http;
    QFile *file;
    QReadWriteLock lock;
    int httpState;

public slots:
    //	void setThreadStopped();

private slots:
    void httpRequestFinished(int requestId, bool error);
    void readResponseHeader(const QHttpResponseHeader &responseHeader);
    void writeData(const QHttpResponseHeader &responseHeader);
    void slotAuthenticationRequired(const QString &, quint16, QAuthenticator *);
    void connectTimeout();
    void connectStateChanged(int state);

#ifndef QT_NO_OPENSSL
    void sslErrors(const QList<QSslError> &errors);
#endif

private:
    DownloadThread *downloadThread;
    QHttpRequestHeader header;
    int httpGetId;
    int connectTimeOutInt;
    int retryDelayInt;
    QTimer *connectTimeoutTimer;
    QString errorString;
    QString unconnectedString;
    QString retryString;
    bool reLocation;
};


#endif // __HTTPDOWNLOAD_H__
