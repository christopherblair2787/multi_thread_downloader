#include "modifytaskdialog.h"

#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QVariant>
#include "modifytaskdialog.h"
#include <QDebug>
ModifyTaskDialog::ModifyTaskDialog(int taskInfoListId, QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);
    this->taskInfoListId = taskInfoListId;

    ulrsTableWidget->verticalHeader()->hide();
    ulrsTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ulrsTableWidget->horizontalHeader()->setHighlightSections(false);
    ulrsTableWidget->setColumnWidth(0,70);
    ulrsTableWidget->setColumnWidth(1, 300);

    singleProxyComboBox->insertItem(0, tr("Direct connection"));

    QSettings settings;
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

    connect(saveToPushButton, SIGNAL(clicked()), this, SLOT(saveToDialog()));
    connect(urlLineEdit, SIGNAL(textChanged (QString)), this, SLOT(urlLineEditChanged(QString)));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptButtonClicked()));
    connect(ulrsTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(ulrItemEdit(int, int)));
    connect(addMultipleProxyPushButton, SIGNAL(clicked()), this, SLOT(addMultipleProxy()));
    connect(deleteMultipleProxyPushButton, SIGNAL(clicked()), this, SLOT(deleteMultipleProxy()));
    connect(addUrlPushButton, SIGNAL(clicked()), this, SLOT(addUrl()));
    connect(deleteUrlPushButton, SIGNAL(clicked()), this, SLOT(deleteUrl()));

    readSettings();
}
void ModifyTaskDialog::readSettings()
{
    QSettings settings("NetFleet","task");
    settings.beginGroup(QString("task%1").arg(taskInfoListId));
    QString urlString = settings.value("url").toString();
    QString referrerString = settings.value("referrer").toString();
    QString saveToString = settings.value("saveTo").toString();
    QString renameString = settings.value("rename").toString();
    int spliteInt = settings.value("split").toInt();
    QString userString = settings.value("user").toString();
    QString passwordString = settings.value("password").toString();
    QString commentString = settings.value("comment").toString();
    bool noProxyBool = settings.value("noProxy", true).toBool();
    bool singleProxyBool = settings.value("singleProxy", false).toBool();
    int singleProxyId = settings.value("singleProxyId", -1).toInt();
    bool mutipleProxyBool = settings.value("mutipleProxy", false).toBool();
    QList <QVariant> mutipleProxyIdsVariant = settings.value("mutipleProxyIds").toList();
    QList <QVariant> urlsListVariant = settings.value("urls").toList();
    settings.endGroup();

    urlLineEdit->setText(urlString);
    referrerLineEdit->setText(referrerString);
    saveToLineEdit->setText(saveToString);
    renameLineEdit->setText(renameString);
    splitSpinBox->setValue(spliteInt);
    userLineEdit->setText(userString);
    passwordLineEdit->setText(passwordString);
    commentTextEdit->setText(commentString);
    manualRadioButton->setChecked(true);

    noProxyRadioButton->setChecked(noProxyBool);
    singleProxyRadioButton->setChecked(singleProxyBool);
    if (singleProxyId+1 < singleProxyComboBox->count())
        singleProxyComboBox->setCurrentIndex(singleProxyId+1);
    else
        singleProxyComboBox->setCurrentIndex(0);
    mutipleProxyRadioButton->setChecked(mutipleProxyBool);
    QList <int> mutipleProxyIds;
    foreach( QVariant mutipleProxyId, mutipleProxyIdsVariant )
        mutipleProxyIds << mutipleProxyId.toInt();
    for (int mutipleProxyId=0;mutipleProxyId<mutipleProxyIds.size();mutipleProxyId++)
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
        if (mutipleProxyIds.at(mutipleProxyId)+1 < mutipleProxyComboBox->count())
            mutipleProxyComboBox->setCurrentIndex(mutipleProxyIds.at(mutipleProxyId)+1);
        else
            mutipleProxyComboBox->setCurrentIndex(0);
    }

    foreach( QVariant url, urlsListVariant )
        urlsList << url.toString();
    for (int urlsListId=0;urlsListId<urlsList.size();urlsListId++)
    {
        int row = ulrsTableWidget->rowCount();
        ulrsTableWidget->insertRow(row);
        QString threadNumberString = QString("thread %1").arg(row);
        ulrsTableWidget->setItem(row, 0, new QTableWidgetItem(threadNumberString));
        ulrsTableWidget->setItem(row, 1, new QTableWidgetItem(urlsList.at(urlsListId)));
    }
}
void ModifyTaskDialog::urlLineEditChanged(QString text)
{
    int n = text.lastIndexOf("?");
    text = text.left(n);
    QFileInfo fileInfo(text);
    renameLineEdit->setText(fileInfo.fileName());
}
void ModifyTaskDialog::saveToDialog()
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
void ModifyTaskDialog::ulrItemEdit(int row, int column)
{
    if (column == 1)
    {
        ulrsTableWidget->editItem(ulrsTableWidget->currentItem());
    }
}
void ModifyTaskDialog::addMultipleProxy()
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
void ModifyTaskDialog::deleteMultipleProxy()
{
    int row = mutipleProxyTableWidget->rowCount() - 1;
    mutipleProxyTableWidget->removeRow(row);
}
void ModifyTaskDialog::addUrl()
{
    int row = ulrsTableWidget->rowCount();
    ulrsTableWidget->insertRow(row);
    QString threadNumberString = QString("thread %1").arg(row);
    ulrsTableWidget->setItem(row, 0, new QTableWidgetItem(threadNumberString));
    ulrsTableWidget->setItem(row, 1, new QTableWidgetItem(""));
}
void ModifyTaskDialog::deleteUrl()
{
    int row = ulrsTableWidget->rowCount() - 1;
    ulrsTableWidget->removeRow(row);
}
void ModifyTaskDialog::acceptButtonClicked()
{
    QString urlString = urlLineEdit->text();
    QString saveToString = saveToLineEdit->text();
    QString renameString = renameLineEdit->text();

    if ( urlString.isEmpty() || saveToString.isEmpty() || renameString.isEmpty() )
    {
        QMessageBox::critical(this, tr("critical"),tr("Invalid input."));
        return;
    }

    QStringList allowedUrlList;
    allowedUrlList<<"http"<<"https"<<"ftp";
    for (int allowedUrlListId=0;allowedUrlListId<allowedUrlList.size();allowedUrlListId++)
    {
        if (urlString.left(allowedUrlList.at(allowedUrlListId).size()) == allowedUrlList.at(allowedUrlListId) )
        {
            accept();
            return;
        }
    }

    QMessageBox::critical(this, tr("critical"),tr("Invalid URL: %1").arg(urlString));
}

