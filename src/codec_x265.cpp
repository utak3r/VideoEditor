// https://trac.ffmpeg.org/wiki/Encode/H.265

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

const AVCodec* CodecX265::getAVCodec() const
{
	return avcodec_find_encoder_by_name("libx265");
}

QStringList CodecX265::getAvailablePresets() const
{
	return {
		"fast", "normal", "best"
	};
}

void CodecX265::setPreset(const QString& preset, AVCodecContext* codecContext)
{
	if (codecContext && codecContext->priv_data)
	{
		if (preset == "fast")
		{
			av_opt_set(codecContext->priv_data, "preset", "fast", 0);
			av_opt_set(codecContext->priv_data, "crf", "36", 0);
		}
		else if (preset == "normal")
		{
			av_opt_set(codecContext->priv_data, "preset", "medium", 0);
			av_opt_set(codecContext->priv_data, "crf", "28", 0);
		}
		else if (preset == "best")
		{
			av_opt_set(codecContext->priv_data, "preset", "veryslow", 0);
			av_opt_set(codecContext->priv_data, "crf", "20", 0);
		}
	}
}
