#include "VideoTranscoder.h"
#include "Tools.h"
#include <QApplication>

// https://ffmpeg.org/doxygen/2.0/group__lavfi.html

VideoTranscoder::VideoTranscoder(IFFmpeg& ffmpeg, QObject* parent)
	: QObject(parent)
    , ff(ffmpeg)
    , theDecoder(&ffmpeg)
    , theEncoder(&ffmpeg)
{
}

VideoTranscoder::~VideoTranscoder()
{
    cleanup();
}

VideoTranscoder::CodecHandlers::~CodecHandlers()
{
	if (scalingContext) ff->sws_freeContext(scalingContext);
	if (resamplingContext) ff->swr_free(&resamplingContext);
	if (audioFifo) ff->av_audio_fifo_free(audioFifo);

    ff->av_frame_free(&rescaledFrame);
    ff->av_frame_free(&resampledFrame);

    if (videoCodecContext) ff->avcodec_free_context(&videoCodecContext);
    if (audioCodecContext) ff->avcodec_free_context(&audioCodecContext);
    if (formatContext)
    {
        if (formatContext->iformat)
        {
            // INPUT context
            ff->avformat_close_input(&formatContext);
            // uwaga: avformat_close_input ustawi formatContext = nullptr
        }
        else if (formatContext->oformat)
        {
            // OUTPUT context
            if (!(formatContext->oformat->flags & AVFMT_NOFILE) && formatContext->pb)
                ff->avio_closep(&formatContext->pb);

            ff->avformat_free_context(formatContext);
            formatContext = nullptr;
        }
    }
}

void VideoTranscoder::cleanup()
{
    //if (theEncoder.scalingContext) sws_freeContext(theEncoder.scalingContext);
    //if (theEncoder.resamplingContext) swr_free(&theEncoder.resamplingContext);
    //if (theEncoder.audioFifo) av_audio_fifo_free(theEncoder.audioFifo);

    //av_frame_free(&theEncoder.rescaledFrame);
    //av_frame_free(&theEncoder.resampledFrame);

    //if (theDecoder.videoCodecContext) avcodec_free_context(&theDecoder.videoCodecContext);
    //if (theDecoder.audioCodecContext) avcodec_free_context(&theDecoder.audioCodecContext);
    //if (theEncoder.videoCodecContext) avcodec_free_context(&theEncoder.videoCodecContext);
    //if (theEncoder.audioCodecContext) avcodec_free_context(&theEncoder.audioCodecContext);

    //if (theEncoder.formatContext) {
    //    if (!(theEncoder.formatContext->oformat->flags & AVFMT_NOFILE))
    //        avio_closep(&theEncoder.formatContext->pb);
    //    avformat_free_context(theEncoder.formatContext);
    //    theEncoder.formatContext = nullptr;
    //}
    //if (theDecoder.formatContext)  avformat_close_input(&theDecoder.formatContext);
}

QString VideoTranscoder::inputFile() const
{
    return theDecoder.fileName;
}

void VideoTranscoder::setInputFile(const QString& filename)
{
    if (theDecoder.fileName != filename)
    {
        theDecoder.fileName = filename;
	}
}

QString VideoTranscoder::outputFile() const
{
    return theEncoder.fileName;
}

void VideoTranscoder::setOutputFile(const QString& filename)
{
    if (theEncoder.fileName != filename)
    {
		theEncoder.fileName = filename;
    }
}

void VideoTranscoder::setMetadata(const QString& key, const QString& value)
{
    theMetadata.insert(key, value);
}

void VideoTranscoder::applyMetadata()
{
    QMapIterator<QString, QString> iter(theMetadata);
    while (iter.hasNext())
    {
        iter.next();
        ff.av_dict_set(&theEncoder.formatContext->metadata,
            iter.key().toStdString().c_str(), 
            iter.value().toStdString().c_str(),
            0);
    }
}

void VideoTranscoder::setOutputFramerate(int num, int den)
{
    if (num > 0 && den > 0)
    {
        theEncoder.framerate = AVRational{ num, den };
        theEncoder.customFramerate = true;
    }
    else
    {
        theEncoder.framerate = AVRational{ 0, 1 };
        theEncoder.customFramerate = false;
	}
}

void VideoTranscoder::setOutputResolution(int w, int h)
{
	theEncoder.videoSize = QSize(w, h);
}

void VideoTranscoder::setOutputScalingFilter(int filter)
{
    int sws_filter = SWS_BILINEAR;
    switch (filter)
    {
    case 0: sws_filter = SWS_FAST_BILINEAR; break;
    case 1: sws_filter = SWS_BILINEAR; break;
    case 2: sws_filter = SWS_BICUBIC; break;
    case 3: sws_filter = SWS_AREA; break;
    case 4: sws_filter = SWS_GAUSS; break;
    case 5: sws_filter = SWS_LANCZOS; break;
    default: sws_filter = SWS_BILINEAR;
    }
    if (theEncoder.scalingFilter != sws_filter)
        theEncoder.scalingFilter = sws_filter;
}

