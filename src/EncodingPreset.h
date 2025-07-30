#pragma once
#include <QDialog>
#include "VideoPreset.h"

namespace Ui {
	class EncodingPreset;
}

class EncodingPreset : public QDialog
{
	Q_OBJECT

public:
	EncodingPreset(QWidget* parent = nullptr);
	~EncodingPreset();

	void setPreset(const VideoPreset& preset);
	VideoPreset getPreset() const;

private:
	Ui::EncodingPreset* ui;

};
