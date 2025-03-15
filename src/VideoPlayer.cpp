#include "VideoPlayer.h"
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QVideoSink>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QElapsedTimer>
#include "TimelineMarks.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// https://ffmpeg.org/doxygen/7.0/index.html

VideoPlayer::VideoPlayer(QWidget* parent)
	: QGraphicsView(parent)
	, theCurrentPosition(-1)
	, theCurrentCropState(VPCropState_Inactive)
	, theCropEnabled(false)
	, theCropColor(QColor(Qt::white))
	, theCropHandleSize(8)
	, theCropRectF(QRectF(0, 0, 0, 0))
	, theVideoImageRectF(QRectF(0, 0, 0, 0))
	, theMarksRange("---")
{
	theViewSize = this->sceneRect().size().toSize();
	theScene = new QGraphicsScene(parent);
	theScene->setBackgroundBrush(Qt::black);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	this->setScene(theScene);
	setAlignment(Qt::AlignCenter);

	theFormatContext = nullptr;
	theVideoCodec = nullptr;
	theCodecContext = nullptr;
	theVideoStream = nullptr;
	theVideoPixelFormat = AV_PIX_FMT_RGB24;

	thePlaybackState = StoppedState;
	thePlayerTimer = new QTimer(this);
	connect(thePlayerTimer, &QTimer::timeout, this, &VideoPlayer::decodeAndDisplayFrame);

	setMouseTracking(true);
}

VideoPlayer::~VideoPlayer()
{
	closeFile();
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
	QGraphicsView::resizeEvent(event);
	theViewSize = event->size();
	
	if (thePlaybackState == PausedState)
	{
		//decodeAndDisplayFrame();
		// Need to find a way of recoding a current frame, without getting a next one.
	}
}

QSize VideoPlayer::sizeHint() const
{
	auto size = theVideoSize;
	if (size.isValid())
		return size;

	return QWidget::sizeHint();
}

/*!
 * \brief VideoPlayer::paintEvent Renders video and draws a crop tool if enabled.
 */
void VideoPlayer::paintEvent(QPaintEvent* event)
{
	QGraphicsView::paintEvent(event);

	// find the video image and center it
	foreach(QGraphicsItem * item, scene()->items())
	{
		QPixmap pixmap = qgraphicsitem_cast<QGraphicsPixmapItem*>(item)->pixmap();
		if (!pixmap.isNull())
		{
			QRectF rect = item->sceneBoundingRect();
			setSceneRect(rect);
			theVideoImageRectF = QRectF(
				mapFromScene(item->sceneBoundingRect().topLeft()),
				mapFromScene(item->sceneBoundingRect().bottomRight())
			);
		}
	}

	if (theCropEnabled)
	{
		QPainter painter(this->viewport());
		paintCrop(&painter);
	}

	QPainter painter(this->viewport());
	paintTimestamps(&painter);

}

qint64 VideoPlayer::frameToMilliseconds(qint64 frame, double fps) const
{
	return (qint64)round(((double)frame / fps) * 1000.0);
}

qint64 VideoPlayer::duration() const
{
	return frameToMilliseconds(theVideoFrameCount, theVideoFPS);
}

qint64 VideoPlayer::position() const
{
	return theCurrentPosition;
}

void VideoPlayer::setMarkers(const QString& range)
{
	theMarksRange = range;
}

void VideoPlayer::paintTimestamps(QPainter* painter)
{
	if (theCurrentPosition > 0)
	{
		double viewWidth = (double)theVideoImageRectF.width();
		double viewHeight = (double)theVideoImageRectF.height();

		// Few magic numbers for size and position
		double widthRatio = 32.0;
		double heightRatio = 12.0;
		double fontSizeRatio = 24.5;

		QFont font = painter->font();
		font.setPixelSize((int)round(viewHeight / fontSizeRatio));
		painter->setFont(font);

		QPen pen = painter->pen();
		pen.setStyle(Qt::SolidLine);
		pen.setColor(Qt::white);
		painter->setPen(pen);

		QString timestamp = TimelineMarks::MillisecondsToTimecode(theCurrentPosition, 1);
		painter->drawText(QPoint(
			(int)round(viewWidth / widthRatio),
			(int)viewHeight - (int)round(viewHeight / heightRatio)),
			timestamp
		);

		painter->drawText(QPoint(
			(int)round(viewWidth / widthRatio),
			(int)viewHeight - (int)round(viewHeight / heightRatio / 2.0)),
			QString("[%1]").arg(theMarksRange)
		);
	}
}

