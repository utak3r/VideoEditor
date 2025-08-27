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
	return ff->avcodec_find_encoder_by_name("aac");
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
		ff->av_channel_layout_default(&codecContext->ch_layout, 2);

		if (preset == "low")
		{
			ff->av_opt_set_int(codecContext->priv_data, "profile", AV_PROFILE_AAC_LOW, 0);
			codecContext->bit_rate = 64000; // 64 kbps
			codecContext->sample_rate = 44100; // 44.1 kHz
			codecContext->time_base = AVRational{ 1, codecContext->sample_rate };
			//av_opt_set_int(codecContext, "bit_rate", 64000, 0);
		}
		else if (preset == "medium")
		{
			ff->av_opt_set_int(codecContext->priv_data, "profile", AV_PROFILE_AAC_HE, 0);
			codecContext->bit_rate = 128000; // 128 kbps
			codecContext->sample_rate = 44100; // 44.1 kHz
			codecContext->time_base = AVRational{ 1, codecContext->sample_rate };
			//av_opt_set_int(codecContext, "bit_rate", 128000, 0);
		}
		else if (preset == "high")
		{
			ff->av_opt_set_int(codecContext->priv_data, "profile", AV_PROFILE_AAC_HE_V2, 0);
			codecContext->bit_rate = 192000; // 192 kbps
			codecContext->sample_rate = 48000; // 48 kHz
			codecContext->time_base = AVRational{ 1, codecContext->sample_rate };
			//av_opt_set_int(codecContext, "bit_rate", 192000, 0);
		}

		codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	}
}
