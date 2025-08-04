#include <QObject>
#include <QSize>
#include "TimelineMarks.h"
#include "Codec.h"

struct AVCodecParameters;
struct AVPacket;
struct SwsContext;

class VideoRecode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString inputPath READ inputPath WRITE setInputPath NOTIFY inputPathChanged)
    Q_PROPERTY(QString outputPath READ outputPath WRITE setOutputPath NOTIFY outputPathChanged)
	Q_PROPERTY(QString videoCodec READ videoCodec WRITE setVideoCodec)
	Q_PROPERTY(QString audioCodec READ audioCodec WRITE setAudioCodec)
	Q_PROPERTY(bool copyVideo READ copyVideo WRITE setCopyVideo)
	Q_PROPERTY(bool copyAudio READ copyAudio WRITE setCopyAudio)
	Q_PROPERTY(QString videoCodecPreset READ videoCodecPreset WRITE setVideoCodecPreset)
	Q_PROPERTY(QString audioCodecPreset READ audioCodecPreset WRITE setAudioCodecPreset)
	Q_PROPERTY(QString videoCodecTune READ videoCodecTune WRITE setVideoCodecTune)
	Q_PROPERTY(QString videoCodecProfile READ videoCodecProfile WRITE setVideoCodecProfile)
	Q_PROPERTY(TimelineMarks marks READ marks WRITE setMarks)
	Q_PROPERTY(bool scalingEnabled READ scalingEnabled WRITE setScalingEnabled)
	Q_PROPERTY(QSize scalingSize READ scalingSize WRITE setScalingSize)
	Q_PROPERTY(int scalingFilter READ scalingFilter WRITE setScalingFilter)
            
    Q_PROPERTY(QString targetFormat READ targetFormat WRITE setTargetFormat)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage CONSTANT)

public:
	Q_SIGNAL void inputPathChanged();
	Q_SIGNAL void outputPathChanged();
	Q_SIGNAL void isRunningChanged();
	Q_SIGNAL void recodeProgress(int progress);
	Q_SIGNAL void recodeFinished();
	Q_SIGNAL void recodeError(const QString& errorMessage);

    struct StreamContext
    {
        std::unique_ptr<Codec> video_codec;
        std::unique_ptr<Codec> audio_codec;
        AVFormatContext* avfc;
        const AVCodec* video_avc;
        const AVCodec* audio_avc;
        AVStream* video_avs;
        AVStream* audio_avs;
        AVCodecContext* video_avcc;
        AVCodecContext* audio_avcc;
        int video_index;
        int audio_index;
        QString filename;
		SwsContext* swsc;

        ~StreamContext();
    };

    struct EncodingParams
    {
        bool copy_video;
        bool copy_audio;
        QString output_extension;
        QString muxer_opt_key;
        QString muxer_opt_value;
        QString video_codec;
        QString audio_codec;
		QString video_codec_preset;
		QString audio_codec_preset;
		QString video_codec_tune;
		QString video_codec_profile;
        QString codec_priv_key;
        QString codec_priv_value;
		bool scaling_enabled;
		QSize scaling_size;
    };

public:
    VideoRecode(QObject *parent = nullptr);

    QString inputPath() const;
    void setInputPath(const QString &inputPath);

    QString outputPath() const;
    void setOutputPath(const QString &outputPath);

	QString videoCodec() const;
    void setVideoCodec(const QString &videoCodec);

    QString audioCodec() const;
    void setAudioCodec(const QString &audioCodec);

    bool copyVideo() const;
    void setCopyVideo(bool copyVideo);

    bool copyAudio() const;
    void setCopyAudio(bool copyAudio);

	QString videoCodecPreset() const;
	void setVideoCodecPreset(const QString& videoCodecPreset);

	QString audioCodecPreset() const;
	void setAudioCodecPreset(const QString& audioCodecPreset);

	QString videoCodecTune() const;
    void setVideoCodecTune(const QString& videoCodecTune);

	QString videoCodecProfile() const;
	void setVideoCodecProfile(const QString& videoCodecProfile);

    QString targetFormat() const;
    void setTargetFormat(const QString &format);

	TimelineMarks marks();
	void setMarks(const TimelineMarks& marks);

	bool scalingEnabled() const;
	void setScalingEnabled(bool enabled);

	QSize scalingSize() const;
	void setScalingSize(const QSize& size);

    int scalingFilter() const;
    void setScalingFilter(int filter);

    QString lastErrorMessage() const;

    static void getAvailableEncoders(QStringList& videoCodecs, QStringList& audioCodecs);

public slots:
    void recode();

private:
    //void openInputCodec();
    int getCodecAndContext(AVStream* avs, const AVCodec** avc, AVCodecContext** avcc);
    int openMedia(const QString& filename, AVFormatContext** avfc);
    int prepareDecoder(StreamContext* sc);
    int prepareVideoEncoder(StreamContext* sc, AVCodecContext* decoder_ctx, AVRational input_framerate, EncodingParams sp);
    int prepareAudioEncoder(StreamContext* sc, int sample_rate, EncodingParams sp);
    int prepareCopy(AVFormatContext* avfc, AVStream** avs, AVCodecParameters* decoder_par);
    int remux(AVPacket** pkt, AVFormatContext** avfc, AVRational decoder_tb, AVRational encoder_tb, int64_t pts_start, int64_t pts_end);
    int encodeVideo(StreamContext* decoder, StreamContext* encoder, AVFrame* input_frame, int64_t pts_start);
    int encodeAudio(StreamContext* decoder, StreamContext* encoder, AVFrame* input_frame, int64_t pts_start);
    int transcodeAudio(StreamContext* decoder, StreamContext* encoder, AVPacket* input_packet, AVFrame* input_frame, int64_t pts_start, int64_t pts_end, bool* mark_out_reached);
    int transcodeVideo(StreamContext* decoder, StreamContext* encoder, 
        AVPacket* input_packet, 
        AVFrame* input_frame, AVFrame* scaled_frame, 
        int64_t pts_start, int64_t pts_end, 
        bool* mark_in_reached, bool* mark_out_reached);
    int64_t millisecondsToTimestamp(qint64 msecs, AVRational timeBase);

private:
    QString theInputPath;
    QString theOutputPath;
	QString theVideoCodec;
	QString theVideoCodecPreset;
    QString theVideoCodecTune;
    QString theVideoCodecProfile;
	QString theAudioCodec;
	QString theAudioCodecPreset;
    bool theCopyVideo;
	bool theCopyAudio;
	TimelineMarks theMarks;
    QString theTargetFormat;
    QString theLastErrorMessage;
    int64_t skipped_pts;
	bool theScalingEnabled;
    QSize theScalingSize;
    int theScalingFilter;
};
