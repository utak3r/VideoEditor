#include "../src/ffmpegWrapper.h"
#include <gtest/gtest.h>

static int returnCode = 0;
AVCodecID lastCodecId = AV_CODEC_ID_NONE;
AVCodecContext* lastCodecContext = nullptr;
AVCodec* lastCodec = nullptr;
struct AVCodecParameters lastCodecParameters;
AVDictionary* lastOptions = nullptr;

namespace FfmpegWrapper {

	const AVCodec* u3_avcodec_find_decoder(enum AVCodecID id)
	{
		lastCodecId = id;
		return ::avcodec_find_decoder(id);
	}

	AVCodecContext* u3_avcodec_alloc_context3(const AVCodec* codec)
	{
		lastCodec = (AVCodec*)codec;
		return ::avcodec_alloc_context3(codec);
	}

	int u3_avcodec_parameters_to_context(AVCodecContext* codec, const struct AVCodecParameters* par)
	{
		lastCodecContext = codec;
		lastCodecParameters = *par;
		return returnCode = 0;
	}

	int u3_avcodec_open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options)
	{
		lastCodecContext = avctx;
		lastCodec = (AVCodec*)codec;
		lastOptions = *options;
		return returnCode = 0;
	}


}

void resetMock()
{
	returnCode = 0;
}

AVCodecID getLastCodecId()
{
	return lastCodecId;
}

int getReturnCode()
{
	return returnCode;
}
