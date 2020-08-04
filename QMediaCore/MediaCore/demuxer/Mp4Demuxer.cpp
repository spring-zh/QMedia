//
//  Mp4Demuxer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Mp4Demuxer.h"
#include "Utils/Logger.h"

#include "mp4/file.h"
#include "mp4/filebytestream.h"
#include "mp4/synthetic_sampletable.h"
#include "mp4/utils.h"
#include "mp4/avc_sample_description.h"
#include "mp4/avc_parser.h"
#include "mp4/adts_parser.h"
#include "mp4/sample_source.h"

#include "MediaCore/protocol/h264_parse.h"
#include "MediaCore/protocol//hevc_parse.h"

class Mp4PacketBuffer: public EncodedPacketBuffer {
public:
    Mp4PacketBuffer(int streamid):_streamid(streamid),_is_bframe(false) {
    }
    Mp4PacketBuffer(int streamid, bool is_bframe):
        _streamid(streamid),_is_bframe(is_bframe) {
    }

    ~Mp4PacketBuffer() {
        _mp4_sample.reset();
    }

    int64_t pts() const override {
        return _mp4_sample.getCts();
    }

    int64_t dts() const override {
        return _mp4_sample.getDts();
    }

    const uint8_t *data() const override {
        return _mp4_buffer.getData();
    }

    size_t size() const override {
        return _mp4_buffer.getDataSize();
    }

    bool is_keyframe() const override {
        return _mp4_sample.isSync();
    }

    bool bframe() const override {
        return _is_bframe;
    }

    bool isCompleteFrame() const override {
        return true;
    }

    int stream_id() const override {
        return _streamid;
    }
    mp4::Sample _mp4_sample;
    mp4::DataBuffer _mp4_buffer;
    int _streamid;
    bool _is_bframe;
};

static int PacketParseAndFill(VideoCodecType codecType, Mp4PacketBuffer* packetBuffer) {
    int iRet = -1;
    if (codecType == VideoCodecType::kH264) {
        switch (packetBuffer->data()[3] & 0x1f) {
            case NAL_PPS:
            {
                SPS sps;
                h264_decode_seq_parameter_set((uint8_t*)packetBuffer->data() + 4, packetBuffer->size() - 4, &sps, true);
            }
                break;
            case NAL_SPS:
            {
//                PPS pps;
//                h264_decode_picture_parameter_set((uint8_t*)packetBuffer->data() + 4, packetBuffer->size() - 4, &sps, &pps, 0);
            }
                break;
            case NAL_SEI:
                break;
            case NAL_IDR_SLICE:
            case NAL_SLICE:
                Slice slice;
                if(0 == h264_decode_slice((uint8_t*)packetBuffer->data() + 5, packetBuffer->size() - 5, &slice, 8)) {
                    if (slice.slice_type == 7)
                        packetBuffer->_mp4_sample.setSync(true);
                    else if (slice.slice_type == 6)
                        packetBuffer->_is_bframe = true;
                }
                iRet = 0;
                break;
            default:
                break;
        }
    } else if (codecType == VideoCodecType::kH265) {

    }

    return iRet;
}

#pragma mark ------------ CodecType Map ----------------
typedef struct VideoCodec2Tag {
    VideoCodecType codecType;
    const char* mp4tag;
}VideoCodec2Tag;

typedef struct AudioCodec2Tag {
    AudioCodecType codecType;
    const char* mp4tag;
}AudioCodec2Tag;

static VideoCodec2Tag videoCodecMap[] = {
        {VideoCodecType::kVP8, "vp08"},
        {VideoCodecType::kVP9, "vp09"},
        {VideoCodecType::kMPEG4, "mp4v"},
        {VideoCodecType::kH264, "avc1"},
        {VideoCodecType::kH264, "avc3"},
        {VideoCodecType::kH265, "hev1"},
        {VideoCodecType::kH265, "hvc1"},
        {VideoCodecType::kUnknown, ""},
};

