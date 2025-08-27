#include "ffmpegWrapper.h"

class FFmpegReal : public IFFmpeg
{
public:

	const AVCodec* avcodec_find_decoder(enum AVCodecID id) override
	{
		return ::avcodec_find_decoder(id);
	}

	AVCodecContext* avcodec_alloc_context3(const AVCodec* codec)
	{
		return ::avcodec_alloc_context3(codec);
	}

	int avcodec_parameters_to_context(AVCodecContext* codec, const struct AVCodecParameters* par)
	{
		return ::avcodec_parameters_to_context(codec, par);
	}

	int avcodec_open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options)
	{
		return ::avcodec_open2(avctx, codec, options);
	}

	int av_read_frame(AVFormatContext* s, AVPacket* pkt)
	{
		return ::av_read_frame(s, pkt);
	}

	int avcodec_get_supported_config(const AVCodecContext* avctx,
		const AVCodec* codec, enum AVCodecConfig config,
		unsigned flags, const void** out_configs,
		int* out_num_configs)
	{
		return ::avcodec_get_supported_config(avctx, codec, config, flags, out_configs, out_num_configs);
	}

	void avcodec_free_context(AVCodecContext** avctx)
	{
		::avcodec_free_context(avctx);
	}

	int avcodec_parameters_from_context(struct AVCodecParameters* par,
		const AVCodecContext* codec)
	{
		return ::avcodec_parameters_from_context(par, codec);
	}

	int avcodec_send_packet(AVCodecContext* avctx, const AVPacket* avpkt)
	{
		return ::avcodec_send_packet(avctx, avpkt);
	}

	int avcodec_receive_frame(AVCodecContext* avctx, AVFrame* frame)
	{
		return ::avcodec_receive_frame(avctx, frame);
	}

	int avcodec_send_frame(AVCodecContext* avctx, const AVFrame* frame)
	{
		return ::avcodec_send_frame(avctx, frame);
	}

	AVPacket* av_packet_alloc(void)
	{
		return ::av_packet_alloc();
	}

	int avcodec_receive_packet(AVCodecContext* avctx, AVPacket* avpkt)
	{
		return ::avcodec_receive_packet(avctx, avpkt);
	}

	void av_packet_unref(AVPacket* pkt)
	{
		::av_packet_unref(pkt);
	}

	void av_packet_free(AVPacket** pkt)
	{
		::av_packet_free(pkt);
	}

	int av_interleaved_write_frame(AVFormatContext* s, AVPacket* pkt)
	{
		return ::av_interleaved_write_frame(s, pkt);
	}

	void avcodec_flush_buffers(AVCodecContext* avctx)
	{
		::avcodec_flush_buffers(avctx);
	}

	void avformat_close_input(AVFormatContext** s)
	{
		::avformat_close_input(s);
	}

	void avformat_free_context(AVFormatContext* s)
	{
		::avformat_free_context(s);
	}

	AVRational av_guess_frame_rate(AVFormatContext* ctx, AVStream* stream,
		struct AVFrame* frame)
	{
		return ::av_guess_frame_rate(ctx, stream, frame);
	}

	int avformat_open_input(AVFormatContext** ps, const char* url,
		const AVInputFormat* fmt, AVDictionary** options)
	{
		return ::avformat_open_input(ps, url, fmt, options);
	}

	int avformat_find_stream_info(AVFormatContext* ic, AVDictionary** options)
	{
		return ::avformat_find_stream_info(ic, options);
	}

	int avformat_alloc_output_context2(AVFormatContext** ctx, const AVOutputFormat* oformat,
		const char* format_name, const char* filename)
	{
		return ::avformat_alloc_output_context2(ctx, oformat, format_name, filename);
	}

	AVStream* avformat_new_stream(AVFormatContext* s, const struct AVCodec* c)
	{
		return ::avformat_new_stream(s, c);
	}

	int avformat_seek_file(AVFormatContext* s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags)
	{
		return ::avformat_seek_file(s, stream_index, min_ts, ts, max_ts, flags);
	}

	int avformat_write_header(AVFormatContext* s, AVDictionary** options)
	{
		return ::avformat_write_header(s, options);
	}

	int av_write_trailer(AVFormatContext* s)
	{
		return ::av_write_trailer(s);
	}

	void av_frame_free(AVFrame** frame)
	{
		::av_frame_free(frame);
	}

	AVFrame* av_frame_alloc(void)
	{
		return ::av_frame_alloc();
	}

	int av_frame_get_buffer(AVFrame* frame, int align)
	{
		return ::av_frame_get_buffer(frame, align);
	}

	void av_frame_unref(AVFrame* frame)
	{
		::av_frame_unref(frame);
	}

	int av_frame_make_writable(AVFrame* frame)
	{
		return ::av_frame_make_writable(frame);
	}

	int avio_open(AVIOContext** s, const char* url, int flags)
	{
		return ::avio_open(s, url, flags);
	}

	int avio_closep(AVIOContext** s)
	{
		return ::avio_closep(s);
	}

	int av_find_best_stream(AVFormatContext* ic,
		enum AVMediaType type,
		int wanted_stream_nb,
		int related_stream,
		const struct AVCodec** decoder_ret,
		int flags)
	{
		return ::av_find_best_stream(ic, type, wanted_stream_nb, related_stream, decoder_ret, flags);
	}

	int av_channel_layout_copy(AVChannelLayout* dst, const AVChannelLayout* src)
	{
		return ::av_channel_layout_copy(dst, src);
	}

	const char* av_get_pix_fmt_name(enum AVPixelFormat pix_fmt)
	{
		return ::av_get_pix_fmt_name(pix_fmt);
	}

	char* av_strdup(const char* s) av_malloc_attrib
	{
		return ::av_strdup(s);
	}

	int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq) av_const
	{
		return ::av_rescale_q(a, bq, cq);
	}

	int64_t av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const
	{
		return ::av_rescale_rnd(a, b, c, rnd);
	}

	void av_packet_rescale_ts(AVPacket* pkt, AVRational tb_src, AVRational tb_dst)
	{
		::av_packet_rescale_ts(pkt, tb_src, tb_dst);
	}

	AVRational av_div_q(AVRational b, AVRational c) av_const
	{
		return ::av_div_q(b, c);
	}

	AVDictionaryEntry* av_dict_get(const AVDictionary* m, const char* key,
		const AVDictionaryEntry* prev, int flags)
	{
		return ::av_dict_get(m, key, prev, flags);
	}

	int av_dict_set(AVDictionary** pm, const char* key, const char* value, int flags)
	{
		return ::av_dict_set(pm, key, value, flags);
	}

	void av_dict_free(AVDictionary** m)
	{
		::av_dict_free(m);
	}

	struct SwsContext* sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
		int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter* srcFilter,
		SwsFilter* dstFilter, const double* param)
	{
		return ::sws_getContext(srcW, srcH, srcFormat,
			dstW, dstH, dstFormat,
			flags, srcFilter,
			dstFilter, param);
	}

	void sws_freeContext(struct SwsContext* swsContext)
	{
		::sws_freeContext(swsContext);
	}

	int sws_scale(struct SwsContext* c, const uint8_t* const srcSlice[],
		const int srcStride[], int srcSliceY, int srcSliceH,
		uint8_t* const dst[], const int dstStride[])
	{
		return ::sws_scale(c, srcSlice, srcStride, srcSliceY, srcSliceH, dst, dstStride);
	}

	int swr_alloc_set_opts2(struct SwrContext** ps,
		const AVChannelLayout* out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
		const AVChannelLayout* in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		int log_offset, void* log_ctx)
	{
		return ::swr_alloc_set_opts2(ps,
			out_ch_layout, out_sample_fmt, out_sample_rate,
			in_ch_layout, in_sample_fmt, in_sample_rate,
			log_offset, log_ctx);
	}

	int swr_init(struct SwrContext* s)
	{
		return ::swr_init(s);
	}

	int64_t swr_get_delay(struct SwrContext* s, int64_t base)
	{
		return ::swr_get_delay(s, base);
	}

	int swr_convert(struct SwrContext* s, uint8_t* const* out, int out_count,
		const uint8_t* const* in, int in_count)
	{
		return ::swr_convert(s, out, out_count, in, in_count);
	}

	void swr_free(struct SwrContext** s)
	{
		::swr_free(s);
	}

	AVAudioFifo* av_audio_fifo_alloc(enum AVSampleFormat sample_fmt, int channels, int nb_samples)
	{
		return ::av_audio_fifo_alloc(sample_fmt, channels, nb_samples);
	}

	int av_audio_fifo_write(AVAudioFifo* af, void* const* data, int nb_samples)
	{
		return ::av_audio_fifo_write(af, data, nb_samples);
	}

	int av_audio_fifo_size(AVAudioFifo* af)
	{
		return ::av_audio_fifo_size(af);
	}

	int av_audio_fifo_read(AVAudioFifo* af, void* const* data, int nb_samples)
	{
		return ::av_audio_fifo_read(af, data, nb_samples);
	}

	void av_audio_fifo_free(AVAudioFifo* af)
	{
		::av_audio_fifo_free(af);
	}

	AVFilterGraph* avfilter_graph_alloc(void)
	{
		return ::avfilter_graph_alloc();
	}

	const AVFilter* avfilter_get_by_name(const char* name)
	{
		return ::avfilter_get_by_name(name);
	}

	int avfilter_graph_create_filter(AVFilterContext** filt_ctx, const AVFilter* filt,
		const char* name, const char* args, void* opaque,
		AVFilterGraph* graph_ctx)
	{
		return ::avfilter_graph_create_filter(filt_ctx, filt, name, args, opaque, graph_ctx);
	}

	AVFilterInOut* avfilter_inout_alloc(void)
	{
		return ::avfilter_inout_alloc();
	}

	int avfilter_graph_parse_ptr(AVFilterGraph* graph, const char* filters,
		AVFilterInOut** inputs, AVFilterInOut** outputs,
		void* log_ctx)
	{
		return ::avfilter_graph_parse_ptr(graph, filters, inputs, outputs, log_ctx);
	}

	int avfilter_graph_config(AVFilterGraph* graphctx, void* log_ctx)
	{
		return ::avfilter_graph_config(graphctx, log_ctx);
	}

	void avfilter_inout_free(AVFilterInOut** inout)
	{
		::avfilter_inout_free(inout);
	}

	int av_buffersink_get_format(const AVFilterContext* ctx)
	{
		return ::av_buffersink_get_format(ctx);
	}

	int av_buffersrc_add_frame_flags(AVFilterContext* buffer_src,
		AVFrame* frame, int flags)
	{
		return ::av_buffersrc_add_frame_flags(buffer_src, frame, flags);
	}

	int av_buffersink_get_frame(AVFilterContext* ctx, AVFrame* frame)
	{
		return ::av_buffersink_get_frame(ctx, frame);
	}

};
