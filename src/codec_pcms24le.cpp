// 

#include "codec_pcms24le.h"
extern "C" {
#include <libavutil/opt.h>
}

Codec::CodecType CodecPCMs24le::type() const
{
	return CodecType::Audio;
}

QString CodecPCMs24le::name() const
{
	return "PCM signed 24-bit little-endian";
}

const AVCodec* CodecPCMs24le::getAVCodec() const
{
	return avcodec_find_encoder_by_name("pcm_s24le");
}

QStringList CodecPCMs24le::getAvailablePresets() const
{
	return {
		"normal"
	};
}

void CodecPCMs24le::setPreset(const QString& preset, AVCodecContext* codecContext)
{
	if (codecContext)
	{
		av_channel_layout_default(&codecContext->ch_layout, 2);

		if (preset == "normal")
		{
			//av_opt_set(codecContext->priv_data, "profile", "aac_he_v2", 0);
			codecContext->sample_rate = 48000;
			codecContext->sample_fmt = AV_SAMPLE_FMT_S32;
			//codecContext->bit_rate = 196000;
		}

		//codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	}
}