void VideoTranscoder::setOutputVideoCodecName(const QString& n)
{
    if (theEncoder.videoCodecName != n)
    {
        theEncoder.videoCodecName = n;
        //theEncoder.videoCodecId = avcodec_find_encoder_by_name(n.toStdString().c_str())->id;
	}
}

void VideoTranscoder::setOutputVideoCodecPreset(const QString& preset)
{
    if (theEncoder.videoCodecPreset != preset)
    {
        theEncoder.videoCodecPreset = preset;
    }
}

void VideoTranscoder::setOutputAudioCodecName(const QString& n)
{
    if (theEncoder.audioCodecName != n)
    {
        theEncoder.audioCodecName = n;
        //theEncoder.audioCodecId = avcodec_find_encoder_by_name(n.toStdString().c_str())->id;
	}
}

void VideoTranscoder::setOutputAudioCodecPreset(const QString& preset)
{
    if (theEncoder.audioCodecPreset != preset)
    {
        theEncoder.audioCodecPreset = preset;
	}
}

void VideoTranscoder::setCropWindow(const QRect& rect)
{
    if (theEncoder.cropWindow != rect)
        theEncoder.cropWindow = rect;
}

TimelineMarks VideoTranscoder::marks()
{
	return theMarks;
}

void VideoTranscoder::setMarks(const TimelineMarks& marks)
{
    if (theMarks != marks)
        theMarks = marks;
}

AVPixelFormat VideoTranscoder::getFirstSupportedPixelFormat(const AVCodecContext* codec_ctx, const AVCodec* codec)
{
    const void* values;
    int nb_values;
    AVPixelFormat pixelFormat = AV_PIX_FMT_NONE;
    int ret = ff.avcodec_get_supported_config(codec_ctx, codec,
        AV_CODEC_CONFIG_PIX_FORMAT, 0,
        &values,
        &nb_values);
    if (ret >= 0)
    {
        const enum AVPixelFormat* fmts = (const enum AVPixelFormat*)values;
        pixelFormat = fmts[0];
    }
    else
    {
        pixelFormat = AV_PIX_FMT_YUV420P;
    }
    return pixelFormat;
}

AVSampleFormat VideoTranscoder::getFirstSupportedSampleFormat(const AVCodecContext* codec_ctx, const AVCodec* codec)
{
    const void* values;
    int nb_values;
    AVSampleFormat sampleFormat = AV_SAMPLE_FMT_NONE;
    int ret = ff.avcodec_get_supported_config(codec_ctx, codec,
        AV_CODEC_CONFIG_SAMPLE_FORMAT, 0,
        &values,
        &nb_values);
    if (ret >= 0)
    {
        const enum AVSampleFormat* fmts = (const enum AVSampleFormat*)values;
        sampleFormat = fmts[0];
    }
    else
    {
        sampleFormat = AV_SAMPLE_FMT_FLTP;
    }
    return sampleFormat;
}

int64_t VideoTranscoder::millisecondsToTimestamp(qint64 msecs, AVRational timeBase) const
{
	double time = (double)msecs / 1000.0; // convert milliseconds to seconds
    double stream_time_base = ff.av_q2d(timeBase);
    return (int64_t)(time / stream_time_base);
}

AVRational VideoTranscoder::pickInputFramerate() const
{
    AVRational fr = ff.av_guess_frame_rate(theDecoder.formatContext, theDecoder.videoStream, nullptr);
    if (fr.num && fr.den) return fr;
    if (theDecoder.videoStream->r_frame_rate.num && theDecoder.videoStream->r_frame_rate.den) return theDecoder.videoStream->r_frame_rate;
    if (theDecoder.videoStream->avg_frame_rate.num && theDecoder.videoStream->avg_frame_rate.den) return theDecoder.videoStream->avg_frame_rate;
    return AVRational{ 25,1 };
}