static AudioCodec2Tag audioCodecMap[] = {
        {AudioCodecType::kAAC, "mp4a"},
        {AudioCodecType::kAAC_LATM, "mp4a"},
        {AudioCodecType::kOpus, "Opus"},
        {AudioCodecType::kMP3, "mp4a"},
        {AudioCodecType::kPcmA, "alaw"},
        {AudioCodecType::kPcmU, "ulaw"},
        {AudioCodecType::kUnknown, ""},
};

VideoCodecType Mp4TagToVideoCodecType(const char * mp4tag) {
    VideoCodecType videoCodec = VideoCodecType::kUnknown;
    for (int i = 0; i < GET_ARRAY_COUNT(videoCodecMap); ++i) {
        if (strncasecmp(videoCodecMap[i].mp4tag, mp4tag,4) == 0) {
            videoCodec = videoCodecMap[i].codecType;
            break;
        }
    }
    return videoCodec;
}

AudioCodecType Mp4TagToAudioCodecType(const char * mp4tag) {
    AudioCodecType audioCodec = AudioCodecType::kUnknown;
    for (int i = 0; i < GET_ARRAY_COUNT(audioCodecMap); ++i) {
        if (strncasecmp(audioCodecMap[i].mp4tag, mp4tag,4) == 0) {
            audioCodec = audioCodecMap[i].codecType;
            break;
        }
    }
    return audioCodec;
}

#pragma mark -------- Mp4Demuxer ------------

#define TO_FIXED(a) (a << 16)
#define FROM_FIXED(a) (a >> 16)

Mp4Demuxer::Mp4Demuxer():
_isOpen(false),
_isEOF(false),
_mp4File(nullptr),
_inputStream(nullptr),
_videoTrackIndex(-1) {

}

Mp4Demuxer::~Mp4Demuxer() {

}

