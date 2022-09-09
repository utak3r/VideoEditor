#ifndef VEMAINWINDOW_H
#define VEMAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <ffmpeg.h>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <TimelineMarks.h>
#include <VideoPreset.h>

QT_BEGIN_NAMESPACE
namespace Ui { class VEMainWindow; }
QT_END_NAMESPACE

class VEMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    VEMainWindow(QWidget *parent = nullptr);
    ~VEMainWindow();

private slots:
    void ExitApp();
    void OpenVideo();
    void Convert();
    void VideoDurationChanged(qint64 duration);
    void VideoPlaybackStateChanged(QMediaPlayer::PlaybackState newState);
    void PlayPause();
    void PlaybackPositionChanged(qint64 position);
    void PlaybackSliderMoved(int value);
    void SetMarkIn();
    void SetMarkOut();
    void ResetMarks();

private:
    Ui::VEMainWindow *ui;
    QFileInfo currentVideoFile;
    FFMPEG* theFFMPEG;
    QMediaPlayer* theMediaPlayer;
    TimelineMarks theMarks;
    QList<VideoPreset> theVideoPresets;
};
#endif // VEMAINWINDOW_H
