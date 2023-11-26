#include <QMessageBox>
#include "editproxydialog.h"

EditProxyDialog::EditProxyDialog(int row, QStringList proxyStringList, QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);
    this->row = row;

    setWindowTitle(tr("Modify Proxy"));
    titleLineEdit->setText(proxyStringList.at(0));
    hostLineEdit->setText(proxyStringList.at(1));
    portSpinBox->setValue(proxyStringList.at(2).toInt());
    QString typeString = proxyStringList.at(3);
    if (typeString == "HTTP") httpRadioButton->setChecked(true);
    if (typeString == "SOCKS 4") socks4RadioButton->setChecked(true);
    if (typeString == "SOCKS 5") socks5RadioButton->setChecked(true);
    userLineEdit->setText(proxyStringList.at(4));
    passwordLineEdit->setText(proxyStringList.at(5));

    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptClicked()));
}
void EditProxyDialog::acceptClicked()
{
    QString titleString = titleLineEdit->text();
    QString hostString = hostLineEdit->text();

    if (titleString.isEmpty())
    {
        QMessageBox::critical(this, tr("critical"),tr("Title should not be empty."));
        return;
    }
    if (hostString.isEmpty())
    {
        QMessageBox::critical(this, tr("critical"),tr("Host should not be empty."));
        return;
    }

    accept();
}
