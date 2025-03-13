#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QGraphicsView>
#include <QGraphicsScene>


struct AVFormatContext;
struct AVCodec;
struct AVCodecContext;
struct AVFrame;
struct AVStream;

class VideoPlayer : public QGraphicsView
{
	Q_OBJECT
	Q_PROPERTY(qint64 duration READ duration)
	Q_PROPERTY(qint64 position READ position)
	Q_PROPERTY(VideoPlayer::PlaybackState playbackState READ playbackState NOTIFY playbackStateChanged)
	Q_PROPERTY(bool CropEnabled READ getCropEnabled WRITE setCropEnabled NOTIFY CropEnabledChanged)

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

	enum VideoPlayerCropHandle
	{
		VPCropHandle_None,
		VPCropHandle_TopLeft,
		VPCropHandle_TopRight,
		VPCropHandle_BottomLeft,
		VPCropHandle_BottomRight
	};
	Q_ENUM(VideoPlayer::VideoPlayerCropHandle)

	enum VideoPlayerCropState
	{
		VPCropState_Inactive,
		VPCropState_Active,
		VPCropState_ResizingTL,
		VPCropState_ResizingTR,
		VPCropState_ResizingBL,
		VPCropState_ResizingBR,
		VPCropState_Translating
	};
	Q_ENUM(VideoPlayer::VideoPlayerCropState)

	VideoPlayer::PlaybackState playbackState() const { return thePlaybackState; }
	qint64 duration() const { return theVideoFrameCount; }
	qint64 position() const { return theCurrentVideoFrame; }
	QSize sizeHint() const override;
	bool getCropEnabled() { return theCropEnabled; }
	void setCropEnabled(bool enabled);
	QPointF mapToVideo(QPointF point);
	QPointF mapFromVideo(QPointF point);

signals:
	void durationChanged(qint64 length);
	void positionChanged(qint64 frameNumber);
	void playbackStateChanged(VideoPlayer::PlaybackState newState);
	void CropEnabledChanged(bool enabled);

public slots:
	bool openFile(const QString filename);
	void setSource(const QUrl& source);
	void closeFile();
	void play();
	void pause();
	void stop();
	void setPosition(qint64 position);

	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;

protected:
	QImage getImageFromFrame(const AVFrame* frame, const QSize dstSize) const;
	int64_t frameToTimestamp(int64_t frame);
	int64_t timestampToFrame(int64_t timestamp);
	bool isInsideCrop(QPointF point);
	std::tuple<bool, VideoPlayerCropHandle> isOverCropHandle(QPointF point);
	void paintCrop(QPainter* painter);
	void paintCropRectangle(QPainter* painter);
	void paintCropHandles(QPainter* painter);
	QRect cropHandleTLRect();
	QRect cropHandleTRRect();
	QRect cropHandleBLRect();
	QRect cropHandleBRRect();

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
	qint64 theCurrentVideoFrame;
	qint64 theVideoFrameCount;
	QString theAudioCodecName;
	int theAudioCodecSampleRate;
	AVFormatContext* theFormatContext;
	const AVCodec* theVideoCodec;
	AVCodecContext* theCodecContext;
	AVStream* theVideoStream;

private:
	bool theCropEnabled;
	VideoPlayer::VideoPlayerCropState theCurrentCropState;
	QRectF theVideoImageRectF;
	QRectF theCropRectF;
	QColor theCropColor;
	int theCropHandleSize;
};

#endif // VIDEOPLAYER_H
