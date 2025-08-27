#include <ffmpeg.h>
#include <gtest/gtest.h>
#include <VideoTranscoder.h>
#include <codec_x264.h>
#include <codec_aac.h>
#include <QFile>

class VideoTranscoderFunctionalTest : public ::testing::Test
{
protected:
	VideoTranscoder* transcoder = nullptr;
	FFmpegReal* ffmpeg = nullptr;

    void SetUp() override
    {
		ffmpeg = new FFmpegReal();
		transcoder = new VideoTranscoder(*ffmpeg);
    }

    void TearDown() override
	{	
		delete transcoder;
		delete ffmpeg;
	}

public:
	static bool openInput(VideoTranscoder& t)
	{
		return t.openInput();
	}

};

TEST_F(VideoTranscoderFunctionalTest, OpenInputFileTest)
{
	transcoder->setInputFile("data/sample.mp4");
	ASSERT_TRUE(VideoTranscoderFunctionalTest::openInput(*transcoder));
	
	AVStream* vstream = transcoder->decoder()->videoStream;
	EXPECT_NE(vstream, nullptr);
	EXPECT_EQ(vstream->time_base.num, 1);
	EXPECT_EQ(vstream->time_base.den, 25000);
	EXPECT_EQ(vstream->nb_frames, 127);

	AVStream* astream = transcoder->decoder()->audioStream;
	EXPECT_NE(astream, nullptr);
	EXPECT_EQ(astream->time_base.num, 1);
	EXPECT_EQ(astream->time_base.den, 44100);
	EXPECT_EQ(astream->nb_frames, 218);
}

TEST_F(VideoTranscoderFunctionalTest, FullTranscodeTest)
{
	static CodecRegistrar<CodecX264> registrarX264("libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
	static CodecRegistrar<CodecAAC> registrarAAC("AAC (Advanced Audio Coding)");

	transcoder->setInputFile("data/sample.mp4");
	QString outFilename = "test_transcoded.mp4";
	transcoder->setOutputFile(outFilename);
	transcoder->setOutputVideoCodecName("libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
	transcoder->setOutputVideoCodecPreset("normal");
	transcoder->setOutputAudioCodecName("AAC (Advanced Audio Coding)");
	transcoder->setOutputAudioCodecPreset("medium");
	transcoder->setCropWindow(QRect(0, 0, 0, 0));
	TimelineMarks marks;
	marks.Reset(5008);
	transcoder->setMarks(marks);

	// perform transcode
	transcoder->transcode();

	// check if the output file exists
	ASSERT_TRUE(QFile::exists(outFilename));

	// open it and test it
	AVFormatContext* fmtCtx = nullptr;
	ASSERT_EQ(ffmpeg->avformat_open_input(&fmtCtx, outFilename.toStdString().c_str(), nullptr, nullptr), 0);
	ASSERT_EQ(ffmpeg->avformat_find_stream_info(fmtCtx, nullptr), 0);
	ASSERT_EQ(fmtCtx->nb_streams, 2);

	AVStream* vstream = nullptr;
	int v_idx = ffmpeg->av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (v_idx >= 0) vstream = fmtCtx->streams[v_idx];
	EXPECT_NE(vstream, nullptr);
	EXPECT_GT(vstream->nb_frames, 100);

	AVStream* astream = nullptr;
	int a_idx = ffmpeg->av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	if (a_idx >= 0) astream = fmtCtx->streams[a_idx];
	EXPECT_NE(astream, nullptr);
	EXPECT_GT(astream->nb_frames, 200);

	ffmpeg->avformat_close_input(&fmtCtx);
}
