#pragma once

#include <QObject>
#include <QSize>
#include "TimelineMarks.h"
#include "Codec.h"

#include <string>
#include <iostream>
#include <stdexcept>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
}

class VideoTranscoder : public QObject
{
    Q_OBJECT
		Q_PROPERTY(QString inputFile READ inputFile WRITE setInputFile)
        Q_PROPERTY(QString outputFile READ outputFile WRITE setOutputFile)
        Q_PROPERTY(TimelineMarks marks READ marks WRITE setMarks)

public:
    VideoTranscoder(QObject* parent = nullptr);
    ~VideoTranscoder();

    QString inputFile() const;
    void setInputFile(const QString& filename);
	QString outputFile() const;
    void setOutputFile(const QString& filename);

    void setOutputFramerate(int num, int den = 1);
    void setOutputResolution(int w, int h);
    void setOutputVideoCodecName(const QString& n);
	void setOutputVideoCodecPreset(const QString& preset);
    void setOutputAudioCodecName(const QString& n);
	void setOutputAudioCodecPreset(const QString& preset);

    TimelineMarks marks();
    void setMarks(const TimelineMarks& marks);

    bool transcode();

    Q_SIGNAL void recodeProgress(int progress);
    Q_SIGNAL void recodeFinished();
    Q_SIGNAL void recodeError(const QString& errorMessage);

    struct CodecHandlers
    {
        QString fileName = "";
        std::unique_ptr<Codec> videoCodec;
        std::unique_ptr<Codec> audioCodec;
        QString videoCodecName = "libx264";
		AVCodecID videoCodecId = AV_CODEC_ID_H264;
		QString videoCodecPreset = "";
		QString audioCodecName = "aac";
		AVCodecID audioCodecId = AV_CODEC_ID_AAC;
		QString audioCodecPreset = "";
		AVStream* videoStream = nullptr;
		AVStream* audioStream = nullptr;
		AVFormatContext* formatContext = nullptr;
		AVCodecContext* videoCodecContext = nullptr;
		AVCodecContext* audioCodecContext = nullptr;
		SwsContext* scalingContext = nullptr;
		SwrContext* resamplingContext = nullptr;
        AVFrame* rescaledFrame = nullptr;
		AVFrame* resampledFrame = nullptr;
		AVAudioFifo* audioFifo = nullptr;
		QSize videoSize{ 0, 0 };
        bool customFramerate = false;
        AVRational framerate;
        int64_t videoStartPts = 0;
        int64_t audioStartPts = 0;
        int64_t videoEndPts = 0;
		bool videoMarkOutReached = false;

        ~CodecHandlers();
    };

private:
    CodecHandlers theDecoder;
	CodecHandlers theEncoder;
    TimelineMarks theMarks;

    int64_t theVideoPts = -1;
    int64_t theAudioPts = 0;

private:
    bool openInput();
    bool openOutput();
    bool prepareVideoDecoder();
    bool prepareAudioDecoder();
    bool prepareVideoEncoder();
    bool prepareAudioEncoder();
    void initScaler();
    void initResampler();

    void handleVideoPacket(AVPacket* pkt);
    void handleAudioPacket(AVPacket* pkt);

    void processDecodedVideo(AVFrame* in);
    void processDecodedAudio(AVFrame* in);

    int64_t assignVideoPts(const AVFrame* in_frame);

    void flushVideo();
    void flushAudio();
    void writePacket(AVPacket* pkt, AVCodecContext* src_enc, AVStream* out_stream);

	int64_t millisecondsToTimestamp(qint64 msecs, AVRational timeBase) const;
    int64_t seekStream(AVFormatContext* fmt_ctx, int stream_index, int64_t ms, AVRational timebase, AVCodecContext* codec_ctx);
    int64_t calculatePts(AVStream* stream, int64_t ms);
    AVRational pickInputFramerate() const;
    QSize calculateOutputSize(int src_width, int src_height, QSize target_size) const;
    void cleanup();
};
