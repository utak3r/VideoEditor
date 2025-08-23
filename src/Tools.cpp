#include "Tools.h"
#include <QSize>
#include <QRect>

void Tools::getAvailableEncoders(QStringList& videoCodecs, QStringList& audioCodecs)
{
    const AVCodec* codec;
    void* iter = nullptr;
    while ((codec = av_codec_iterate(&iter)))
    {
        if (codec->type == AVMEDIA_TYPE_VIDEO)
        {
            if (av_codec_is_encoder(codec))
                videoCodecs.append(QString("%1: %2").arg(codec->name, codec->long_name));
        }
        else if (codec->type == AVMEDIA_TYPE_AUDIO)
        {
            if (av_codec_is_encoder(codec))
                audioCodecs.append(QString("%1: %2").arg(codec->name, codec->long_name));
        }
    }
}

qint64 Tools::millisecondsToTimestamp(qint64 msecs, AVRational timeBase)
{
    double time = (double)msecs / 1000.0; // convert milliseconds to seconds
    double stream_time_base = av_q2d(timeBase);
    return (qint64)(time / stream_time_base);
}

qint64 Tools::seekStreamBackIFrame(AVFormatContext* fmt_ctx, int stream_index, qint64 ms, AVRational timebase, AVCodecContext* codec_ctx)
{
    qint64 start_pts = millisecondsToTimestamp(ms, timebase);
    int seek_ret = avformat_seek_file(fmt_ctx, stream_index, 0, start_pts, start_pts, AVSEEK_FLAG_BACKWARD);
    if (seek_ret >= 0) avcodec_flush_buffers(codec_ctx);
    return start_pts;
}

qint64 Tools::seekStreamExactAnyFrame(AVFormatContext* fmt_ctx, int stream_index, qint64 ms, AVRational timebase, AVCodecContext* codec_ctx)
{
    qint64 start_pts = millisecondsToTimestamp(ms, timebase);
	int seek_ret = avformat_seek_file(fmt_ctx, stream_index, 0, start_pts, start_pts, AVSEEK_FLAG_ANY);
    if (seek_ret >= 0) avcodec_flush_buffers(codec_ctx);
    return start_pts;
}

QString Tools::ffmpegErrorString(int errnum)
{
    char buf[AV_ERROR_MAX_STRING_SIZE] = { 0 };
    av_strerror(errnum, buf, sizeof(buf));
    return QString::fromUtf8(buf);
}

QRect Tools::makeRectYUVCompliant(const QRect& rect, 
                                AVPixelFormat format, 
                                bool forceHeight4, 
                                bool forceHeight16)
{
    int x = rect.x();
    int y = rect.y();
    int w = rect.width();
    int h = rect.height();

    switch (format)
    {
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
    case AV_PIX_FMT_NV12:
		if (x % 2 != 0) x--;
		if (y % 2 != 0) y--;
        if (w % 2 != 0) w--;
        if (h % 2 != 0) h--;
		break;
    case AV_PIX_FMT_YUV422P:
        if (x % 2 != 0) x--;
        if (w % 2 != 0) w--;
		break;
	case AV_PIX_FMT_YUV444P:
	case AV_PIX_FMT_RGB24:
	case AV_PIX_FMT_BGR24:
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_ABGR:
	case AV_PIX_FMT_RGBA:
	case AV_PIX_FMT_BGRA:
		// no requirements
        break;
    default:
        // unknown format, assume YUV420P
        if (x % 2 != 0) x--;
        if (y % 2 != 0) y--;
        if (w % 2 != 0) w--;
        if (h % 2 != 0) h--;
        break;
    }

    if (forceHeight4)
    {
        while (h % 4 != 0) h--;
	}
    if (forceHeight16)
    {
        while (h % 16 != 0) h--;
	}

    return QRect(x, y, w, h);
}
