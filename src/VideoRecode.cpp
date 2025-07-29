#include <QObject>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QStandardPaths>
#include <QDebug>
#include <QApplication>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavcodec/codec_par.h>
}

#include "VideoRecode.h"

static QString AVError2QString(int errnum) 
{
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    av_make_error_string(errbuf, AV_ERROR_MAX_STRING_SIZE, errnum);
    return QString(errbuf);
}

VideoRecode::VideoRecode(QObject *parent) 
: 
    QObject(parent),
    theInputPath(""), theOutputPath(""), 
    theVideoCodec("libx264"), theAudioCodec("aac"),
    theCopyVideo(false), theCopyAudio(false),
	theVideoCodecPreset("medium"), theVideoCodecTune(""), theVideoCodecProfile(""),
    theTargetFormat("mp4"),
	theLastErrorMessage("")
{
}

QString VideoRecode::inputPath() const
{
	return theInputPath;
}

void VideoRecode::setInputPath(const QString& inputPath)
{
    if (theInputPath != inputPath)
    {
        theInputPath = inputPath;
        emit inputPathChanged();
	}
}

QString VideoRecode::outputPath() const
{
	return theOutputPath;
}

void VideoRecode::setOutputPath(const QString& outputPath)
{
    if (theOutputPath != outputPath)
    {
        theOutputPath = outputPath;
        emit outputPathChanged();
	}
}

QString VideoRecode::videoCodec() const
{
    return theVideoCodec;
}

void VideoRecode::setVideoCodec(const QString& videoCodec)
{
    if (theVideoCodec != videoCodec)
    {
        theVideoCodec = videoCodec;
	}
}

QString VideoRecode::audioCodec() const
{
    return theAudioCodec;
}

void VideoRecode::setAudioCodec(const QString& audioCodec)
{
    if (theAudioCodec != audioCodec)
    {
        theAudioCodec = audioCodec;
	}
}

bool VideoRecode::copyVideo() const
{
    return theCopyVideo;
}

void VideoRecode::setCopyVideo(bool copyVideo)
{
    if (theCopyVideo != copyVideo)
    {
        theCopyVideo = copyVideo;
	}
}

bool VideoRecode::copyAudio() const
{
    return theCopyAudio;
}

void VideoRecode::setCopyAudio(bool copyAudio)
{
    if (theCopyAudio != copyAudio)
    {
        theCopyAudio = copyAudio;
	}
}

QString VideoRecode::videoCodecPreset() const
{
    return theVideoCodecPreset;
}

void VideoRecode::setVideoCodecPreset(const QString& videoCodecPreset)
{
    if (theVideoCodecPreset != videoCodecPreset)
    {
        theVideoCodecPreset = videoCodecPreset;
	}
}

QString VideoRecode::videoCodecTune() const
{
    return theVideoCodecTune;
}

void VideoRecode::setVideoCodecTune(const QString& videoCodecTune)
{
    if (theVideoCodecTune != videoCodecTune)
    {
        theVideoCodecTune = videoCodecTune;
	}
}

QString VideoRecode::videoCodecProfile() const
{
    return theVideoCodecProfile;
}

void VideoRecode::setVideoCodecProfile(const QString& videoCodecProfile)
{
    if (theVideoCodecProfile != videoCodecProfile)
    {
        theVideoCodecProfile = videoCodecProfile;
	}
}

QString VideoRecode::targetFormat() const
{
	return theTargetFormat;
}

void VideoRecode::setTargetFormat(const QString& format)
{
    if (theTargetFormat != format)
    {
        theTargetFormat = format;
	}
}

QString VideoRecode::lastErrorMessage() const
{
    return theLastErrorMessage;
}

int VideoRecode::getCodecAndContext(AVStream* avs, const AVCodec** avc, AVCodecContext** avcc)
{
    *avc = avcodec_find_decoder(avs->codecpar->codec_id);
    if (!*avc) 
    {
		qDebug() << "Failed to find codec for stream" << avs->index << "with codec ID" << avs->codecpar->codec_id;
        return -1;
    }

    *avcc = avcodec_alloc_context3(*avc);
    if (!*avcc) 
    { 
		qDebug() << "Failed to allocate codec context for stream" << avs->index;
        return -1;
    }

    if (avcodec_parameters_to_context(*avcc, avs->codecpar) < 0) 
    { 
		qDebug() << "Failed to fill codec context for stream" << avs->index;
		avcodec_free_context(avcc);
        return -1;
    }

    if (avcodec_open2(*avcc, *avc, NULL) < 0) 
    { 
		qDebug() << "Failed to open codec for stream" << avs->index;
		avcodec_free_context(avcc);
		*avc = nullptr;
        return -1;
    }
    return 0;
}

