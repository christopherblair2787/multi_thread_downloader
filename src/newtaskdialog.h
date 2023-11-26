#ifndef __NEWTASKDIALOG_H__
#define __NEWTASKDIALOG_H__

#include <QDialog>
#include "ui_newtask.h"

class NewTaskDialog : public QDialog, public Ui::newTask
{
    Q_OBJECT

public:
    NewTaskDialog( QString urlString = "", QWidget * parent = 0, Qt::WFlags f = 0 );

public slots:
    void urlLineEditChanged(QString text);
    void acceptButtonClicked();
    void ulrItemEdit(int row, int column);
    void addMultipleProxy();
    void deleteMultipleProxy();
    void addUrl();
    void deleteUrl();

private slots:
    void saveToDialog();

private:
    QWidget *parent;
    QString saveToDirectory;

};

#endif // __NEWTASKDIALOG_H__
