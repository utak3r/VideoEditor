// https://trac.ffmpeg.org/wiki/Encode/H.264

#include "codec_x264.h"
extern "C" {
#include <libavutil/opt.h>
}

Codec::CodecType CodecX264::type() const
{
	return CodecType::Video;
}

QString CodecX264::name() const
{
	return "libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10";
}

const AVCodec* CodecX264::getAVCodec() const
{
	return avcodec_find_encoder_by_name("libx264");
}

QStringList CodecX264::getAvailablePresets() const
{
	return {
		"ultrafast", "fast", "normal", "normal animation", "high", "high animation", "best", "best animation"
	};
}

void CodecX264::setPreset(const QString& preset, AVCodecContext* codecContext)
{
	if (codecContext && codecContext->priv_data)
	{
		if (preset == "ultrafast")
		{
			av_opt_set(codecContext->priv_data, "preset", "ultrafast", 0);
			av_opt_set(codecContext->priv_data, "profile", "baseline", 0);
		}
		else if (preset == "fast")
		{
			av_opt_set(codecContext->priv_data, "preset", "fast", 0);
			av_opt_set(codecContext->priv_data, "profile", "main", 0);
		}
		else if (preset == "normal")
		{
			av_opt_set(codecContext->priv_data, "preset", "medium", 0);
			av_opt_set(codecContext->priv_data, "tune", "film", 0);
		}
		else if (preset == "normal animation")
		{
			av_opt_set(codecContext->priv_data, "preset", "medium", 0);
			av_opt_set(codecContext->priv_data, "tune", "animation", 0);
		}
		else if (preset == "high")
		{
			av_opt_set(codecContext->priv_data, "preset", "slow", 0);
			av_opt_set(codecContext->priv_data, "tune", "film", 0);
		}
		else if (preset == "high animation")
		{
			av_opt_set(codecContext->priv_data, "preset", "slow", 0);
			av_opt_set(codecContext->priv_data, "tune", "animation", 0);
		}
		else if (preset == "best")
		{
			av_opt_set(codecContext->priv_data, "preset", "veryslow", 0);
			av_opt_set(codecContext->priv_data, "tune", "film", 0);
		}
		else if (preset == "best animation")
		{
			av_opt_set(codecContext->priv_data, "preset", "veryslow", 0);
			av_opt_set(codecContext->priv_data, "tune", "animation", 0);
		}

		//if (codecContext->codec_id == AV_CODEC_ID_H264)
		//	av_opt_set(codecContext->priv_data, "x264opts", "keyint=250:min-keyint=25:no-scenecut", 0);
	}
}
