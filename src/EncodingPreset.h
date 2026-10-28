#pragma once
#include "VideoPreset.h"
#include "ffmpegInterface.h"
#include <QComboBox>
#include <QDialog>

namespace Ui
{
class EncodingPreset;
}

class EncodingPreset : public QDialog
{
    Q_OBJECT

  public:
    EncodingPreset(IFFmpeg &ffmpeg, QWidget *parent = nullptr);
    ~EncodingPreset();

    void getAvailablePresets(const QString &codecName, QComboBox *comboBox);
    void setPreset(const VideoPreset &preset);
    VideoPreset getPreset() const;

  private:
    Ui::EncodingPreset *ui;
    IFFmpeg &ff;
};
