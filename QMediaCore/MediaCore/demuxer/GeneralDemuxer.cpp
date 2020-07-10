//
//  GeneralDemuxer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GeneralDemuxer.h"
#include "Utils/Logger.h"
#include "MediaCore/decoder/SoftwareDecoder.h"

extern "C" {
#include "libavformat/avformat.h"
}

#pragma mark ------------ CodecType Map ----------------
typedef struct VideoCodec2AVCodecID {
    VideoCodecType codecType;
    AVCodecID avCodecId;
}VideoCodec2AVCodecID;

typedef struct AudioCodec2AVCodecID {
    AudioCodecType codecType;
    AVCodecID avCodecId;
}AudioCodec2AVCodecID;

typedef struct VideoFormat2AVPixelFormat {
    RawVideoFormat videoFormat;
    AVPixelFormat pixelFormat;
}VideoFormat2AVPixelFormat;

typedef struct AudioFormat2AVSampleFormat {
    RawAudioFormat audioFormat;
    AVSampleFormat sampleFormat;
}AudioFormat2AVSampleFormat;

static VideoCodec2AVCodecID videoCodecMap[] = {
        {VideoCodecType::kVP8, AV_CODEC_ID_VP8},
        {VideoCodecType::kVP9, AV_CODEC_ID_VP9},
        {VideoCodecType::kMPEG4, AV_CODEC_ID_MPEG4},
        {VideoCodecType::kH264, AV_CODEC_ID_H264},
        {VideoCodecType::kH265, AV_CODEC_ID_HEVC},
        {VideoCodecType::kUnknown, AV_CODEC_ID_NONE},
};

static AudioCodec2AVCodecID audioCodecMap[] = {
        {AudioCodecType::kAAC, AV_CODEC_ID_AAC},
        {AudioCodecType::kAAC_LATM, AV_CODEC_ID_AAC_LATM},
        {AudioCodecType::kOpus, AV_CODEC_ID_OPUS},
        {AudioCodecType::kMP3, AV_CODEC_ID_MP3},
        {AudioCodecType::kPcmA, AV_CODEC_ID_PCM_ALAW},
        {AudioCodecType::kPcmU, AV_CODEC_ID_PCM_MULAW},
        {AudioCodecType::kUnknown, AV_CODEC_ID_NONE},
};

static VideoFormat2AVPixelFormat videoFormatMap[] = {
        {RawVideoFormat::kI420, AV_PIX_FMT_YUV420P},
        {RawVideoFormat::kI420, AV_PIX_FMT_YUVJ420P},
        {RawVideoFormat::kNV12, AV_PIX_FMT_NV12},
        {RawVideoFormat::kNV21, AV_PIX_FMT_NV21},
        {RawVideoFormat::kARGB, AV_PIX_FMT_ARGB},
        {RawVideoFormat::kUnknown, AV_PIX_FMT_NONE}
};

static AudioFormat2AVSampleFormat audioFormatMap[] = {
        {RawAudioFormat::kU8, AV_SAMPLE_FMT_U8},
        {RawAudioFormat::kS16, AV_SAMPLE_FMT_S16},
        {RawAudioFormat::kS32, AV_SAMPLE_FMT_S32},
        {RawAudioFormat::kFLOAT, AV_SAMPLE_FMT_FLT},
        {RawAudioFormat::kFLOAT, AV_SAMPLE_FMT_FLTP},
        {RawAudioFormat::kUnknown, AV_SAMPLE_FMT_NONE},
};

VideoCodecType FFCodecTypeToVideoCodecType(AVCodecID ff_codec_type) {
    VideoCodecType videoCodec = VideoCodecType::kUnknown;
    for (int i = 0; i < GET_ARRAY_COUNT(videoCodecMap); ++i) {
        if (videoCodecMap[i].avCodecId == ff_codec_type) {
            videoCodec = videoCodecMap[i].codecType;
            break;
        }
    }
    return videoCodec;
}

AudioCodecType FFCodecTypeToAudioCodecType(AVCodecID ff_codec_type) {
    AudioCodecType audioCodec = AudioCodecType::kUnknown;
    for (int i = 0; i < GET_ARRAY_COUNT(audioCodecMap); ++i) {
        if (audioCodecMap[i].avCodecId == ff_codec_type) {
            audioCodec = audioCodecMap[i].codecType;
            break;
        }
    }
    return audioCodec;
}

