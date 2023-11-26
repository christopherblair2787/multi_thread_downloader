#include <QDebug>
#include "downloadthread.h"

//#define DEBUG

DownloadThread::DownloadThread(QObject *parent) : httpDownload(0), ftpDownload(0)
{
    threadTaskInfomation.taskInfoListId = 0;
    threadTaskInfomation.taskThreadListId = 0;
    threadTaskInfomation.size = 0;
    threadTaskInfomation.completed = 0;
    threadTaskInfomation.startPosition = 0;
    threadTaskInfomation.speed = 0;
}
void DownloadThread::run()
{
    if (httpDownload)
    {
        delete httpDownload;
        httpDownload = 0;
    }
    if (ftpDownload)
    {
        delete ftpDownload;
        ftpDownload = 0;
    }
    if (threadTaskInfomation.url.toString().left(4) == "http")
    {
#if defined(DEBUG)
        qDebug()<<qPrintable(QString("thread%1 started").arg(threadTaskInfomation.taskThreadListId));
#endif
        httpDownload = new HttpDownload(this);
        connect(httpDownload, SIGNAL(returnThreadMessages(int, int, int, QString)), this, SLOT(getThreadMessages(int, int, int, QString)));
        connect(httpDownload, SIGNAL(fileDownloadFinished(int, int)), this, SLOT(getFileDownloadFinished(int, int)));
        connect(httpDownload, SIGNAL(returnFileSize(int, qint64)), this, SLOT(getFileSize(int, qint64)));
        connect(httpDownload, SIGNAL(finished()), this, SLOT(quit()));
        httpDownload->downloadFile();
        this->exec();
    }
    if (threadTaskInfomation.url.toString().left(3) == "ftp")
    {
        ftpDownload = new FtpDownload(this);
        connect(ftpDownload, SIGNAL(returnThreadMessages(int, int, int, QString)), this, SLOT(getThreadMessages(int, int, int, QString)));
        connect(ftpDownload, SIGNAL(fileDownloadFinished(int, int)), this, SLOT(getFileDownloadFinished(int, int)));
        connect(ftpDownload, SIGNAL(returnFileSize(int, qint64)), this, SLOT(getFileSize(int, qint64)));
        connect(ftpDownload, SIGNAL(finished()), this, SLOT(quit()));

        ftpDownload->downloadFile();
        this->exec();
    }
}
void DownloadThread::setThreadStopped()
{
#if defined(DEBUG)
    qDebug()<<"setThreadStopped";
#endif
    if (waitTimer)
    {
        disconnect(waitTimer, SIGNAL(timeout()), this, SLOT(setThreadStopped()));
        waitTimer->stop();
        delete waitTimer;
        waitTimer = 0;
        threadStopped = true;
        quit();
        return;
    }

    if (httpDownload)
    {
        httpDownload->http->abort();
        if (httpDownload->httpState  == QHttp::Unconnected)
        {
            emit returnThreadMessages(threadTaskInfomation.taskInfoListId, threadTaskInfomation.taskThreadListId, infomationMessages, tr("File download Finished."));
            threadStopped = true;
            quit();
        }
    }

    if (ftpDownload)
    {
        ftpDownload->ftp->abort();
        ftpDownload->lock.unlock();
        if (ftpDownload->file)
        {
            ftpDownload->file->close();
            delete ftpDownload->file;
            ftpDownload->file = 0;
        }
        emit returnThreadMessages(threadTaskInfomation.taskInfoListId, threadTaskInfomation.taskThreadListId, infomationMessages, tr("File download Finished."));
        threadStopped = true;
        quit();
    }
}
void DownloadThread::getThreadMessages(int taskInfoListId, int taskThreadListId, int messagesType, QString strResponseHeader)
{
    emit returnThreadMessages(taskInfoListId, taskThreadListId, messagesType, strResponseHeader);
}
void DownloadThread::getFileDownloadFinished(int taskInfoListId, int taskThreadListId)
{
    emit fileDownloadFinished(taskInfoListId, taskThreadListId);
}
void DownloadThread::getFileSize(int taskInfoListId, qint64 fileSize)
{
    emit returnFileSize(taskInfoListId, fileSize);
}
DownloadThread::~DownloadThread()
{
    if (httpDownload)
    {
        delete httpDownload;
        httpDownload = 0;
    }
    if (ftpDownload)
    {
        delete ftpDownload;
        ftpDownload = 0;
    }
}