/*!
 * \brief VideoPlayer::paintCrop Draws a crop tool
 */
void VideoPlayer::paintCrop(QPainter* painter)
{
	paintCropRectangle(painter);
	paintCropHandles(painter);
	//qDebug() << "Crop: " << mapToVideo(rect.topLeft()) << ", " << mapToVideo(rect.bottomRight());
}

/*!
 * \brief VideoPlayer::paintCropRectangle Draws a rectangle of a crop tool.
 */
void VideoPlayer::paintCropRectangle(QPainter* painter)
{
	QPen linePen;
	linePen.setColor(theCropColor);
	linePen.setStyle(Qt::DashLine);
	linePen.setWidth(2);

	painter->setPen(linePen);
	painter->drawRect(theCropRectF);
}

void VideoPlayer::play()
{
	if (!thePlayerTimer->isActive())
	{
		if (theVideoFPS > 0) thePlayerTimer->start(1000.0 / theVideoFPS);
		thePlaybackState = PlayingState;
		emit playbackStateChanged(thePlaybackState);
	}
}

void VideoPlayer::pause()
{
	if (thePlayerTimer->isActive())
	{
		thePlayerTimer->stop();
		thePlaybackState = PausedState;
		emit playbackStateChanged(thePlaybackState);
	}
}

void VideoPlayer::stop()
{
	if (thePlayerTimer->isActive())
	{
		thePlayerTimer->stop();
		setPosition(0);
		thePlaybackState = StoppedState;
		emit playbackStateChanged(thePlaybackState);
	}
}

int64_t VideoPlayer::millisecondsToTimestamp(qint64 msecs, AVRational timeBase)
{
	double time = (double)msecs / 1000.0;
	double stream_time_base = av_q2d(timeBase);
	return (int64_t)(time / stream_time_base);
}

qint64 VideoPlayer::timestampToMilliseconds(int64_t timestamp, AVRational timeBase)
{
	double stream_time_base = av_q2d(timeBase);
	double time = (double)timestamp * stream_time_base;
	return (qint64)round(time * 1000.0);
}

int64_t VideoPlayer::frameToTimestamp(int64_t frame)
{
	double time = (double)frame / theVideoFPS;
	double stream_time_base = av_q2d(theVideoStream->time_base);
	return (int64_t)(time / stream_time_base);
}

int64_t VideoPlayer::timestampToFrame(int64_t timestamp)
{
	double stream_time_base = av_q2d(theVideoStream->time_base);
	double time = (double)timestamp * stream_time_base;
	return (int64_t)(time * theVideoFPS);
}

void VideoPlayer::setPosition(qint64 position)
{
	if (theFormatContext)
	{
		QElapsedTimer execution_timer;
		execution_timer.start();

		int64_t target_ts = millisecondsToTimestamp(position, theVideoStream->time_base);
		if (0 <= av_seek_frame(theFormatContext, theVideoStreamIndex, target_ts, 0))
		{
			avcodec_flush_buffers(theCodecContext);
			AVPacket* packet = av_packet_alloc();
			AVFrame* frame = av_frame_alloc();
			bool frame_decoded = false;
			while (!frame_decoded && av_read_frame(theFormatContext, packet) >= 0)
			{
				if (packet->stream_index == theVideoStreamIndex)
				{
					if (0 == avcodec_send_packet(theCodecContext, packet))
					{
						if (0 == avcodec_receive_frame(theCodecContext, frame))
						{
							frame_decoded = true;
							theCurrentPosition = timestampToMilliseconds(frame->pkt_dts, theVideoStream->time_base);
						}
					}
				}
				av_packet_unref(packet);
			}
			av_packet_free(&packet);
			av_frame_free(&frame);

			decodeAndDisplayFrame();
			qDebug() << "Seeked to " << theCurrentPosition << "ms in " << execution_timer.elapsed() << "ms";
		}
		else
		{
			qDebug() << "Seeking to " << position << "ms has failed.";
		}
	}
}

