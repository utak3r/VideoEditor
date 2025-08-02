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

QStringList CodecAAC::getAvailablePresets() const
{
	return {
		"ultrafast", "superfast", "veryfast", "faster", "fast",
		"medium", "slow", "slower", "veryslow"
	};
}

void CodecAAC::setPreset(const QString& preset, AVCodecContext* codecContext)
{
	if (codecContext)
	{
		int OUTPUT_CHANNELS = 2;
		int OUTPUT_BIT_RATE = 196000;
		av_channel_layout_default(&codecContext->ch_layout, OUTPUT_CHANNELS);
		//codecContext->sample_rate = sample_rate;
		//codecContext->sample_fmt = avc->sample_fmts[0];
		codecContext->bit_rate = OUTPUT_BIT_RATE;
		//codecContext->time_base = AVRational{ 1, sample_rate };

		codecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
	}
}

const AVCodec* CodecAAC::getAVCodec() const
{
	return avcodec_find_encoder_by_name("aac");
}
