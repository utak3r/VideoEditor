#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog(Settings *settings, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , theSettings(settings)
{
    ui->setupUi(this);
    ui->tableVideoPresets->horizontalHeader()->setStretchLastSection(true);
    ui->tabWidget->setCurrentIndex(0);

    connect(ui->btnClose, &QPushButton::clicked, this, [=]() { setResult(QDialog::Accepted); accept(); });
    connect(ui->btnFFMPEGBinPath, &QPushButton::clicked, this, &SettingsDialog::SearchForFFMPEGBinary);
    ReadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::ReadSettings()
{
    theSettings->ReadSettings();
    ui->txtFFMPEGBinPath->setText(theSettings->ffmpeg());

    disconnect(ui->tableVideoPresets);
    int presetsCount = theSettings->videoPresets()->count();
    if (presetsCount > 0)
    {
        ui->tableVideoPresets->setRowCount(presetsCount);
        int i = 0;
        foreach (VideoPreset preset, (*theSettings->videoPresets()))
        {
            QTableWidgetItem *item1 = new QTableWidgetItem(preset.Name);
            ui->tableVideoPresets->setItem(i, 0, item1);
            QTableWidgetItem *item2 = new QTableWidgetItem(preset.Extension);
            ui->tableVideoPresets->setItem(i, 1, item2);
            QTableWidgetItem *item3 = new QTableWidgetItem(preset.CommandLine);
            ui->tableVideoPresets->setItem(i, 2, item3);
            i++;
        }
        connect(ui->tableVideoPresets, &QTableWidget::cellChanged, this, &SettingsDialog::VideoPresetChanged);
    }

}

void SettingsDialog::SearchForFFMPEGBinary()
{
    QString binPath = QFileDialog::getOpenFileName(this, tr("Find FFMPEG..."),
                                                   theSettings->ffmpeg(),
                                                   tr("Executable files (*.exe)"));
    if (!binPath.isEmpty())
    {
        theSettings->setffmpeg(binPath);
        theSettings->WriteSettings();
    }
}

void SettingsDialog::VideoPresetChanged(int row, int column)
{
    QString newValue = ui->tableVideoPresets->item(row, column)->text();
    switch (column)
    {
    case 0:
        {
        (*theSettings->videoPresets())[row].Name = newValue;
        break;
        }
    case 1:
        {
        (*theSettings->videoPresets())[row].Extension = newValue;
        break;
        }
    case 2:
        {
        (*theSettings->videoPresets())[row].CommandLine = newValue;
        break;
        }
    }
    theSettings->WriteSettings();
}