int VideoRecode::openMedia(const QString& filename, AVFormatContext** avfc)
{
    *avfc = avformat_alloc_context();
    if (!*avfc) 
    { 
		qDebug() << "Failed to allocate AVFormatContext";
        return -1; 
    }

    if (avformat_open_input(avfc, filename.toStdString().c_str(), NULL, NULL) != 0)
    { 
		qDebug() << "Failed to open input file" << filename;
		avformat_free_context(*avfc);
        return -1;
    }

    if (avformat_find_stream_info(*avfc, NULL) < 0) 
    { 
		qDebug() << "Failed to find stream info for file" << filename;
		avformat_free_context(*avfc);
		*avfc = nullptr;
        return -1; 
    }
    return 0;
}

int VideoRecode::prepareDecoder(StreamContext* sc)
{
    for (int i = 0; i < sc->avfc->nb_streams; i++) 
    {
        if (sc->avfc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) 
        {
            sc->video_avs = sc->avfc->streams[i];
            sc->video_index = i;

            if (getCodecAndContext(sc->video_avs, &sc->video_avc, &sc->video_avcc))
            { 
                return -1; 
            }
        }
        else if (sc->avfc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) 
        {
            sc->audio_avs = sc->avfc->streams[i];
            sc->audio_index = i;

            if (getCodecAndContext(sc->audio_avs, &sc->audio_avc, &sc->audio_avcc))
            { 
                return -1; 
            }
        }
        else 
        {
			qDebug() << "Skipping stream" << i << "with type" << sc->avfc->streams[i]->codecpar->codec_type;
        }
    }

    return 0;
}

int VideoRecode::prepareVideoEncoder(StreamContext* sc, AVCodecContext* decoder_ctx, AVRational input_framerate, EncodingParams params)
{
    sc->video_avs = avformat_new_stream(sc->avfc, NULL);

    sc->video_avc = avcodec_find_encoder_by_name(params.video_codec.toStdString().c_str());
    if (!sc->video_avc)
    {
		qDebug() << "Failed to find the proper codec" << params.video_codec;
        return -1;
    }

    sc->video_avcc = avcodec_alloc_context3(sc->video_avc);
    if (!sc->video_avcc)
    {
		qDebug() << "Failed to allocate codec context for video encoder";
        return -1;
    }

    if (!params.video_codec_preset.isEmpty())
        av_opt_set(sc->video_avcc->priv_data, "preset", params.video_codec_preset.toStdString().c_str(), 0);
    if (!params.video_codec_tune.isEmpty())
        av_opt_set(sc->video_avcc->priv_data, "tune", params.video_codec_tune.toStdString().c_str(), 0);
	if (!params.video_codec_profile.isEmpty())
		av_opt_set(sc->video_avcc->priv_data, "profile", params.video_codec_profile.toStdString().c_str(), 0);
    //if (!params.codec_priv_key.isNull() && !params.codec_priv_value.isNull())
    //    av_opt_set(sc->video_avcc->priv_data, params.codec_priv_key.toStdString().c_str(), params.codec_priv_value.toStdString().c_str(), 0);

    sc->video_avcc->height = decoder_ctx->height;
    sc->video_avcc->width = decoder_ctx->width;
    sc->video_avcc->sample_aspect_ratio = decoder_ctx->sample_aspect_ratio;
    if (sc->video_avc->pix_fmts)
        sc->video_avcc->pix_fmt = sc->video_avc->pix_fmts[0];
    else
        sc->video_avcc->pix_fmt = decoder_ctx->pix_fmt;

    //sc->video_avcc->bit_rate = 2 * 1000 * 1000;
    //sc->video_avcc->rc_buffer_size = 4 * 1000 * 1000;
    //sc->video_avcc->rc_max_rate = 2 * 1000 * 1000;
    //sc->video_avcc->rc_min_rate = 2.5 * 1000 * 1000;

    sc->video_avcc->time_base = av_inv_q(input_framerate);
    sc->video_avs->time_base = sc->video_avcc->time_base;

    if (avcodec_open2(sc->video_avcc, sc->video_avc, NULL) < 0)
    {
		qDebug() << "Failed to open video codec" << sc->video_avc->name;
        return -1;
    }
    avcodec_parameters_from_context(sc->video_avs->codecpar, sc->video_avcc);
    return 0;
}

