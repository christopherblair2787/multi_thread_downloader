#include "httpdownload.h"
#include <QDebug>

//#define DEBUG

HttpDownload::HttpDownload(DownloadThread *downloadThread, QObject *parent)
{
    this->downloadThread = downloadThread;

    http = new QHttp(this);

    connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(httpRequestFinished(int, bool)));
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(http,SIGNAL(readyRead(const QHttpResponseHeader &)),this,SLOT(writeData( const QHttpResponseHeader &)));
    connect(http,SIGNAL(stateChanged(int)),this,SLOT(connectStateChanged(int)));
    connect(http, SIGNAL(authenticationRequired(const QString &, quint16, QAuthenticator *)), this, SLOT(slotAuthenticationRequired(const QString &, quint16, QAuthenticator *)));
#ifndef QT_NO_OPENSSL
    connect(http, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErrors(const QList<QSslError> &)));
#endif

    QSettings settings;
    settings.beginGroup("connection");
    connectTimeOutInt = settings.value("connectTimeOut", 30).toInt();
    retryDelayInt = settings.value("retryDelay", 5).toInt();
    settings.endGroup();
}
void HttpDownload::downloadFile()
{
    downloadThread->threadTaskInfomation.httpRequestFinished = false;
    downloadThread->threadTaskInfomation.httpRequestAborted = false;
    unconnectedString = "";
    errorString = "";
    retryString = "";
    reLocation = false;
    file = 0;
    httpState = 0;
    downloadThread->threadStopped = false;
    downloadThread->waitTimer = 0;
    downloadThread->threadError = false;
    downloadThread->threadTaskInfomation.completed = 0;

    QString fileName = downloadThread->threadTaskInfomation.fileName;
    if (fileName.isEmpty())
    {
        downloadThread->threadError = true;
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, tr("File name is empty."));
        downloadThread->threadStopped = true;
        emit finished();
        return;
    }

    file = new QFile(fileName);
    if (!file->open(QIODevice::ReadWrite))
    {
        downloadThread->threadError = true;
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, tr("Unable to save the file %1: %2.").arg(fileName).arg(file->errorString()));
        delete file;
        file = 0;
        downloadThread->threadStopped = true;
        emit finished();
        return;
    }
    file->seek(downloadThread->threadTaskInfomation.startPosition);
    QHttp::ConnectionMode mode = downloadThread->threadTaskInfomation.url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
    http->setHost(downloadThread->threadTaskInfomation.url.host(), mode, downloadThread->threadTaskInfomation.url.port() == -1 ? 0 : downloadThread->threadTaskInfomation.url.port());
    if (!downloadThread->threadTaskInfomation.url.userName().isEmpty())
        http->setUser(downloadThread->threadTaskInfomation.url.userName(), downloadThread->threadTaskInfomation.url.password());

    downloadThread->threadTaskInfomation.httpRequestAborted = false;
    QByteArray path = QUrl::toPercentEncoding(downloadThread->threadTaskInfomation.url.path(), "!$&'()*+,;=:@/");
    if (path.isEmpty())
        path = "/";
    header.setRequest("GET", downloadThread->threadTaskInfomation.url.path());
    QString userAgent = "NetFleet/0.1";
    header.setValue("Host", downloadThread->threadTaskInfomation.url.host());
    header.setValue("Accept", "*/*");
    header.setValue("User-Agent", userAgent);
    if (downloadThread->threadTaskInfomation.startPosition != 0)
    {
        QString temp=(QString("bytes=%1-").arg(downloadThread->threadTaskInfomation.startPosition));
        header.setValue("Range", temp);
    }
    if (downloadThread->threadTaskInfomation.referrer != "")
        header.setValue("Referer", downloadThread->threadTaskInfomation.referrer);
    else
        header.setValue("Referer", downloadThread->threadTaskInfomation.url.toString());

    header.setValue("Cache-Control", "no-cache");
    header.setValue("Connection", "close");
    if (downloadThread->threadTaskInfomation.proxyHost != "")
    {
        http->setProxy(downloadThread->threadTaskInfomation.proxyHost, downloadThread->threadTaskInfomation.proxyPort, downloadThread->threadTaskInfomation.proxyUser, downloadThread->threadTaskInfomation.proxyPassword);
#if defined(DEBUG)
        qDebug()<<"proxyHost:"<<downloadThread->threadTaskInfomation.proxyHost
                <<";proxyPort:"<<downloadThread->threadTaskInfomation.proxyPort
                <<";proxyUser:"<<downloadThread->threadTaskInfomation.proxyUser
                <<";proxyPassword:"<<downloadThread->threadTaskInfomation.proxyPassword;
#endif
    }
    httpGetId = http->request(header);
}
void HttpDownload::connectStateChanged(int state)
{
    httpState = state;
    int port;
    port = downloadThread->threadTaskInfomation.url.port();
    if (port == -1) port = 80;
    switch (state)
    {
    case QHttp::Unconnected:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("There is no connection to the host."));
        if (unconnectedString != "")
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, unconnectedString);
            unconnectedString = "";
        }
        if (downloadThread->threadError)
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, errorString);
            errorString = "";
        }
        if (retryString != "")
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, retryString);
            downloadThread->waitTimer = new QTimer(this);
            connect(downloadThread->waitTimer, SIGNAL(timeout()), downloadThread, SLOT(setThreadStopped()));
            downloadThread->waitTimer->start(retryDelayInt*1000);
            retryString = "";
        }
        else
        {
            if (reLocation == false)
            {
                downloadThread->threadStopped = true;
                emit finished();
            }
            else
            {
                downloadFile();
            }

        }
        break;
    case QHttp::HostLookup:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("A host name lookup is in progress."));
        break;
    case QHttp::Connecting:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Connecting %1:%2.").arg(downloadThread->threadTaskInfomation.url.host()).arg(port));
        connectTimeoutTimer = new QTimer(this);
        connectTimeoutTimer->setSingleShot(true);
        connect(connectTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectTimeout()));
        connectTimeoutTimer->start(connectTimeOutInt*1000);
        break;
    case QHttp::Sending:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("The client is sending its request to the server."));
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, header.toString());
        if (connectTimeoutTimer->isActive())
        {
            connectTimeoutTimer->stop();
            delete connectTimeoutTimer;
            connectTimeoutTimer = 0;
        }
        break;
    case QHttp::Reading:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId,downloadThread-> threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("The client is reading the server's response."));
        break;
    case QHttp::Connected:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Host is connected."));
        break;
    case QHttp::Closing:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Host is Closing."));
        break;
    }
}
void HttpDownload::httpRequestFinished(int requestId, bool error)
{
    if (requestId != httpGetId)
    {
        return;
    }
    if (downloadThread->threadTaskInfomation.httpRequestFinished)
    {
        if (file)
        {
            file->close();
            delete file;
            file = 0;
        }
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("File download finished"));

        return;
    }
    if (downloadThread->threadTaskInfomation.httpRequestAborted)
    {
        if (file)
        {
            file->close();
            delete file;
            file = 0;
        }
        unconnectedString += tr("File download stopped.\r\n");
        return;
    }
    if (requestId != httpGetId)
        return;
    if (error)
    {
        downloadThread->threadError = true;
        //		emit returnThreadMessages(threadTaskInfomation.taskInfoListId, threadTaskInfomation.taskThreadListId, errorMessages, tr("Download failed: %1.").arg(http->errorString()));
        errorString += tr("Download failed: %1.\r\n").arg(http->errorString());
        if (file)
        {
            file->close();
            delete file;
            file = 0;
        }
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, tr("File download failed."));
        downloadThread->waitTimer = new QTimer(this);
        connect(downloadThread->waitTimer, SIGNAL(timeout()), downloadThread, SLOT(setThreadStopped()));
        downloadThread->waitTimer->start(retryDelayInt*1000);
        returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Retry after %1 seconds.\r\n").arg(retryDelayInt));
        return;
    }
    if (file)
    {
        file->close();
        delete file;
        file = 0;
    }
    emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("File download finished"));
    if (downloadThread->threadTaskInfomation.startPosition==0 && downloadThread->threadTaskInfomation.size==0)
        emit fileDownloadFinished(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId);
}

