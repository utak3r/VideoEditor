#include "VEMainWindow.h"
#include "./ui_VEMainWindow.h"
#include <QFileDialog>
#include <QTimer>
#include <QProgressBar>
#include <SettingsDialog.h>
#include <VideoTranscoder.h>
#include <../version.h>
#include "Codec.h"
#include "codec_x264.h"
#include "codec_x265.h"
#include "codec_dnxhd.h"
#include "codec_aac.h"
#include "codec_pcms24le.h"


#define VIDEO_FILE_EXTENSIONS "*.mp4 *.mov *.avi *.mts *.mxf *.webm *.mkv *.m4v"

VEMainWindow::VEMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VEMainWindow)
    , theFFMPEG(nullptr)
    , theLastDir(".")
    , theMediaPlayer(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(PROJECT_VERSION_STRING_SHORT);

    // until cropping tool is finished
    //ui->grpCropping->hide();

    theVideoPlayer = ui->videoPlayer;

    ReloadSettings();
    theLastDir = theSettings.lastDir();
    this->setGeometry(theSettings.mainWndGeometry());
    theMarks.Reset(0);

    connect(ui->btnExit, &QPushButton::clicked, this, &VEMainWindow::ExitApp);
    connect(ui->btnOpenVideo, &QPushButton::clicked, this, &VEMainWindow::OpenVideo);

    theVideoPlayer->show();

    connect(ui->btnMarkIn, &QPushButton::clicked, this, &VEMainWindow::SetMarkIn);
    connect(ui->btnMarkOut, &QPushButton::clicked, this, &VEMainWindow::SetMarkOut);
    connect(ui->btnResetMarks, &QPushButton::clicked, this, &VEMainWindow::ResetMarks);
    connect(theVideoPlayer, &VideoPlayer::durationChanged, this, &VEMainWindow::VideoDurationChanged);
    connect(theVideoPlayer, &VideoPlayer::playbackStateChanged, this, &VEMainWindow::VideoPlaybackStateChanged);
    connect(theVideoPlayer, &VideoPlayer::positionChanged, this, &VEMainWindow::PlaybackPositionChanged);
    connect(ui->videoPosSlider, &QSlider::sliderMoved, this, &VEMainWindow::PlaybackSliderMoved);
    connect(ui->btnPlayPause, &QPushButton::clicked, this, &VEMainWindow::PlayPause);
    connect(ui->btnConvert, &QPushButton::clicked, this, &VEMainWindow::Convert);
    connect(ui->btnSettings, &QPushButton::clicked, this, &VEMainWindow::ShowSettings);
//    connect(ui->videoPlayer, &VideoPlayer::VideoSizeChanged, this, &VEMainWindow::VideoSizeChanged);
    connect(ui->grpCropping, &QGroupBox::toggled, this, [=](bool on) { theVideoPlayer->setCropEnabled(on); });

    // Register codecs
    static CodecRegistrar<CodecX264> registrarX264("libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
    static CodecRegistrar<CodecX265> registrarX265("libx265 H.265 / HEVC");
	static CodecRegistrar<CodecDnXHD> registrarDnXHD("SMPTE VC-3/DNxHD");
    static CodecRegistrar<CodecAAC> registrarAAC("AAC (Advanced Audio Coding)");
	static CodecRegistrar<CodecPCMs24le> registrarPCMs24le("PCM signed 24-bit little-endian");

    for (const auto& name : CodecFactory::instance().availablePlugins())
    {
        qDebug() << "Zarejestrowany codec:" << name;
    }

#ifdef QT_DEBUG
    //theVideoPlayer->openFile("c:\\Users\\piotr\\devel\\sandbox\\VideoEditor\\test_video.mp4");
    //currentVideoFile = QFileInfo("c:\\Users\\piotr\\devel\\sandbox\\VideoEditor\\test_video.mp4");
    theVideoPlayer->openFile("c:\\Users\\piotr\\Videos\\Division2_20250312_GoldenBullet_vs_boss_DoloresJones.mp4");
    currentVideoFile = QFileInfo("c:\\Users\\piotr\\Videos\\Division2_20250312_GoldenBullet_vs_boss_DoloresJones.mp4");
    theVideoPlayer->play();

	QStringList videoCodecs, audioCodecs;
    VideoTranscoder::getAvailableEncoders(videoCodecs, audioCodecs);
#endif
}

VEMainWindow::~VEMainWindow()
{
    if (theMediaPlayer)
    if (theMediaPlayer->playbackState() == QMediaPlayer::PlayingState)
        theMediaPlayer->stop();

    //theSettings.setffmpeg(theFFMPEG->binPath());
    theSettings.setLastDir(theLastDir);
    theSettings.setMainWndGeometry(this->geometry());
    theSettings.setVideoPresets(theVideoPresets);
    theSettings.setScalingWidth(ui->valScaleWidth->value());
    theSettings.setScalingHeight(ui->valScaleHeight->value());
    theSettings.setScalingFilter(ui->cbxScalingFlags->currentIndex());
    theSettings.WriteSettings();
    delete theMediaPlayer;
    delete theFFMPEG;
    delete ui;
}

void VEMainWindow::ExitApp()
{
    close();
}

void VEMainWindow::ReloadSettings()
{
    theSettings.ReadSettings();

    theVideoPresets.clear();
    theVideoPresets.append((*theSettings.videoPresets()));
    ui->cbxPresets->clear();
    foreach (VideoPreset preset, theVideoPresets)
    {
        ui->cbxPresets->addItem(preset.Name, preset.asVariant());
    }

    ui->valScaleWidth->setValue(theSettings.scalingWidth());
    ui->valScaleHeight->setValue(theSettings.scalingHeight());
    ui->cbxScalingFlags->setCurrentIndex(theSettings.scalingFilter());

    ui->statusbar->showMessage(QString("%1").arg(PROJECT_VERSION_STRING_FULL));
}

void VEMainWindow::OpenVideo()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open video file"),
                                                    theLastDir,
                                                    tr("Video Files") + QLatin1String(" (") + VIDEO_FILE_EXTENSIONS + QLatin1String(")")
                                                    );
    if (!filename.isEmpty())
    {
        theVideoPlayer->openFile(filename);
        currentVideoFile = QFileInfo(filename);
        theLastDir = currentVideoFile.absoluteDir().absolutePath();
        theVideoPlayer->play();
    }
}

