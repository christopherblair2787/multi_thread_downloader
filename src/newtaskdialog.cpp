#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "newtaskdialog.h"
#include <QDebug>
NewTaskDialog::NewTaskDialog(QString urlString, QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);
    urlLineEdit->setText(urlString);
    urlLineEditChanged(urlString);

    QSettings settings;
    settings.beginGroup("savePath");
    QString saveToString;
    bool useLastPathBool = settings.value("useLastPath", true).toBool();
    bool useDefaultPathBool = settings.value("useDefaultPath", false).toBool();
    if (useLastPathBool)
        saveToString = settings.value("lastPath").toString();
    if (useDefaultPathBool)
        saveToString = settings.value("defaultPath").toString();
    settings.endGroup();
    saveToLineEdit->setText(saveToString);

    ulrsTableWidget->verticalHeader()->hide();
    ulrsTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ulrsTableWidget->horizontalHeader()->setHighlightSections(false);
    ulrsTableWidget->setColumnWidth(0,70);
    ulrsTableWidget->setColumnWidth(1, 300);

    singleProxyComboBox->insertItem(0, tr("Direct connection"));
    settings.beginGroup("proxys");
    int n=0;
    while (true)
    {
        QStringList proxyStringList;
        proxyStringList << settings.value(QString("proxy%1").arg(n)).toStringList();
        if ( proxyStringList.isEmpty() ) break;
        singleProxyComboBox->insertItem( n+1, proxyStringList.at(0) );
        n++;
    }
    settings.endGroup();

    mutipleProxyTableWidget->setColumnWidth(0,70);
    mutipleProxyTableWidget->setColumnWidth(1, 250);

    QApplication::setActiveWindow(this);

    connect(saveToPushButton, SIGNAL(clicked()), this, SLOT(saveToDialog()));
    connect(urlLineEdit, SIGNAL(textChanged (QString)), this, SLOT(urlLineEditChanged(QString)));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptButtonClicked()));
    connect(ulrsTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(ulrItemEdit(int, int)));
    connect(addMultipleProxyPushButton, SIGNAL(clicked()), this, SLOT(addMultipleProxy()));
    connect(deleteMultipleProxyPushButton, SIGNAL(clicked()), this, SLOT(deleteMultipleProxy()));
    connect(addUrlPushButton, SIGNAL(clicked()), this, SLOT(addUrl()));
    connect(deleteUrlPushButton, SIGNAL(clicked()), this, SLOT(deleteUrl()));
}
void NewTaskDialog::urlLineEditChanged(QString text)
{
    int n = text.lastIndexOf("?");
    text = text.left(n);
    QFileInfo fileInfo(text);
    renameLineEdit->setText(fileInfo.fileName());
}
void NewTaskDialog::saveToDialog()
{
    QFileDialog dialog;
    QString saveToString = saveToLineEdit->text();
    dialog.setDirectory(saveToString);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    if (dialog.exec() == QDialog::Accepted)
    {
        QStringList directoryNames = dialog.selectedFiles();
        QStringListIterator iterator(directoryNames);
        saveToDirectory = directoryNames.at(0);
        saveToLineEdit->setText(saveToDirectory);
    }
}
void NewTaskDialog::ulrItemEdit(int row, int column)
{
    if (column == 1)
    {
        ulrsTableWidget->editItem(ulrsTableWidget->currentItem());
    }
}
void NewTaskDialog::addMultipleProxy()
{
    int row = mutipleProxyTableWidget->rowCount();
    mutipleProxyTableWidget->insertRow(row);
    QString threadNumberString = QString("thread %1").arg(row);
    mutipleProxyTableWidget->setItem(row, 0, new QTableWidgetItem(threadNumberString));
    mutipleProxyTableWidget->setCellWidget(row, 1, new QComboBox);
    QComboBox *mutipleProxyComboBox = dynamic_cast<QComboBox*>(mutipleProxyTableWidget->cellWidget(row, 1));
    mutipleProxyComboBox->insertItem(0, tr("Direct connection"));
    QSettings settings;
    settings.beginGroup("proxys");
    int n=0;
    while (true)
    {
        QStringList proxyStringList;
        proxyStringList << settings.value(QString("proxy%1").arg(n)).toStringList();
        if ( proxyStringList.isEmpty() ) break;
        mutipleProxyComboBox->insertItem( n+1, proxyStringList.at(0) );
        n++;
    }
    settings.endGroup();
}
void NewTaskDialog::deleteMultipleProxy()
{
    int row = mutipleProxyTableWidget->rowCount() - 1;
    mutipleProxyTableWidget->removeRow(row);
}
void NewTaskDialog::addUrl()
{
    int row = ulrsTableWidget->rowCount();
    ulrsTableWidget->insertRow(row);
    QString threadNumberString = QString("thread %1").arg(row);
    ulrsTableWidget->setItem(row, 0, new QTableWidgetItem(threadNumberString));
    ulrsTableWidget->setItem(row, 1, new QTableWidgetItem(""));
}
void NewTaskDialog::deleteUrl()
{
    int row = ulrsTableWidget->rowCount() - 1;
    ulrsTableWidget->removeRow(row);
}
void NewTaskDialog::acceptButtonClicked()
{
    QString urlString2 = urlLineEdit->text();
    QString saveToString = saveToLineEdit->text();
    QString renameString = renameLineEdit->text();

    if ( urlString2.isEmpty() || saveToString.isEmpty() || renameString.isEmpty() )
    {
        QMessageBox::critical(this, tr("critical"),tr("Invalid input."));
        return;
    }

    int taskInfoListId = 0;
    while ( true )
    {
        QSettings settings("NetFleet","task");
        settings.beginGroup(QString("task%1").arg(taskInfoListId));
        QString existedUrlString = settings.value("url").toString();
        settings.endGroup();
        if (urlString2 == existedUrlString)
        {
            QMessageBox::critical(this, tr("critical"),tr("URL: %1 already exists.").arg(urlString2));
            return;
        }
        if (existedUrlString.isEmpty()) break;

        taskInfoListId++;
    }


    QStringList allowedUrlList;
    allowedUrlList<<"http"<<"https"<<"ftp";
    for (int allowedUrlListId=0;allowedUrlListId<allowedUrlList.size();allowedUrlListId++)
    {
        if (urlString2.left(allowedUrlList.at(allowedUrlListId).size()) == allowedUrlList.at(allowedUrlListId) )
        {
            accept();
            return;
        }
    }

    QMessageBox::critical(this, tr("critical"),tr("Invalid URL: %1").arg(urlString2));
}