void HttpDownload::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    switch (responseHeader.statusCode()) {
    case 200:                   // Ok
    case 206:                   // Partial Content
    case 301:                   // Moved Permanently
    case 302:                   // Found
    case 303:                   // See Other
    case 307:                   // Temporary Redirect
        // these are not error conditions
        break;

    default:
        downloadThread->threadError = true;
    }

    QString locationString = responseHeader.value("Location");
    if (locationString != "")
    {
        reLocation = true;
        downloadThread->threadTaskInfomation.url.setUrl(locationString);
    }

    if (responseHeader.hasContentLength())
    {
        if (responseHeader.statusCode()==200 || responseHeader.statusCode()==206)
        {
            QString sizeString = responseHeader.value("Content-Length");
            qint64 contentLengthQint64 = sizeString.toLongLong();
            if (contentLengthQint64 >= Q_INT64_C(4*1024*1024*1024))
            {
                errorString += tr("Files larger than 4GB.\r\n");

                http->close();
                downloadThread->threadError = true;

                return;
            }

            if (downloadThread->threadTaskInfomation.startPosition == 0)
            {
                downloadThread->threadTaskInfomation.size = contentLengthQint64;
                emit returnFileSize(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.size);
            }

            qint64 leftFileSize = downloadThread->threadTaskInfomation.size - downloadThread->threadTaskInfomation.startPosition;
            if (downloadThread->threadTaskInfomation.size>0 && leftFileSize>=0 && leftFileSize!=contentLengthQint64)
            {
                downloadThread->threadError = true;
                QString strResponseHeader = responseHeader.toString();
                //				emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->downloadMessages, strResponseHeader);
                errorString += tr("File size incorrect.\r\n");
                retryString += tr("Retry after %1 seconds.\r\n").arg(retryDelayInt);
                downloadThread->threadTaskInfomation.httpRequestAborted = true;
                http->abort();
                http->close();
                return;
            }
        }
    }
    else
    {
        downloadThread->threadTaskInfomation.size = 0;
        QString strResponseHeader = responseHeader.toString();
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->downloadMessages, strResponseHeader);
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Could not get contentLength."));
        return;
    }

    QString strResponseHeader = responseHeader.toString();
    emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->downloadMessages, strResponseHeader);
    if (downloadThread->threadError == true)
    {
        errorString += tr("Download failed:%1\r\n").arg(responseHeader.reasonPhrase());
        retryString += tr("Retry after %1 seconds.\r\n").arg(retryDelayInt);
        downloadThread->threadTaskInfomation.httpRequestAborted = true;
        http->abort();
    }
}
void HttpDownload::slotAuthenticationRequired(const QString &hostName, quint16, QAuthenticator *authenticator)
{
    emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, QString(tr("Authentication required.")));
}
void HttpDownload::writeData(const QHttpResponseHeader &responseHeader)
{
    QByteArray tempByte;
    tempByte=http->readAll();
    lock.lockForWrite();
    file->write(tempByte,tempByte.size());
    lock.unlock();
    downloadThread->threadTaskInfomation.completed += tempByte.size();

    if (downloadThread->threadTaskInfomation.httpRequestFinished)
    {
        http->abort();
        return;
    }
    if (downloadThread->threadTaskInfomation.httpRequestAborted)
    {
        http->abort();
        return;
    }
}
#ifndef QT_NO_OPENSSL
void HttpDownload::sslErrors(const QList<QSslError> &errors)
{
    foreach (const QSslError &error, errors)
    {
        if (!errorString.isEmpty())
            errorString += ", ";
        errorString += error.errorString();
    }
    errorString = tr("One or more SSL errors has occurred:%1").arg(errorString);
    http->ignoreSslErrors();
}
#endif

void HttpDownload::connectTimeout()
{
    errorString = tr("Connected timeout.");
    http->abort();
    disconnect(connectTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectTimeout()));
    connectTimeoutTimer->stop();
    delete connectTimeoutTimer;
    connectTimeoutTimer = 0;
    downloadThread->threadError = true;
}
HttpDownload::~HttpDownload()
{
    delete http;
    http = 0;
}
