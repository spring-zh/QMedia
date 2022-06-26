//
//  SoftwareDecoder.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

extern "C" {
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
}

#include "SoftwareDecoder.h"

extern RawVideoFormat AVPixelFormatToVideoFormat(AVPixelFormat format);
extern RawAudioFormat AVSampleFormatToAudioFormat(AVSampleFormat format);
extern AVPixelFormat VideoFormatToAVPixelFormat(RawVideoFormat videoFormat);
extern AVSampleFormat AudioFormatToAVSampleFormat(RawAudioFormat audioFormat);

static const AVCodec* VideoCodecToFFMap[(int)VideoCodecType::MAX_NUM] = {0};
static const AVCodec* AudioCodecToFFMap[(int)AudioCodecType ::MAX_NUM] = {0};

#pragma mark ---------- SoftwareEncodedPacketBuffer ----------

SoftwareEncodedPacketBuffer::SoftwareEncodedPacketBuffer(AVPacket* packet, bool is_bframe):
_avPacket(packet),
_is_bframe(is_bframe){

}
SoftwareEncodedPacketBuffer::~SoftwareEncodedPacketBuffer() {
    av_packet_free(&_avPacket);
}

int64_t SoftwareEncodedPacketBuffer::pts() const {
    return _avPacket->pts;
}

int64_t SoftwareEncodedPacketBuffer::dts() const {
    return _avPacket->dts;
}

const uint8_t *SoftwareEncodedPacketBuffer::data() const {
    return _avPacket->data;
}

size_t SoftwareEncodedPacketBuffer::size() const {
    return _avPacket->size;
}

bool SoftwareEncodedPacketBuffer::is_keyframe() const {
    return _avPacket->flags & AV_PKT_FLAG_KEY;
}

bool SoftwareEncodedPacketBuffer::bframe() const {
    return _is_bframe;
}
int SoftwareEncodedPacketBuffer::stream_id() const {
    return _avPacket->stream_index;
}

#pragma mark ---------- SoftwareDecodedFrameBuffer ----------

SoftwareDecodedFrameBuffer::SoftwareDecodedFrameBuffer(MediaType mediaType,AVFrame* frame):
_mediaType(mediaType), _avFrame(frame),
_sample_bits(16){
    if (mediaType == MediaType::Video) {
        pixel_format_ = AVPixelFormatToVideoFormat((AVPixelFormat)_avFrame->format);
    } else if (mediaType == MediaType::Audio) {
        sample_format_ = AVSampleFormatToAudioFormat((AVSampleFormat)_avFrame->format);
    }
}
SoftwareDecodedFrameBuffer::~SoftwareDecodedFrameBuffer() {
    av_frame_free(&_avFrame);
}

int SoftwareDecodedFrameBuffer::Channels() const {
    return _avFrame->channels;
}

int SoftwareDecodedFrameBuffer::SampleRate() const {
    return _avFrame->sample_rate;
}

int SoftwareDecodedFrameBuffer::SampleBits() const {
    return _sample_bits;
}

int SoftwareDecodedFrameBuffer::Samples() const {
    return _avFrame->nb_samples;
}

const uint8_t *SoftwareDecodedFrameBuffer::Data() const {
    return _avFrame->data[0];
}

uint32_t SoftwareDecodedFrameBuffer::Size() const {
    int size;
    av_samples_get_buffer_size(&size, _avFrame->channels, _avFrame->nb_samples, (AVSampleFormat)_avFrame->format, 1);
    return size;
}

ChannelLayout SoftwareDecodedFrameBuffer::ChannelLayout() const {
    return _avFrame->channels==1? kMono : kStereo;
}

int SoftwareDecodedFrameBuffer::width() const {
    return _avFrame->width;
}

int SoftwareDecodedFrameBuffer::height() const {
    return _avFrame->height;
}

const uint8_t *SoftwareDecodedFrameBuffer::DataY() const {
    return _avFrame->data[0];
}

const uint8_t *SoftwareDecodedFrameBuffer::DataU() const {
    return _avFrame->data[1];
}

