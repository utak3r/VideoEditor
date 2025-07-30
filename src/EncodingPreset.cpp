#include "EncodingPreset.h"
#include "ui_EncodingPreset.h"

EncodingPreset::EncodingPreset(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::EncodingPreset)
{
	ui->setupUi(this);

	ui->cbVideoCodec->addItems({ "libx264", "libx265", "libvpx-vp9", "libaom-av1" });
	ui->cbVideoPreset->addItems({ "ultrafast", "superfast", "veryfast", "faster", "fast", "medium", "slow", "slower", "veryslow" });
	ui->cbVideoTune->addItems({ "none", "film", "animation", "grain", "stillimage", "fastdecode", "zerolatency" });
	ui->cbVideoProfile->addItems({ "baseline", "main", "high", "high10", "high422", "high444" });

	ui->cbVideoCodec->setCurrentIndex(0); // libx264
	ui->cbVideoPreset->setCurrentIndex(5); // medium
	ui->cbVideoTune->setCurrentIndex(0); // none
	ui->cbVideoProfile->setCurrentIndex(2); // high

	ui->cbxAudioCopy->setChecked(true);
}

EncodingPreset::~EncodingPreset()
{
	delete ui;
}

void EncodingPreset::setPreset(const VideoPreset& preset)
{
	ui->lineEdit->setText(preset.Name);
	ui->cbVideoCodec->setCurrentText(preset.VideoCodec);
	ui->cbVideoPreset->setCurrentText(preset.VideoCodecPreset);
	ui->cbVideoTune->setCurrentText(preset.VideoCodecTune);
	ui->cbVideoProfile->setCurrentText(preset.VideoCodecProfile);
	ui->cbxAudioCopy->setChecked(preset.AudioCodec.isEmpty());
}

VideoPreset EncodingPreset::getPreset() const
{
	auto preset = VideoPreset(
		ui->lineEdit->text().trimmed(),
		QString(".mp4"),
		ui->cbVideoCodec->currentText().trimmed(),
		ui->cbVideoPreset->currentText().trimmed(),
		ui->cbVideoTune->currentText().trimmed() == "none" ? QString("") : ui->cbVideoTune->currentText().trimmed(),
		ui->cbVideoProfile->currentText().trimmed(),
		ui->cbxAudioCopy->isChecked() ? QString("") : QString("")
	);
	qDebug() << "EncodingPreset::getPreset:" << preset;
	return preset;
}
