#include "tools.h"
Tools::Tools( QObject * parent, Qt::WFlags f)
{

}
QString Tools::get_size( qint64 byte )
{
    double kb=0,mb=0,gb=0;
    QString size;
    if ( byte > 1024 ) kb = byte/1024;
    if ( kb > 1024 ) mb = kb/1024;
    if ( mb > 1024 ) gb = mb/1024;
    size = tr("%1B").arg(byte);
    if ( kb != 0 ) size=tr("%1KB").arg(kb,0,'f',2);
    if ( mb != 0 ) size=tr("%1MB").arg(mb,0,'f',2);
    if ( gb != 0 ) size=tr("%1GB").arg(gb,0,'f',2);

    return size;
}
QString Tools::getTimeLeft( qint64 size, qint64 completed, qint64 speed )
{
    QString timeLeft;
    if ( speed == 0 )
    {
        timeLeft = tr("0h:0m:0s");
        return timeLeft;
    }
    qint64 sizeLeft = size - completed;
    qint64 seconds = sizeLeft/speed;
    qint64 minutes = seconds/60;
    seconds = seconds%60;
    qint64 hours = minutes/60;
    minutes = minutes%60;
    timeLeft = tr("%1h:%2m:%3s").arg(hours).arg(minutes).arg(seconds);

    return timeLeft;
}
QString Tools::getTime( qint64 seconds )
{
    QString timeString;
    qint64 minutes = seconds/60;
    seconds = seconds%60;
    qint64 hours = minutes/60;
    minutes = minutes%60;
    timeString = tr("%1h:%2m:%3s").arg(hours).arg(minutes).arg(seconds);

    return timeString;
}