AVCodecID VideoCodecTypeToFFCodecType(VideoCodecType videoCodecType) {
    AVCodecID codecId = AV_CODEC_ID_NONE;
    for (int i = 0; i < GET_ARRAY_COUNT(videoCodecMap); ++i) {
        if (videoCodecMap[i].codecType == videoCodecType) {
            codecId = videoCodecMap[i].avCodecId;
            break;
        }
    }
    return codecId;
}

AVCodecID AudioCodecTypeToFFCodecType(AudioCodecType audioCodecType) {
    AVCodecID codecId = AV_CODEC_ID_NONE;
    for (int i = 0; i < GET_ARRAY_COUNT(audioCodecMap); ++i) {
        if (audioCodecMap[i].codecType == audioCodecType) {
            codecId = audioCodecMap[i].avCodecId;
            break;
        }
    }
    return codecId;
}

RawVideoFormat AVPixelFormatToVideoFormat(AVPixelFormat format) {
    RawVideoFormat videoFormat = RawVideoFormat::kUnknown;
    for (int i = 0; i < GET_ARRAY_COUNT(videoFormatMap); ++i) {
        if (videoFormatMap[i].pixelFormat == format) {
            videoFormat = videoFormatMap[i].videoFormat;
            break;
        }
    }
    return videoFormat;
}

AVPixelFormat VideoFormatToAVPixelFormat(RawVideoFormat videoFormat) {
    AVPixelFormat format = AV_PIX_FMT_NONE;
    for (int i = 0; i < GET_ARRAY_COUNT(videoFormatMap); ++i) {
        if (videoFormatMap[i].videoFormat == videoFormat) {
            format = videoFormatMap[i].pixelFormat;
            break;
        }
    }
    return format;
}

RawAudioFormat AVSampleFormatToAudioFormat(AVSampleFormat format) {
    RawAudioFormat audioFormat = RawAudioFormat::kUnknown;
    for (int i = 0; i < GET_ARRAY_COUNT(audioFormatMap); ++i) {
        if (audioFormatMap[i].sampleFormat == format) {
            audioFormat = audioFormatMap[i].audioFormat;
            break;
        }
    }
    return audioFormat;
}
AVSampleFormat AudioFormatToAVSampleFormat(RawAudioFormat audioFormat) {
    AVSampleFormat format = AV_SAMPLE_FMT_NONE;
    for (int i = 0; i < GET_ARRAY_COUNT(audioFormatMap); ++i) {
        if (audioFormatMap[i].audioFormat == audioFormat) {
            format = audioFormatMap[i].sampleFormat;
            break;
        }
    }
    return format;
}


#pragma mark -------- GeneralDemuxer ------------
GeneralDemuxer::GeneralDemuxer():
_isOpen(false),
_isEOF(false),
_formatContext(NULL) {

}

GeneralDemuxer::~GeneralDemuxer() {

}

bool GeneralDemuxer::initMediaStreamDescribe(AVStream* avstream, MediaStreamDescribe& mediaStreamDescribe) {

    AVCodecParameters *codecpar = avstream->codecpar;
    MediaDescribe describe;
    describe.mediatype_ = MediaType::Unkonw;
    if(codecpar->codec_type == AVMEDIA_TYPE_VIDEO ) {
        //TODO: initialize video stream
        describe.mediatype_ = MediaType::Video;
        describe._videodesc.codectype = FFCodecTypeToVideoCodecType(codecpar->codec_id);
        describe._videodesc.format = AVPixelFormatToVideoFormat((AVPixelFormat)codecpar->format);
        describe._videodesc.width = codecpar->width;
        describe._videodesc.height = codecpar->height;
        describe._videodesc.bitrate = static_cast<int>(codecpar->bit_rate);
        describe._videodesc.framerate = static_cast<float>(av_q2d(avstream->r_frame_rate));
        describe._videodesc.timeScale = 1000 * av_q2d(avstream->time_base);

    } else if(codecpar->codec_type == AVMEDIA_TYPE_AUDIO ) {
        //TODO: initialize audio stream
        describe.mediatype_ = MediaType::Audio;
        describe._audiodesc.codectype = FFCodecTypeToAudioCodecType(codecpar->codec_id);
        describe._audiodesc.format = AVSampleFormatToAudioFormat((AVSampleFormat)codecpar->format);
        describe._audiodesc.samplerate = codecpar->sample_rate;
        describe._audiodesc.nchannel = codecpar->channels;
        describe._audiodesc.bitrate = static_cast<int>(codecpar->bit_rate);
        describe._audiodesc.bitwidth = codecpar->bits_per_raw_sample;
    } else if(codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE ) {
        //TODO: initialize subtitle stream
    }

    mediaStreamDescribe.describe = describe;
    if (codecpar->extradata && codecpar->extradata_size > 0) {
        mediaStreamDescribe.specificInfo.addExtraData(codecpar->extradata, codecpar->extradata_size);
    }
    mediaStreamDescribe.duraion = avstream->duration != AV_NOPTS_VALUE?avstream->duration * AV_TIME_BASE * av_q2d(avstream->time_base): _formatContext->duration /1000;

    return mediaStreamDescribe.describe.mediatype_ != MediaType::Unkonw;
}

