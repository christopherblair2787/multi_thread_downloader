#ifndef __PREFERENCESDIALOG_H__
#define __PREFERENCESDIALOG_H__

#include <QDialog>
#include "ui_preferences.h"

class PreferencesDialog : public QDialog, public Ui::preferences
{
    Q_OBJECT

public slots:
    void acceptClicked();

public:
    PreferencesDialog( QWidget * parent = 0, Qt::WFlags f = 0 );

private slots:
    void addProxyClicked();
    void editProxyClicked();
    void deleteProxyClicked();
    void saveToDialog();

private:
    void readSettings();

    QString saveToDirectory;
};

#endif // __PREFERENCESDIALOG_H__
