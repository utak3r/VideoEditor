#include "VideoPlayer.h"
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QVideoSink>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// https://ffmpeg.org/doxygen/7.0/index.html

VideoPlayer::VideoPlayer(QWidget* parent)
	: QGraphicsView(parent)
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
	theCurrentVideoFrame = -1;

	thePlaybackState = StoppedState;
	thePlayerTimer = new QTimer(this);
	connect(thePlayerTimer, &QTimer::timeout, this, &VideoPlayer::decodeAndDisplayFrame);
}

VideoPlayer::~VideoPlayer()
{
	closeFile();
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
	QGraphicsView::resizeEvent(event);
	theViewSize = event->size();
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

void VideoPlayer::setPosition(qint64 position)
{
	if (theFormatContext)
	{
		av_seek_frame(theFormatContext, theVideoStreamIndex, (int64_t)position, AVSEEK_FLAG_ANY | AVSEEK_FLAG_FRAME);
		avformat_flush(theFormatContext);
		theCurrentVideoFrame = position;
		decodeAndDisplayFrame();
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
						emit durationChanged(theVideoFrameCount);
						AVCodecContext* codecContext = avcodec_alloc_context3(codec);
						avcodec_parameters_to_context(codecContext, codecParams);
						avcodec_open2(codecContext, codec, NULL);
						theVideoCodec = videoCodec;
						theCodecContext = codecContext;
						theVideoStream = stream;
					}
					else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO)
					{
						theAudioCodecName = codec->long_name;
						theAudioCodecSampleRate = codecParams->sample_rate;
					}
					printf("\tCodec %s ID %d bit_rate %lld", codec->long_name, codec->id, codecParams->bit_rate);

				}
			}			
		}
		theFormatContext = formatCtx;
	}
	thePlaybackState = StoppedState;
	emit playbackStateChanged(thePlaybackState);
	return open;
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
		if (theCurrentVideoFrame != (qint64)theCodecContext->frame_num)
		{
			theCurrentVideoFrame = (qint64)theCodecContext->frame_num;
			emit positionChanged(theCurrentVideoFrame);
		}
		QPixmap pixmap = QPixmap::fromImage(image);
		theScene->clear();
		theScene->addPixmap(pixmap);
		this->viewport()->update();
	}

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
