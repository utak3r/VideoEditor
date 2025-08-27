#include "ffmpegMock.h"
#include <gtest/gtest.h>
#include "../src/VideoTranscoder.h"
#include "codec_x264.h"
#include "codec_aac.h"


using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::StrEq;
using ::testing::AnyNumber;

class VideoTranscoderTest : public ::testing::Test
{
protected:
	FFmpegMock ff;
	VideoTranscoder transcoder;

	VideoTranscoder::CodecHandlers* decoder = nullptr;
	VideoTranscoder::CodecHandlers* encoder = nullptr;

	VideoTranscoderTest() : transcoder(ff) {}

	void SetUp() override
	{
		decoder = transcoder.decoder();
		encoder = transcoder.encoder();

		// Let's ignore calls on cleanup
		// most of them come from destructor of CodecHandlers struct.

		EXPECT_CALL(ff, av_frame_free(_))
			.Times(AnyNumber())
			.WillRepeatedly([](AVFrame** f) {
			if (f) *f = nullptr;
				});

		EXPECT_CALL(ff, avcodec_free_context(_))
			.Times(AnyNumber())
			.WillRepeatedly([](AVCodecContext** ctx) {
			if (ctx) *ctx = nullptr;
				});

		EXPECT_CALL(ff, avformat_close_input(_))
			.Times(AnyNumber());

		EXPECT_CALL(ff, avformat_free_context(_))
			.Times(AnyNumber());

		EXPECT_CALL(ff, sws_freeContext(_))
			.Times(AnyNumber());

		EXPECT_CALL(ff, swr_free(_))
			.Times(AnyNumber());

		EXPECT_CALL(ff, av_audio_fifo_free(_))
			.Times(AnyNumber());

		EXPECT_CALL(ff, avio_closep(_))
			.Times(AnyNumber());

	}

	void TearDown() override
	{
		
	}

public:
	static void applyMetadata(VideoTranscoder& t)
	{
		t.applyMetadata();
	}
	static bool openInput(VideoTranscoder& t)
	{
		return t.openInput();
	}
	static bool openOutput(VideoTranscoder& t)
	{
		return t.openOutput();
	}
	static bool prepareVideoDecoder(VideoTranscoder& t)
	{
		return t.prepareVideoDecoder();
	}
	static bool prepareVideoEncoder(VideoTranscoder& t)
	{
		return t.prepareVideoEncoder();
	}

};


TEST_F(VideoTranscoderTest, FilenameTest_Success)
{
	transcoder.setInputFile("input.mp4");
	transcoder.setOutputFile("output.mp4");
	EXPECT_EQ(transcoder.inputFile(), "input.mp4");
	EXPECT_EQ(transcoder.outputFile(), "output.mp4");
}

TEST_F(VideoTranscoderTest, SettingOutputParamsTest_Success)
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

TEST_F(VideoTranscoderTest, MarkInOutTest_Success)
{
	TimelineMarks marks;
	marks.setMarkIn(5.0);
	marks.setMarkOut(15.0);
	transcoder.setMarks(marks);
	TimelineMarks retMarks = transcoder.marks();
	EXPECT_DOUBLE_EQ(retMarks.MarkIn(), 5.0);
	EXPECT_DOUBLE_EQ(retMarks.MarkOut(), 15.0);
}

TEST_F(VideoTranscoderTest, MetadataTest_Success)
{
	transcoder.setMetadata("title", "Test Video");
	transcoder.setMetadata("author", "Unit Tester");

	//AVFormatContext fakeFormatContext{};
	//encoder->formatContext = &fakeFormatContext;
	avformat_alloc_output_context2(&encoder->formatContext, nullptr, nullptr, "output.mp4");

	EXPECT_CALL(ff, av_dict_set(&encoder->formatContext->metadata, StrEq("title"), StrEq("Test Video"), 0))
		.WillOnce(testing::Return(0));
	EXPECT_CALL(ff, av_dict_set(&encoder->formatContext->metadata, StrEq("author"), StrEq("Unit Tester"), 0))
		.WillOnce(testing::Return(0));

	VideoTranscoderTest::applyMetadata(transcoder);
}

TEST_F(VideoTranscoderTest, OpenInput_SuccessWithVideoAndAudio)
{
	AVStream videoStream{};
	AVStream audioStream{};
	AVStream* streams[2] = { &videoStream, &audioStream };
	AVFormatContext ctx{};
	ctx.streams = streams;
	ctx.nb_streams = 2;

	EXPECT_CALL(ff, avformat_open_input(_, _, _, _))
		.WillOnce(DoAll(SetArgPointee<0>(&ctx), Return(0)));
	EXPECT_CALL(ff, avformat_find_stream_info(&ctx, nullptr))
		.WillOnce(Return(0));
	EXPECT_CALL(ff, av_find_best_stream(&ctx, AVMEDIA_TYPE_VIDEO, _, _, _, _))
		.WillOnce(Return(0));
	EXPECT_CALL(ff, av_find_best_stream(&ctx, AVMEDIA_TYPE_AUDIO, _, _, _, _))
		.WillOnce(Return(1));

	decoder->fileName = "dummy.mp4";
	EXPECT_TRUE(VideoTranscoderTest::openInput(transcoder));
	EXPECT_EQ(decoder->videoStream, &videoStream);
	EXPECT_EQ(decoder->audioStream, &audioStream);
}

TEST_F(VideoTranscoderTest, OpenInput_FailsOnOpen)
{
	EXPECT_CALL(ff, avformat_open_input(_, _, _, _))
		.WillOnce(Return(-1));

	decoder->fileName = "dummy.mp4";
	EXPECT_FALSE(VideoTranscoderTest::openInput(transcoder));
}

