#include <QMessageBox>
#include "newproxydialog.h"

NewProxyDialog::NewProxyDialog(QWidget *parent, Qt::WFlags f) : QDialog(parent, f)
{
    setupUi(this);

    connect(httpRadioButton, SIGNAL(toggled(bool)), this, SLOT(httpToggle(bool)));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptClicked()));
}
void NewProxyDialog::httpToggle(bool toggle)
{
    if (toggle)
    {
        userLineEdit->setEnabled(true);
        passwordLineEdit->setEnabled(true);
    }
    else
    {
        userLineEdit->setEnabled(false);
        passwordLineEdit->setEnabled(false);
    }
}
void NewProxyDialog::acceptClicked()
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
