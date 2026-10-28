#ifndef VEMAINWINDOW_H
#define VEMAINWINDOW_H

#include <QFileInfo>
#include <QMainWindow>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <Settings.h>
#include <TimelineMarks.h>
#include <VideoPlayer.h>
#include <VideoPreset.h>

QT_BEGIN_NAMESPACE
namespace Ui
{
class VEMainWindow;
}
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
    void CropWindowChanged(const QRect &cropRect);
    void CropValueChanged(int value);

  private:
    Ui::VEMainWindow *ui;
    Settings theSettings;
    QFileInfo currentVideoFile;
    QString theLastDir;
    QMediaPlayer *theMediaPlayer;
    VideoPlayer *theVideoPlayer;
    TimelineMarks theMarks;
    QList<VideoPreset> theVideoPresets;
};
#endif // VEMAINWINDOW_H
