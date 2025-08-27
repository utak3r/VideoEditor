#include <ffmpeg.h>
#include <gtest/gtest.h>
#include <VideoTranscoder.h>
#include <codec_x264.h>
#include <codec_aac.h>

class VideoTranscoderFunctionalTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }

    void TearDown() override
	{	
	}

public:
	static bool openInput(VideoTranscoder& t)
	{
		return t.openInput();
	}
	static bool openOutput(VideoTranscoder& t)
	{
		return t.openOutput();
	}

};

TEST_F(VideoTranscoderFunctionalTest, OpenInputFile)
{
	FFmpegReal* ffmpeg = new FFmpegReal();
	VideoTranscoder* transcoder = new VideoTranscoder(*ffmpeg);
	transcoder->setInputFile("data/sample.mp4");
	ASSERT_TRUE(VideoTranscoderFunctionalTest::openInput(*transcoder));
	EXPECT_NE(transcoder->decoder()->videoStream, nullptr);
	EXPECT_NE(transcoder->decoder()->audioStream, nullptr);
}
