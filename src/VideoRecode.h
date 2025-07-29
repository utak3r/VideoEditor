#include <QObject>
#include <QSize>

struct AVCodecParameters;
struct AVPacket;

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
	Q_PROPERTY(QString videoCodecTune READ videoCodecTune WRITE setVideoCodecTune)
	Q_PROPERTY(QString videoCodecProfile READ videoCodecProfile WRITE setVideoCodecProfile)
            
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
		QString video_codec_preset; // ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow
		QString video_codec_tune; // film, animation, grain, stillimage, zerolatency
		QString video_codec_profile; // baseline, main, high, high10, high422, high444, dnxhr_hq, dnxhr_hqx, dnxhr_444, etc.
        QString codec_priv_key;
        QString codec_priv_value;
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

	QString videoCodecTune() const;
    void setVideoCodecTune(const QString& videoCodecTune);

	QString videoCodecProfile() const;
	void setVideoCodecProfile(const QString& videoCodecProfile);

    QString targetFormat() const;
    void setTargetFormat(const QString &format);

    QString lastErrorMessage() const;

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
    int remux(AVPacket** pkt, AVFormatContext** avfc, AVRational decoder_tb, AVRational encoder_tb);
    int encodeVideo(StreamContext* decoder, StreamContext* encoder, AVFrame* input_frame);
    int encodeAudio(StreamContext* decoder, StreamContext* encoder, AVFrame* input_frame);
    int transcodeAudio(StreamContext* decoder, StreamContext* encoder, AVPacket* input_packet, AVFrame* input_frame);
    int transcodeVideo(StreamContext* decoder, StreamContext* encoder, AVPacket* input_packet, AVFrame* input_frame);

private:
    QString theInputPath;
    QString theOutputPath;
	QString theVideoCodec;
	QString theVideoCodecPreset;
    QString theVideoCodecTune;
    QString theVideoCodecProfile;
	QString theAudioCodec;
	bool theCopyVideo;
	bool theCopyAudio;

    QString theTargetFormat;
    QString theLastErrorMessage;
};