const uint8_t *SoftwareDecodedFrameBuffer::DataV() const {
    return _avFrame->data[2];
}

int SoftwareDecodedFrameBuffer::StrideY() const {
    return _avFrame->linesize[0];
}

int SoftwareDecodedFrameBuffer::StrideU() const {
    return _avFrame->linesize[1];
}

int SoftwareDecodedFrameBuffer::StrideV() const {
    return _avFrame->linesize[2];
}

int SoftwareDecodedFrameBuffer::getFlags() const {
    return _avFrame->flags;
}

#pragma mark ---------- SoftwareDecoder ----------
SoftwareDecoder::SoftwareDecoder():
_avcodec(NULL),_avcontext(NULL),
_callback(NULL),
_outputRequest(false),
_swrContext(NULL), _swsContext(NULL),
_isInit(false) {

}

int32_t SoftwareDecoder::InitDecode(const MediaDescribe& media_describe,
                   int32_t number_of_cores, const CodecSpecificInfo& codec_specific_info) {
    _mediaDescribe = media_describe;
    if (media_describe.mediatype_ == MediaType::Video) {
        _avcodec = VideoCodecToFFMap[(int)media_describe._videodesc.codectype];
    } else if (media_describe.mediatype_ == MediaType ::Audio) {
        _avcodec = AudioCodecToFFMap[(int)media_describe._audiodesc.codectype];
    }

    if (!_avcodec) return -1;

    _avcontext = avcodec_alloc_context3(_avcodec);
    if (!_avcontext) return -1;

    AVCodecParameters* codec_parameters = avcodec_parameters_alloc();
    if (media_describe.mediatype_ == MediaType::Video) {
        codec_parameters->codec_id = _avcodec->id;
        codec_parameters->codec_type = AVMEDIA_TYPE_VIDEO;
        codec_parameters->format = AV_PIX_FMT_YUV420P;
        codec_parameters->width = media_describe._videodesc.width;
        codec_parameters->height = media_describe._videodesc.height;
        codec_parameters->bit_rate = media_describe._videodesc.bitrate;
    } else if (media_describe.mediatype_ == MediaType::Audio) {
        codec_parameters->codec_id = _avcodec->id;
        codec_parameters->codec_type = AVMEDIA_TYPE_AUDIO;
        codec_parameters->format = AV_SAMPLE_FMT_S16;
        codec_parameters->sample_rate = media_describe._audiodesc.samplerate;
        codec_parameters->channels = media_describe._audiodesc.nchannel;
        codec_parameters->bits_per_coded_sample = media_describe._audiodesc.bitwidth;
        codec_parameters->bit_rate = media_describe._audiodesc.bitrate;
    }

    //set codec_specific_infos
    if (codec_specific_info.getExtraDatas().size() > 0) {
        std::vector<uint8_t > extra_temp;
        extra_temp.clear();
        for (auto& extra_data : codec_specific_info.getExtraDatas()) {
            extra_temp.insert(extra_temp.end(),extra_data.begin(),extra_data.end());
        }
        codec_parameters->extradata = static_cast<uint8_t *>(av_malloc(extra_temp.size()));
        std::memcpy(codec_parameters->extradata, extra_temp.data(), extra_temp.size());
        codec_parameters->extradata_size = extra_temp.size();
    }
    avcodec_parameters_to_context(_avcontext, codec_parameters);
//    _avcontext->refcounted_frames = 1;

    AVDictionary* opt = NULL;
    if (media_describe.mediatype_ == MediaType ::Video) {
        av_dict_set(&opt, "threads", "auto", 0);
    } else
        _avcontext->request_sample_fmt = AV_SAMPLE_FMT_S16;
    int ret = avcodec_open2(_avcontext, _avcodec, &opt);
    av_dict_free(&opt);

    if (ret < 0)
    {
        av_free(_avcontext);

        _avcontext = NULL;
        _avcodec = NULL;
        return false;
    }
    _isInit = true;
    return 0;
}

