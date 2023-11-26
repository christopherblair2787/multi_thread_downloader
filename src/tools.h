#ifndef __TOOLS_H__
#define __TOOLS_H__
#include <QObject>
#include <QString>

class Tools: public QObject
{
    Q_OBJECT

public:
    Tools( QObject * parent = 0, Qt::WFlags f = 0 );;
    QString get_size( qint64 byte );
    QString getTimeLeft( qint64 size, qint64 completed, qint64 speed );
    QString getTime( qint64 seconds );
};

#endif // __TOOLS_H__
