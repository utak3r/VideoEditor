#include <gtest/gtest.h>
#include "../src/VideoTranscoder.h"
#include "../src/ffmpegWrapper.h"

TEST(VideoTranscoderTest, BasicTest)
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
