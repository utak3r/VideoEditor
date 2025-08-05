// 

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
	return "SMPTE VC-3/DNxHD";
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
			//av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p10", 0);
			//av_opt_set(codecContext->priv_data, "b", "90000000", 0);
			codecContext->pix_fmt = AV_PIX_FMT_YUV422P10LE;
			codecContext->bit_rate = 90000000;
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
		else if (preset == "1280x720p 220Mbps yuv422p")
		{
			av_opt_set(codecContext->priv_data, "profile", "dnxhr_hq", 0);
			//av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p", 0);
			//av_opt_set(codecContext->priv_data, "b", "220000000", 0);
			codecContext->pix_fmt = AV_PIX_FMT_YUV422P;
			codecContext->bit_rate = 220000000;
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
		else if (preset == "1920x1080p 185Mbps yuv422p10")
		{
			av_opt_set(codecContext->priv_data, "profile", "dnxhr_185", 0);
			//av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p10", 0);
			//av_opt_set(codecContext->priv_data, "b", "185000000", 0);
			codecContext->pix_fmt = AV_PIX_FMT_YUV422P10LE;
			codecContext->bit_rate = 185000000;
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
		else if (preset == "1920x1080p 365Mbps yuv422p10")
		{
			av_opt_set(codecContext->priv_data, "profile", "dnxhr_444", 0);
			//av_opt_set(codecContext->priv_data, "pix_fmt", "yuv422p10", 0);
			//av_opt_set(codecContext->priv_data, "b", "365000000", 0);
			codecContext->pix_fmt = AV_PIX_FMT_YUV422P10LE;
			codecContext->bit_rate = 365000000;
			codecContext->framerate = AVRational({ 60000, 1001 });
		}
	}
}
