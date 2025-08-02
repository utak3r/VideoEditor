// https://trac.ffmpeg.org/wiki/Encode/AAC

#include "codec_aac.h"
extern "C" {
#include <libavutil/opt.h>
}

Codec::CodecType CodecAAC::type() const
{
	return CodecType::Audio;
}

QString CodecAAC::name() const
{
	return "AAC (Advanced Audio Coding)";
}

const AVCodec* CodecAAC::getAVCodec() const
{
	return avcodec_find_encoder_by_name("aac");
}

QStringList CodecAAC::getAvailablePresets() const
{
	return {
		"low", "medium", "high"
	};
}

void CodecAAC::setPreset(const QString& preset, AVCodecContext* codecContext)
{
	if (codecContext)
	{
		av_channel_layout_default(&codecContext->ch_layout, 2);

		if (preset == "low")
		{
			av_opt_set(codecContext->priv_data, "profile", "aac_low", 0);
			codecContext->sample_rate = 44100;
			codecContext->bit_rate = 128000;
		}
		else if (preset == "medium")
		{
			av_opt_set(codecContext->priv_data, "profile", "aac_he_v2", 0);
			codecContext->sample_rate = 44100;
			codecContext->bit_rate = 160000;
		}
		else if (preset == "high")
		{
			av_opt_set(codecContext->priv_data, "profile", "aac_he_v2", 0);
			codecContext->sample_rate = 48000;
			codecContext->bit_rate = 196000;
		}

		codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	}
}
