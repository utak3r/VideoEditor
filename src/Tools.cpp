#include "Tools.h"
#include <QSize>

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

QSize Tools::forceSizeMultipleOf16(const QSize& size)
{
	if (size.width() <= 0 || size.height() <= 0) return QSize(0, 0);
    int w = (size.width() / 16) * 16;
    int h = (size.height() / 16) * 16;
	return QSize(w, h);
}

QSize Tools::forceSizeEven(const QSize& size)
{
    if (size.width() <= 0 || size.height() <= 0) return QSize(0, 0);
    int w = (size.width() / 2) * 2;
    int h = (size.height() / 2) * 2;
	return QSize(w, h);
}
