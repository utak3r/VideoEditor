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

class IFFmpeg
{
public:
	virtual ~IFFmpeg() = default;

	virtual const AVCodec* avcodec_find_encoder_by_name(const char* name) = 0;
	virtual const AVCodec* avcodec_find_decoder(enum AVCodecID id) = 0;
	virtual AVCodecContext* avcodec_alloc_context3(const AVCodec* codec) = 0;
	virtual int avcodec_parameters_to_context(AVCodecContext* codec, const struct AVCodecParameters* par) = 0;
	virtual int avcodec_open2(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options) = 0;
	virtual int av_read_frame(AVFormatContext* s, AVPacket* pkt) = 0;
	virtual int avcodec_get_supported_config(const AVCodecContext* avctx,
		const AVCodec* codec, enum AVCodecConfig config,
		unsigned flags, const void** out_configs,
		int* out_num_configs) = 0;
	virtual void avcodec_free_context(AVCodecContext** avctx) = 0;
	virtual int avcodec_parameters_from_context(struct AVCodecParameters* par, const AVCodecContext* codec) = 0;
	virtual int avcodec_send_packet(AVCodecContext* avctx, const AVPacket* avpkt) = 0;
	virtual int avcodec_receive_frame(AVCodecContext* avctx, AVFrame* frame) = 0;
	virtual int avcodec_send_frame(AVCodecContext* avctx, const AVFrame* frame) = 0;
	virtual AVPacket* av_packet_alloc(void) = 0;
	virtual int avcodec_receive_packet(AVCodecContext* avctx, AVPacket* avpkt) = 0;
	virtual void av_packet_unref(AVPacket* pkt) = 0;
	virtual void av_packet_free(AVPacket** pkt) = 0;
	virtual int av_interleaved_write_frame(AVFormatContext* s, AVPacket* pkt) = 0;
	virtual void avcodec_flush_buffers(AVCodecContext* avctx) = 0;

	virtual void avformat_close_input(AVFormatContext** s) = 0;
	virtual void avformat_free_context(AVFormatContext* s) = 0;
	virtual AVRational av_guess_frame_rate(AVFormatContext* ctx, AVStream* stream, struct AVFrame* frame) = 0;
	virtual int avformat_open_input(AVFormatContext** ps, const char* url,
		const AVInputFormat* fmt, AVDictionary** options) = 0;
	virtual int avformat_find_stream_info(AVFormatContext* ic, AVDictionary** options) = 0;
	virtual int avformat_alloc_output_context2(AVFormatContext** ctx, const AVOutputFormat* oformat,
		const char* format_name, const char* filename) = 0;
	virtual AVStream* avformat_new_stream(AVFormatContext* s, const struct AVCodec* c) = 0;
	virtual int avformat_seek_file(AVFormatContext* s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags) = 0;
	virtual int avformat_write_header(AVFormatContext* s, AVDictionary** options) = 0;
	virtual int av_write_trailer(AVFormatContext* s) = 0;

	virtual void av_frame_free(AVFrame** frame) = 0;
	virtual AVFrame* av_frame_alloc(void) = 0;
	virtual int av_frame_get_buffer(AVFrame* frame, int align) = 0;
	virtual void av_frame_unref(AVFrame* frame) = 0;
	virtual int av_frame_make_writable(AVFrame* frame) = 0;

	virtual int avio_open(AVIOContext** s, const char* url, int flags) = 0;
	virtual int avio_closep(AVIOContext** s) = 0;
	virtual int av_find_best_stream(AVFormatContext* ic,
		enum AVMediaType type,
		int wanted_stream_nb,
		int related_stream,
		const struct AVCodec** decoder_ret,
		int flags) = 0;
	virtual int av_channel_layout_copy(AVChannelLayout* dst, const AVChannelLayout* src) = 0;
	virtual void av_channel_layout_default(AVChannelLayout* ch_layout, int nb_channels) = 0;
	virtual const char* av_get_pix_fmt_name(enum AVPixelFormat pix_fmt) = 0;

