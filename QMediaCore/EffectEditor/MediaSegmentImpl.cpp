//
//  MediaSegmentImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaSegmentImpl.h"
#include "Utils/Logger.h"

#include "medianode/AudioRenderNodeImpl.h"
#include "medianode/VideoGraphicNode.h"

extern "C" {
#include "libavformat/avformat.h"
}

namespace QMedia { namespace Api {

#define TIMD_DIFF 100

static int64_t RangeLength(MediaRange range) {
    return range.end - range.start;
}

static bool IsRangeContain(MediaRange range, int64_t time_ms) {
    return (range.start <= time_ms) && (range.end > time_ms);
}

static struct VideoCodec2AVCodecID {
    int media_codec;
    int av_codec;
} videoCodecMap[] = {
    {MediaStreamInfo::CODEC_VP8, AV_CODEC_ID_VP8},
    {MediaStreamInfo::CODEC_VP9, AV_CODEC_ID_VP9},
    {MediaStreamInfo::CODEC_MPEG4, AV_CODEC_ID_MPEG4},
    {MediaStreamInfo::CODEC_H264, AV_CODEC_ID_H264},
    {MediaStreamInfo::CODEC_H265, AV_CODEC_ID_HEVC},
    
    {MediaStreamInfo::CODEC_AAC, AV_CODEC_ID_AAC},
    {MediaStreamInfo::CODEC_AAC, AV_CODEC_ID_AAC_LATM},
    {MediaStreamInfo::CODEC_OPUS, AV_CODEC_ID_OPUS},
    {MediaStreamInfo::CODEC_MP3, AV_CODEC_ID_MP3},
    {MediaStreamInfo::CODEC_PCMA, AV_CODEC_ID_PCM_ALAW},
    {MediaStreamInfo::CODEC_PCMU, AV_CODEC_ID_PCM_MULAW},
    
    {MediaStreamInfo::MEDIA_UNKNOW, AV_CODEC_ID_NONE},
};

int FFCodecTypeToStreamCodecType(AVCodecID ff_codec_type) {
    int videoCodec = MediaStreamInfo::MEDIA_UNKNOW;
    for (int i = 0; i < sizeof(videoCodecMap)/ sizeof(struct VideoCodec2AVCodecID); ++i) {
        if (videoCodecMap[i].av_codec == ff_codec_type) {
            videoCodec = videoCodecMap[i].media_codec;
            break;
        }
    }
    return videoCodec;
}

static struct VideoFormat2AVPixelFormat {
    int video_format;
    int ff_video_format;
} videoFormatMap[] = {
        {VideoDescription::FORMAT_I420, AV_PIX_FMT_YUV420P},
        {VideoDescription::FORMAT_I420, AV_PIX_FMT_YUVJ420P},
        {VideoDescription::FORMAT_NV12, AV_PIX_FMT_NV12},
        {VideoDescription::FORMAT_NV21, AV_PIX_FMT_NV21},
        {VideoDescription::FORMAT_RGBA, AV_PIX_FMT_ARGB},
        {VideoDescription::FORMAT_UNKNOW, AV_PIX_FMT_NONE}
};

static struct AudioFormat2AVSampleFormat {
    int audio_format;
    int ff_audio_format;
} audioFormatMap[] = {
        {AudioDescription::FORMAT_U8, AV_SAMPLE_FMT_U8},
        {AudioDescription::FORMAT_S16, AV_SAMPLE_FMT_S16},
        {AudioDescription::FORMAT_S32, AV_SAMPLE_FMT_S32},
        {AudioDescription::FORMAT_FLOAT, AV_SAMPLE_FMT_FLT},
        {AudioDescription::FORMAT_FLOAT, AV_SAMPLE_FMT_FLTP},
        {AudioDescription::FORMAT_UNKNOW, AV_SAMPLE_FMT_NONE},
};

int AVPixelFormatToVideoFormat(AVPixelFormat format) {
    int videoFormat = VideoDescription::FORMAT_UNKNOW;
    for (int i = 0; i < sizeof(videoFormatMap)/ sizeof(VideoFormat2AVPixelFormat); ++i) {
        if (videoFormatMap[i].ff_video_format == format) {
            videoFormat = videoFormatMap[i].video_format;
            break;
        }
    }
    return videoFormat;
}
int AVSampleFormatToAudioFormat(AVSampleFormat format) {
    int audioFormat = AudioDescription::FORMAT_UNKNOW;
    for (int i = 0; i < sizeof(audioFormatMap)/ sizeof(AudioFormat2AVSampleFormat); ++i) {
        if (audioFormatMap[i].ff_audio_format == format) {
            audioFormat = audioFormatMap[i].audio_format;
            break;
        }
    }
    return audioFormat;
}

static bool AVStreamToMediaStreamInfo(AVStream* avstream, MediaStreamInfo& stream_info, int flag) {

    AVCodecParameters *codecpar = avstream->codecpar;
    stream_info.media_type = MediaStreamInfo::MEDIA_UNKNOW;
    stream_info.stream_id = avstream->index;
    stream_info.timebase_den = avstream->time_base.den;
    stream_info.timebase_num = avstream->time_base.num;
    if(codecpar) stream_info.bitrate = static_cast<int>(codecpar->bit_rate);
    if(codecpar->codec_type == AVMEDIA_TYPE_VIDEO && (flag & MediaSegment::FLAG_VIDEO)) {
        //TODO: initialize video stream
        stream_info.media_type = MediaStreamInfo::MEDIA_VIDEO;
        stream_info.codec_type = FFCodecTypeToStreamCodecType(codecpar->codec_id);
        stream_info.v_desc.pixel_format = AVPixelFormatToVideoFormat((AVPixelFormat)codecpar->format);
        stream_info.v_desc.width = codecpar->width;
        stream_info.v_desc.height = codecpar->height;
        stream_info.v_desc.framerate = static_cast<float>(av_q2d(avstream->r_frame_rate));
        AVDictionaryEntry *tag = av_dict_get(avstream->metadata, "rotate", NULL, 0);
        if (tag) {
            stream_info.v_desc.rotation = atoi(tag->value) % 360;
        } else
            stream_info.v_desc.rotation = 0;
        
        stream_info.v_desc.profile = codecpar->profile;
        stream_info.v_desc.level = codecpar->level;
        stream_info.v_desc.color_range = codecpar->color_range;
        stream_info.v_desc.color_space = codecpar->color_space;
        stream_info.v_desc.color_trc = codecpar->color_trc;
        stream_info.v_desc.color_primaries = codecpar->color_primaries;
        // av_reduce
        stream_info.v_desc.aspect_ratio_den = codecpar->sample_aspect_ratio.den;
        stream_info.v_desc.aspect_ratio_num = codecpar->sample_aspect_ratio.num;

    } else if(codecpar->codec_type == AVMEDIA_TYPE_AUDIO && (flag & MediaSegment::FLAG_AUDIO)) {
        //TODO: initialize audio stream
        stream_info.media_type = MediaStreamInfo::MEDIA_AUDIO;
        stream_info.codec_type = FFCodecTypeToStreamCodecType(codecpar->codec_id);
        stream_info.a_desc.smaple_format = AVSampleFormatToAudioFormat((AVSampleFormat)codecpar->format);
        stream_info.a_desc.sample_rate = codecpar->sample_rate;
        stream_info.a_desc.nchannel = codecpar->channels;
        stream_info.a_desc.sample_bits = codecpar->bits_per_raw_sample;
    } else if(codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE && (flag & MediaSegment::FLAG_SUBTITLE)) {
        //TODO: initialize subtitle stream
    }
    
    stream_info.duration = (avstream->duration != AV_NOPTS_VALUE)? avstream->duration * av_q2d(avstream->time_base) * /*AV_TIME_BASE*/1000: -1;

    return stream_info.media_type != MediaStreamInfo::MEDIA_UNKNOW;
}


MediaSegmentImpl::MediaSegmentImpl(const char* file_name, int flag) : file_name_(file_name) {
    is_valid_ = Load(file_name, flag) >= 0;
    audio_node_ = std::shared_ptr<AudioRenderNodeImpl>(new AudioRenderNodeImpl(this));
    video_node_ = std::shared_ptr<VideoGraphicNode>(new VideoGraphicNode(this));
}
MediaSegmentImpl::~MediaSegmentImpl() {
    
}

std::shared_ptr<VideoRenderNode> MediaSegmentImpl::getVideo() {
    return video_node_;;
}

std::shared_ptr<AudioRenderNode> MediaSegmentImpl::getAudio() {
    return audio_node_;
}

std::vector<MediaStreamInfo> MediaSegmentImpl::getMediaStreamInfo() {
    return media_streams_;
}

bool MediaSegmentImpl::isValid() {
    return is_valid_;
}

std::string MediaSegmentImpl::getFileName() {
    return file_name_;
}

void MediaSegmentImpl::setTimescale(float time_scale) {
    time_scale_ = time_scale;
}

float MediaSegmentImpl::getTimescale() {
    return time_scale_;
}

void MediaSegmentImpl::setSourceRange(const MediaRange & range) {
    source_range_ = range;
}

MediaRange MediaSegmentImpl::getSourceRange() {
    if (source_range_) return source_range_.value();
    return MediaRange(0, media_duration_);
}

void MediaSegmentImpl::setDisplayRange(const MediaRange & range) {
    display_range_ = range;
}

MediaRange MediaSegmentImpl::getDisplayRange() {
    if (!display_range_) {
        display_range_ = MediaRange(0, RangeLength(getSourceRange()) * time_scale_);
    }
    return display_range_.value();
}

int64_t MediaSegmentImpl::getMediaDuration() {
    return media_duration_;
}


int32_t MediaSegmentImpl::Load(const char *urlpath, int flags) {

    int err = -1;
    file_name_ = urlpath;

    do {
        AVFormatContext *fmt_ctx = avformat_alloc_context();
        if (! fmt_ctx) {
            LOGE("avformat_alloc_context Could not allocate context.");
            break;
        }

        int err = avformat_open_input(&fmt_ctx, urlpath, NULL, NULL);
        if (err < 0) {
            avformat_free_context(fmt_ctx);
            LOGE("avformat_open_input %s error %d errstr: %s ", urlpath, err ,av_err2str(err));
            break;
        }

        err = avformat_find_stream_info(fmt_ctx, NULL);
        if (err < 0) {
            LOGE("avformat_find_stream_info %s error %d errstr: %s", urlpath, err, av_err2str(err));
        }

        media_duration_ = fmt_ctx->duration/1000;
        media_streams_.clear();

        for (int i=0; i < fmt_ctx->nb_streams && i < 5; i++) {
            MediaStreamInfo stream_info;
            if(!AVStreamToMediaStreamInfo(fmt_ctx->streams[i], stream_info, flags)) {
                //TODO: discard unsupport stream
                fmt_ctx->streams[i]->discard = AVDISCARD_ALL;
            } else {
                if (stream_info.duration == -1) {
                    stream_info.duration = media_duration_;
                }
                media_streams_.push_back(stream_info);
            }
        }
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);
        err = 0;
    }while (0);

