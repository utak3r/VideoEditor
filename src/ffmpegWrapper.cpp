#include "ffmpegWrapper.h"

namespace FfmpegWrapper {

	const AVCodec* u3_avcodec_find_decoder(enum AVCodecID id)
	{
		return ::avcodec_find_decoder(id);
	}

	AVCodecContext* u3_avcodec_alloc_context3(const AVCodec* codec)
	{
		return ::avcodec_alloc_context3(codec);
	}

	int u3_avcodec_parameters_to_context(AVCodecContext* codec, const struct AVCodecParameters* par)
	{
		return ::avcodec_parameters_to_context(codec, par);
	}

	int u3_avcodec_open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options)
	{
		return ::avcodec_open2(avctx, codec, options);
	}


}