int VideoRecode::prepareAudioEncoder(StreamContext* sc, int sample_rate, EncodingParams sp)
{
    sc->audio_avs = avformat_new_stream(sc->avfc, NULL);

    sc->audio_avc = avcodec_find_encoder_by_name(sp.audio_codec.toStdString().c_str());
    if (!sc->audio_avc) 
    {
		qDebug() << "Failed to find the proper codec" << sp.audio_codec;
        return -1;
    }

    sc->audio_avcc = avcodec_alloc_context3(sc->audio_avc);
    if (!sc->audio_avcc)
    {
		qDebug() << "Failed to allocate codec context for audio encoder";
        return -1;
    }

    int OUTPUT_CHANNELS = 2;
    int OUTPUT_BIT_RATE = 196000;
	av_channel_layout_default(&sc->audio_avcc->ch_layout, OUTPUT_CHANNELS);
    sc->audio_avcc->sample_rate = sample_rate;
    sc->audio_avcc->sample_fmt = sc->audio_avc->sample_fmts[0];
    sc->audio_avcc->bit_rate = OUTPUT_BIT_RATE;
    sc->audio_avcc->time_base = AVRational{ 1, sample_rate };

    sc->audio_avcc->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    sc->audio_avs->time_base = sc->audio_avcc->time_base;

    if (avcodec_open2(sc->audio_avcc, sc->audio_avc, NULL) < 0)
    {
		qDebug() << "Failed to open audio codec" << sc->audio_avc->name;
        return -1;
    }
    avcodec_parameters_from_context(sc->audio_avs->codecpar, sc->audio_avcc);
    return 0;
}

int VideoRecode::prepareCopy(AVFormatContext* avfc, AVStream** avs, AVCodecParameters* decoder_par)
{
    *avs = avformat_new_stream(avfc, NULL);
    avcodec_parameters_copy((*avs)->codecpar, decoder_par);
    return 0;
}

int VideoRecode::remux(AVPacket** pkt, AVFormatContext** avfc, AVRational decoder_tb, AVRational encoder_tb)
{
    av_packet_rescale_ts(*pkt, decoder_tb, encoder_tb);
    if (av_interleaved_write_frame(*avfc, *pkt) < 0)
    {
		qDebug() << "Error while copying stream packet";
        return -1;
    }
    return 0;
}

int VideoRecode::encodeVideo(StreamContext* decoder, StreamContext* encoder, AVFrame* input_frame)
{
    if (input_frame) input_frame->pict_type = AV_PICTURE_TYPE_NONE;

    AVPacket* output_packet = av_packet_alloc();
    if (!output_packet)
    {
		qDebug() << "could not allocate memory for output packet";
        return -1;
    }

    int response = avcodec_send_frame(encoder->video_avcc, input_frame);

    while (response >= 0)
    {
        response = avcodec_receive_packet(encoder->video_avcc, output_packet);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            break;
        }
        else if (response < 0)
        {
			qDebug() << "Error while receiving packet from encoder:" << AVError2QString(response);
            return -1;
        }

        output_packet->stream_index = decoder->video_index;
        output_packet->duration = encoder->video_avs->time_base.den / encoder->video_avs->time_base.num / decoder->video_avs->avg_frame_rate.num * decoder->video_avs->avg_frame_rate.den;

        av_packet_rescale_ts(output_packet, decoder->video_avs->time_base, encoder->video_avs->time_base);
        response = av_interleaved_write_frame(encoder->avfc, output_packet);
        if (response != 0)
        {
			qDebug() << "Error" << response << "while receiving packet from decoder:" << AVError2QString(response);
            return -1;
        }
    }
    av_packet_unref(output_packet);
    av_packet_free(&output_packet);
    return 0;
}

int VideoRecode::encodeAudio(StreamContext* decoder, StreamContext* encoder, AVFrame* input_frame)
{
    AVPacket* output_packet = av_packet_alloc();
    if (!output_packet)
    {
		qDebug() << "could not allocate memory for output packet";
        return -1;
    }

    int response = avcodec_send_frame(encoder->audio_avcc, input_frame);

    while (response >= 0)
    {
        response = avcodec_receive_packet(encoder->audio_avcc, output_packet);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            break;
        }
        else if (response < 0)
        {
			qDebug() << "Error while receiving packet from encoder:" << AVError2QString(response);
            return -1;
        }

        output_packet->stream_index = decoder->audio_index;

        av_packet_rescale_ts(output_packet, decoder->audio_avs->time_base, encoder->audio_avs->time_base);
        response = av_interleaved_write_frame(encoder->avfc, output_packet);
        if (response != 0) 
        {
			qDebug() << "Error" << response << "while receiving packet from decoder:" << AVError2QString(response);
            return -1;
        }
    }
    av_packet_unref(output_packet);
    av_packet_free(&output_packet);
    return 0;
}

