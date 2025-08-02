#include "EncodingPreset.h"
#include "ui_EncodingPreset.h"
#include "Codec.h"

EncodingPreset::EncodingPreset(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::EncodingPreset)
{
	ui->setupUi(this);

	// Get list of available codecs
	for (const auto& name : CodecFactory::instance().availablePlugins())
	{
		Codec* codec = CodecFactory::instance().create(name);
		if (codec)
		{
			if (codec->type() == Codec::CodecType::Video)
			{
				ui->cbVideoCodec->addItem(name);
			}
			else if (codec->type() == Codec::CodecType::Audio)
			{
				ui->cbAudioCodec->addItem(name);
			}
			delete codec;
		}
	}

	// Get available presets for chosen video codec
	ui->cbVideoCodec->setCurrentIndex(0); // Default to first codec
	getAvailablePresets(ui->cbVideoCodec->currentText(), ui->cbVideoPreset);
	ui->cbVideoPreset->setCurrentIndex(0);

	connect(ui->cbVideoCodec, &QComboBox::currentTextChanged, this, [=](const QString& codecName) {
		ui->cbVideoPreset->clear();
		getAvailablePresets(codecName, ui->cbVideoPreset);
		ui->cbVideoPreset->setCurrentIndex(0); // Reset to first preset
		});

	// Get available presets for chosen audio codec
	ui->cbAudioCodec->setCurrentIndex(0); // Default to first audio codec
	getAvailablePresets(ui->cbAudioCodec->currentText(), ui->cbAudioPreset);
	ui->cbAudioPreset->setCurrentIndex(0);
	connect(ui->cbAudioCodec, &QComboBox::currentTextChanged, this, [=](const QString& codecName) {
		ui->cbAudioPreset->clear();
		getAvailablePresets(codecName, ui->cbAudioPreset);
		ui->cbAudioPreset->setCurrentIndex(0); // Reset to first preset
		});
}

EncodingPreset::~EncodingPreset()
{
	delete ui;
}

void EncodingPreset::getAvailablePresets(const QString& codecName, QComboBox* comboBox)
{
	if (!codecName.isEmpty())
	{
		Codec* codec = CodecFactory::instance().create(codecName);
		comboBox->addItems(codec->getAvailablePresets());
		delete codec;
	}
}

void EncodingPreset::setPreset(const VideoPreset& preset)
{
	ui->lineEdit->setText(preset.Name);
	ui->cbVideoCodec->setCurrentText(preset.VideoCodec);
	ui->cbVideoPreset->setCurrentText(preset.VideoCodecPreset);
	ui->cbAudioCodec->setCurrentText(preset.AudioCodec);
	ui->cbAudioPreset->setCurrentText(preset.AudioCodecPreset);
}

VideoPreset EncodingPreset::getPreset() const
{
	auto preset = VideoPreset(
		ui->lineEdit->text().trimmed(),
		QString(".mp4"),
		ui->cbVideoCodec->currentText().trimmed(),
		ui->cbVideoPreset->currentText().trimmed(),
		ui->cbAudioCodec->currentText().trimmed(),
		ui->cbAudioPreset->currentText().trimmed()
	);
	qDebug() << "EncodingPreset::getPreset:" << preset;
	return preset;
}