bool VideoPlayer::openFile(const QString filename)
{
	closeFile();
	bool open = false;
	AVFormatContext* formatCtx = nullptr;
	const AVCodec* videoCodec = nullptr;
	if (0 == avformat_open_input(&formatCtx, filename.toStdString().c_str(), NULL, NULL))
	{
		theVideoFormatName = formatCtx->iformat->long_name;
		if (0 <= avformat_find_stream_info(formatCtx, NULL))
		{
			for (int i = 0; i < (int)formatCtx->nb_streams; i++)
			{
				AVStream* stream = formatCtx->streams[i];
				AVCodecParameters* codecParams = stream->codecpar;
				const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
				if (codec)
				{
					if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO)
					{
						theVideoStreamIndex = i;
						theVideoCodecName = codec->long_name;
						theVideoSize = QSize(codecParams->width, codecParams->height);
						theVideoFPS = av_q2d(stream->avg_frame_rate);
						theVideoPixelFormat = codecParams->format;
						theVideoFrameCount = (qint64)stream->nb_frames;
						emit durationChanged(frameToMilliseconds(theVideoFrameCount, theVideoFPS));
						AVCodecContext* codecContext = avcodec_alloc_context3(codec);
						avcodec_parameters_to_context(codecContext, codecParams);
						avcodec_open2(codecContext, codec, NULL);
						theVideoCodec = videoCodec;
						theCodecContext = codecContext;
						theVideoStream = stream;
						qInfo() << "Video stream found at index " << i << " codec " << codec->long_name << " size " << codecParams->width << "x" << codecParams->height << " fps " << theVideoFPS;
					}
					else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO)
					{
						theAudioCodecName = codec->long_name;
						theAudioCodecSampleRate = codecParams->sample_rate;
						qInfo() << "Audio stream found codec " << codec->long_name << " sample rate " << codecParams->sample_rate;
					}
				}
			}			
		}
		theFormatContext = formatCtx;
	}
	thePlaybackState = StoppedState;
	emit playbackStateChanged(thePlaybackState);
	return open;
}

void VideoPlayer::setSource(const QUrl& source)
{
	QString filename = source.toLocalFile();
	openFile(filename);
}

void VideoPlayer::closeFile()
{
	if (theCodecContext != nullptr && theCodecContext != NULL)
	{
		avcodec_free_context(&theCodecContext);
	}
	if (theFormatContext != nullptr && theFormatContext != NULL)
	{
		avformat_close_input(&theFormatContext);
	}
}

void VideoPlayer::decodeAndDisplayFrame()
{
	QElapsedTimer execution_timer;
	execution_timer.start();

	bool frame_decoded = false;
	QImage image;
	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();

	double aspect = (double)theVideoSize.width() / (double)theVideoSize.height();
	QSize dstSize = theViewSize;
	dstSize.setHeight((double)dstSize.width() / aspect);

	while (!frame_decoded && av_read_frame(theFormatContext, packet) >= 0)
	{
		if (packet->stream_index == theVideoStreamIndex)
		{
			if (0 == avcodec_send_packet(theCodecContext, packet))
			{
				if (0 == avcodec_receive_frame(theCodecContext, frame))
				{
					image = getImageFromFrame(frame, dstSize);
					frame_decoded = true;
				}
			}
		}
		av_packet_unref(packet);
	}

	if (frame_decoded)
	{
		qint64 frame_position = timestampToMilliseconds(frame->pkt_dts, theVideoStream->time_base);
		if (theCurrentPosition != frame_position)
		{
			theCurrentPosition = frame_position;
			emit positionChanged(theCurrentPosition);
		}
		QPixmap pixmap = QPixmap::fromImage(image);
		theScene->clear();
		QGraphicsPixmapItem* item = theScene->addPixmap(pixmap);
		fitInView(item, Qt::KeepAspectRatio);
		theVideoImageRectF = item->boundingRect();
	}

	qDebug() << "Frame decoded in " << execution_timer.elapsed() << "ms";
}

QImage VideoPlayer::getImageFromFrame(const AVFrame* frame, const QSize dstSize) const
{
	int dstWidth = dstSize.width();
	int dstHeight = dstSize.height();
	QImage image(dstWidth, dstHeight, QImage::Format_RGB888);

	SwsContext* img_convert_ctx = sws_getContext(
		frame->width,
		frame->height,
		(AVPixelFormat)theVideoPixelFormat,
		dstWidth,
		dstHeight,
		AV_PIX_FMT_RGB24,
		SWS_BICUBIC, NULL, NULL, NULL);
	if (img_convert_ctx)
	{
		AVFrame* rgbFrame = av_frame_alloc();
		av_image_alloc(rgbFrame->data, rgbFrame->linesize, dstWidth, dstHeight, AV_PIX_FMT_RGB24, 1);
		if (dstHeight == sws_scale(img_convert_ctx,
			frame->data,
			frame->linesize, 0,
			frame->height,
			rgbFrame->data,
			rgbFrame->linesize))
		{
			for (int y = 0; y < dstHeight; y++)
			{
				memcpy(image.scanLine(y), rgbFrame->data[0] + y * 3 * dstWidth, 3 * dstWidth);
			}
		}
		av_freep(rgbFrame->data);
		av_frame_unref(rgbFrame);
		av_frame_free(&rgbFrame);
		sws_freeContext(img_convert_ctx);
	}
	//image.save("frame.png");
	return image;
}