int VideoRecode::transcodeAudio(StreamContext* decoder, StreamContext* encoder, AVPacket* input_packet, AVFrame* input_frame)
{
    int response = avcodec_send_packet(decoder->audio_avcc, input_packet);
    if (response < 0)
    {
		qDebug() << "Error while sending packet to decoder:" << AVError2QString(response);
        return response;
    }

    while (response >= 0)
    {
        response = avcodec_receive_frame(decoder->audio_avcc, input_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            break;
        }
        else if (response < 0)
        {
			qDebug() << "Error while receiving frame from decoder:" << AVError2QString(response);
            return response;
        }

        if (response >= 0)
        {
            if (encodeAudio(decoder, encoder, input_frame)) return -1;
        }
        av_frame_unref(input_frame);
    }
    return 0;
}

int VideoRecode::transcodeVideo(StreamContext* decoder, StreamContext* encoder, AVPacket* input_packet, AVFrame* input_frame)
{
    int response = avcodec_send_packet(decoder->video_avcc, input_packet);
    if (response < 0)
    {
		qDebug() << "Error while sending packet to decoder:" << AVError2QString(response);
        return response;
    }

    while (response >= 0)
    {
        response = avcodec_receive_frame(decoder->video_avcc, input_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
        {
            break;
        }
        else if (response < 0)
        {
			qDebug() << "Error while receiving frame from decoder:" << AVError2QString(response);
            return response;
        }

        if (response >= 0)
        {
            if (encodeVideo(decoder, encoder, input_frame)) return -1;
        }
        av_frame_unref(input_frame);
    }
    return 0;
}

void VideoRecode::recode()
{
    EncodingParams params = { 0 };
    params.copy_audio = theCopyAudio;
    params.copy_video = theCopyVideo;
	params.video_codec = theVideoCodec;
	params.video_codec_preset = theVideoCodecPreset;
	params.video_codec_tune = theVideoCodecTune;
	params.video_codec_profile = theVideoCodecProfile;

    if (params.video_codec.isNull() || params.video_codec.isEmpty())
    {
        params.copy_video = true; // If no video codec is specified, copy video
    }
	params.audio_codec = theAudioCodec;
    if (params.audio_codec.isNull() || params.audio_codec.isEmpty())
    {
        params.copy_audio = true; // If no audio codec is specified, copy audio
	}
    
	StreamContext* decoder = new StreamContext();
    decoder->filename = theInputPath;

	StreamContext* encoder = new StreamContext();
	encoder->filename = theOutputPath;

    // replace the filename extension, if provided
    if (!params.output_extension.isNull())
    {
        QFileInfo info(encoder->filename);
        encoder->filename = info.path() + "/" + info.completeBaseName() + params.output_extension;
    }        

    if (openMedia(decoder->filename, &decoder->avfc) == 0)
    {
        if (prepareDecoder(decoder) == 0)
        {
            avformat_alloc_output_context2(&encoder->avfc, NULL, NULL, encoder->filename.toStdString().c_str());
            if (!encoder->avfc)
            {
				qDebug() << "Failed to allocate output format context for" << encoder->filename;
				theLastErrorMessage = "Failed to allocate output format context for " + encoder->filename;
				delete decoder;
				delete encoder;
				//free(decoder);
				emit recodeError(theLastErrorMessage);
                return;
            }

            for (int i = 0; i < decoder->avfc->nb_streams; i++)
            {
                if (decoder->avfc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    if (!params.copy_video)
                    {
                        AVRational input_framerate = av_guess_frame_rate(decoder->avfc, decoder->video_avs, NULL);
                        prepareVideoEncoder(encoder, decoder->video_avcc, input_framerate, params);
                    }
                    else
                    {
                        if (prepareCopy(encoder->avfc, &encoder->video_avs, decoder->video_avs->codecpar)) return;
                    }
                }

                if (decoder->avfc->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
                {
                    if (!params.copy_audio)
                    {
                        if (prepareAudioEncoder(encoder, decoder->audio_avcc->sample_rate, params)) return;
                    }
                    else
                    {
                        if (prepareCopy(encoder->avfc, &encoder->audio_avs, decoder->audio_avs->codecpar)) return;
                    }
                }
            }

            if (encoder->avfc->oformat->flags & AVFMT_GLOBALHEADER)
                encoder->avfc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            if (!(encoder->avfc->oformat->flags & AVFMT_NOFILE))
            {
                if (avio_open(&encoder->avfc->pb, encoder->filename.toStdString().c_str(), AVIO_FLAG_WRITE) < 0)
                {
					theLastErrorMessage = "Could not open output file: " + encoder->filename;
					qDebug() << "Could not open output file" << encoder->filename;
                    emit recodeError(theLastErrorMessage);
                    return;
                }
            }

            AVDictionary* muxer_opts = NULL;
            if (!params.muxer_opt_key.isNull() && !params.muxer_opt_value.isNull())
            {
                av_dict_set(&muxer_opts, params.muxer_opt_key.toStdString().c_str(), params.muxer_opt_value.toStdString().c_str(), 0);
            }

            if (avformat_write_header(encoder->avfc, &muxer_opts) < 0)
            {
				theLastErrorMessage = "An error occurred when opening output file: " + encoder->filename;
				qDebug() << "An error occurred when opening output file" << encoder->filename;
                emit recodeError(theLastErrorMessage);
                return;
            }

            AVFrame* input_frame = av_frame_alloc();
            if (!input_frame)
            {
				theLastErrorMessage = "Failed to allocate memory for AVFrame";
				qDebug() << "Failed to allocate memory for AVFrame";
                emit recodeError(theLastErrorMessage);
                return;
            }

            AVPacket* input_packet = av_packet_alloc();
            if (!input_packet)
            {
				theLastErrorMessage = "Failed to allocate memory for AVPacket";
				qDebug() << "Failed to allocate memory for AVPacket";
                emit recodeError(theLastErrorMessage);
                return;
            }

            int frame_count = 0;
            qint64 total_frames = (qint64)decoder->video_avs->nb_frames;

            while (av_read_frame(decoder->avfc, input_packet) >= 0)
            {
                if (decoder->avfc->streams[input_packet->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    if (!params.copy_video)
                    {
                        // TODO: refactor to be generic for audio and video (receiving a function pointer to the differences)
                        if (transcodeVideo(decoder, encoder, input_packet, input_frame)) return;
                        av_packet_unref(input_packet);
                    }
                    else
                    {
                        if (remux(&input_packet, &encoder->avfc, decoder->video_avs->time_base, encoder->video_avs->time_base)) return;
                    }
					frame_count++;
                }
                else if (decoder->avfc->streams[input_packet->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
                {
                    if (!params.copy_audio)
                    {
                        if (transcodeAudio(decoder, encoder, input_packet, input_frame)) return;
                        av_packet_unref(input_packet);
                    }
                    else
                    {
                        if (remux(&input_packet, &encoder->avfc, decoder->audio_avs->time_base, encoder->audio_avs->time_base)) return;
                    }
                }
                else
                {
					av_packet_unref(input_packet);
                    //theLastErrorMessage = "Ignoring non-video and non-audio packet with stream index " + QString::number(input_packet->stream_index);
                    //qDebug() << "Ignoring non-video and non-audio packet with stream index" << input_packet->stream_index;
                }
                emit recodeProgress((int)((double)frame_count / (double)total_frames * 100.0));
				qApp->processEvents();
            }

            if (!params.copy_video)
                if (encodeVideo(decoder, encoder, nullptr)) return;
            if (!params.copy_audio)
                if (encodeAudio(decoder, encoder, nullptr)) return;

            av_write_trailer(encoder->avfc);

            if (muxer_opts != nullptr)
            {
                av_dict_free(&muxer_opts);
                muxer_opts = nullptr;
            }

            if (input_frame != nullptr)
            {
                av_frame_free(&input_frame);
                input_frame = nullptr;
            }

            if (input_packet != nullptr)
            {
                av_packet_free(&input_packet);
                input_packet = nullptr;
            }

            avformat_close_input(&decoder->avfc);

            avformat_free_context(decoder->avfc); decoder->avfc = nullptr;
            avformat_free_context(encoder->avfc); encoder->avfc = nullptr;

            avcodec_free_context(&decoder->video_avcc); decoder->video_avcc = nullptr;
            avcodec_free_context(&decoder->audio_avcc); decoder->audio_avcc = nullptr;

            delete decoder; decoder = nullptr;
			delete encoder; encoder = nullptr;

			emit recodeFinished();
        }

    }

}