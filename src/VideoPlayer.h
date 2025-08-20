#pragma once
// https://doc.qt.io/qt-6/graphicsview.html

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QResizeEvent>
#include <QTime>
#include "CropRectangle.h"

class VideoPlayer : public QGraphicsView
{
    Q_OBJECT
        Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
        Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
        Q_PROPERTY(VideoPlayer::PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)
        Q_PROPERTY(bool CropEnabled READ getCropEnabled WRITE setCropEnabled NOTIFY CropEnabledChanged)

public:
    enum AspectMode
    {
        FitInView,
        CropToFill
    };
	Q_ENUM(VideoPlayer::AspectMode)

    enum PlaybackState
    {
        StoppedState,
        PlayingState,
        PausedState
    };
    Q_ENUM(VideoPlayer::PlaybackState)

    explicit VideoPlayer(QWidget* parent = nullptr);

    QMediaPlayer* player() const;
    QAudioOutput* audioOutput() const;

    void setAspectMode(AspectMode mode);
    VideoPlayer::PlaybackState playbackState() const;
    qint64 position() const;
    qint64 duration() const;
    bool getCropEnabled();
    void setCropEnabled(bool enabled);

signals:
    void playbackStateChanged(VideoPlayer::PlaybackState newState);
    void positionChanged(qint64 position);
	void durationChanged(qint64 duration);
    void CropEnabledChanged(bool enabled);

public slots:
    void openFile(const QString& filename);
    void play();
    void pause();
    void stop();
    void setPosition(qint64 position);
    void setMarkers(const QString& range);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateTimestamp(qint64 ms);
	void updatePosition(qint64 position);
    void updateDuration(qint64 duration);

private:
    void resizeScene();
    void repositionTimestamp();

    QGraphicsScene* theScene;
    QMediaPlayer* theMediaPlayer;
    QAudioOutput* theAudioOutput;
    QGraphicsVideoItem* theVideoItem;
    QGraphicsTextItem* theTimestampItem;
    QGraphicsRectItem* theTimestampBgItem;
    AspectMode theAspectMode;
    qint64 theDuration;
    QString theMarksRange;
    bool theCropEnabled;
	CropRectangle* theCropRectItem;
    QRectF theCropRectF;
};