    return err;
}

int64_t MediaSegmentImpl::SourceTimeToDisplayTime(int64_t source_time) {
    auto dsp_range = getDisplayRange();
    auto src_range = getSourceRange();
    if (source_time > src_range.end) {
        return dsp_range.end;
    }else {
        return dsp_range.start + (source_time - src_range.start) * time_scale_;
    }
}
int64_t MediaSegmentImpl::DisplayTimeToSourceTime(int64_t display_time) {
    auto display_range = getDisplayRange();
    auto src_range = getSourceRange();
    float src_distance = (float)(display_time - display_range.start) / time_scale_;
    return src_range.start + ((src_distance > RangeLength(src_range)) ? RangeLength(src_range): src_distance);
}

int64_t MediaSegmentImpl::GetTargetDisplayPosition(int64_t time_ms) {
    auto active_range = getDisplayRange();
    if (time_ms < active_range.start ) return active_range.start;
    if (time_ms > active_range.start + GetActiveDisplayLength()) return active_range.start + GetActiveDisplayLength();
    return time_ms;
}

int64_t MediaSegmentImpl::GetActiveDisplayLength() {
    return static_cast<long long>( RangeLength(getSourceRange()) * time_scale_);
}

bool MediaSegmentImpl::IsContinuous(MediaSegmentImpl* next) {
    if (IsSamePath(next)) {
        if (abs(getSourceRange().end - next->getSourceRange().start) < TIMD_DIFF) {
            return true;
        }
    }
    return false;
}
bool MediaSegmentImpl::IsSamePath(const MediaSegmentImpl* other) const {
    return !file_name_.compare(other->file_name_);
}
bool MediaSegmentImpl::IsContain(int64_t time_ms) {
    return IsRangeContain(getDisplayRange(), time_ms);
}

}
}
