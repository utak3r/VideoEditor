#ifndef VEMAINWINDOW_H
#define VEMAINWINDOW_H

#include <QMainWindow>
#include <Settings.h>
#include <QFileInfo>
#include <ffmpeg.h>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <TimelineMarks.h>
#include <VideoPreset.h>
#include <VideoPlayer.h>

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
    void ReloadSettings();
    void OpenVideo();
    void Convert();
    void ShowSettings();
    void VideoDurationChanged(qint64 duration);
    void VideoPlaybackStateChanged(VideoPlayer::PlaybackState newState);
    void VideoSizeChanged(QSizeF videoSize);
    void PlayPause();
    void PlaybackPositionChanged(qint64 position);
    void PlaybackSliderMoved(int value);
    void SetMarkIn();
    void SetMarkOut();
    void ResetMarks();
	void CropWindowChanged(const QRect& cropRect);
    void CropValueChanged(int value);

private:
    Ui::VEMainWindow *ui;
    Settings theSettings;
    QFileInfo currentVideoFile;
    FFMPEG* theFFMPEG;
    QString theLastDir;
    QMediaPlayer* theMediaPlayer;
	VideoPlayer* theVideoPlayer;
    TimelineMarks theMarks;
    QList<VideoPreset> theVideoPresets;
};
#endif // VEMAINWINDOW_H
