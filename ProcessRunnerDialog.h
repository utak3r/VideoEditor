#ifndef PROCESSRUNNERDIALOG_H
#define PROCESSRUNNERDIALOG_H

#include <QDialog>

namespace Ui {
class ProcessRunnerDialog;
}

class ProcessRunnerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessRunnerDialog(QWidget *parent = nullptr);
    ~ProcessRunnerDialog();

public slots:
    void processStarted();
    void processFinished();
    void logAdded(QString text);

private slots:
    void DlgOk();
    void DlgCancel();

signals:
    void Cancel();

private:
    Ui::ProcessRunnerDialog *ui;
};

#endif // PROCESSRUNNERDIALOG_H
