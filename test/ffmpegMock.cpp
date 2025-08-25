#include "../src/ffmpegWrapper.h"
#include <gtest/gtest.h>

static AVCodecID lastCodecId = AV_CODEC_ID_NONE;
static AVCodecContext* lastCodecContext = nullptr;
static AVCodec* lastCodec = nullptr;
static AVDictionary** lastOptions = nullptr;
static int returnCode = 0;

void resetMock()
{
	lastCodecContext = nullptr;
	lastCodec = nullptr;
	lastOptions = nullptr;
	returnCode = 0;
}

AVCodecContext* getLastCtx() { return lastCodecContext; }
const AVCodec* getLastCodec() { return lastCodec; }
AVDictionary** getLastOpts() { return lastOptions; }
void setReturnCode(int code) { returnCode = code; }

namespace FfmpegWrapper {

	const AVCodec* u3_avcodec_find_decoder(enum AVCodecID id)
	{
		lastCodecId = id;
		return ::avcodec_find_decoder(id);
	}

	AVCodecContext* u3_avcodec_alloc_context3(const AVCodec* codec)
	{
		return ::avcodec_alloc_context3(codec);
	}

	int u3_avcodec_parameters_to_context(AVCodecContext* codec, const struct AVCodecParameters* par)
	{
		return ::avcodec_parameters_to_context(codec, par);
	}

