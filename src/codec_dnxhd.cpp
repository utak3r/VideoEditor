// https://trac.ffmpeg.org/wiki/Encode/H.264

#include "codec_dnxhd.h"
extern "C" {
#include <libavutil/opt.h>
}

Codec::CodecType CodecDnXHD::type() const
{
	return CodecType::Video;
}

QString CodecDnXHD::name() const
{
	return "VC3/DNxHD";
}

const AVCodec* CodecDnXHD::getAVCodec() const
{
	return avcodec_find_encoder_by_name("dnxhd");
}

QStringList CodecDnXHD::getAvailablePresets() const
{
	return {
		"1280x720p 90Mbps yuv422p10", "1280x720p 220Mbps yuv422p",
		"1920x1080p 185Mbps yuv422p10", "1920x1080p 365Mbps yuv422p10"
	};
}

void CodecDnXHD::setPreset(const QString& preset, AVCodecContext* codecContext)
{
	if (codecContext && codecContext->priv_data)
	{
		if (preset == "1280x720p 90Mbps yuv422p10")
		{
			av_opt_set(codecContext->priv_data, "profile", "dnxhr_90", 0);
			av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p10", 0);
			av_opt_set(codecContext->priv_data, "s", "1280x720", 0);
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
		else if (preset == "1280x720p 220Mbps yuv422p")
		{
			av_opt_set(codecContext->priv_data, "profile", "dnxhr_hq", 0);
			av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p", 0);
			av_opt_set(codecContext->priv_data, "s", "1280x720", 0);
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
		else if (preset == "1920x1080p 185Mbps yuv422p10")
		{
			av_opt_set(codecContext->priv_data, "profile", "dnxhr_185", 0);
			av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p10", 0);
			av_opt_set(codecContext->priv_data, "s", "1920x1080", 0);
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
		else if (preset == "1920x1080p 365Mbps yuv422p10")
		{
			av_opt_set(codecContext->priv_data, "profile", "dnxhr_444", 0);
			av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p10", 0);
			av_opt_set(codecContext->priv_data, "s", "1920x1080", 0);
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
	}
}
