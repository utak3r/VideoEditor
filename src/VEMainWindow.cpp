#include "VEMainWindow.h"
#include "./ui_VEMainWindow.h"
#include <QFileDialog>
#include <SettingsDialog.h>
#include <../version.h>

#define VIDEO_FILE_EXTENSIONS "*.mp4 *.mov *.avi *.mts *.mxf *.webm"

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
    ui->grpCropping->hide();

    ReloadSettings();
    theLastDir = theSettings.lastDir();
    this->setGeometry(theSettings.mainWndGeometry());
    theMarks.Reset(0);

    connect(ui->btnExit, &QPushButton::clicked, this, &VEMainWindow::ExitApp);
    connect(ui->btnOpenVideo, &QPushButton::clicked, this, &VEMainWindow::OpenVideo);

    theMediaPlayer = new QMediaPlayer(this);
    theMediaPlayer->setLoops(QMediaPlayer::Infinite);
    theMediaPlayer->setVideoOutput(ui->videoPlayer->getVideoOutput());
    ui->videoPlayer->show();

    connect(ui->btnMarkIn, &QPushButton::clicked, this, &VEMainWindow::SetMarkIn);
    connect(ui->btnMarkOut, &QPushButton::clicked, this, &VEMainWindow::SetMarkOut);
    connect(ui->btnResetMarks, &QPushButton::clicked, this, &VEMainWindow::ResetMarks);
    connect(theMediaPlayer, &QMediaPlayer::durationChanged, this, &VEMainWindow::VideoDurationChanged);
    connect(theMediaPlayer, &QMediaPlayer::playbackStateChanged, this, &VEMainWindow::VideoPlaybackStateChanged);
    connect(theMediaPlayer, &QMediaPlayer::positionChanged, this, &VEMainWindow::PlaybackPositionChanged);
    connect(ui->videoPosSlider, &QSlider::sliderMoved, this, &VEMainWindow::PlaybackSliderMoved);
    connect(ui->btnPlayPause, &QPushButton::clicked, this, &VEMainWindow::PlayPause);
    connect(ui->btnConvert, &QPushButton::clicked, this, &VEMainWindow::Convert);
    connect(ui->btnSettings, &QPushButton::clicked, this, &VEMainWindow::ShowSettings);
    connect(ui->videoPlayer, &VideoPlayer::VideoSizeChanged, this, &VEMainWindow::VideoSizeChanged);
    connect(ui->grpCropping, &QGroupBox::toggled, this, [=](bool on) { ui->videoPlayer->setCropEnabled(on); });

#ifdef QT_DEBUG
    theMediaPlayer->setSource(QUrl::fromLocalFile("d:\\devel\\sandbox\\VideoEditor\\flip.mp4"));
    currentVideoFile = QFileInfo("d:\\devel\\sandbox\\VideoEditor\\flip.mp4");
    theMediaPlayer->play();
#endif
}

VEMainWindow::~VEMainWindow()
{
    if (theMediaPlayer->playbackState() == QMediaPlayer::PlayingState)
        theMediaPlayer->stop();

    theSettings.setffmpeg(theFFMPEG->binPath());
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
    theFFMPEG = new FFMPEG(theSettings.ffmpeg(), this);
    ui->statusbar->showMessage(theFFMPEG->binVersion());

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
        theMediaPlayer->setSource(QUrl::fromLocalFile(filename));
        currentVideoFile = QFileInfo(filename);
        theLastDir = currentVideoFile.absoluteDir().absolutePath();
        theMediaPlayer->play();
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

void VEMainWindow::VideoPlaybackStateChanged(QMediaPlayer::PlaybackState newState)
{
    if (newState == QMediaPlayer::PausedState || newState == QMediaPlayer::StoppedState)
    {
        ui->btnPlayPause->setText(tr("Play"));
    }
    else if (newState == QMediaPlayer::PlayingState)
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
    if (theMediaPlayer->playbackState() == QMediaPlayer::PausedState || theMediaPlayer->playbackState() == QMediaPlayer::StoppedState)
    {
        theMediaPlayer->play();
    }
    else if (theMediaPlayer->playbackState() == QMediaPlayer::PlayingState)
    {
        theMediaPlayer->pause();
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
    theMediaPlayer->setPosition(value);
}

void VEMainWindow::SetMarkIn()
{
    theMarks.setMarkIn(theMediaPlayer->position());
    ui->videoPosSlider->setMarkIn((int)theMarks.MarkIn());
}

void VEMainWindow::SetMarkOut()
{
    theMarks.setMarkOut(theMediaPlayer->position());
    ui->videoPosSlider->setMarkOut((int)theMarks.MarkOut());
}

void VEMainWindow::ResetMarks()
{
    theMarks.Reset(theMediaPlayer->duration());
    ui->videoPosSlider->resetMarks();
}

void VEMainWindow::Convert()
{
    if (theFFMPEG)
    {
        if (!theFFMPEG->binPath().isEmpty())
        {
            if (theFFMPEG->state() == FFMPEG::Available)
            {
                if (ui->cbxPresets->currentData().canConvert<VideoPreset>())
                {
                    VideoPreset codec = ui->cbxPresets->currentData().value<VideoPreset>();
                    QString outFilename = QFileDialog::getSaveFileName(this, tr("Save video as..."),
                                                                       currentVideoFile.absoluteFilePath() + "_converted" + codec.Extension,
                                                                       tr("Video Files") + QLatin1String(" (") + VIDEO_FILE_EXTENSIONS + QLatin1String(")"));
                    if (!outFilename.isEmpty())
                    {
                        std::tuple<bool, int, int, QString> scaling = FFMPEG::getScalingTuple(ui->grpScaling->isChecked(), ui->valScaleWidth->value(), ui->valScaleHeight->value(), ui->cbxScalingFlags->currentIndex());
                        theFFMPEG->Convert(currentVideoFile.absoluteFilePath(), codec.CommandLine, &theMarks, scaling, outFilename);
                    }
                }
            }
        }
    }
}
