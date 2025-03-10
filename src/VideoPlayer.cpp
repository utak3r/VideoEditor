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

	AVFormatContext* theFormatContext = nullptr;
	const AVCodec* theVideoCodec = nullptr;
	AVCodecContext* theCodecContext = nullptr;
	theVideoPixelFormat = AV_PIX_FMT_RGB24;
	theCurrentVideoFrame = -1;

	openFile();
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &VideoPlayer::decodeAndDisplayFrame);
	if (theVideoFPS > 0) timer->start(1000.0 / theVideoFPS);
}

VideoPlayer::~VideoPlayer()
{
	// avcodec_free_context()
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
	QGraphicsView::resizeEvent(event);
	theViewSize = event->size();
}

bool VideoPlayer::openFile()
{
	bool open = false;
	AVFormatContext* formatCtx = nullptr;
	const AVCodec* videoCodec = nullptr;
	if (0 == avformat_open_input(&formatCtx, "C:\\Users\\piotr\\devel\\sandbox\\VideoEditor\\flip.mp4", NULL, NULL))
	{
		// Test video is:
		// "C:\\Users\\piotr\\devel\\sandbox\\VideoEditor\\flip.mp4"
		// Kodek: H264 - MPEG-4 AVC (part 10) (avc1)
		// Rozdzielczoœæ obrazu: 1600x900
		// Wymiary bufora: 1600x912
		// Liczba klatek/s: 60
		// Orientacja: Górna lewa
		// Barwy podstawowe: ITU-R BT.601 (525 linii, 60 Hz)
		// Funkcja transferu kolorów: ITU-R BT.709
		// Przestrzeñ barw: Zakres ITU-R BT.601
		// Po³o¿enie chromy: Lewy

		theVideoFormatName = formatCtx->iformat->long_name;
		if (0 <= avformat_find_stream_info(formatCtx, NULL))
		{
			for (int i = 0; i < formatCtx->nb_streams; i++)
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
						theVideoFrameCount = stream->nb_frames;
						emit VideoLengthChanged(theVideoFrameCount);
					}
					else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO)
					{
						theAudioCodecName = codec->long_name;
						theAudioCodecSampleRate = codecParams->sample_rate;
					}
					printf("\tCodec %s ID %d bit_rate %lld", codec->long_name, codec->id, codecParams->bit_rate);

					AVCodecContext* codecContext = avcodec_alloc_context3(codec);
					avcodec_parameters_to_context(codecContext, codecParams);
					avcodec_open2(codecContext, codec, NULL);

					theVideoCodec = videoCodec;
					theCodecContext = codecContext;
				}
			}			
		}
		theFormatContext = formatCtx;
	}

	return open;
}

void VideoPlayer::decodeAndDisplayFrame()
{
	bool frame_decoded = false;
	QImage image;

	// Demuxing (av_read_frame)
	// Demuxes packets based on file format (mp4/avi/mkv etc.) 
	//   until you have a packet for the stream that you want (eg. video)
	// Feeds the decoder with the packet (avcodec_send_packet)
	// Starts the decoding process until it has enough packets to 
	//   give you the first frame (decodes based on DTS)
	// Checks whether a frame is ready to be presented (avcodec_receive_frame)
	// Asks the decoder if it has a frame to be presented after feeding it. 
	//   It might not be ready and you need to re-feed it or even it might give you 
	//   more than 1 frames at once. (Frames comes out based on PTS)

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
		if (theCurrentVideoFrame != theCodecContext->frame_num)
		{
			theCurrentVideoFrame = theCodecContext->frame_num;
			emit FrameNumberChanged(theCurrentVideoFrame);
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
	//image.save("C:\\Users\\piotr\\devel\\sandbox\\VideoEditor\\frame.png");
	return image;
}
