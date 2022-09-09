#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <Settings.h>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(Settings *settings, QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void SearchForFFMPEGBinary();
    void VideoPresetChanged(int row, int column);

private:
    void ReadSettings();

    Ui::SettingsDialog *ui;
    Settings* theSettings;
};

#endif // SETTINGSDIALOG_H