bool VideoTranscoder::openInput()
{
    if (ff.avformat_open_input(&theDecoder.formatContext, theDecoder.fileName.toStdString().c_str(), nullptr, nullptr) < 0) return false;
    if (ff.avformat_find_stream_info(theDecoder.formatContext, nullptr) < 0) return false;

    int v_idx = ff.av_find_best_stream(theDecoder.formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (v_idx >= 0) theDecoder.videoStream = theDecoder.formatContext->streams[v_idx];
    int a_idx = ff.av_find_best_stream(theDecoder.formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (a_idx >= 0) theDecoder.audioStream = theDecoder.formatContext->streams[a_idx];

    return theDecoder.videoStream || theDecoder.audioStream;
}

bool VideoTranscoder::openOutput()
{
    if (ff.avformat_alloc_output_context2(&theEncoder.formatContext, nullptr, nullptr, theEncoder.fileName.toStdString().c_str()) < 0 || !theEncoder.formatContext)
        return false;
    return true;
}

bool VideoTranscoder::prepareVideoDecoder()
{
    if (!theDecoder.videoStream) return true;
    const AVCodec* dec = ff.avcodec_find_decoder(theDecoder.videoStream->codecpar->codec_id);
    if (!dec) return false;
    theDecoder.videoCodecContext = ff.avcodec_alloc_context3(dec);
    if (!theDecoder.videoCodecContext) return false;
    if (ff.avcodec_parameters_to_context(theDecoder.videoCodecContext, theDecoder.videoStream->codecpar) < 0) return false;
    if (ff.avcodec_open2(theDecoder.videoCodecContext, dec, nullptr) < 0) return false;
    return true;
}

bool VideoTranscoder::prepareAudioDecoder()
{
    if (!theDecoder.audioStream) return true;
    const AVCodec* dec = ff.avcodec_find_decoder(theDecoder.audioStream->codecpar->codec_id);
    if (!dec) return false;
    theDecoder.audioCodecContext = ff.avcodec_alloc_context3(dec);
    if (!theDecoder.audioCodecContext) return false;
    if (ff.avcodec_parameters_to_context(theDecoder.audioCodecContext, theDecoder.audioStream->codecpar) < 0) return false;
    if (ff.avcodec_open2(theDecoder.audioCodecContext, dec, nullptr) < 0) return false;
    return true;
}

QSize VideoTranscoder::calculateOutputSize(int src_width, int src_height, QSize target_size) const
{
    int target_width = target_size.width();
    int target_height = target_size.height();
    int source_width = src_width;
	int source_height = src_height;

    // Cropping, if needed.
    if (theEncoder.cropWindow.width() > 0 &&
        theEncoder.cropWindow.height() > 0)
    {
        source_width = theEncoder.cropWindow.width();
        source_height = theEncoder.cropWindow.height();
    }

    if (target_width == -1 && target_height == -1)
    {
        // No scaling specified, use original video size
        target_width = source_width;
        target_height = source_height;
    }
    else if (target_width == -1)
    {
        // Use aspect ratio to calculate width, specified height
        target_width = source_width * target_height / source_height;
    }
    else if (target_height == -1)
    {
        // Use aspect ratio to calculate height, specified width
        target_height = source_height * target_width / source_width;
    }
    // use original size if target is zero
    if (target_width == 0) target_width = source_width;
    if (target_height == 0) target_height = source_height;
    return QSize(target_width, target_height);
}

bool VideoTranscoder::prepareVideoEncoder()
{
    if (!theDecoder.videoStream) return true;

	theEncoder.videoCodec = std::unique_ptr<Codec>(CodecFactory::instance().create(theEncoder.videoCodecName, &ff));

    const AVCodec* enc = theEncoder.videoCodec->getAVCodec();

    theEncoder.videoCodecContext = ff.avcodec_alloc_context3(enc);
    if (!theEncoder.videoCodecContext) return false;

    AVRational fps = theEncoder.customFramerate ? theEncoder.framerate : pickInputFramerate();
    theEncoder.videoCodecContext->time_base = ff.av_inv_q(fps);
    theEncoder.videoCodecContext->framerate = fps;

    theEncoder.videoCodecContext->pix_fmt = getFirstSupportedPixelFormat(theEncoder.videoCodecContext, enc);
    theEncoder.videoCodecContext->sample_aspect_ratio = ff.av_make_q(1, 1);

    QSize targetSize = calculateOutputSize(
        theDecoder.videoCodecContext->width,
        theDecoder.videoCodecContext->height,
        theEncoder.videoSize);
    theEncoder.videoCodecContext->width = targetSize.width();
    theEncoder.videoCodecContext->height = targetSize.height();

    theEncoder.videoCodecContext->gop_size = 12;
    if (theEncoder.formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        theEncoder.videoCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (!theEncoder.videoCodecPreset.isEmpty())
    {
        theEncoder.videoCodec->setPreset(theEncoder.videoCodecPreset, theEncoder.videoCodecContext);
    }

    theEncoder.videoStream = ff.avformat_new_stream(theEncoder.formatContext, nullptr);
    if (!theEncoder.videoStream) return false;
    theEncoder.videoStream->time_base = theEncoder.videoCodecContext->time_base;

    if (int ret = ff.avcodec_open2(theEncoder.videoCodecContext, enc, nullptr); ret < 0)
    {
		emit transcodingError(tr("Could not open video encoder: %1").arg(Tools::ffmpegErrorString(ret)));
        return false;
    }
    if (int ret = ff.avcodec_parameters_from_context(theEncoder.videoStream->codecpar, theEncoder.videoCodecContext); ret < 0)
    {
		emit transcodingError(tr("Could not initialize video stream parameters: %1").arg(Tools::ffmpegErrorString(ret)));
        return false;
    }

    if (theEncoder.cropWindow.width() > 0 && theEncoder.cropWindow.height() > 0)
        initCropFilter();
    initScaler();

    return true;
}

bool VideoTranscoder::prepareAudioEncoder()
{
    if (!theDecoder.audioStream) return true;

	theEncoder.audioCodec = std::unique_ptr<Codec>(CodecFactory::instance().create(theEncoder.audioCodecName, &ff));

	const AVCodec* enc = theEncoder.audioCodec->getAVCodec();

    theEncoder.audioCodecContext = ff.avcodec_alloc_context3(enc);
    if (!theEncoder.audioCodecContext) return false;

    theEncoder.audioCodecContext->sample_rate = theDecoder.audioCodecContext->sample_rate;
    ff.av_channel_layout_copy(&theEncoder.audioCodecContext->ch_layout, &theDecoder.audioCodecContext->ch_layout);
    theEncoder.audioCodecContext->time_base = AVRational{ 1, theEncoder.audioCodecContext->sample_rate };

    theEncoder.audioCodecContext->sample_fmt = getFirstSupportedSampleFormat(theEncoder.audioCodecContext, enc);

    if (theEncoder.formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        theEncoder.audioCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (!theEncoder.audioCodecPreset.isEmpty())
    {
        theEncoder.audioCodec->setPreset(theEncoder.audioCodecPreset, theEncoder.audioCodecContext);
    }

    theEncoder.audioStream = ff.avformat_new_stream(theEncoder.formatContext, nullptr);
    if (!theEncoder.audioStream) return false;
    theEncoder.audioStream->time_base = theEncoder.audioCodecContext->time_base;

    if (ff.avcodec_open2(theEncoder.audioCodecContext, enc, nullptr) < 0) return false;
    if (ff.avcodec_parameters_from_context(theEncoder.audioStream->codecpar, theEncoder.audioCodecContext) < 0) return false;

    initResampler();
    return true;
}

void VideoTranscoder::initCropFilter()
{
    char args[512];
    int ret = 0;

    theDecoder.cropFilterGraph = ff.avfilter_graph_alloc();
    if (!theDecoder.cropFilterGraph) return;

    // Input to a filtergraph
    const AVFilter* buffersrc = ff.avfilter_get_by_name("buffer");
    const AVFilter* buffersink = ff.avfilter_get_by_name("buffersink");
    if (!buffersrc || !buffersink) return;

    // Set up the buffer source filter arguments
    AVRational time_base = theDecoder.videoCodecContext->time_base;
    if (time_base.num <= 0 || time_base.den <= 0)
    {
        time_base = theDecoder.videoStream->time_base;
    }
    snprintf(args, sizeof(args),
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        theDecoder.videoCodecContext->width, theDecoder.videoCodecContext->height, theDecoder.videoCodecContext->pix_fmt,
        time_base.num, time_base.den,
        theDecoder.videoCodecContext->sample_aspect_ratio.num, theDecoder.videoCodecContext->sample_aspect_ratio.den);
    qDebug() << "Crop filter args:" << args;

    ret = ff.avfilter_graph_create_filter(&theDecoder.cropFilterSrc, buffersrc, "in",
        args, nullptr, theDecoder.cropFilterGraph);
    if (ret < 0) return;

    ret = ff.avfilter_graph_create_filter(&theDecoder.cropFilterSink, buffersink, "out",
        nullptr, nullptr, theDecoder.cropFilterGraph);
    if (ret < 0) return;

    // crop filter
    // crop=w:h:x:y
    char filterDesc[128];
    snprintf(filterDesc, sizeof(filterDesc),
        "crop=%d:%d:%d:%d,setsar=1,scale=%d:%d",
        theEncoder.cropWindow.width(), theEncoder.cropWindow.height(),
        theEncoder.cropWindow.x(), theEncoder.cropWindow.y(), 
        theEncoder.cropWindow.width(), theEncoder.cropWindow.height());
    qDebug() << "Crop filter definition: " << filterDesc;

    AVFilterInOut* inputs = ff.avfilter_inout_alloc();
    AVFilterInOut* outputs = ff.avfilter_inout_alloc();

    outputs->name = ff.av_strdup("in");
    outputs->filter_ctx = theDecoder.cropFilterSrc;
    outputs->pad_idx = 0;
    outputs->next = nullptr;

    inputs->name = ff.av_strdup("out");
    inputs->filter_ctx = theDecoder.cropFilterSink;
    inputs->pad_idx = 0;
    inputs->next = nullptr;

    ret = ff.avfilter_graph_parse_ptr(theDecoder.cropFilterGraph, filterDesc, &inputs, &outputs, nullptr);
    if (ret < 0)
    {
		emit transcodingError(tr("Could not parse crop filter: %1\nCrop filter: %2").arg(Tools::ffmpegErrorString(ret)).arg(filterDesc));
        return;
    }

    ret = ff.avfilter_graph_config(theDecoder.cropFilterGraph, nullptr);
    if (ret < 0) return;

    ff.avfilter_inout_free(&inputs);
    ff.avfilter_inout_free(&outputs);
}

void VideoTranscoder::initScaler()
{
    int width = theDecoder.videoCodecContext->width;
	int height = theDecoder.videoCodecContext->height;
	AVPixelFormat srcPixFmt = theDecoder.videoCodecContext->pix_fmt;

    if (theEncoder.cropWindow.width() > 0 && theEncoder.cropWindow.height() > 0 &&
        theDecoder.cropFilterGraph && theDecoder.cropFilterSrc && theDecoder.cropFilterSink)
    {
		width = theEncoder.cropWindow.width();
		height = theEncoder.cropWindow.height();
        srcPixFmt = (AVPixelFormat)ff.av_buffersink_get_format(theDecoder.cropFilterSink);
    }

    theEncoder.scalingContext = ff.sws_getContext(width, height, srcPixFmt,
        theEncoder.videoCodecContext->width, theEncoder.videoCodecContext->height, theEncoder.videoCodecContext->pix_fmt,
        theEncoder.scalingFilter, nullptr, nullptr, nullptr);

    theEncoder.rescaledFrame = ff.av_frame_alloc();
    theEncoder.rescaledFrame->format = theEncoder.videoCodecContext->pix_fmt;
    theEncoder.rescaledFrame->width = theEncoder.videoCodecContext->width;
    theEncoder.rescaledFrame->height = theEncoder.videoCodecContext->height;
    if (ff.av_frame_get_buffer(theEncoder.rescaledFrame, 32) < 0)
        throw std::runtime_error("av_frame_get_buffer(v_frame_conv) failed");
}

void VideoTranscoder::initResampler()
{
    theEncoder.resamplingContext = nullptr;
    if (ff.swr_alloc_set_opts2(&theEncoder.resamplingContext,
        &theEncoder.audioCodecContext->ch_layout, theEncoder.audioCodecContext->sample_fmt, theEncoder.audioCodecContext->sample_rate,
        &theDecoder.audioCodecContext->ch_layout, theDecoder.audioCodecContext->sample_fmt, theDecoder.audioCodecContext->sample_rate,
        0, nullptr) < 0)
    {
        throw std::runtime_error("swr_alloc_set_opts2 failed");
    }
    if (ff.swr_init(theEncoder.resamplingContext) < 0)
        throw std::runtime_error("swr_init failed");

    theEncoder.audioFifo = ff.av_audio_fifo_alloc(theEncoder.audioCodecContext->sample_fmt, theEncoder.audioCodecContext->ch_layout.nb_channels, 1);
    if (!theEncoder.audioFifo) throw std::runtime_error("av_audio_fifo_alloc failed");

    theEncoder.resampledFrame = ff.av_frame_alloc();
    if (!theEncoder.resampledFrame) throw std::runtime_error("av_frame_alloc a_frame_conv failed");
    theEncoder.resampledFrame->format = theEncoder.audioCodecContext->sample_fmt;
    theEncoder.resampledFrame->sample_rate = theEncoder.audioCodecContext->sample_rate;
    ff.av_channel_layout_copy(&theEncoder.resampledFrame->ch_layout, &theEncoder.audioCodecContext->ch_layout);
}

void VideoTranscoder::handleVideoPacket(AVPacket* pkt)
{
    if (!theDecoder.videoCodecContext) return;
    if (ff.avcodec_send_packet(theDecoder.videoCodecContext, pkt) < 0) return;

    AVFrame* in = ff.av_frame_alloc();
    while (ff.avcodec_receive_frame(theDecoder.videoCodecContext, in) == 0)
    {
        if (in->pts >= theEncoder.videoStartPts && (in->pts <= theEncoder.videoEndPts || in->pts == -1))
            processDecodedVideo(in);
        else if (in->pts > theEncoder.videoEndPts && theEncoder.videoEndPts > 0)
			theEncoder.videoMarkOutReached = true;
        ff.av_frame_unref(in);
    }
    ff.av_frame_free(&in);
}

void VideoTranscoder::handleAudioPacket(AVPacket* pkt)
{
    if (!theDecoder.audioCodecContext) return;
    if (ff.avcodec_send_packet(theDecoder.audioCodecContext, pkt) < 0) return;

    AVFrame* in = av_frame_alloc();
    while (ff.avcodec_receive_frame(theDecoder.audioCodecContext, in) == 0)
    {
		if (in->pts >= theEncoder.audioStartPts)
            processDecodedAudio(in);
        ff.av_frame_unref(in);
    }
    ff.av_frame_free(&in);
}

int64_t VideoTranscoder::assignVideoPts(const AVFrame* in_frame)
{
    int64_t new_pts;
    if (in_frame->pts != AV_NOPTS_VALUE)
    {
		// Rescale PTS to encoder timebase. Adjust for start PTS if needed.
        new_pts = ff.av_rescale_q(in_frame->pts - theEncoder.videoStartPts, theDecoder.videoStream->time_base, theEncoder.videoCodecContext->time_base);
		if (theVideoPts >= 0 && new_pts <= theVideoPts) new_pts = theVideoPts + 1; // force monotonic PTS
    }
    else
    {
        new_pts = (theVideoPts >= 0) ? theVideoPts + 1 : 0;
    }
    theVideoPts = new_pts;
    return new_pts;
}

void VideoTranscoder::processDecodedVideo(AVFrame* in)
{
    if (theEncoder.cropWindow.width() > 0 && theEncoder.cropWindow.height() > 0 &&
		theDecoder.cropFilterGraph && theDecoder.cropFilterSrc && theDecoder.cropFilterSink)
    {
        if (ff.av_buffersrc_add_frame_flags(theDecoder.cropFilterSrc, in, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) return;
        AVFrame* filtFrame = ff.av_frame_alloc();
        while (ff.av_buffersink_get_frame(theDecoder.cropFilterSink, filtFrame) >= 0)
        {
            if (ff.av_frame_make_writable(theEncoder.rescaledFrame) < 0) return;
            qDebug() << "crop frame format:" << ff.av_get_pix_fmt_name((AVPixelFormat)filtFrame->format);
            qDebug() << "crop frame size:" << filtFrame->width << "x" << filtFrame->height;
            filtFrame->sample_aspect_ratio = ff.av_make_q(1, 1);
            ff.sws_scale(theEncoder.scalingContext,
                filtFrame->data, filtFrame->linesize,
                0, filtFrame->height,
                theEncoder.rescaledFrame->data,
                theEncoder.rescaledFrame->linesize);
        }
        ff.av_frame_unref(filtFrame);
        ff.av_frame_free(&filtFrame);
    }
    else
    {
        if (ff.av_frame_make_writable(theEncoder.rescaledFrame) < 0) return;
        ff.sws_scale(theEncoder.scalingContext, in->data, in->linesize, 0, theDecoder.videoCodecContext->height,
            theEncoder.rescaledFrame->data, theEncoder.rescaledFrame->linesize);
    }

    theEncoder.rescaledFrame->pts = assignVideoPts(in);

    qDebug() << "Sending to codec frame format:" << ff.av_get_pix_fmt_name((AVPixelFormat)theEncoder.rescaledFrame->format);
    qDebug() << "Sending to codec frame size:" << theEncoder.rescaledFrame->width << "x" << theEncoder.rescaledFrame->height;
    if (ff.avcodec_send_frame(theEncoder.videoCodecContext, theEncoder.rescaledFrame) < 0) return;

    AVPacket* pkt = ff.av_packet_alloc();
    while (ff.avcodec_receive_packet(theEncoder.videoCodecContext, pkt) == 0)
    {
        writePacket(pkt, theEncoder.videoCodecContext, theEncoder.videoStream);
        ff.av_packet_unref(pkt);
    }
    ff.av_packet_free(&pkt);
}

void VideoTranscoder::processDecodedAudio(AVFrame* in)
{
    if (!theDecoder.audioCodecContext) return;

    const int64_t delay = ff.swr_get_delay(theEncoder.resamplingContext, theDecoder.audioCodecContext->sample_rate);
    int dst_nb_samples = (int)ff.av_rescale_rnd(delay + in->nb_samples,
        theEncoder.audioCodecContext->sample_rate,
        theDecoder.audioCodecContext->sample_rate,
        AV_ROUND_UP);

    if (dst_nb_samples <= 0)
    {
        return;
    }

    ff.av_frame_unref(theEncoder.resampledFrame);
    theEncoder.resampledFrame->format = theEncoder.audioCodecContext->sample_fmt;
    theEncoder.resampledFrame->sample_rate = theEncoder.audioCodecContext->sample_rate;
    ff.av_channel_layout_copy(&theEncoder.resampledFrame->ch_layout, &theEncoder.audioCodecContext->ch_layout);
    theEncoder.resampledFrame->nb_samples = dst_nb_samples;

    if (ff.av_frame_get_buffer(theEncoder.resampledFrame, 0) < 0)
    {
        std::cerr << "av_frame_get_buffer(a_frame_conv) failed\n";
        return;
    }

    int out_samples = ff.swr_convert(theEncoder.resamplingContext,
        theEncoder.resampledFrame->data, theEncoder.resampledFrame->nb_samples,
        (const uint8_t**)in->data, in->nb_samples);
    if (out_samples < 0)
    {
        std::cerr << "swr_convert failed\n";
        return;
    }
    theEncoder.resampledFrame->nb_samples = out_samples;

    if (out_samples > 0)
    {
        ff.av_audio_fifo_write(theEncoder.audioFifo, (void**)theEncoder.resampledFrame->data, out_samples);
    }

    if (theEncoder.audioCodecContext->frame_size > 0)
    {
        while (ff.av_audio_fifo_size(theEncoder.audioFifo) >= theEncoder.audioCodecContext->frame_size)
        {
            AVFrame* out = ff.av_frame_alloc();
            out->nb_samples = theEncoder.audioCodecContext->frame_size;
            out->format = theEncoder.audioCodecContext->sample_fmt;
            out->sample_rate = theEncoder.audioCodecContext->sample_rate;
            ff.av_channel_layout_copy(&out->ch_layout, &theEncoder.audioCodecContext->ch_layout);
            ff.av_frame_get_buffer(out, 0);

            ff.av_audio_fifo_read(theEncoder.audioFifo, (void**)out->data, theEncoder.audioCodecContext->frame_size);

            out->pts = theAudioPts;
            theAudioPts += out->nb_samples;

            if (ff.avcodec_send_frame(theEncoder.audioCodecContext, out) >= 0)
            {
                AVPacket* pkt = ff.av_packet_alloc();
                while (ff.avcodec_receive_packet(theEncoder.audioCodecContext, pkt) == 0)
                {
                    writePacket(pkt, theEncoder.audioCodecContext, theEncoder.audioStream);
                    ff.av_packet_unref(pkt);
                }
                ff.av_packet_free(&pkt);
            }
            ff.av_frame_free(&out);
        }
    }
    else
    {
        while (ff.av_audio_fifo_size(theEncoder.audioFifo) > 0)
        {
            const int nb = ff.av_audio_fifo_size(theEncoder.audioFifo);
            AVFrame* out = ff.av_frame_alloc();
            out->nb_samples = nb;
            out->format = theEncoder.audioCodecContext->sample_fmt;
            out->sample_rate = theEncoder.audioCodecContext->sample_rate;
            ff.av_channel_layout_copy(&out->ch_layout, &theEncoder.audioCodecContext->ch_layout);
            ff.av_frame_get_buffer(out, 0);

            ff.av_audio_fifo_read(theEncoder.audioFifo, (void**)out->data, nb);

            out->pts = theAudioPts;
            theAudioPts += out->nb_samples;

            if (ff.avcodec_send_frame(theEncoder.audioCodecContext, out) >= 0)
            {
                AVPacket* pkt = ff.av_packet_alloc();
                while (ff.avcodec_receive_packet(theEncoder.audioCodecContext, pkt) == 0)
                {
                    writePacket(pkt, theEncoder.audioCodecContext, theEncoder.audioStream);
                    ff.av_packet_unref(pkt);
                }
                ff.av_packet_free(&pkt);
            }
            ff.av_frame_free(&out);
        }
    }
}

void VideoTranscoder::flushVideo()
{
    if (!theEncoder.videoCodecContext) return;
    ff.avcodec_send_frame(theEncoder.videoCodecContext, nullptr);
    AVPacket* pkt = ff.av_packet_alloc();
    while (ff.avcodec_receive_packet(theEncoder.videoCodecContext, pkt) == 0)
    {
        writePacket(pkt, theEncoder.videoCodecContext, theEncoder.videoStream);
        ff.av_packet_unref(pkt);
    }
    ff.av_packet_free(&pkt);
}

void VideoTranscoder::flushAudio()
{
    if (!theEncoder.audioCodecContext) return;

    if (theEncoder.audioCodecContext->frame_size > 0)
    {
        while (ff.av_audio_fifo_size(theEncoder.audioFifo) > 0)
        {
            const int nb = std::min(ff.av_audio_fifo_size(theEncoder.audioFifo), theEncoder.audioCodecContext->frame_size);
            AVFrame* out = ff.av_frame_alloc();
            out->nb_samples = nb;
            out->format = theEncoder.audioCodecContext->sample_fmt;
            out->sample_rate = theEncoder.audioCodecContext->sample_rate;
            ff.av_channel_layout_copy(&out->ch_layout, &theEncoder.audioCodecContext->ch_layout);
            ff.av_frame_get_buffer(out, 0);

            ff.av_audio_fifo_read(theEncoder.audioFifo, (void**)out->data, nb);

            out->pts = theAudioPts;
            theAudioPts += out->nb_samples;

            ff.avcodec_send_frame(theEncoder.audioCodecContext, out);
            AVPacket* pkt = ff.av_packet_alloc();
            while (ff.avcodec_receive_packet(theEncoder.audioCodecContext, pkt) == 0)
            {
                writePacket(pkt, theEncoder.audioCodecContext, theEncoder.audioStream);
                ff.av_packet_unref(pkt);
            }
            ff.av_packet_free(&pkt);
            ff.av_frame_free(&out);
        }
    }

    ff.avcodec_send_frame(theEncoder.audioCodecContext, nullptr);
    AVPacket* pkt = ff.av_packet_alloc();
    while (ff.avcodec_receive_packet(theEncoder.audioCodecContext, pkt) == 0)
    {
        writePacket(pkt, theEncoder.audioCodecContext, theEncoder.audioStream);
        ff.av_packet_unref(pkt);
    }
    ff.av_packet_free(&pkt);
}

void VideoTranscoder::writePacket(AVPacket* pkt, AVCodecContext* src_enc, AVStream* out_stream)
{
    ff.av_packet_rescale_ts(pkt, src_enc->time_base, out_stream->time_base);
    pkt->stream_index = out_stream->index;
    ff.av_interleaved_write_frame(theEncoder.formatContext, pkt);
}

int64_t VideoTranscoder::seekStream(AVFormatContext* fmt_ctx, int stream_index, 
                                    int64_t ms, AVRational timebase, 
                                    AVCodecContext* codec_ctx)
{
    int64_t start_pts = millisecondsToTimestamp(ms, timebase);
    int seek_ret = ff.avformat_seek_file(fmt_ctx, stream_index, 0, start_pts, start_pts, AVSEEK_FLAG_BACKWARD);
    if (seek_ret >= 0) ff.avcodec_flush_buffers(codec_ctx);
	return start_pts;
}

int64_t VideoTranscoder::calculatePts(AVStream* stream, int64_t ms)
{
    AVRational one_pts = ff.av_div_q(ff.av_inv_q(stream->time_base), stream->avg_frame_rate);
    int64_t end_frame_num = ms / 1000 * stream->avg_frame_rate.num / stream->avg_frame_rate.den;
    int64_t pts = end_frame_num * one_pts.num / one_pts.den;
	return pts;
}

bool VideoTranscoder::transcode()
{
    emit transcodingProgress(0);
    if (!openInput())
    {
        emit transcodingError("Nie można otworzyć pliku wejściowego: " + theDecoder.fileName);
		return false;
    }
    if (!openOutput())
    {
		emit transcodingError("Nie można otworzyć pliku wyjściowego: " + theEncoder.fileName);
		return false;
    }

    if (!prepareVideoDecoder())
    {
        emit transcodingError("Nie można otworzyć dekodera wideo: " + theDecoder.fileName);
		return false;
    }
    if (!prepareAudioDecoder())
    {
		emit transcodingError("Nie można otworzyć dekodera audio: " + theDecoder.fileName);
		return false;
	}
    if (!prepareVideoEncoder())
    {
		emit transcodingError("Nie można otworzyć enkodera wideo: " + theEncoder.videoCodecName);
		return false;
    }
    if (!prepareAudioEncoder())
    {
		emit transcodingError("Nie można otworzyć enkodera audio: " + theEncoder.audioCodecName);
		return false;
    }
    if (!(theEncoder.formatContext->oformat->flags & AVFMT_NOFILE))
    {
        if (ff.avio_open(&theEncoder.formatContext->pb, theEncoder.fileName.toStdString().c_str(), AVIO_FLAG_WRITE) < 0)
        {
            emit transcodingError("Nie można otworzyć pliku wyjściowego: " + theEncoder.fileName);
			return false;
        }
    }

    // faststart
    AVDictionary* mux_opts = nullptr;
    ff.av_dict_set(&mux_opts, "movflags", "faststart", 0);
    applyMetadata();
    if (ff.avformat_write_header(theEncoder.formatContext, &mux_opts) < 0) { av_dict_free(&mux_opts); return false; }
    ff.av_dict_free(&mux_opts);

    // seek to mark in
	theEncoder.videoStartPts = seekStream(theDecoder.formatContext, theDecoder.videoStream->index,
		theMarks.MillisecondsStart(), theDecoder.videoStream->time_base, 
        theDecoder.videoCodecContext);
    theEncoder.audioStartPts = seekStream(theDecoder.formatContext, theDecoder.audioStream->index,
        theMarks.MillisecondsStart(), theDecoder.audioStream->time_base, 
		theDecoder.audioCodecContext);
	// get PTS of the end mark
    theEncoder.videoEndPts = calculatePts(theDecoder.videoStream, theMarks.MillisecondsEnd());

    // transcoding loop
    AVPacket* pkt = ff.av_packet_alloc();
    int frame_count = 0;
    qint64 total_frames = (qint64)theDecoder.videoStream->nb_frames;
    while (ff.av_read_frame(theDecoder.formatContext, pkt) >= 0)
    {
        if (theDecoder.videoStream && pkt->stream_index == theDecoder.videoStream->index)
        {
            handleVideoPacket(pkt);
            frame_count++;
        }
        else if (theDecoder.audioStream && pkt->stream_index == theDecoder.audioStream->index)
            handleAudioPacket(pkt);
        ff.av_packet_unref(pkt);
        if (theEncoder.videoMarkOutReached)
        {
			// break if mark out is reached
            emit transcodingProgress(100);
            break;
		}
        if (total_frames > 0)
            emit transcodingProgress((int)(((double)frame_count / (double)total_frames) * 100.0));
        qApp->processEvents();
    }
    ff.av_packet_free(&pkt);

    flushVideo();
    flushAudio();

    ff.av_write_trailer(theEncoder.formatContext);
    emit transcodingProgress(100);
	emit transcodingFinished();
    return true;
}