int32_t SoftwareDecoder::Decode(const EncodedPacket &input_packet) {
    if (!_isInit)
        return DEC_UNKNOW;

    AVPacket avPacket = { 0 };
    av_init_packet(&avPacket);
    std::shared_ptr<EncodedPacketBuffer> encoded_buffer = input_packet.getEncodedBuffer();
    if (encoded_buffer) {
        avPacket.data = const_cast<uint8_t *>(encoded_buffer->data());
        avPacket.size = encoded_buffer->size();
        avPacket.pts = input_packet.ntp_time_ms();
        avPacket.dts = encoded_buffer->dts();
        if(input_packet.flags() & EncodedPacket::FLAG_SYNC) avPacket.flags &= AV_PKT_FLAG_KEY;
        if(input_packet.flags() & EncodedPacket::FLAG_DISCARD) avPacket.flags &= AV_PKT_FLAG_DISCARD;
    } else {
        avPacket.pts = input_packet.ntp_time_ms();
    }

    int ret = avcodec_send_packet(_avcontext, &avPacket);
    if (ret == AVERROR(EAGAIN)) {
        __DoReceiveFrame();
        return DEC_EAGAIN;
    }

    return __DoReceiveFrame();
}

int32_t SoftwareDecoder::__DoReceiveFrame() {
    int32_t retCode = DEC_UNKNOW;
    do {
        AVFrame *decFrame = av_frame_alloc();
        int ret = avcodec_receive_frame(_avcontext, decFrame);
        if (ret >= 0) {
//            __retrieveAVFrame(&decFrame); //transform frame format
            std::shared_ptr<SoftwareDecodedFrameBuffer> decodedFrameBuffer = std::shared_ptr<SoftwareDecodedFrameBuffer>(new SoftwareDecodedFrameBuffer(_mediaDescribe.mediatype_, decFrame));
            int64_t frame_pts = decFrame->pts != AV_NOPTS_VALUE ? decFrame->pts : decFrame->pkt_pts;
            DecodedFrame decodedFrame(decodedFrameBuffer, decFrame->pts, frame_pts);
            _callback->OnDecoded(decodedFrame);
            retCode = DEC_OK;
        } else {
            av_frame_free(&decFrame);
            if (ret == AVERROR_EOF) {
                //decoder stream end
                retCode = DEC_END;
                _callback->OnDecodedEnd();
            } else if (ret == AVERROR(EAGAIN)) {
                retCode = DEC_OK;
            }
            break;
        }
    }while (1);

    return retCode;
}

int32_t SoftwareDecoder::Flush(bool wait_completed) {
    if (!_isInit)
        return -2;
    if (wait_completed) {
        EncodedPacket input_packet(nullptr, -1);
        Decode(input_packet);
    }
    avcodec_flush_buffers(_avcontext);
    return 0;
}

int32_t SoftwareDecoder::Release() {
    if (_isInit) {
        if (_avcontext) av_free(_avcontext);
        _avcontext = NULL;
        _avcodec = NULL;
    }
    if (_swrContext) {
        swr_free(&_swrContext);
        _swrContext = NULL;
    }
    if (_swsContext) {
        sws_freeContext(_swsContext);
        _swsContext = NULL;
    }
    _outputRequest = false;
    _isInit = false;
    return 0;
}

void SoftwareDecoder::setOutputRequest(const MediaDescribe& mediaDescribe)  {
    _outputRequest = true;
    _outputDescribe = mediaDescribe;
}

