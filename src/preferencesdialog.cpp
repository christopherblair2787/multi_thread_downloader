#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "preferencesdialog.h"
#include "newproxydialog.h"
#include "editproxydialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);
    proxyTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    proxyTableWidget->horizontalHeader()->setHighlightSections(false);
    proxyTableWidget->setColumnHidden(4, true);
    proxyTableWidget->setColumnHidden(5, true);
    QSettings settings;
    settings.beginGroup("proxys");
    int n=0;
    while (true)
    {
        QStringList proxyStringList;
        proxyStringList << settings.value(QString("proxy%1").arg(n)).toStringList();
        if (proxyStringList.isEmpty()) break;
        int row = proxyTableWidget->rowCount();
        proxyTableWidget->insertRow(row);
        proxyTableWidget->setItem(row, 0, new QTableWidgetItem(proxyStringList.at(0)));
        proxyTableWidget->setItem(row, 1, new QTableWidgetItem(proxyStringList.at(1)));
        proxyTableWidget->setItem(row, 2, new QTableWidgetItem(proxyStringList.at(2)));
        proxyTableWidget->setItem(row, 3, new QTableWidgetItem(proxyStringList.at(3)));
        proxyTableWidget->setItem(row, 4, new QTableWidgetItem(proxyStringList.at(4)));
        proxyTableWidget->setItem(row, 5, new QTableWidgetItem(proxyStringList.at(5)));
        n++;
    }
    settings.endGroup();

    connect(addProxyPushButton, SIGNAL(clicked()), this, SLOT(addProxyClicked()));
    connect(editProxyPushButton, SIGNAL(clicked()), this, SLOT(editProxyClicked()));
    connect(deleteProxyPushButton, SIGNAL(clicked()), this, SLOT(deleteProxyClicked()));
    connect(saveToPushButton, SIGNAL(clicked()), this, SLOT(saveToDialog()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptClicked()));

    readSettings();
}
void PreferencesDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup("task");
    int maxRunningTasksInt = settings.value("maxRunningTasks", 5).toInt();
    int splitInt = settings.value("split", 5).toInt();
    qint64 minimumSplitSizeInt64 = settings.value("minimumSplitSize", 300).toLongLong();
    settings.endGroup();

    settings.beginGroup("savePath");
    bool useLastPathBool = settings.value("useLastPath", true).toBool();
    bool useDefaultPathBool = settings.value("useDefaultPath", false).toBool();
    QString saveToString = settings.value("defaultPath").toString();
    settings.endGroup();

    settings.beginGroup("connection");
    int connectTimeOutInt = settings.value("connectTimeOut", 30).toInt();
    int maxRetryNumberInt = settings.value("maxRetryNumber", 99).toInt();
    int retryDelayInt = settings.value("retryDelay", 5).toInt();
    settings.endGroup();

    settings.beginGroup("speedGraph");
    qint64 maxSpeedGraph = settings.value("maxSpeed", 250).toLongLong();
    int maxHeightGraph = settings.value("maxHeight", 60).toInt();
    int pixelWidthGraph = settings.value("pixelWidth", 2).toInt();
    settings.endGroup();

    settings.beginGroup("blocksGraph");
    qint64 blockSizeGraph = settings.value("blockSize", 50).toLongLong();
    settings.endGroup();

    settings.beginGroup("monitor");
    bool monitorClipboardBool = settings.value("monitorClipboard", true).toBool();
    QString defaultMonitorTypes = ".zip;.rar;.gz;.z;.bz2;.tar;.arj;.lzh;.exe;.bin;.mp3;.mp4";
    QString monitorTypesString = settings.value("monitorTypes", defaultMonitorTypes).toString();
    settings.endGroup();

    maxRunningTasksSpinBox->setValue(maxRunningTasksInt);
    splitSpinBox->setValue(splitInt);
    minimumSplitSizeSpinBox->setValue(minimumSplitSizeInt64);

    useLastPathRadioButton->setChecked(useLastPathBool);
    useDefaultPathRadioButton->setChecked(useDefaultPathBool);
    saveToLineEdit->setText(saveToString);

    connectTimeOutSpinBox->setValue(connectTimeOutInt);
    if (maxRetryNumberInt == -1)
    {
        alwaysRetryCheckBox->setChecked(true);
        maxRetryNumberSpinBox->setEnabled(false);
    }
    else
    {
        alwaysRetryCheckBox->setChecked(false);
        maxRetryNumberSpinBox->setValue(maxRetryNumberInt);
        maxRetryNumberSpinBox->setEnabled(true);
    }
    retryDelaySpinBox->setValue(retryDelayInt);

    maxSpeedSpinBox->setValue(maxSpeedGraph);
    maxHeightSpinBox->setValue(maxHeightGraph);
    pixelWidthSpinBox->setValue(pixelWidthGraph);

    blockSizeSpinBox->setValue(blockSizeGraph);

    monitorClipboardCheckBox->setChecked(monitorClipboardBool);
    monitorTypesTextEdit->setPlainText(monitorTypesString);
}
void PreferencesDialog::addProxyClicked()
{
    NewProxyDialog *newProxyDialog = new NewProxyDialog();
    if ( newProxyDialog->exec() == QDialog::Accepted )
    {
        QString titleString = newProxyDialog->titleLineEdit->text();
        QString hostString = newProxyDialog->hostLineEdit->text();
        int portInt = newProxyDialog->portSpinBox->value();
        bool httpBool = newProxyDialog->httpRadioButton->isChecked();
        bool socks4Bool = newProxyDialog->socks4RadioButton->isChecked();
        bool socks5Bool = newProxyDialog->socks5RadioButton->isChecked();
        QString userString = newProxyDialog->userLineEdit->text();
        QString passwordString = newProxyDialog->passwordLineEdit->text();

        int row = proxyTableWidget->rowCount();
        proxyTableWidget->insertRow(row);
        proxyTableWidget->setItem(row, 0, new QTableWidgetItem(titleString));
        proxyTableWidget->setItem(row, 1, new QTableWidgetItem(hostString));
        proxyTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(portInt)));
        if (httpBool) proxyTableWidget->setItem(row, 3, new QTableWidgetItem("HTTP"));
        if (socks4Bool) proxyTableWidget->setItem(row, 3, new QTableWidgetItem("SOCKS 4"));
        if (socks5Bool) proxyTableWidget->setItem(row, 3, new QTableWidgetItem("SOCKS 5"));
        proxyTableWidget->setItem(row, 4, new QTableWidgetItem(userString));
        proxyTableWidget->setItem(row, 5, new QTableWidgetItem(passwordString));
    }
}
void PreferencesDialog::editProxyClicked()
{
    int row = proxyTableWidget->currentRow();
    if (row < 0) return;

    QStringList proxyStringList;
    proxyStringList << proxyTableWidget->item(row, 0)->text();
    proxyStringList << proxyTableWidget->item(row, 1)->text();
    proxyStringList << proxyTableWidget->item(row, 2)->text();
    proxyStringList << proxyTableWidget->item(row, 3)->text();
    proxyStringList << proxyTableWidget->item(row, 4)->text();
    proxyStringList << proxyTableWidget->item(row, 5)->text();
    EditProxyDialog *editProxyDialog = new EditProxyDialog(row, proxyStringList);
    if ( editProxyDialog->exec() == QDialog::Accepted )
    {
        QString titleString = editProxyDialog->titleLineEdit->text();
        QString hostString = editProxyDialog->hostLineEdit->text();
        int portInt = editProxyDialog->portSpinBox->value();
        bool httpBool = editProxyDialog->httpRadioButton->isChecked();
        bool socks4Bool = editProxyDialog->socks4RadioButton->isChecked();
        bool socks5Bool = editProxyDialog->socks5RadioButton->isChecked();
        QString userString = editProxyDialog->userLineEdit->text();
        QString passwordString = editProxyDialog->passwordLineEdit->text();

        proxyTableWidget->setItem(row, 0, new QTableWidgetItem(titleString));
        proxyTableWidget->setItem(row, 1, new QTableWidgetItem(hostString));
        proxyTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(portInt)));
        if (httpBool) proxyTableWidget->setItem(row, 3, new QTableWidgetItem("HTTP"));
        if (socks4Bool) proxyTableWidget->setItem(row, 3, new QTableWidgetItem("SOCKS 4"));
        if (socks5Bool) proxyTableWidget->setItem(row, 3, new QTableWidgetItem("SOCKS 5"));
        proxyTableWidget->setItem(row, 4, new QTableWidgetItem(userString));
        proxyTableWidget->setItem(row, 5, new QTableWidgetItem(passwordString));
    }
}
void PreferencesDialog::deleteProxyClicked()
{
    int row = proxyTableWidget->currentRow();
    proxyTableWidget->removeRow(row);
}
void PreferencesDialog::saveToDialog()
{
    QFileDialog dialog;
    dialog.setDirectory(".");
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    if (dialog.exec() == QDialog::Accepted)
    {
        QStringList directoryNames = dialog.selectedFiles();
        QStringListIterator iterator(directoryNames);
        saveToDirectory = directoryNames.at(0);
        saveToLineEdit->setText(saveToDirectory);
    }
}
void PreferencesDialog::acceptClicked()
{
    QString saveToString = saveToLineEdit->text();
    if (useDefaultPathRadioButton->isChecked() && saveToString.isEmpty() )
    {
        QMessageBox::critical(this, tr("critical"),tr("Default save path should not be empty."));
        return;
    }
    accept();
}
