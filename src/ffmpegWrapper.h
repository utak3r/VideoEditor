#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
}

namespace FfmpegWrapper {

	const AVCodec* u3_avcodec_find_decoder(enum AVCodecID id);
	AVCodecContext* u3_avcodec_alloc_context3(const AVCodec* codec);
	int u3_avcodec_parameters_to_context(AVCodecContext* codec, const struct AVCodecParameters* par);
	int u3_avcodec_open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options);

}
