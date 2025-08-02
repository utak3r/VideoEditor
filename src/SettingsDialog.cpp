#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "EncodingPreset.h"
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
    connect(ui->btnAddPreset, &QPushButton::clicked, this, &SettingsDialog::AddVideoPreset);
	connect(ui->btnEditPreset, &QPushButton::clicked, this, &SettingsDialog::EditVideoPreset);
    connect(ui->btnRemovePreset, &QPushButton::clicked, this, &SettingsDialog::RemoveVideoPreset);
    ReadSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::ReadSettings()
{
    theSettings->ReadSettings();

    int presetsCount = theSettings->videoPresets()->count();
    if (presetsCount > 0)
    {
        ui->tableVideoPresets->setRowCount(presetsCount);
        int i = 0;
        foreach (VideoPreset preset, (*theSettings->videoPresets()))
        {
			FillRowInfo(ui->tableVideoPresets, i, preset);
            i++;
        }
    }

}

void SettingsDialog::FillRowInfo(QTableWidget *table, int row, const VideoPreset &preset)
{
    table->setItem(row, 0, new QTableWidgetItem(preset.Name));
    table->setItem(row, 1, new QTableWidgetItem(preset.Extension));
    QString codecSettings = QString("Video: %1, preset: %2. Audio: %3, preset: %4")
        .arg(preset.VideoCodec)
        .arg(preset.VideoCodecPreset)
        .arg(preset.AudioCodec)
        .arg(preset.AudioCodecPreset);
    table->setItem(row, 2, new QTableWidgetItem(codecSettings));
}

void SettingsDialog::AddVideoPreset()
{
	auto newPresetDlg = new EncodingPreset(this);
	newPresetDlg->setWindowTitle(tr("Add Video Preset"));
	newPresetDlg->setModal(true);
	newPresetDlg->exec();
	auto preset = newPresetDlg->getPreset();
	qDebug() << "Adding new video preset:" << preset;
    ui->tableVideoPresets->insertRow(ui->tableVideoPresets->rowCount());
	FillRowInfo(ui->tableVideoPresets, ui->tableVideoPresets->rowCount() - 1, preset);
    theSettings->videoPresets()->append(preset);
    theSettings->WriteSettings();
}

void SettingsDialog::EditVideoPreset()
{
    if (int toEdit = ui->tableVideoPresets->currentRow(); toEdit >= 0)
    {
		auto editPresetDlg = new EncodingPreset(this);
		editPresetDlg->setWindowTitle(tr("Edit Video Preset"));
		editPresetDlg->setModal(true);
		editPresetDlg->setPreset((*theSettings->videoPresets())[toEdit]);
		editPresetDlg->exec();
		auto preset = editPresetDlg->getPreset();
        qDebug() << "Editing video preset:" << preset;
        (*theSettings->videoPresets())[toEdit] = preset;
        FillRowInfo(ui->tableVideoPresets, toEdit, preset);
        theSettings->WriteSettings();
    }
    else
    {
		qDebug() << "No preset selected for editing.";
    }
}

void SettingsDialog::RemoveVideoPreset()
{
    if (int toRemove = ui->tableVideoPresets->currentRow(); toRemove >= 0)
    {
        theSettings->videoPresets()->remove(toRemove);
        ui->tableVideoPresets->removeRow(toRemove);
        theSettings->WriteSettings();
    }
}

