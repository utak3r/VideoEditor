#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QGraphicsView>
#include <QGraphicsScene>


class AVFormatContext;
class AVCodec;
class AVCodecContext;
class AVFrame;

class VideoPlayer : public QGraphicsView
{
	Q_OBJECT
public:
	VideoPlayer(QWidget* parent = nullptr);
	~VideoPlayer();

	enum PlaybackState
	{
		StoppedState,
		PlayingState,
		PausedState
	};
	Q_ENUM(VideoPlayer::PlaybackState)

	VideoPlayer::PlaybackState playbackState() const { return thePlaybackState; }

signals:
	void durationChanged(quint64 length);
	void positionChanged(quint64 frameNumber);
	void playbackStateChanged(VideoPlayer::PlaybackState newState);

public slots:
	bool openFile(const QString filename);
	void closeFile();
	void play();
	void pause();
	//void stop();
	void setPosition(quint64 position);

protected:
	QImage getImageFromFrame(const AVFrame* frame, const QSize dstSize) const;
	void resizeEvent(QResizeEvent* event) override;

private:
	QGraphicsScene* theScene;
	void decodeAndDisplayFrame();

private:
	QTimer* thePlayerTimer;
	VideoPlayer::PlaybackState thePlaybackState;
	QSize theViewSize;
	QString theVideoFormatName;
	QString theVideoCodecName;
	QSize theVideoSize;
	int theVideoStreamIndex;
	double theVideoFPS;
	int theVideoPixelFormat;
	uint64_t theCurrentVideoFrame;
	uint64_t theVideoFrameCount;
	QString theAudioCodecName;
	int theAudioCodecSampleRate;
	AVFormatContext* theFormatContext;
	const AVCodec* theVideoCodec;
	AVCodecContext* theCodecContext;
};

#endif // VIDEOPLAYER_H