bool SoftwareDecoder::__retrieveAVFrame(struct AVFrame ** pFrame) {
    bool bRet = true;
    AVFrame * input_frame = *pFrame;
    if (_outputRequest) {
        if (_outputDescribe.mediatype_ == MediaType::Video) {
            VideoDescribe& videoOutputDescribe = _outputDescribe._videodesc;
            if (videoOutputDescribe.width != (*pFrame)->width || videoOutputDescribe.height != (*pFrame)->height || videoOutputDescribe.format != AVPixelFormatToVideoFormat((AVPixelFormat)(*pFrame)->format)) {
                //TODO: need sws video frame
                VideoDescribe& videoOutputDescribe = _outputDescribe._videodesc;
                AVPixelFormat out_pixel_format = VideoFormatToAVPixelFormat(_outputDescribe._videodesc.format);
                if (out_pixel_format != input_frame->format || videoOutputDescribe.width != input_frame->width || videoOutputDescribe.height != input_frame->height) {
                    int ret;
                    if (!_swsContext) {
                        _swsContext = sws_getContext(input_frame->width, input_frame->height, (AVPixelFormat)input_frame->format,
                                videoOutputDescribe.width, videoOutputDescribe.height, out_pixel_format, SWS_FAST_BILINEAR, NULL, NULL, NULL);
                    }
                    AVFrame *outframe = av_frame_alloc();
                    av_frame_copy_props(outframe, input_frame); //need to copy pts, dts, flags
                    outframe->width = videoOutputDescribe.width;
                    outframe->height = videoOutputDescribe.height;
                    outframe->format = out_pixel_format;
                    av_frame_get_buffer(outframe,0);
                    ret = sws_scale(_swsContext, input_frame->data, input_frame->linesize, 0, input_frame->height, outframe->data, outframe->linesize);
                    if (ret >= 0) {
                        av_frame_unref(input_frame);
                        (*pFrame) = outframe;
                    } else {
                        av_frame_unref(outframe);
                        bRet = false;
                    }
                }
            }
        } else if (_outputDescribe.mediatype_ == MediaType::Audio) {
            AudioDescribe& audioOutputDescribe = _outputDescribe._audiodesc;
            uint64_t out_ch_layout = audioOutputDescribe.nchannel == 2 ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO;
            AVSampleFormat out_sample_format = AudioFormatToAVSampleFormat(audioOutputDescribe.format);
            int out_samplerate = audioOutputDescribe.samplerate;
            if (audioOutputDescribe.nchannel != input_frame->channels || out_samplerate != input_frame->sample_rate || out_sample_format != (AVSampleFormat)input_frame->format) {
                //TODO: need swr audio frame
                int ret;
                if (!_swrContext) {
                    /* create resampler context */
                    _swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_format,
                                                     out_samplerate, input_frame->channel_layout, (AVSampleFormat)input_frame->format, input_frame->sample_rate, 0, NULL);
//                    /* initialize the resampling context */
                    if ((ret = swr_init(_swrContext)) < 0) {

                    }
                }

                int dst_nb_samples = swr_get_out_samples(_swrContext, input_frame->nb_samples);

                AVFrame *outframe = av_frame_alloc();
                av_frame_copy_props(outframe, input_frame); //need to copy pts, dts, flags
                outframe->channel_layout = out_ch_layout;
                outframe->channels = audioOutputDescribe.nchannel;
                outframe->format = out_sample_format;
                outframe->sample_rate = out_samplerate;
                outframe->nb_samples = dst_nb_samples;
                av_frame_get_buffer(outframe,0);
                ret = swr_convert(_swrContext, outframe->data, outframe->nb_samples,
                                  (const uint8_t **)input_frame->data, input_frame->nb_samples);
//                ret = swr_convert_frame(_swrContext, outframe, input_frame);
                if (ret >= 0) {
                    outframe->nb_samples = ret;
                    outframe->linesize[0] = ret;
                    av_frame_unref(input_frame);
                    (*pFrame) = outframe;
                } else {
                    av_frame_unref(outframe);
                    bRet = false;
                }
            }
        }
    }
    return bRet;
}


#pragma mark ---------- SoftwareDecoderFactory ----------

SoftwareDecoderFactory* SoftwareDecoderFactory::instance() {
    static SoftwareDecoderFactory decoderFactory;
    return &decoderFactory;
}