bool Mp4Demuxer::initMediaStreamDescribe(mp4::Track* track, MediaStreamDescribe& mediaStreamDescribe) {
    MediaDescribe describe;
    describe.mediatype_ = MediaType::Unkonw;

    auto sampleDescription = track->getSampleTable()->getSampleDescription(0);
    if (!sampleDescription)
        return false;

    if(track->getType() == mp4::Track::TYPE_VIDEO) {
        describe.mediatype_ = MediaType::Video;
        mp4::String codec;
        sampleDescription->getCodecString(codec);
        describe._videodesc.codectype = Mp4TagToVideoCodecType(codec.getChars());
        describe._videodesc.format = RawVideoFormat::kI420;
        describe._videodesc.width = FROM_FIXED(track->getWidth());
        describe._videodesc.height = FROM_FIXED(track->getHeight());
        describe._videodesc.framerate = track->getSampleCount() / track->getDurationMs() / 1000.0f;
        describe._videodesc.timeScale = track->getMediaTimeScale();
        describe._videodesc.rotation = VideoRotation::kVideoRotation_0;
        //get 3x3 matrix
        const mp4::SI32 * matrix = track->getTrakAtom()->getTkhd()->getMatrix();
        //TODO: check rotation
        if (matrix[0] == 0 && matrix[1] == TO_FIXED(1) && matrix[3] == -TO_FIXED(1) && matrix[4] == 0) {
            describe._videodesc.rotation = VideoRotation::kVideoRotation_90;
        } else if (matrix[0] == -TO_FIXED(1) && matrix[1] == 0 && matrix[3] == 0 && matrix[4] == -TO_FIXED(1)) {
            describe._videodesc.rotation = VideoRotation::kVideoRotation_180;
        } else if (matrix[0] == TO_FIXED(1) && matrix[1] == 0 && matrix[3] == 0 && matrix[4] == TO_FIXED(1)) {
            describe._videodesc.rotation = VideoRotation::kVideoRotation_270;
        }

        switch(sampleDescription->getType()) {
            case mp4::SampleDescription::TYPE_MPEG:
            {
                mp4::MpegSampleDescription* mpegSampleDescription = dynamic_cast<mp4::MpegSampleDescription*>(sampleDescription);
                if (mpegSampleDescription) {
                    describe._videodesc.bitrate = mpegSampleDescription->getAvgBitRate();
                    mediaStreamDescribe.specificInfo.addExtraData((uint8_t*)mpegSampleDescription->getDecoderInfo().getData(), mpegSampleDescription->getDecoderInfo().getDataSize());
                }
            }
                break;
            case mp4::SampleDescription::TYPE_AVC:
            case mp4::SampleDescription::TYPE_HEVC:
            {
                mp4::AvcSampleDescription* avcSampleDescription = dynamic_cast<mp4::AvcSampleDescription*>(sampleDescription);
                if (avcSampleDescription) {
                    describe._videodesc.width = avcSampleDescription->getWidth();
                    describe._videodesc.height = avcSampleDescription->getHeight();
                    mediaStreamDescribe.specificInfo.addExtraData((uint8_t*)avcSampleDescription->getRawBytes().getData(), avcSampleDescription->getRawBytes().getDataSize());
                }
            }
                break;
            default:
                break;
        }

    } else if(track->getType() == mp4::Track::TYPE_AUDIO) {
        describe.mediatype_ = MediaType::Audio;
        mp4::String codec;
        sampleDescription->getCodecString(codec);
        describe._audiodesc.codectype = Mp4TagToAudioCodecType(codec.getChars());
        mp4::AudioSampleDescription* audioSampleDescription = dynamic_cast<mp4::AudioSampleDescription*>(sampleDescription);
        if (audioSampleDescription) {
            describe._audiodesc.samplerate = audioSampleDescription->getSampleRate();
            describe._audiodesc.nchannel = audioSampleDescription->getChannelCount();
            describe._audiodesc.bitwidth = audioSampleDescription->getSampleSize();
        }
        if (sampleDescription->getType() == mp4::SampleDescription::TYPE_MPEG) {
            mp4::MpegAudioSampleDescription* mpegSampleDescription = dynamic_cast<mp4::MpegAudioSampleDescription*>(sampleDescription);
            describe._audiodesc.bitrate = mpegSampleDescription->getAvgBitRate();
            mediaStreamDescribe.specificInfo.addExtraData((uint8_t*)mpegSampleDescription->getDecoderInfo().getData(), mpegSampleDescription->getDecoderInfo().getDataSize());
        }
    }

    mediaStreamDescribe.describe = describe;
    mediaStreamDescribe.duraion = track->getDurationMs();

    return mediaStreamDescribe.describe.mediatype_ != MediaType::Unkonw;
}

int Mp4Demuxer::findBestVideoStream() {
    int min_duration_ms = std::numeric_limits<int>::max();
    int stream_id = -1;
    for (int i = 0; i < _mediaStreamDescribes.size(); ++i) {
        if (_mediaStreamDescribes[i].describe.mediatype_ == MediaType::Video) {
            int distance = std::abs((int)_mediaSampleSources[i]->getDurationMs() - (int)_mp4File->getMovie()->getDurationMs());
            if (distance < min_duration_ms) {
                min_duration_ms = distance;
                stream_id = i;
            }
        }
    }
    return stream_id;
}

