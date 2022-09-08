#include "VEMainWindow.h"
#include "./ui_VEMainWindow.h"
#include <QFileDialog>

VEMainWindow::VEMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VEMainWindow)
    , theFFMPEG(nullptr)
    , theMediaPlayer(nullptr)
{
    ui->setupUi(this);
    connect(ui->btnExit, &QPushButton::clicked, this, &VEMainWindow::ExitApp);
    connect(ui->btnOpenVideo, &QPushButton::clicked, this, &VEMainWindow::OpenVideo);

    theFFMPEG = new FFMPEG("c:\\Program Files\\ffmpeg\\bin\\ffmpeg.exe", this);
    theMarks.Reset(0);

    theVideoPresets.clear();
    theVideoPresets.append(VideoPreset("H.264 AAC", ".mp4", "-c:v libx264 -preset medium -tune film -c:a aac"));
    theVideoPresets.append(VideoPreset("DNxHD 185Mbps PCM s24LE", ".mov", "-c:v dnxhd -b:v 185M -c:a pcm_s24le"));
    theVideoPresets.append(VideoPreset("ProRes YUV422", ".mov", "-c:v prores_ks -profile:v 3 -vendor ap10 -pix_fmt yuv422p10le"));
    theVideoPresets.append(VideoPreset("H.264 720p AAC", ".mp4", "-vf scale=-1:720:flags=lanczos -c:v libx264 -preset medium -tune film -c:a aac"));
    theVideoPresets.append(VideoPreset("H.264 no audio", ".mp4", "-c:v libx264 -preset medium -tune film -an"));
    theVideoPresets.append(VideoPreset("Crop & GIF", ".gif", "-filter_complex \"[0:v] fps=10,crop=305:203:42:660,scale=240:-1,split [a][b];[a] palettegen [p];[b][p] paletteuse\""));

    ui->cbxPresets->clear();
    foreach (VideoPreset preset, theVideoPresets)
    {
        ui->cbxPresets->addItem(preset.Name, preset.asVariant());
    }


    theMediaPlayer = new QMediaPlayer(this);
    theMediaPlayer->setLoops(QMediaPlayer::Infinite);
    theMediaPlayer->setVideoOutput(ui->videoPlayer);
    ui->videoPlayer->show();


    connect(ui->btnMarkIn, &QPushButton::clicked, this, &VEMainWindow::SetMarkIn);
    connect(ui->btnMarkOut, &QPushButton::clicked, this, &VEMainWindow::SetMarkOut);
    connect(theMediaPlayer, &QMediaPlayer::durationChanged, this, &VEMainWindow::VideoDurationChanged);
    connect(theMediaPlayer, &QMediaPlayer::playbackStateChanged, this, &VEMainWindow::VideoPlaybackStateChanged);
    connect(theMediaPlayer, &QMediaPlayer::positionChanged, this, &VEMainWindow::PlaybackPositionChanged);
    connect(ui->videoPosSlider, &QSlider::sliderMoved, this, &VEMainWindow::PlaybackSliderMoved);
    connect(ui->btnPlayPause, &QPushButton::clicked, this, &VEMainWindow::PlayPause);
    connect(ui->btnConvert, &QPushButton::clicked, this, &VEMainWindow::Convert);

    // for debug
    theMediaPlayer->setSource(QUrl::fromLocalFile("d:\\devel\\sandbox\\VideoEditor\\flip.mp4"));
    currentVideoFile = QFileInfo("d:\\devel\\sandbox\\VideoEditor\\flip.mp4");
    theMediaPlayer->play();

}

VEMainWindow::~VEMainWindow()
{
    if (theMediaPlayer->playbackState() == QMediaPlayer::PlayingState)
        theMediaPlayer->stop();
    delete theMediaPlayer;
    delete theFFMPEG;
    delete ui;
}

void VEMainWindow::ExitApp()
{
    close();
}

void VEMainWindow::OpenVideo()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open video file"),
                                                    ".",
                                                    tr("Video Files (*.mp4 *.mov *.avi)")
                                                    );
    if (!filename.isEmpty())
    {
        theMediaPlayer->setSource(QUrl::fromLocalFile(filename));
        currentVideoFile = QFileInfo(filename);
        theMediaPlayer->play();
    }
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
    theMarks.MarkIn = theMediaPlayer->position();
    ui->videoPosSlider->setMarkIn((int)theMarks.MarkIn);
}

void VEMainWindow::SetMarkOut()
{
    theMarks.MarkOut = theMediaPlayer->position();
    ui->videoPosSlider->setMarkOut((int)theMarks.MarkOut);
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
                                                                       tr("Video files (*.mp4 *.mov *.avi)"));
                    if (!outFilename.isEmpty())
                    {
                        theFFMPEG->Convert(currentVideoFile.absoluteFilePath(), codec.CommandLine, &theMarks, outFilename);
                    }
                }
            }
        }
    }
}
