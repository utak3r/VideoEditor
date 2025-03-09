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

signals:
	void VideoLengthChanged(quint64 length);
	void FrameNumberChanged(quint64 frameNumber);

protected:
	bool openFile();
	QImage getImageFromFrame(const AVFrame* frame, const QSizeF dstSize) const;

private:
	QGraphicsScene* theScene;
	void decodeAndDisplayFrame();

private:
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
