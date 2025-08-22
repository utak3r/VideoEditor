#pragma once
#include <QObject>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

class Tools
{
public:
	/**
	* Retrieves the list of available video and audio encoders.
	*/
	static void getAvailableEncoders(QStringList& videoCodecs, QStringList& audioCodecs);
	
	/** 
	* Converts time in mseconds to a timestamp based on the given time base.
	*/
	static qint64 millisecondsToTimestamp(qint64 msecs, AVRational timeBase);
	
	/**
	* Seeks given stream to the nearest I-frame before the specified time in milliseconds.
	*/
	static qint64 seekStreamBackIFrame(AVFormatContext* fmt_ctx, int stream_index, qint64 ms, AVRational timebase, AVCodecContext* codec_ctx);

	/**
	* Seeks given stream to the exact frame at the specified time in milliseconds.
	*/
	static qint64 seekStreamExactAnyFrame(AVFormatContext* fmt_ctx, int stream_index, qint64 ms, AVRational timebase, AVCodecContext* codec_ctx);

	/**
	* Returns a human-readable error string for the given FFmpeg error code.
	*/
	static QString ffmpegErrorString(int errnum);
};