int32_t GeneralDemuxer::Open(const char *urlpath, int flags) {

    int ret = -1;
    _isEOF = false;
    _url = urlpath;

    do {
        _formatContext = avformat_alloc_context();
        if (! _formatContext) {
            LOGE("avformat_alloc_context Could not allocate context.");
            break;
        }

        int err = avformat_open_input(&_formatContext, urlpath, NULL, NULL);
        if (err < 0) {
            avformat_free_context(_formatContext);
            _formatContext = NULL;
            LOGE("avformat_open_input %s error %d errstr: %s ", urlpath, err ,av_err2str(err));
            break;
        }

        err = avformat_find_stream_info(_formatContext, NULL);
        if (err < 0) {
            LOGE("avformat_find_stream_info %s error %d errstr: %s", urlpath, err, av_err2str(err));
        }

        _mediaStreamDescribes.clear();

        for (int i=0; i < _formatContext->nb_streams && i < 5; i++) {
            MediaStreamDescribe streamDescribe;
            if(!initMediaStreamDescribe(_formatContext->streams[i], streamDescribe)) {
                //TODO: discard unsupport stream
                _formatContext->streams[i]->discard = AVDISCARD_ALL;
            }
            _mediaStreamDescribes.push_back(streamDescribe);
        }

        _isOpen = true;
        ret = 0;
    }while (0);

    return ret;
}

int32_t GeneralDemuxer::Close() {
    _mediaStreamDescribes.clear();
    if (_isOpen) {
        avformat_flush(_formatContext);
        avformat_close_input(&_formatContext);
        avformat_free_context(_formatContext);
        _formatContext = NULL;
        _isOpen = false;
    }
    return 0;
}

int32_t GeneralDemuxer::ReadPacket(EncodedPacket& packet) {

    AVPacket *avPacket = av_packet_alloc();
    if (avPacket == NULL)
        return AVERROR_UNKNOWN;
    int ret = av_read_frame(_formatContext, avPacket);
    if (ret >= 0) {
        int64_t render_time_ms = avPacket->pts * 1000 * av_q2d(_formatContext->streams[avPacket->stream_index]->time_base);
        std::shared_ptr<EncodedPacketBuffer> packetBuffer = std::shared_ptr<EncodedPacketBuffer>(new SoftwareEncodedPacketBuffer(avPacket));
        packet = EncodedPacket(packetBuffer, render_time_ms);
        packet.setFlags(packetBuffer->is_keyframe()? EncodedPacket::FLAG_SYNC : 0);
    } else {
        if (ret ==  AVERROR_EOF)
            _isEOF = true;
        av_packet_unref(avPacket);
    }
    return ret;
}

int32_t GeneralDemuxer::Seek(int64_t seek_time, int flags) {
    _isEOF = false;
    return avformat_seek_file(_formatContext, -1, INT64_MIN, seek_time * 1000, INT64_MAX, flags == 1 ? AVSEEK_FLAG_ANY : 0);
}

int32_t GeneralDemuxer::Flush() {
    avio_flush(_formatContext->pb);
    return avformat_flush(_formatContext);
}

void GeneralDemuxer::setIgnoreBFrame(bool ignore) {
    for (int i=0; i < _formatContext->nb_streams && i < 5; i++) {
        if(ignore)
//            _formatContext->streams[i]->discard |= AVDISCARD_BIDIR;
            _formatContext->streams[i]->discard = AVDISCARD_BIDIR;
        else
//            _formatContext->streams[i]->discard &= ~AVDISCARD_BIDIR;
            _formatContext->streams[i]->discard = AVDISCARD_DEFAULT;
    }
}

int64_t GeneralDemuxer::getDuration() const {
    return  _isOpen ? _formatContext->duration/1000 : 0;
}