	int u3_avcodec_open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options)
	{
		int ret = 0;
		lastCodecContext = avctx;
		lastCodec = (AVCodec*)codec;
		lastOptions = options;
		if (!avctx) ret = -1;
		return ret;
	}

	int u3_av_read_frame(AVFormatContext* s, AVPacket* pkt)
	{
		return ::av_read_frame(s, pkt);
	}

	int u3_avcodec_get_supported_config(const AVCodecContext* avctx,
		const AVCodec* codec, enum AVCodecConfig config,
		unsigned flags, const void** out_configs,
		int* out_num_configs)
	{
		return ::avcodec_get_supported_config(avctx, codec, config, flags, out_configs, out_num_configs);
	}

	void u3_avcodec_free_context(AVCodecContext** avctx)
	{
		::avcodec_free_context(avctx);
	}

	int u3_avcodec_parameters_from_context(struct AVCodecParameters* par,
		const AVCodecContext* codec)
	{
		return ::avcodec_parameters_from_context(par, codec);
	}

	int u3_avcodec_send_packet(AVCodecContext* avctx, const AVPacket* avpkt)
	{
		return ::avcodec_send_packet(avctx, avpkt);
	}

	int u3_avcodec_receive_frame(AVCodecContext* avctx, AVFrame* frame)
	{
		return ::avcodec_receive_frame(avctx, frame);
	}

	int u3_avcodec_send_frame(AVCodecContext* avctx, const AVFrame* frame)
	{
		return ::avcodec_send_frame(avctx, frame);
	}

	AVPacket* u3_av_packet_alloc(void)
	{
		return ::av_packet_alloc();
	}

	int u3_avcodec_receive_packet(AVCodecContext* avctx, AVPacket* avpkt)
	{
		return ::avcodec_receive_packet(avctx, avpkt);
	}

	void u3_av_packet_unref(AVPacket* pkt)
	{
		::av_packet_unref(pkt);
	}

	void u3_av_packet_free(AVPacket** pkt)
	{
		::av_packet_free(pkt);
	}

	int u3_av_interleaved_write_frame(AVFormatContext* s, AVPacket* pkt)
	{
		return ::av_interleaved_write_frame(s, pkt);
	}

	void u3_avcodec_flush_buffers(AVCodecContext* avctx)
	{
		::avcodec_flush_buffers(avctx);
	}

	void u3_avformat_close_input(AVFormatContext** s)
	{
		::avformat_close_input(s);
	}

	void u3_avformat_free_context(AVFormatContext* s)
	{
		::avformat_free_context(s);
	}

	AVRational u3_av_guess_frame_rate(AVFormatContext* ctx, AVStream* stream,
		struct AVFrame* frame)
	{
		return ::av_guess_frame_rate(ctx, stream, frame);
	}

	int u3_avformat_open_input(AVFormatContext** ps, const char* url,
		const AVInputFormat* fmt, AVDictionary** options)
	{
		return ::avformat_open_input(ps, url, fmt, options);
	}

	int u3_avformat_find_stream_info(AVFormatContext* ic, AVDictionary** options)
	{
		return ::avformat_find_stream_info(ic, options);
	}

	int u3_avformat_alloc_output_context2(AVFormatContext** ctx, const AVOutputFormat* oformat,
		const char* format_name, const char* filename)
	{
		return ::avformat_alloc_output_context2(ctx, oformat, format_name, filename);
	}

	AVStream* u3_avformat_new_stream(AVFormatContext* s, const struct AVCodec* c)
	{
		return ::avformat_new_stream(s, c);
	}

	int u3_avformat_seek_file(AVFormatContext* s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags)
	{
		return ::avformat_seek_file(s, stream_index, min_ts, ts, max_ts, flags);
	}

	int u3_avformat_write_header(AVFormatContext* s, AVDictionary** options)
	{
		return ::avformat_write_header(s, options);
	}

	int u3_av_write_trailer(AVFormatContext* s)
	{
		return ::av_write_trailer(s);
	}

	void u3_av_frame_free(AVFrame** frame)
	{
		::av_frame_free(frame);
	}

	AVFrame* u3_av_frame_alloc(void)
	{
		return ::av_frame_alloc();
	}

	int u3_av_frame_get_buffer(AVFrame* frame, int align)
	{
		return ::av_frame_get_buffer(frame, align);
	}

	void u3_av_frame_unref(AVFrame* frame)
	{
		::av_frame_unref(frame);
	}

	int u3_av_frame_make_writable(AVFrame* frame)
	{
		return ::av_frame_make_writable(frame);
	}

	int u3_avio_open(AVIOContext** s, const char* url, int flags)
	{
		return ::avio_open(s, url, flags);
	}

	int u3_avio_closep(AVIOContext** s)
	{
		return ::avio_closep(s);
	}

	int u3_av_find_best_stream(AVFormatContext* ic,
		enum AVMediaType type,
		int wanted_stream_nb,
		int related_stream,
		const struct AVCodec** decoder_ret,
		int flags)
	{
		return ::av_find_best_stream(ic, type, wanted_stream_nb, related_stream, decoder_ret, flags);
	}

	int u3_av_channel_layout_copy(AVChannelLayout* dst, const AVChannelLayout* src)
	{
		return ::av_channel_layout_copy(dst, src);
	}

	const char* u3_av_get_pix_fmt_name(enum AVPixelFormat pix_fmt)
	{
		return ::av_get_pix_fmt_name(pix_fmt);
	}

	char* u3_av_strdup(const char* s) av_malloc_attrib
	{
		return ::av_strdup(s);
	}

	int64_t u3_av_rescale_q(int64_t a, AVRational bq, AVRational cq) av_const
	{
		return ::av_rescale_q(a, bq, cq);
	}

	int64_t u3_av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const
	{
		return ::av_rescale_rnd(a, b, c, rnd);
	}

	void u3_av_packet_rescale_ts(AVPacket* pkt, AVRational tb_src, AVRational tb_dst)
	{
		::av_packet_rescale_ts(pkt, tb_src, tb_dst);
	}

	AVRational u3_av_div_q(AVRational b, AVRational c) av_const
	{
		return ::av_div_q(b, c);
	}

	AVDictionaryEntry* u3_av_dict_get(const AVDictionary* m, const char* key,
		const AVDictionaryEntry* prev, int flags)
	{
		return ::av_dict_get(m, key, prev, flags);
	}

	int u3_av_dict_set(AVDictionary** pm, const char* key, const char* value, int flags)
	{
		return ::av_dict_set(pm, key, value, flags);
	}

	void u3_av_dict_free(AVDictionary** m)
	{
		::av_dict_free(m);
	}

	struct SwsContext* u3_sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
		int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter* srcFilter,
		SwsFilter* dstFilter, const double* param)
	{
		return ::sws_getContext(srcW, srcH, srcFormat,
			dstW, dstH, dstFormat,
			flags, srcFilter,
			dstFilter, param);
	}

	void u3_sws_freeContext(struct SwsContext* swsContext)
	{
		::sws_freeContext(swsContext);
	}

	int u3_sws_scale(struct SwsContext* c, const uint8_t* const srcSlice[],
		const int srcStride[], int srcSliceY, int srcSliceH,
		uint8_t* const dst[], const int dstStride[])
	{
		return ::sws_scale(c, srcSlice, srcStride, srcSliceY, srcSliceH, dst, dstStride);
	}

	int u3_swr_alloc_set_opts2(struct SwrContext** ps,
		const AVChannelLayout* out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
		const AVChannelLayout* in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		int log_offset, void* log_ctx)
	{
		return ::swr_alloc_set_opts2(ps,
			out_ch_layout, out_sample_fmt, out_sample_rate,
			in_ch_layout, in_sample_fmt, in_sample_rate,
			log_offset, log_ctx);
	}

	int u3_swr_init(struct SwrContext* s)
	{
		return ::swr_init(s);
	}

	int64_t u3_swr_get_delay(struct SwrContext* s, int64_t base)
	{
		return ::swr_get_delay(s, base);
	}

	int u3_swr_convert(struct SwrContext* s, uint8_t* const* out, int out_count,
		const uint8_t* const* in, int in_count)
	{
		return ::swr_convert(s, out, out_count, in, in_count);
	}

	void u3_swr_free(struct SwrContext** s)
	{
		::swr_free(s);
	}

	AVAudioFifo* u3_av_audio_fifo_alloc(enum AVSampleFormat sample_fmt, int channels, int nb_samples)
	{
		return ::av_audio_fifo_alloc(sample_fmt, channels, nb_samples);
	}

	int u3_av_audio_fifo_write(AVAudioFifo* af, void* const* data, int nb_samples)
	{
		return ::av_audio_fifo_write(af, data, nb_samples);
	}

	int u3_av_audio_fifo_size(AVAudioFifo* af)
	{
		return ::av_audio_fifo_size(af);
	}

	int u3_av_audio_fifo_read(AVAudioFifo* af, void* const* data, int nb_samples)
	{
		return ::av_audio_fifo_read(af, data, nb_samples);
	}

	void u3_av_audio_fifo_free(AVAudioFifo* af)
	{
		::av_audio_fifo_free(af);
	}

	AVFilterGraph* u3_avfilter_graph_alloc(void)
	{
		return ::avfilter_graph_alloc();
	}

	const AVFilter* u3_avfilter_get_by_name(const char* name)
	{
		return ::avfilter_get_by_name(name);
	}

	int u3_avfilter_graph_create_filter(AVFilterContext** filt_ctx, const AVFilter* filt,
		const char* name, const char* args, void* opaque,
		AVFilterGraph* graph_ctx)
	{
		return ::avfilter_graph_create_filter(filt_ctx, filt, name, args, opaque, graph_ctx);
	}

	AVFilterInOut* u3_avfilter_inout_alloc(void)
	{
		return ::avfilter_inout_alloc();
	}

	int u3_avfilter_graph_parse_ptr(AVFilterGraph* graph, const char* filters,
		AVFilterInOut** inputs, AVFilterInOut** outputs,
		void* log_ctx)
	{
		return ::avfilter_graph_parse_ptr(graph, filters, inputs, outputs, log_ctx);
	}

	int u3_avfilter_graph_config(AVFilterGraph* graphctx, void* log_ctx)
	{
		return ::avfilter_graph_config(graphctx, log_ctx);
	}

	void u3_avfilter_inout_free(AVFilterInOut** inout)
	{
		::avfilter_inout_free(inout);
	}

	int u3_av_buffersink_get_format(const AVFilterContext* ctx)
	{
		return ::av_buffersink_get_format(ctx);
	}

	int u3_av_buffersrc_add_frame_flags(AVFilterContext* buffer_src,
		AVFrame* frame, int flags)
	{
		return ::av_buffersrc_add_frame_flags(buffer_src, frame, flags);
	}

	int u3_av_buffersink_get_frame(AVFilterContext* ctx, AVFrame* frame)
	{
		return ::av_buffersink_get_frame(ctx, frame);
	}


}

