#ifndef EDITPROXYDIALOG_H
#define EDITPROXYDIALOG_H

#include <QDialog>
#include "ui_newproxy.h"

class EditProxyDialog : public QDialog, public Ui::newproxy
{
    Q_OBJECT

public:
    EditProxyDialog( int row, QStringList proxyStringList, QWidget * parent = 0, Qt::WFlags f = 0 );

    int row;

private slots:
    void acceptClicked();

};
#endif // EDITPROXYDIALOG_H