TEST_F(VideoTranscoderTest, OpenInput_OnlyVideoStream)
{
	AVStream videoStream{};
	AVStream* streams[1] = { &videoStream };
	AVFormatContext ctx{};
	ctx.streams = streams;
	ctx.nb_streams = 1;

	EXPECT_CALL(ff, avformat_open_input(_, _, _, _))
		.WillOnce(DoAll(SetArgPointee<0>(&ctx), Return(0)));
	EXPECT_CALL(ff, avformat_find_stream_info(&ctx, nullptr))
		.WillOnce(Return(0));
	EXPECT_CALL(ff, av_find_best_stream(&ctx, AVMEDIA_TYPE_VIDEO, _, _, _, _))
		.WillOnce(Return(0));
	EXPECT_CALL(ff, av_find_best_stream(&ctx, AVMEDIA_TYPE_AUDIO, _, _, _, _))
		.WillOnce(Return(-1));

	decoder->fileName = "dummy.mp4";
	bool result = VideoTranscoderTest::openInput(transcoder);

	EXPECT_TRUE(result);
	EXPECT_EQ(decoder->videoStream, &videoStream);
	EXPECT_EQ(decoder->audioStream, nullptr);
}

TEST_F(VideoTranscoderTest, OpenOutputTest_Success)
{
	transcoder.setOutputFile("test_output.mp4");
	
	encoder->formatContext = nullptr;
	AVFormatContext ctx{};
	AVOutputFormat fakeOutFormat{};
	ctx.oformat = &fakeOutFormat;

	
	EXPECT_CALL(ff, avformat_alloc_output_context2(&encoder->formatContext, nullptr, nullptr, StrEq("test_output.mp4")))
		.WillOnce(DoAll(SetArgPointee<0>(&ctx), Return(0)));
	EXPECT_TRUE(VideoTranscoderTest::openOutput(transcoder));
	EXPECT_NE(encoder->formatContext, nullptr);
}

TEST_F(VideoTranscoderTest, prepareVideoDecoderTest_Success)
{
	AVStream fakeStream{};
	AVCodecParameters fakeCodecParams{};
	fakeCodecParams.codec_id = AV_CODEC_ID_H264;
	fakeStream.codecpar = &fakeCodecParams;
	AVCodec fakeCodec;
	decoder->videoStream = &fakeStream;

	EXPECT_CALL(ff, avcodec_find_decoder(testing::_))
		.WillOnce(testing::Return(&fakeCodec));
	EXPECT_CALL(ff, avcodec_alloc_context3(testing::_))
		.WillOnce(testing::Return((AVCodecContext*)0x1));
	EXPECT_CALL(ff, avcodec_parameters_to_context(testing::_, testing::_))
		.WillOnce(testing::Return(0));
	EXPECT_CALL(ff, avcodec_open2(testing::_, testing::_, nullptr))
		.WillOnce(testing::Return(0));

	EXPECT_TRUE(VideoTranscoderTest::prepareVideoDecoder(transcoder));
}

TEST_F(VideoTranscoderTest, PrepareVideoEncoder_Success)
{
	static CodecRegistrar<CodecX264> registrarX264("libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10");
	static CodecRegistrar<CodecAAC> registrarAAC("AAC (Advanced Audio Coding)");
	encoder->videoCodecName = "libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10";
	encoder->videoCodecPreset = "best";
	AVCodec fakeCodec{};
	AVCodecContext fakeOutCtx{};
	AVCodecContext fakeInCtx{};
	fakeInCtx.width = 1920;
	fakeInCtx.height = 1080;
	decoder->videoCodecContext = &fakeInCtx;
	encoder->videoSize = QSize(1920, 1080);
	AVRational fakeFps{ 1, 30 };
	AVFormatContext fakeFormatCtx{};
	AVOutputFormat fakeOutFormat{};
	fakeFormatCtx.oformat = &fakeOutFormat;
	encoder->formatContext = &fakeFormatCtx;
	AVStream fakeStream{};
	decoder->videoStream = &fakeStream;
	AVCodecParameters fakeCodecParams{};
	fakeStream.codecpar = &fakeCodecParams;
	AVFrame fakeFrame{};


	EXPECT_CALL(ff, avcodec_find_encoder_by_name(StrEq("libx264")))
		.WillOnce(Return(&fakeCodec));

	EXPECT_CALL(ff, avcodec_alloc_context3(testing::_))
		.WillOnce(testing::Return(&fakeOutCtx));

	EXPECT_CALL(ff, av_guess_frame_rate(_, _, _))
		.WillOnce(Return(fakeFps));

	EXPECT_CALL(ff, avcodec_get_supported_config(_, _, AV_CODEC_CONFIG_PIX_FORMAT, _, _, _))
		.WillOnce([](const AVCodecContext*, const AVCodec*, enum AVCodecConfig,
			int, const void** values, int* nb_values) {
				static const AVPixelFormat fakeFormats[] = { AV_PIX_FMT_YUV420P };
				*values = fakeFormats;
				*nb_values = 1;
				return 0;
			});

	EXPECT_CALL(ff, avformat_new_stream(testing::_, testing::_))
		.WillOnce(testing::Return(&fakeStream));
	EXPECT_CALL(ff, avcodec_open2(&fakeOutCtx, testing::_, nullptr))
		.WillOnce(testing::Return(0));

	EXPECT_CALL(ff, av_frame_alloc()).WillOnce(Return(&fakeFrame));

	EXPECT_TRUE(VideoTranscoderTest::prepareVideoEncoder(transcoder));
}
