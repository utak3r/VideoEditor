#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <Settings.h>
#include <ffmpegInterface.h>

namespace Ui {
class SettingsDialog;
}

class QTableWidget;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(Settings *settings, IFFmpeg& ffmpeg, QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void AddVideoPreset();
	void EditVideoPreset();
    void RemoveVideoPreset();

private:
    void ReadSettings();
    void FillRowInfo(QTableWidget* table, int row, const VideoPreset& preset);

    Ui::SettingsDialog *ui;
    Settings* theSettings;
    IFFmpeg& ff;
};

#endif // SETTINGSDIALOG_H