SoftwareDecoderFactory::SoftwareDecoderFactory() {

#define CHECK_AND_SET_VIDEOCODEC(videoCodec, ff_codec_id) \
    { \
        AVCodec * codec = avcodec_find_decoder(ff_codec_id); \
        if (codec) { \
            VideoCodecToFFMap[(int)videoCodec] = codec; \
            video_codecs_list_.push_back(videoCodec); \
        } \
    }

#define CHECK_AND_SET_AUDIOCODEC(audioCodec, ff_codec_id) \
    { \
        AVCodec * codec = avcodec_find_decoder(ff_codec_id); \
        if (codec) { \
            AudioCodecToFFMap[(int)audioCodec] = codec; \
            audio_codecs_list_.push_back(audioCodec); \
        } \
    }

    std::memset(VideoCodecToFFMap, 0 , sizeof(VideoCodecToFFMap));
    std::memset(AudioCodecToFFMap, 0 , sizeof(AudioCodecToFFMap));

    CHECK_AND_SET_VIDEOCODEC(VideoCodecType::kVP8, AV_CODEC_ID_VP8)
    CHECK_AND_SET_VIDEOCODEC(VideoCodecType::kVP9, AV_CODEC_ID_VP9)
    CHECK_AND_SET_VIDEOCODEC(VideoCodecType::kMPEG4, AV_CODEC_ID_MPEG4)
    CHECK_AND_SET_VIDEOCODEC(VideoCodecType::kH264, AV_CODEC_ID_H264)
    CHECK_AND_SET_VIDEOCODEC(VideoCodecType::kH265, AV_CODEC_ID_HEVC)

    CHECK_AND_SET_AUDIOCODEC(AudioCodecType::kAAC, AV_CODEC_ID_AAC);
    CHECK_AND_SET_AUDIOCODEC(AudioCodecType::kAAC_LATM, AV_CODEC_ID_AAC_LATM);
    CHECK_AND_SET_AUDIOCODEC(AudioCodecType::kMP3, AV_CODEC_ID_MP3);
    CHECK_AND_SET_AUDIOCODEC(AudioCodecType::kPcmA, AV_CODEC_ID_PCM_ALAW);
    CHECK_AND_SET_AUDIOCODEC(AudioCodecType::kPcmU, AV_CODEC_ID_PCM_MULAW);

#if 0
    void *opaque = NULL;
    const AVCodec* codec;
    while ((codec = av_codec_iterate(&opaque)) != NULL) {
        if(av_codec_is_decoder(codec)){
            if (codec->type == AVMEDIA_TYPE_VIDEO) {
                VideoCodecType videoCodecType = FFCodecTypeToVideoCodecType(codec->id);
                if (videoCodecType != VideoCodecType::kUnknown) {
                    video_codecs_list_.push_back(videoCodecType);
                    VideoCodecToFFMap[(int)videoCodecType] = codec;
                }

            } else if (codec->type == AVMEDIA_TYPE_AUDIO) {
                AudioCodecType audioCodecType = FFCodecTypeToAudioCodecType(codec->id);
                if (audioCodecType != AudioCodecType ::kUnknown) {
                    audio_codecs_list_.push_back(audioCodecType);
                    AudioCodecToFFMap[(int)audioCodecType] = codec;
                }
            }
        }
    }
#endif
}
SoftwareDecoderFactory::~SoftwareDecoderFactory() {

}

Decoder *SoftwareDecoderFactory::CreateDecoder(const MediaDescribe& mediaDescribe) {
    Decoder* decoderRet = nullptr;
    bool hasDecoder = false;
    if (mediaDescribe.mediatype_ == MediaType::Video) {
        hasDecoder = std::find(video_codecs_list_.begin(),video_codecs_list_.end(), mediaDescribe._videodesc.codectype) != video_codecs_list_.end();
    }
    if (mediaDescribe.mediatype_ == MediaType::Audio) {
        hasDecoder = std::find(audio_codecs_list_.begin(),audio_codecs_list_.end(), mediaDescribe._audiodesc.codectype) != audio_codecs_list_.end();
    }
    if (hasDecoder) decoderRet = new SoftwareDecoder();
    return decoderRet;
}

void SoftwareDecoderFactory::DestroyDecoder(Decoder *decoder) {
    decoder->Release();
    delete decoder;
}