int32_t Mp4Demuxer::Open(const char *urlpath, int flags) {

    int ret = -1;
    _isEOF = false;
    _url = urlpath;

    do {
        //create file input stream
        auto result = mp4::FileByteStream::create(urlpath, mp4::FileByteStream::STREAM_MODE_READ, _inputStream);
        if (result != mp4::SUCCESS) {
            LOGE("FileByteStream::create Failed.");
            break;
        }
        // open the file
        _mp4File = new mp4::File(*_inputStream);
        _mediaStreamDescribes.clear();
        for (mp4::Cardinal i = 0; i < _mp4File->getMovie()->getTracks().itemCount(); ++i){
            MediaStreamDescribe streamDescribe;
            mp4::Track* track;
            result = _mp4File->getMovie()->getTracks().get(i, track);
            if(result == mp4::SUCCESS) {
                if(initMediaStreamDescribe( track, streamDescribe) || true) {
                    _mediaStreamDescribes.push_back(streamDescribe);
                    _mediaSampleSources.push_back(new mp4::TrackSampleSource(track));
                }
            }
        }
        _videoTrackIndex = findBestVideoStream();
        _isOpen = true;
        ret = 0;
    }while (0);

    return ret;
}

int32_t Mp4Demuxer::Close() {
    _mediaStreamDescribes.clear();
    if (_isOpen) {
        SAFE_DELETE(_mp4File);
        _inputStream->release();
        for (int i = 0; i < _mediaSampleSources.size(); ++i) {
            delete _mediaSampleSources[i];
        }
        _mediaSampleSources.clear();
        _isOpen = false;
    }
    return 0;
}

int32_t Mp4Demuxer::ReadPacket(EncodedPacket& packet) {

    //find the min timestamp of track to read
    mp4::SI64 minTimeMs = std::numeric_limits<mp4::SI64>::max();
    int readIndex = -1;
    for (int i = 0; i < _mediaSampleSources.size(); ++i) {
        mp4::SI64 sampleTime;
        if (_mediaSampleSources[i]->getNextSampleTime(sampleTime) == mp4::SUCCESS) {
            if (sampleTime < minTimeMs) {
                readIndex = i;
                minTimeMs = sampleTime;
            }
        }
    }

    if (readIndex >= 0) {
        Mp4PacketBuffer* mp4PacketBuffer = new Mp4PacketBuffer(readIndex);
        mp4::UI32 trackId;
        mp4::SampleSource * trackSampleSource = _mediaSampleSources[readIndex];
        mp4::Result result = trackSampleSource->readNextSample(mp4PacketBuffer->_mp4_sample, mp4PacketBuffer->_mp4_buffer, trackId);
        if (result == mp4::SUCCESS) {
            packet = EncodedPacket(std::shared_ptr<EncodedPacketBuffer>(mp4PacketBuffer), minTimeMs);
            packet.setFlags(mp4PacketBuffer->is_keyframe()? EncodedPacket::FLAG_SYNC : 0);

            return packet.getEncodedBuffer()->size();
        }
    } else {
        _isEOF = true;
    }
    return -1;
}

int32_t Mp4Demuxer::Seek(int64_t seek_time, int flags) {
    _isEOF = false;
    mp4::Result mp4Ret = mp4::FAILURE;
    if (_isOpen)
    {
        mp4::SI64 syncTime = seek_time;
        if (_videoTrackIndex >= 0) {
            mp4Ret = _mediaSampleSources[_videoTrackIndex]->seekToTime(syncTime);
            mp4::SI64 videoStreamTime;
            if(_mediaSampleSources[_videoTrackIndex]->getNextSampleTime(videoStreamTime) == mp4::SUCCESS) {
                syncTime = videoStreamTime;
            }
        }

        for (int i = 0; i < _mediaSampleSources.size(); ++i) {
            if (_videoTrackIndex == i) continue;
            if(_mediaSampleSources[i]->seekToTime(syncTime) == mp4::SUCCESS)
                mp4Ret = mp4::SUCCESS;
        }
    }

    return (mp4Ret == mp4::SUCCESS) ? 0 : -1;
}

int32_t Mp4Demuxer::Flush() {
    return -1;
}

void Mp4Demuxer::setIgnoreBFrame(bool ignore) {

}

int64_t Mp4Demuxer::getDuration() const {
    return _mp4File->getMovie()->getDurationMs();
}