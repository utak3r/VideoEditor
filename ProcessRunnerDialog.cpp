#include "ProcessRunnerDialog.h"
#include "ui_ProcessRunnerDialog.h"

ProcessRunnerDialog::ProcessRunnerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessRunnerDialog)
{
    ui->setupUi(this);
    connect(ui->btnOk, &QPushButton::clicked, this, &ProcessRunnerDialog::DlgOk);
    connect(ui->btnCancel, &QPushButton::clicked, this, &ProcessRunnerDialog::DlgCancel);
}

ProcessRunnerDialog::~ProcessRunnerDialog()
{
    delete ui;
}

void ProcessRunnerDialog::processStarted()
{
    ui->btnOk->setEnabled(false);
    ui->btnCancel->setEnabled(true);
}

void ProcessRunnerDialog::processFinished()
{
    ui->btnOk->setEnabled(true);
    ui->btnCancel->setEnabled(false);
}

void ProcessRunnerDialog::logAdded(QString text)
{
    text.remove("\r\n");
    text.remove("\r");
    ui->logText->appendPlainText(text);
}

void ProcessRunnerDialog::DlgOk()
{
    setResult(QDialog::Accepted);
    close();
}

void ProcessRunnerDialog::DlgCancel()
{
    emit Cancel();
}


