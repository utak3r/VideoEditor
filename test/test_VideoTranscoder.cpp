#include <gtest/gtest.h>
#include "../src/VideoTranscoder.h"
#include "../src/ffmpegWrapper.h"

class VideoTranscoderTest : public ::testing::Test
{
protected:
	VideoTranscoder transcoder;

	void SetUp() override
	{
		// Code here will be called immediately after the constructor (right before each test).
	}

	void TearDown() override
	{
		// Code here will be called immediately after each test (right before the destructor).
	}

public:
	static void applyMetadata(VideoTranscoder& t)
	{
		t.applyMetadata();
	}

};

TEST_F(VideoTranscoderTest, BasicTest)
{
	// For now this test only checks the mock functions in ffmpegWrapper
	// Once it's finished, we can add more tests for real VideoTranscoder methods

	const AVCodec* dec = FfmpegWrapper::u3_avcodec_find_decoder(AV_CODEC_ID_H264);
	EXPECT_EQ(dec->id, AV_CODEC_ID_H264);

	AVCodecContext* ctx = FfmpegWrapper::u3_avcodec_alloc_context3(dec);
	EXPECT_EQ(ctx->codec_id, AV_CODEC_ID_H264);

	AVDictionary* options = nullptr;
	av_dict_set(&options, "option1", "value1", 0);
	int open = FfmpegWrapper::u3_avcodec_open2(ctx, dec, &options);
	EXPECT_EQ(open, 0);
	
}

TEST_F(VideoTranscoderTest, FilenameTest)
{
	transcoder.setInputFile("input.mp4");
	transcoder.setOutputFile("output.mp4");
	EXPECT_EQ(transcoder.inputFile(), "input.mp4");
	EXPECT_EQ(transcoder.outputFile(), "output.mp4");
}

TEST_F(VideoTranscoderTest, SettingOutputParamsTest)
{
	transcoder.setOutputFramerate(30, 1);
	transcoder.setOutputResolution(1920, 1080);
	transcoder.setOutputScalingFilter(4); // Index 4 of UI combobox is SWS_GAUSS
	transcoder.setOutputVideoCodecName("libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
	transcoder.setOutputVideoCodecPreset("best");
	transcoder.setOutputAudioCodecName("AAC (Advanced Audio Coding)");
	transcoder.setOutputAudioCodecPreset("high");
	transcoder.setCropWindow(QRect(0, 0, 1280, 720));

	VideoTranscoder::CodecHandlers* encoder = transcoder.encoder();
	EXPECT_EQ(encoder->framerate.num, 30);
	EXPECT_EQ(encoder->framerate.den, 1);
	EXPECT_EQ(encoder->videoSize, QSize(1920, 1080));
	EXPECT_EQ(encoder->scalingFilter, SWS_GAUSS);
	EXPECT_EQ(encoder->videoCodecName, "libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
	EXPECT_EQ(encoder->videoCodecPreset, "best");
	EXPECT_EQ(encoder->audioCodecName, "AAC (Advanced Audio Coding)");
	EXPECT_EQ(encoder->audioCodecPreset, "high");
	EXPECT_EQ(encoder->cropWindow, QRect(0, 0, 1280, 720));
}

TEST_F(VideoTranscoderTest, MarkInOutTest)
{
	TimelineMarks marks;
	marks.setMarkIn(5.0);
	marks.setMarkOut(15.0);
	transcoder.setMarks(marks);
	TimelineMarks retMarks = transcoder.marks();
	EXPECT_DOUBLE_EQ(retMarks.MarkIn(), 5.0);
	EXPECT_DOUBLE_EQ(retMarks.MarkOut(), 15.0);
}

TEST_F(VideoTranscoderTest, MetadataTest)
{
	transcoder.setMetadata("title", "Test Video");
	transcoder.setMetadata("author", "Unit Tester");

	VideoTranscoder::CodecHandlers* encoder = transcoder.encoder();
	FfmpegWrapper::u3_avformat_alloc_output_context2(&encoder->formatContext, nullptr, nullptr, "output.mp4");

	VideoTranscoderTest::applyMetadata(transcoder);
	AVDictionary* metadata = encoder->formatContext->metadata;
	auto entry = FfmpegWrapper::u3_av_dict_get(metadata, "title", nullptr, 0);
	EXPECT_STREQ(entry->value, "Test Video");
	entry = FfmpegWrapper::u3_av_dict_get(metadata, "author", nullptr, 0);
	EXPECT_STREQ(entry->value, "Unit Tester");
}

