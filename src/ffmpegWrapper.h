#pragma once
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
}

namespace FfmpegWrapper {

	const AVCodec* u3_avcodec_find_decoder(enum AVCodecID id);
	AVCodecContext* u3_avcodec_alloc_context3(const AVCodec* codec);
	int u3_avcodec_parameters_to_context(AVCodecContext* codec, const struct AVCodecParameters* par);
	int u3_avcodec_open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options);
	int u3_av_read_frame(AVFormatContext* s, AVPacket* pkt);
	int u3_avcodec_get_supported_config(const AVCodecContext* avctx,
		const AVCodec* codec, enum AVCodecConfig config,
		unsigned flags, const void** out_configs,
		int* out_num_configs);
	void u3_avcodec_free_context(AVCodecContext** avctx);
	int u3_avcodec_parameters_from_context(struct AVCodecParameters* par,
		const AVCodecContext* codec);
	int u3_avcodec_send_packet(AVCodecContext* avctx, const AVPacket* avpkt);
	int u3_avcodec_receive_frame(AVCodecContext* avctx, AVFrame* frame);
	int u3_avcodec_send_frame(AVCodecContext* avctx, const AVFrame* frame);
	AVPacket* u3_av_packet_alloc(void);
	int u3_avcodec_receive_packet(AVCodecContext* avctx, AVPacket* avpkt);
	void u3_av_packet_unref(AVPacket* pkt);
	void u3_av_packet_free(AVPacket** pkt);
	int u3_av_interleaved_write_frame(AVFormatContext* s, AVPacket* pkt);
	void u3_avcodec_flush_buffers(AVCodecContext* avctx);

	void u3_avformat_close_input(AVFormatContext** s);
	void u3_avformat_free_context(AVFormatContext* s);
	AVRational u3_av_guess_frame_rate(AVFormatContext* ctx, AVStream* stream,
		struct AVFrame* frame);
	int u3_avformat_open_input(AVFormatContext** ps, const char* url,
		const AVInputFormat* fmt, AVDictionary** options);
	int u3_avformat_find_stream_info(AVFormatContext* ic, AVDictionary** options);
	int u3_avformat_alloc_output_context2(AVFormatContext** ctx, const AVOutputFormat* oformat,
		const char* format_name, const char* filename);
	AVStream* u3_avformat_new_stream(AVFormatContext* s, const struct AVCodec* c);
	int u3_avformat_seek_file(AVFormatContext* s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags);
	int u3_avformat_write_header(AVFormatContext* s, AVDictionary** options);
	int u3_av_write_trailer(AVFormatContext* s);

	void u3_av_frame_free(AVFrame** frame);
	AVFrame* u3_av_frame_alloc(void);
	int u3_av_frame_get_buffer(AVFrame* frame, int align);
	void u3_av_frame_unref(AVFrame* frame);
	int u3_av_frame_make_writable(AVFrame* frame);

	int u3_avio_closep(AVIOContext** s);
	int u3_av_find_best_stream(AVFormatContext* ic,
		enum AVMediaType type,
		int wanted_stream_nb,
		int related_stream,
		const struct AVCodec** decoder_ret,
		int flags);
	int u3_av_channel_layout_copy(AVChannelLayout* dst, const AVChannelLayout* src);
	const char* u3_av_get_pix_fmt_name(enum AVPixelFormat pix_fmt);
	char* u3_av_strdup(const char* s) av_malloc_attrib;
	int64_t u3_av_rescale_q(int64_t a, AVRational bq, AVRational cq) av_const;
	int64_t u3_av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const;
	void u3_av_packet_rescale_ts(AVPacket* pkt, AVRational tb_src, AVRational tb_dst);
	AVRational u3_av_div_q(AVRational b, AVRational c) av_const;
	static av_always_inline AVRational u3_av_inv_q(AVRational q)
	{
		AVRational r = { q.den, q.num };
		return r;
	}
	int u3_av_dict_set(AVDictionary** pm, const char* key, const char* value, int flags);
	void u3_av_dict_free(AVDictionary** m);

	struct SwsContext* u3_sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
		int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter* srcFilter,
		SwsFilter* dstFilter, const double* param);
	void u3_sws_freeContext(struct SwsContext* swsContext);
	int u3_sws_scale(struct SwsContext* c, const uint8_t* const srcSlice[],
		const int srcStride[], int srcSliceY, int srcSliceH,
		uint8_t* const dst[], const int dstStride[]);

	int u3_swr_alloc_set_opts2(struct SwrContext** ps,
		const AVChannelLayout* out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
		const AVChannelLayout* in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		int log_offset, void* log_ctx);
	int u3_swr_init(struct SwrContext* s);
	int64_t u3_swr_get_delay(struct SwrContext* s, int64_t base);
	int u3_swr_convert(struct SwrContext* s, uint8_t* const* out, int out_count,
		const uint8_t* const* in, int in_count);
	void u3_swr_free(struct SwrContext** s);

	AVAudioFifo* u3_av_audio_fifo_alloc(enum AVSampleFormat sample_fmt, int channels,
		int nb_samples);
	int u3_av_audio_fifo_write(AVAudioFifo* af, void* const* data, int nb_samples);
	int u3_av_audio_fifo_size(AVAudioFifo* af);
	int u3_av_audio_fifo_read(AVAudioFifo* af, void* const* data, int nb_samples);
	void u3_av_audio_fifo_free(AVAudioFifo* af);

	AVFilterGraph* u3_avfilter_graph_alloc(void);
	const AVFilter* u3_avfilter_get_by_name(const char* name);
	int u3_avfilter_graph_create_filter(AVFilterContext** filt_ctx, const AVFilter* filt,
		const char* name, const char* args, void* opaque,
		AVFilterGraph* graph_ctx);
	AVFilterInOut* u3_avfilter_inout_alloc(void);
	int u3_avfilter_graph_parse_ptr(AVFilterGraph* graph, const char* filters,
		AVFilterInOut** inputs, AVFilterInOut** outputs,
		void* log_ctx);
	int u3_avfilter_graph_config(AVFilterGraph* graphctx, void* log_ctx);
	void u3_avfilter_inout_free(AVFilterInOut** inout);
	int u3_av_buffersink_get_format(const AVFilterContext* ctx);
	int u3_av_buffersrc_add_frame_flags(AVFilterContext* buffer_src,
		AVFrame* frame, int flags);
	int u3_av_buffersink_get_frame(AVFilterContext* ctx, AVFrame* frame);

}
