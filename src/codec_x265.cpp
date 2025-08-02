#include "codec_x265.h"
extern "C" {
#include <libavutil/opt.h>
}

Codec::CodecType CodecX265::type() const
{
	return CodecType::Video;
}

QString CodecX265::name() const
{
	return "libx265 H.265 / HEVC";
}

QStringList CodecX265::getAvailablePresets() const
{
	return {
		"ultrafast", "superfast", "veryfast", "faster", "fast",
		"medium", "slow", "slower", "veryslow"
	};
}

void CodecX265::setPreset(const QString& preset, AVCodecContext* codecContext)
{
	if (codecContext && codecContext->priv_data)
	{
		av_opt_set(codecContext->priv_data, "preset", "fast", 0);
		av_opt_set(codecContext->priv_data, "tune", "", 0);
		av_opt_set(codecContext->priv_data, "profile", "main", 0);
	}
}

const AVCodec* CodecX265::getAVCodec() const
{
	return avcodec_find_encoder_by_name("libx265");
}