void VEMainWindow::ShowSettings()
{
    SettingsDialog* dlg = new SettingsDialog(&theSettings, this);
    dlg->setModal(true);
    dlg->exec();
    ReloadSettings();
}

void VEMainWindow::VideoDurationChanged(qint64 duration)
{
    theMarks.Reset(duration);
    ui->videoPosSlider->setRange(0, duration);
    ui->videoPosSlider->setSingleStep(duration / 200);
    ui->videoPosSlider->setPageStep(duration / 2000);
    ui->videoPosSlider->setValue(0);
}

void VEMainWindow::VideoPlaybackStateChanged(VideoPlayer::PlaybackState newState)
{
    if (newState == VideoPlayer::PausedState || newState == VideoPlayer::StoppedState)
    {
        ui->btnPlayPause->setText(tr("Play"));
    }
    else if (newState == VideoPlayer::PlayingState)
    {
        ui->btnPlayPause->setText(tr("Pause"));
    }
}

void VEMainWindow::VideoSizeChanged(QSizeF videoSize)
{
    ui->valScaleWidth->setValue(videoSize.width());
    ui->valScaleHeight->setValue(videoSize.height());
}

void VEMainWindow::PlayPause()
{
    if (theVideoPlayer->playbackState() == VideoPlayer::PausedState || theVideoPlayer->playbackState() == VideoPlayer::StoppedState)
    {
        theVideoPlayer->play();
    }
    else if (theVideoPlayer->playbackState() == VideoPlayer::PlayingState)
    {
        theVideoPlayer->pause();
    }
}

