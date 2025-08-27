#pragma once
#include "../src/ffmpegInterface.h"
#include <gmock/gmock.h>

class FFmpegMock : public IFFmpeg
{
public:
	MOCK_METHOD(const AVCodec*, avcodec_find_encoder_by_name, (const char* name), (override));
	MOCK_METHOD(const AVCodec*, avcodec_find_decoder, (enum AVCodecID id), (override));
	MOCK_METHOD(AVCodecContext*, avcodec_alloc_context3, (const AVCodec* codec), (override));
	MOCK_METHOD(int, avcodec_parameters_to_context, (AVCodecContext* codec, const struct AVCodecParameters* par), (override));
	MOCK_METHOD(int, avcodec_open2, (AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options), (override));
	MOCK_METHOD(int, av_read_frame, (AVFormatContext* s, AVPacket* pkt), (override));
	MOCK_METHOD(int, avcodec_get_supported_config, (const AVCodecContext* avctx,
		const AVCodec* codec, enum AVCodecConfig config,
		unsigned flags, const void** out_configs,
		int* out_num_configs), (override));
	MOCK_METHOD(void, avcodec_free_context, (AVCodecContext** avctx), (override));
	MOCK_METHOD(int, avcodec_parameters_from_context, (struct AVCodecParameters* par,
		const AVCodecContext* codec), (override));
	MOCK_METHOD(int, avcodec_send_packet, (AVCodecContext* avctx, const AVPacket* avpkt), (override));
	MOCK_METHOD(int, avcodec_receive_frame, (AVCodecContext* avctx, AVFrame* frame), (override));
	MOCK_METHOD(int, avcodec_send_frame, (AVCodecContext* avctx, const AVFrame* frame), (override));
	MOCK_METHOD(AVPacket*, av_packet_alloc, (), (override));
	MOCK_METHOD(int, avcodec_receive_packet, (AVCodecContext* avctx, AVPacket* avpkt), (override));
	MOCK_METHOD(void, av_packet_unref, (AVPacket* pkt), (override));
	MOCK_METHOD(void, av_packet_free, (AVPacket** pkt), (override));
	MOCK_METHOD(int, av_interleaved_write_frame, (AVFormatContext* s, AVPacket* pkt), (override));
	MOCK_METHOD(void, avcodec_flush_buffers, (AVCodecContext* avctx), (override));
	MOCK_METHOD(void, avformat_close_input, (AVFormatContext** s), (override));
	MOCK_METHOD(void, avformat_free_context, (AVFormatContext* s), (override));
	MOCK_METHOD(AVRational, av_guess_frame_rate, (AVFormatContext* ctx, AVStream* stream,
		struct AVFrame* frame), (override));
	MOCK_METHOD(int, avformat_open_input, (AVFormatContext** ps, const char* url, 
		const AVInputFormat* fmt, AVDictionary** options), (override));
	MOCK_METHOD(int, avformat_find_stream_info, (AVFormatContext* ic, AVDictionary** options), (override));
	MOCK_METHOD(int, avformat_alloc_output_context2, (AVFormatContext** ctx, const AVOutputFormat* oformat,
		const char* format_name, const char* filename), (override));
	MOCK_METHOD(AVStream*, avformat_new_stream, (AVFormatContext* s, const struct AVCodec* c), (override));
	MOCK_METHOD(int, avformat_seek_file, (AVFormatContext* s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags), (override));
	MOCK_METHOD(int, avformat_write_header, (AVFormatContext* s, AVDictionary** options), (override));
	MOCK_METHOD(int, av_write_trailer, (AVFormatContext* s), (override));
	MOCK_METHOD(void, av_frame_free, (AVFrame** frame), (override));
	MOCK_METHOD(AVFrame*, av_frame_alloc, (), (override));
	MOCK_METHOD(int, av_frame_get_buffer, (AVFrame* frame, int align), (override));
	MOCK_METHOD(void, av_frame_unref, (AVFrame* frame), (override));
	MOCK_METHOD(int, av_frame_make_writable, (AVFrame* frame), (override));
	MOCK_METHOD(int, avio_open, (AVIOContext** s, const char* url, int flags), (override));
	MOCK_METHOD(int, avio_closep, (AVIOContext** s), (override));
	MOCK_METHOD(int, av_find_best_stream, (AVFormatContext* ic,
		enum AVMediaType type,
		int wanted_stream_nb,
		int related_stream,
		const struct AVCodec** decoder_ret,
		int flags), (override));
	MOCK_METHOD(int, av_channel_layout_copy, (AVChannelLayout* dst, const AVChannelLayout* src), (override));
	MOCK_METHOD(void, av_channel_layout_default, (AVChannelLayout* ch_layout, int nb_channels), (override));
	MOCK_METHOD(const char*, av_get_pix_fmt_name, (enum AVPixelFormat pix_fmt), (override));
	MOCK_METHOD(int, av_opt_set, (void* obj, const char* name, const char* val, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_int, (void* obj, const char* name, int64_t val, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_double, (void* obj, const char* name, double val, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_q, (void* obj, const char* name, AVRational val, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_bin, (void* obj, const char* name, const uint8_t* val, int size, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_image_size, (void* obj, const char* name, int w, int h, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_pixel_fmt, (void* obj, const char* name, enum AVPixelFormat fmt, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_sample_fmt, (void* obj, const char* name, enum AVSampleFormat fmt, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_video_rate, (void* obj, const char* name, AVRational val, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_chlayout, (void* obj, const char* name, const AVChannelLayout* layout, int search_flags), (override));
	MOCK_METHOD(int, av_opt_set_dict_val, (void* obj, const char* name, const AVDictionary* val, int search_flags), (override));
	MOCK_METHOD(char*, av_strdup, (const char* s), (override));
	MOCK_METHOD(int64_t, av_rescale_q, (int64_t a, AVRational bq, AVRational cq), (override));
	MOCK_METHOD(int64_t, av_rescale_rnd, (int64_t a, int64_t b, int64_t c, enum AVRounding rnd), (override));
	MOCK_METHOD(void, av_packet_rescale_ts, (AVPacket* pkt, AVRational tb_src, AVRational tb_dst), (override));
	MOCK_METHOD(AVRational, av_div_q, (AVRational b, AVRational c), (override));
	MOCK_METHOD(AVDictionaryEntry*, av_dict_get, (const AVDictionary* m, const char* key,
		const AVDictionaryEntry* prev, int flags), (override));
	MOCK_METHOD(int, av_dict_set, (AVDictionary** pm, const char* key, const char* value, int flags), (override));
	MOCK_METHOD(void, av_dict_free, (AVDictionary** m), (override));
	MOCK_METHOD(struct SwsContext*, sws_getContext, (int srcW, int srcH, enum AVPixelFormat srcFormat,
		int dstW, int dstH, enum AVPixelFormat dstFormat,
		int flags, SwsFilter* srcFilter,
		SwsFilter* dstFilter, const double* param), (override));
	MOCK_METHOD(void, sws_freeContext, (struct SwsContext* swsContext), (override));
	MOCK_METHOD(int, sws_scale, (struct SwsContext* c, const uint8_t* const srcSlice[],
		const int srcStride[], int srcSliceY, int srcSliceH,
		uint8_t* const dst[], const int dstStride[]), (override));
	MOCK_METHOD(int, swr_alloc_set_opts2, (struct SwrContext** ps,
		const AVChannelLayout* out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
		const AVChannelLayout* in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
		int log_offset, void* log_ctx), (override));
	MOCK_METHOD(int, swr_init, (struct SwrContext* s), (override));
	MOCK_METHOD(int64_t, swr_get_delay, (struct SwrContext* s, int64_t base), (override));
	MOCK_METHOD(int, swr_convert, (struct SwrContext* s, uint8_t* const* out, int out_count,
		const uint8_t* const* in, int in_count), (override));
	MOCK_METHOD(void, swr_free, (struct SwrContext** s), (override));
	MOCK_METHOD(AVAudioFifo*, av_audio_fifo_alloc, (enum AVSampleFormat sample_fmt, int channels, int nb_samples), (override));
	MOCK_METHOD(int, av_audio_fifo_write, (AVAudioFifo* af, void* const* data, int nb_samples), (override));
	MOCK_METHOD(int, av_audio_fifo_size, (AVAudioFifo* af), (override));
	MOCK_METHOD(int, av_audio_fifo_read, (AVAudioFifo* af, void* const* data, int nb_samples), (override));
	MOCK_METHOD(void, av_audio_fifo_free, (AVAudioFifo* af), (override));
	MOCK_METHOD(AVFilterGraph*, avfilter_graph_alloc, (), (override));
	MOCK_METHOD(const AVFilter*, avfilter_get_by_name, (const char* name), (override));
	MOCK_METHOD(int, avfilter_graph_create_filter, (AVFilterContext** filt_ctx, const AVFilter* filt,
		const char* name, const char* args, void* opaque,
		AVFilterGraph* graph_ctx), (override));
	MOCK_METHOD(AVFilterInOut*, avfilter_inout_alloc, (), (override));
	MOCK_METHOD(int, avfilter_graph_parse_ptr, (AVFilterGraph* graph, const char* filters,
		AVFilterInOut** inputs, AVFilterInOut** outputs,
		void* log_ctx), (override));
	MOCK_METHOD(int, avfilter_graph_config, (AVFilterGraph* graphctx, void* log_ctx), (override));
	MOCK_METHOD(void, avfilter_inout_free, (AVFilterInOut** inout), (override));
	MOCK_METHOD(int, av_buffersink_get_format, (const AVFilterContext* ctx), (override));
	MOCK_METHOD(int, av_buffersrc_add_frame_flags, (AVFilterContext* buffer_src,
		AVFrame* frame, int flags), (override));
	MOCK_METHOD(int, av_buffersink_get_frame, (AVFilterContext* ctx, AVFrame* frame), (override));
};
