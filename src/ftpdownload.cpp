#include "ftpdownload.h"
#include <QDebug>

//#define DEBUG

FtpDownload::FtpDownload(DownloadThread *downloadThread, QObject *parent) : ftp(0)
{
    this->downloadThread = downloadThread;

    ftp = new QFtp(this);
    connect(ftp, SIGNAL(done(bool)), this, SLOT(ftpDone(bool)));
    connect(ftp,SIGNAL(readyRead()),this,SLOT(writeData()));
    connect(ftp, SIGNAL(commandStarted(int)), this, SLOT(ftpCommandStarted(int)));
    connect(ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(connectStateChanged(int)));
    connect(ftp, SIGNAL(rawCommandReply(int, const QString &)), this, SLOT(ftpRawCommandReply(int, const QString &)));


    QSettings settings;
    settings.beginGroup("connection");
    connectTimeOutInt = settings.value("connectTimeOut", 30).toInt();
    retryDelayInt = settings.value("retryDelay", 5).toInt();
    settings.endGroup();
}
void FtpDownload::downloadFile()
{
    downloadThread->threadTaskInfomation.httpRequestFinished = false;
    downloadThread->threadTaskInfomation.httpRequestAborted = false;
    unconnectedString = "";
    errorString = "";
    retryString = "";
    rawCommandNumber = 0;
    file=0;
    downloadThread->threadStopped = false;
    downloadThread->waitTimer = 0;
    downloadThread->threadError = false;
    downloadThread->threadTaskInfomation.completed = 0;

    QString fileName = downloadThread->threadTaskInfomation.fileName;
    if (fileName.isEmpty())
    {
        fileName = "index.html";
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, tr("File name is empty."));
        downloadThread->threadError = true;
        downloadThread->threadStopped = true;
        emit finished();
        return;
    }

    file = new QFile(fileName);
    if (!file->open(QIODevice::ReadWrite)) {
        downloadThread->threadError = true;
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, QString(tr("Unable to save the file %1: %2.")).arg(fileName).arg(file->errorString()));
        delete file;
        file = 0;
        downloadThread->threadStopped = true;
        emit finished();
        return;
    }
    file->seek(downloadThread->threadTaskInfomation.startPosition);
    if (downloadThread->threadTaskInfomation.proxyHost != "")
    {
        ftp->setProxy(downloadThread->threadTaskInfomation.proxyHost, downloadThread->threadTaskInfomation.proxyPort);
#if defined(DEBUG)
        qDebug()<<"proxyHost:"<<downloadThread->threadTaskInfomation.proxyHost
                <<";proxyPort:"<<downloadThread->threadTaskInfomation.proxyPort;
#endif
    }

    ftp->setTransferMode(QFtp::Passive);
    ftp->connectToHost(downloadThread->threadTaskInfomation.url.host(), downloadThread->threadTaskInfomation.url.port(21));
    if (!downloadThread->threadTaskInfomation.url.userName().isEmpty())
        ftp->login(downloadThread->threadTaskInfomation.url.userName(), downloadThread->threadTaskInfomation.url.password());
    else
        ftp->login();
    ftp->rawCommand(QString("TYPE I"));
    ftp->rawCommand(QString("SIZE %1").arg(downloadThread->threadTaskInfomation.url.path()));
    if (downloadThread->threadTaskInfomation.startPosition > 0)
        ftp->rawCommand(QString("REST %1").arg(downloadThread->threadTaskInfomation.startPosition));
    ftp->get(downloadThread->threadTaskInfomation.url.path());
    ftp->close();
}
void FtpDownload::connectStateChanged(int state)
{
    int port;
    port = downloadThread->threadTaskInfomation.url.port();
    if (port == -1) port = 21;
    switch (state)
    {
    case QFtp::Unconnected:
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
            if (downloadThread->waitTimer == 0)
            {
                downloadThread->waitTimer = new QTimer(this);
                connect(downloadThread->waitTimer, SIGNAL(timeout()), downloadThread, SLOT(setThreadStopped()));
                downloadThread->waitTimer->start(retryDelayInt*1000);
            }
            retryString = "";
        }

        break;
    case QFtp::HostLookup:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("A host name lookup is in progress."));

        break;
    case QFtp::Connecting:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Connecting %1:%2.").arg(downloadThread->threadTaskInfomation.url.host()).arg(port));
        if (downloadThread->waitTimer == 0)
        {
            connectTimeoutTimer = new QTimer(this);
            connectTimeoutTimer->setSingleShot(true);
            connect(connectTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectTimeout()));
            connectTimeoutTimer->start(connectTimeOutInt*1000);
        }
        break;
    case QFtp::Connected:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Host is connected."));
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("The client is sending its request to the server."));
        if (connectTimeoutTimer->isActive())
        {
            connectTimeoutTimer->stop();
            delete connectTimeoutTimer;
            connectTimeoutTimer = 0;
        }

        break;
    case QFtp::LoggedIn:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Host is logged in."));

        break;
    case QFtp::Closing:
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Host is Closing."));
        break;
    }
}
void FtpDownload::writeData()
{
    QByteArray tempByte;
    tempByte=ftp->readAll();
    lock.lockForWrite();
    file->write(tempByte,tempByte.size());
    lock.unlock();
    downloadThread->threadTaskInfomation.completed += tempByte.size();

    if (downloadThread->threadTaskInfomation.httpRequestFinished)
    {
        ftp->abort();

        if (file)
        {
            file->close();
            delete file;
            file = 0;
        }
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("File download Finished."));
        downloadThread->threadStopped = true;
        emit finished();
        return;
    }
    if (downloadThread->threadTaskInfomation.httpRequestAborted)
    {
        ftp->abort();
        ftp->close();
        if (file)
        {
            file->close();
            delete file;
            file = 0;
        }
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("File download stopped."));
        downloadThread->threadStopped = true;
        emit finished();
        return;
    }
}
void FtpDownload::ftpDone(bool error)
{
    if (downloadThread->threadTaskInfomation.httpRequestFinished)
    {
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("File download finished"));
        downloadThread->threadStopped = true;
        emit finished();
        return;
    }

    if (error)
    {
        downloadThread->threadError = true;
        errorString += tr("Download failed: %1.\r\n").arg(ftp->errorString());
        if (file)
        {
            file->close();
            delete file;
            file = 0;
        }
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("File download stopped."));

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
    if (downloadThread->waitTimer == 0)
    {
        downloadThread->threadStopped = true;
        emit finished();
    }
}
void FtpDownload::ftpCommandStarted(int id)
{
    if (ftp->currentCommand() == QFtp::ConnectToHost)
    {
        int port;
        port = downloadThread->threadTaskInfomation.url.port();
        if (port == -1) port = 21;
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, tr("connect %1:%2.").arg(downloadThread->threadTaskInfomation.url.host()).arg(port));
    }
    if (ftp->currentCommand() == QFtp::Login)
    {
        if (!downloadThread->threadTaskInfomation.url.userName().isEmpty())
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, tr("User login %1:%2.").arg(downloadThread->threadTaskInfomation.url.userName()).arg(downloadThread->threadTaskInfomation.url.password()));
        else
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, tr("User login"));

    }
    if (ftp->currentCommand() == QFtp::RawCommand)
    {
        if (rawCommandNumber == 0)
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, tr("TYPE I"));
            rawCommandNumber++;
        }
        if (rawCommandNumber == 1)
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, tr("SIZE %1").arg(downloadThread->threadTaskInfomation.url.path()));
            rawCommandNumber++;
        }
        if (rawCommandNumber == 2)
        {
            if (downloadThread->threadTaskInfomation.startPosition > 0)
                emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, tr("REST %1").arg(downloadThread->threadTaskInfomation.startPosition));
            rawCommandNumber++;
        }
    }
    if (ftp->currentCommand() == QFtp::Get)
    {
        emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->uploadMessages, tr("RETR %1").arg(downloadThread->threadTaskInfomation.url.path()));

    }
}
void FtpDownload::ftpCommandFinished(int id, bool error)
{
    if (ftp->currentCommand() == QFtp::ConnectToHost)
    {
        if (error)
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, tr("Unable to connect to the FTP server at %1. Please check that the host name is correct.").arg(downloadThread->threadTaskInfomation.url.host()));
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Retry after %1 seconds.\r\n").arg(retryDelayInt));
            if (downloadThread->waitTimer == 0)
            {
                downloadThread->waitTimer = new QTimer(this);
                connect(downloadThread->waitTimer, SIGNAL(timeout()), downloadThread, SLOT(setThreadStopped()));
                downloadThread->waitTimer->start(retryDelayInt*1000);
            }
            return;
        }
    }
    if (ftp->currentCommand() == QFtp::Login)
    {
        if (error)
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, tr("Login failed."));
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Retry after %1 seconds.\r\n").arg(retryDelayInt));
            if (downloadThread->waitTimer == 0)
            {
                downloadThread->waitTimer = new QTimer(this);
                connect(downloadThread->waitTimer, SIGNAL(timeout()), downloadThread, SLOT(setThreadStopped()));
                downloadThread->waitTimer->start(retryDelayInt*1000);
            }
            return;
        }
    }
    if (ftp->currentCommand() == QFtp::Get)
    {
        if (error)
        {
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->errorMessages, tr("download failed."));
            emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->infomationMessages, tr("Retry after %1 seconds.\r\n").arg(retryDelayInt));
            if (downloadThread->waitTimer == 0)
            {
                downloadThread->waitTimer = new QTimer(this);
                connect(downloadThread->waitTimer, SIGNAL(timeout()), downloadThread, SLOT(setThreadStopped()));
                downloadThread->waitTimer->start(retryDelayInt*1000);
            }

            return;
        }
    }
}
void FtpDownload::ftpRawCommandReply(int replyCode, const QString & detail)
{
    //get file size
    if (downloadThread->threadTaskInfomation.startPosition==0 && replyCode==213)
    {
        downloadThread->threadTaskInfomation.size = detail.toLongLong();
        emit returnFileSize(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.size);
    }
    emit returnThreadMessages(downloadThread->threadTaskInfomation.taskInfoListId, downloadThread->threadTaskInfomation.taskThreadListId, downloadThread->downloadMessages, detail);

    //	int leftFileSize = downloadThread->threadTaskInfomation.size - downloadThread->threadTaskInfomation.startPosition;
    if (downloadThread->threadTaskInfomation.size>0 && downloadThread->threadTaskInfomation.size!=detail.toLongLong() && replyCode==213)
    {
        downloadThread->threadError = true;
        errorString += tr("File size incorrect.\r\n");
        retryString += tr("Retry after %1 seconds.\r\n").arg(retryDelayInt);
        ftp->abort();
        ftp->close();

        return;
    }
}
void FtpDownload::connectTimeout()
{
    errorString = tr("Connected timeout.");
    ftp->abort();
    ftp->close();

    disconnect(connectTimeoutTimer, SIGNAL(timeout()), this, SLOT(connectTimeout()));
    connectTimeoutTimer->stop();
    delete connectTimeoutTimer;
    connectTimeoutTimer = 0;
    downloadThread->threadError = true;
}
FtpDownload::~FtpDownload()
{
    delete ftp;
    ftp = 0;
}