void VEMainWindow::PlaybackPositionChanged(qint64 position)
{
    if (position >= 0)
        if (!ui->videoPosSlider->isSliderDown())
            ui->videoPosSlider->setValue(position);
}

void VEMainWindow::PlaybackSliderMoved(int value)
{
    theVideoPlayer->setPosition(value);
}

void VEMainWindow::SetMarkIn()
{
    theMarks.setMarkIn(theVideoPlayer->position());
    ui->videoPosSlider->setMarkIn((int)theMarks.MarkIn());
    theVideoPlayer->setMarkers(theMarks.CurrentRange(1));
}

void VEMainWindow::SetMarkOut()
{
    theMarks.setMarkOut(theVideoPlayer->position());
    ui->videoPosSlider->setMarkOut((int)theMarks.MarkOut());
    theVideoPlayer->setMarkers(theMarks.CurrentRange(1));
}

void VEMainWindow::ResetMarks()
{
    theMarks.Reset(theVideoPlayer->duration());
    ui->videoPosSlider->resetMarks();
    theVideoPlayer->setMarkers(theMarks.CurrentRange(1));
}

void VEMainWindow::Convert()
{
    QString outFilename = QFileDialog::getSaveFileName(this, tr("Save video as..."),
        currentVideoFile.absoluteFilePath() + "_converted.mp4",
        tr("Video Files") + QLatin1String(" (") + VIDEO_FILE_EXTENSIONS + QLatin1String(")"));
    if (!outFilename.isEmpty())
    {
		std::unique_ptr<VideoTranscoder> transcoder(new VideoTranscoder(this));
		transcoder->setInputFile(currentVideoFile.absoluteFilePath());
        transcoder->setOutputFile(outFilename);
		transcoder->setMarks(theMarks);

        VideoPreset codec = ui->cbxPresets->currentData().value<VideoPreset>();
		transcoder->setOutputVideoCodecName(codec.VideoCodec);
        transcoder->setOutputVideoCodecPreset(codec.VideoCodecPreset);
		transcoder->setOutputAudioCodecName(codec.AudioCodec);
		transcoder->setOutputAudioCodecPreset(codec.AudioCodecPreset);

        transcoder->setMetadata("encoder", QString("%1").arg(PROJECT_VERSION_STRING_FULL));
        transcoder->setMetadata("comment", QString("Processed with %1").arg(PROJECT_VERSION_STRING_FULL));

        if (ui->grpScaling->isChecked())
        {
			transcoder->setOutputResolution(ui->valScaleWidth->value(), ui->valScaleHeight->value());
            transcoder->setOutputScalingFilter(ui->cbxScalingFlags->currentIndex());
        }

        connect(transcoder.get(), &VideoTranscoder::recodeProgress, this, [=](int progress)
            {
                ui->statusbar->showMessage(tr("Recode progress: %1%").arg(progress));
			});
        connect(transcoder.get(), &VideoTranscoder::recodeFinished, this, [=]()
            {
                ui->statusbar->showMessage(tr("Recode finished"));
                QTimer::singleShot(2000, this, [=]()
                    {
                        ui->statusbar->showMessage(QString("%1").arg(PROJECT_VERSION_STRING_FULL));
                    });
            });
        connect(transcoder.get(), &VideoTranscoder::recodeError, this, [=](const QString& errorMessage)
            {
                ui->statusbar->showMessage(tr("Recode error: %1").arg(errorMessage));
                QTimer::singleShot(2000, this, [=]()
                    {
                        ui->statusbar->showMessage(QString("%1").arg(PROJECT_VERSION_STRING_FULL));
                    });
			});

        transcoder->transcode();
    }

}
