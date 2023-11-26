#ifndef __NEWPROXYDIALOG_H__
#define __NEWPROXYDIALOG_H__

#include <QDialog>
#include "ui_newproxy.h"

class NewProxyDialog : public QDialog, public Ui::newproxy
{
    Q_OBJECT

public:
    NewProxyDialog( QWidget * parent = 0, Qt::WFlags f = 0 );

private slots:
    void httpToggle(bool toggle);
    void acceptClicked();

};

#endif // __NEWPROXYDIALOG_H__
