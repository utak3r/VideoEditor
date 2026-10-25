#include "Tools.h"
#include <QSize>
#include <QRect>
#include <algorithm>

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
    if (rect.isEmpty() || rect.width() <= 0 || rect.height() <= 0)
        return QRect(0, 0, 0, 0);

    int x = rect.x();
    int y = rect.y();
    int w = rect.width();
    int h = rect.height();

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    int h_shift = 0;
    int v_shift = 0;
    if (av_pix_fmt_get_chroma_sub_sample(format, &h_shift, &v_shift) < 0)
    {
        // Unknown format, assume YUV420P fallback
        h_shift = 1;
        v_shift = 1;
    }

    // Alignment required for chroma subsampling
    int align_x = 1 << h_shift;
    int align_y = 1 << v_shift;

    // For video encoding (e.g. H.264/YUV420P target), width and height
    // must always be at least even (multiple of 2), even if the input format is RGB/444.
    int align_w = std::max(align_x, 2);
    int align_h = std::max(align_y, 2);

    if (forceHeight16)
    {
        align_h = std::max(align_h, 16);
    }
    else if (forceHeight4)
    {
        align_h = std::max(align_h, 4);
    }

    // Align crop origin down to multiple of alignment
    if (align_x > 1)
        x = (x / align_x) * align_x;
    if (align_y > 1)
        y = (y / align_y) * align_y;

    // Align crop dimensions down to multiple of alignment
    if (align_w > 1)
        w = (w / align_w) * align_w;
    if (align_h > 1)
        h = (h / align_h) * align_h;

    if (w < align_w) w = align_w;
    if (h < align_h) h = align_h;

    return QRect(x, y, w, h);
}
