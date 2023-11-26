#ifndef __DELETETASKDIALOG_H__
#define __DELETETASKDIALOG_H__

#include <QDialog>
#include "ui_delete.h"

class DeleteTaskDialog : public QDialog, public Ui::deleteTask
{
    Q_OBJECT

public:
    DeleteTaskDialog( QWidget * parent = 0, Qt::WFlags f = 0 );

};

#endif // __DELETETASKDIALOG_H__