void VideoPlayer::setCropEnabled(bool enabled)
{
	theCropEnabled = enabled;
	theCurrentCropState = (theCropEnabled) ? VPCropState_Active : VPCropState_Inactive;
	if (theCropEnabled)
	{
		theCropRectF = theVideoImageRectF;
	}
	repaint();
	emit CropEnabledChanged(theCropEnabled);
}

QPointF VideoPlayer::mapToVideo(QPointF point)
{
	double x = point.x() / theViewSize.width();
	double y = point.y() / theViewSize.height();
	return QPointF(x * theVideoSize.width(), y * theVideoSize.height());
}

QPointF VideoPlayer::mapFromVideo(QPointF point)
{
	double x = point.x() / theVideoSize.width();
	double y = point.y() / theVideoSize.height();
	return QPointF(x * theViewSize.width(), y * theViewSize.height());
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a top left handle of a crop tool.
 */
QRect VideoPlayer::cropHandleTLRect()
{
	return QRect(theCropRectF.x() - theCropHandleSize / 2,
		theCropRectF.y() - theCropHandleSize / 2,
		theCropHandleSize,
		theCropHandleSize);
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a top right handle of a crop tool.
 */
QRect VideoPlayer::cropHandleTRRect()
{
	return QRect(theCropRectF.x() + theCropRectF.width() - theCropHandleSize / 2,
		theCropRectF.y() - theCropHandleSize / 2,
		theCropHandleSize,
		theCropHandleSize
	);
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a bottom left handle of a crop tool.
 */
QRect VideoPlayer::cropHandleBLRect()
{
	return QRect(theCropRectF.x() - theCropHandleSize / 2,
		theCropRectF.y() + theCropRectF.height() - theCropHandleSize / 2,
		theCropHandleSize,
		theCropHandleSize
	);
}

/*!
 * \brief VideoPlayer::cropHandleTLRect Draws a bottom right handle of a crop tool.
 */
QRect VideoPlayer::cropHandleBRRect()
{
	return QRect(theCropRectF.x() + theCropRectF.width() - theCropHandleSize / 2,
		theCropRectF.y() + theCropRectF.height() - theCropHandleSize / 2,
		theCropHandleSize,
		theCropHandleSize
	);
}

/*!
 * \brief VideoPlayer::paintCropHandles Draws all handles of a crop tool.
 */
void VideoPlayer::paintCropHandles(QPainter* painter)
{
	QPen handlePen;
	handlePen.setColor(theCropColor);
	handlePen.setStyle(Qt::SolidLine);
	handlePen.setWidth(2);
	QBrush handleBrush;
	handleBrush.setColor(theCropColor);
	handleBrush.setStyle(Qt::SolidPattern);
	painter->setPen(handlePen);
	painter->setBrush(handleBrush);

	painter->drawRect(cropHandleTLRect());
	painter->drawRect(cropHandleTRRect());
	painter->drawRect(cropHandleBLRect());
	painter->drawRect(cropHandleBRRect());

}

/*!
 * \brief VideoPlayer::isInsideCrop Checks if a given point is inside a crop tool.
 * \param point Point to be checked.
 * \return True, if inside a crop tool, false otherwise.
 */
bool VideoPlayer::isInsideCrop(QPointF point)
{
	return theCropRectF.contains(point);
}

std::tuple<bool, VideoPlayer::VideoPlayerCropHandle> VideoPlayer::isOverCropHandle(QPointF point)
{
	bool isOver = false;
	VideoPlayerCropHandle which = VideoPlayer::VPCropHandle_None;

	if (cropHandleTLRect().contains(point.toPoint()))
	{
		isOver = true;
		which = VideoPlayer::VPCropHandle_TopLeft;
	}
	else if (cropHandleTRRect().contains(point.toPoint()))
	{
		isOver = true;
		which = VideoPlayer::VPCropHandle_TopRight;
	}
	else if (cropHandleBLRect().contains(point.toPoint()))
	{
		isOver = true;
		which = VideoPlayer::VPCropHandle_BottomLeft;
	}
	else if (cropHandleBRRect().contains(point.toPoint()))
	{
		isOver = true;
		which = VideoPlayer::VPCropHandle_BottomRight;
	}

	return std::make_tuple(isOver, which);
}

void VideoPlayer::mousePressEvent(QMouseEvent* event)
{
	if (theCurrentCropState == VideoPlayer::VPCropState_Active && this->rect().contains(event->pos()))
	{
		if (event->button() == Qt::LeftButton)
		{
			std::tuple<bool, VideoPlayer::VideoPlayerCropHandle> overHandle = isOverCropHandle(event->pos());
			if (std::get<0>(overHandle))
			{
				if (std::get<1>(overHandle) == VideoPlayer::VPCropHandle_TopLeft)
					theCurrentCropState = VideoPlayer::VPCropState_ResizingTL;
				else if (std::get<1>(overHandle) == VideoPlayer::VPCropHandle_TopRight)
					theCurrentCropState = VideoPlayer::VPCropState_ResizingTR;
				else if (std::get<1>(overHandle) == VideoPlayer::VPCropHandle_BottomLeft)
					theCurrentCropState = VideoPlayer::VPCropState_ResizingBL;
				else if (std::get<1>(overHandle) == VideoPlayer::VPCropHandle_BottomRight)
					theCurrentCropState = VideoPlayer::VPCropState_ResizingBR;
				event->accept();
			}
			else
			{
				theCurrentCropState = VideoPlayer::VPCropState_Translating;
				event->accept();
			}
		}
	}
}

void VideoPlayer::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && this->rect().contains(event->pos()))
	{
		if (theCurrentCropState == VideoPlayer::VPCropState_ResizingTL)
		{
			theCurrentCropState = VideoPlayer::VPCropState_Active;
			event->accept();
		}
		if (theCurrentCropState == VideoPlayer::VPCropState_ResizingTR)
		{
			theCurrentCropState = VideoPlayer::VPCropState_Active;
			event->accept();
		}
		if (theCurrentCropState == VideoPlayer::VPCropState_ResizingBL)
		{
			theCurrentCropState = VideoPlayer::VPCropState_Active;
			event->accept();
		}
		if (theCurrentCropState == VideoPlayer::VPCropState_ResizingBR)
		{
			theCurrentCropState = VideoPlayer::VPCropState_Active;
			event->accept();
		}
		else if (theCurrentCropState == VideoPlayer::VPCropState_Translating)
		{
			theCurrentCropState = VideoPlayer::VPCropState_Active;
			event->accept();
		}
	}
}

void VideoPlayer::mouseMoveEvent(QMouseEvent* event)
{
	if (theCurrentCropState == VideoPlayer::VPCropState_Active && this->rect().contains(event->pos()))
	{
		if (isInsideCrop(event->pos()) && event->button() == Qt::NoButton)
		{
			if (cursor().shape() != Qt::SizeAllCursor)
				setCursor(Qt::SizeAllCursor);
		}
		else
		{
			if (cursor().shape() != Qt::ArrowCursor)
				setCursor(Qt::ArrowCursor);
		}

		std::tuple<bool, VideoPlayer::VideoPlayerCropHandle> overHandle = isOverCropHandle(event->pos());
		if (std::get<0>(overHandle) && event->button() == Qt::NoButton)
		{
			if (std::get<1>(overHandle) == VideoPlayer::VPCropHandle_TopLeft ||
				std::get<1>(overHandle) == VideoPlayer::VPCropHandle_BottomRight)
			{
				if (cursor().shape() != Qt::SizeFDiagCursor)
					setCursor(Qt::SizeFDiagCursor);
			}
			else
			{
				if (cursor().shape() != Qt::SizeBDiagCursor)
					setCursor(Qt::SizeBDiagCursor);
			}
		}

		if (event->button() == Qt::LeftButton)
		{
			std::tuple<bool, VideoPlayer::VideoPlayerCropHandle> overHandle = isOverCropHandle(event->pos());
			if (std::get<0>(overHandle))
			{
				// resizing
			}
			else
			{
				// moving
			}
		}
	}
	if (theCurrentCropState == VideoPlayer::VPCropState_ResizingBR && this->rect().contains(event->pos()))
	{
		QRectF newCrop = theCropRectF;
		newCrop.setBottomRight(event->pos());
		theCropRectF = newCrop;
		repaint();
	}
}