	virtual int av_opt_set(void* obj, const char* name, const char* val, int search_flags) = 0;
	virtual int av_opt_set_int(void* obj, const char* name, int64_t val, int search_flags) = 0;
	virtual int av_opt_set_double(void* obj, const char* name, double val, int search_flags) = 0;
	virtual int av_opt_set_q(void* obj, const char* name, AVRational val, int search_flags) = 0;
	virtual int av_opt_set_bin(void* obj, const char* name, const uint8_t* val, int size, int search_flags) = 0;
	virtual int av_opt_set_image_size(void* obj, const char* name, int w, int h, int search_flags) = 0;
	virtual int av_opt_set_pixel_fmt(void* obj, const char* name, enum AVPixelFormat fmt, int search_flags) = 0;
	virtual int av_opt_set_sample_fmt(void* obj, const char* name, enum AVSampleFormat fmt, int search_flags) = 0;
	virtual int av_opt_set_video_rate(void* obj, const char* name, AVRational val, int search_flags) = 0;
	virtual int av_opt_set_chlayout(void* obj, const char* name, const AVChannelLayout* layout, int search_flags) = 0;
	virtual int av_opt_set_dict_val(void* obj, const char* name, const AVDictionary* val, int search_flags) = 0;

	virtual char* av_strdup(const char* s) av_malloc_attrib = 0;
	virtual int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq) av_const = 0;
	virtual int64_t av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) av_const = 0;
	virtual void av_packet_rescale_ts(AVPacket* pkt, AVRational tb_src, AVRational tb_dst) = 0;
	virtual AVRational av_div_q(AVRational b, AVRational c) av_const = 0;
	static av_always_inline AVRational av_inv_q(AVRational q)
	{
		AVRational r = { q.den, q.num };
		return r;
	}
	static inline double av_q2d(AVRational a) {
		return a.num / (double)a.den;
	}
	static inline AVRational av_make_q(int num, int den)
	{
		AVRational r = { num, den };
		return r;
	}
	virtual AVDictionaryEntry* av_dict_get(const AVDictionary* m, const char* key,
		const AVDictionaryEntry* prev, int flags) = 0;
	virtual int av_dict_set(AVDictionary** pm, const char* key, const char* value, int flags) = 0;
	virtual void av_dict_free(AVDictionary** m) = 0;

	virtual struct SwsContext* sws_getContext(int srcW, int srcH, enum AVPixelFormat srcFormat,
		int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter* srcFilter,
		SwsFilter* dstFilter, const double* param) = 0;
	virtual void sws_freeContext(struct SwsContext* swsContext) = 0;
	virtual int sws_scale(struct SwsContext* c, const uint8_t* const srcSlice[],
		const int srcStride[], int srcSliceY, int srcSliceH,
		uint8_t* const dst[], const int dstStride[]) = 0;

	virtual int swr_alloc_set_opts2(struct SwrContext** ps,
		const AVChannelLayout* out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
		const AVChannelLayout* in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		int log_offset, void* log_ctx) = 0;
	virtual int swr_init(struct SwrContext* s) = 0;
	virtual int64_t swr_get_delay(struct SwrContext* s, int64_t base) = 0;
	virtual int swr_convert(struct SwrContext* s, uint8_t* const* out, int out_count,
		const uint8_t* const* in, int in_count) = 0;
	virtual void swr_free(struct SwrContext** s) = 0;

	virtual AVAudioFifo* av_audio_fifo_alloc(enum AVSampleFormat sample_fmt, int channels,
		int nb_samples) = 0;
	virtual int av_audio_fifo_write(AVAudioFifo* af, void* const* data, int nb_samples) = 0;
	virtual int av_audio_fifo_size(AVAudioFifo* af) = 0;
	virtual int av_audio_fifo_read(AVAudioFifo* af, void* const* data, int nb_samples) = 0;
	virtual void av_audio_fifo_free(AVAudioFifo* af) = 0;

	virtual AVFilterGraph* avfilter_graph_alloc(void) = 0;
	virtual const AVFilter* avfilter_get_by_name(const char* name) = 0;
	virtual int avfilter_graph_create_filter(AVFilterContext** filt_ctx, const AVFilter* filt,
		const char* name, const char* args, void* opaque,
		AVFilterGraph* graph_ctx) = 0;
	virtual AVFilterInOut* avfilter_inout_alloc(void) = 0;
	virtual int avfilter_graph_parse_ptr(AVFilterGraph* graph, const char* filters,
		AVFilterInOut** inputs, AVFilterInOut** outputs,
		void* log_ctx) = 0;
	virtual int avfilter_graph_config(AVFilterGraph* graphctx, void* log_ctx) = 0;
	virtual void avfilter_inout_free(AVFilterInOut** inout) = 0;
	virtual int av_buffersink_get_format(const AVFilterContext* ctx) = 0;
	virtual int av_buffersrc_add_frame_flags(AVFilterContext* buffer_src,
		AVFrame* frame, int flags) = 0;
	virtual int av_buffersink_get_frame(AVFilterContext* ctx, AVFrame* frame) = 0;
};
