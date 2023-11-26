#ifndef __MODIFYTASKDIALOG_H__
#define __MODIFYTASKDIALOG_H__

#include <QDialog>
#include "ui_newtask.h"

class ModifyTaskDialog : public QDialog, public Ui::newTask
{
    Q_OBJECT

public:
    ModifyTaskDialog(int taskInfoListId, QWidget * parent = 0, Qt::WFlags f = 0 );

public slots:
    void urlLineEditChanged(QString text);
    void acceptButtonClicked();
    void ulrItemEdit(int row, int column);
    void addMultipleProxy();
    void deleteMultipleProxy();
    void addUrl();
    void deleteUrl();

private:
    QWidget *parent;
    QString saveToDirectory;
    int taskInfoListId;
    QList<QString> urlsList;

private slots:
    void saveToDialog();
    void readSettings();

};

#endif // __MODIFYTASKDIALOG_H__
