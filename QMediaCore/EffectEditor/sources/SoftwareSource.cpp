//
//  SoftwareSource.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Utils/Logger.h"
#include "SoftwareSource.h"
#include "MediaCore/demuxer/GeneralDemuxer.h"
#include "SoftwareFrameDrawer.h"

#include "MediaCore/core/SteadyClock.h"
#include "Utils/Logger.h"

void SoftwareSource::run() {

    if (_demuxerIndex < 0 && _demuxerIndex < _demuxerList.size()) {
        LOGE("SoftwareSource::run _selectIndex < 0 or _selectIndex overflow");
        return;
    }

    while (_isStarted) {

        bool bPacketOverCache = true;
        for(auto& stream : _mediaStreams) {
            bPacketOverCache &= (stream->getEncodedPacketQueue().size() >= _packetCacheConut);
        }

        if (bPacketOverCache) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        //demuxer is read end,  wait for seek or stop
        std::unique_lock<std::mutex> lock(_mutex);
        while (_isStarted && _demuxerIndex >= _demuxerList.size()) {
            //send empty packet to finish decode
            for (auto&  stream : _mediaStreams) {
                EncodedPacket empty_packet(nullptr,std::numeric_limits<int64_t >::max());
                stream->getEncodedPacketQueue().addPacket(empty_packet);
            }
            LOGI("source read end and wait");
            _condition.wait(lock);
        }

        if (!_isStarted)
            break;

        EncodedPacket packet(nullptr,0);
        Demuxer* demuxer = _demuxerList[_demuxerIndex].get();
        int readRet = demuxer->ReadPacket(packet);
        if (readRet >= 0) {
            int media_stream_idx = _stream_map[packet.getEncodedBuffer()->stream_id()];
            if (media_stream_idx >= 0) {
                MediaStream *stream = _mediaStreams[media_stream_idx].get();
                if (stream) {
                    packet.set_ntp_time_ms(packet.ntp_time_ms() + _duration_offset);
                    stream->getEncodedPacketQueue().addPacket(packet);
                }
            }
        } else if(demuxer->IsEOF()) {
            //TODO: Demuxer read end
            _demuxerIndex ++;
            _duration_offset += demuxer->getDuration();
            if (_demuxerIndex < _demuxerList.size())
                _demuxerList[_demuxerIndex]->Seek(0, _videoStreamIndex != -1 ? 0 : 1);
        }
    }
}

SoftwareSource::SoftwareSource(std::vector<std::string>& fileList, bool enableVideo, bool enableAudio):
_fileList(fileList), _isLoaded(false),
_enableVideo(enableVideo), _enableAudio(enableAudio),
_videoStreamIndex(-1), _audioStreamIndex(-1),
_demuxerIndex(-1){
    std::memset(_stream_map, -1, sizeof(_stream_map));
    _packetCacheConut = 25;
}

SoftwareSource::~SoftwareSource() {

}

bool SoftwareSource::load() {
    if (_isLoaded)
        return true;

    for(auto& file : _fileList) {
        DemuxerRef demuxer = DemuxerRef(new GeneralDemuxer());
        if( demuxer->Open(file.c_str(), 0) >= 0) {
            if (!_isLoaded) {
                for (int i = 0; i < demuxer->getMediaStreamDescribes().size() && i < MAX_STREAMS; ++i) {
                    auto stream_describes = demuxer->getMediaStreamDescribes()[i];
                    if (stream_describes.describe.mediatype_ == MediaType::Video && _videoStreamIndex == -1 && _enableVideo) {
                        _stream_map[i] = static_cast<int>(_mediaStreams.size());
                        _mediaStreams.push_back(MediaStreamRef(new MediaStream(stream_describes)));
                        _mediaDescs.push_back(stream_describes.describe);
                        _videoStreamIndex = i;
                    } else if (stream_describes.describe.mediatype_ == MediaType::Audio && _audioStreamIndex == -1 && _enableAudio) {
                        _stream_map[i] = static_cast<int>(_mediaStreams.size());
                        _mediaStreams.push_back(MediaStreamRef(new MediaStream(stream_describes)));
                        _mediaDescs.push_back(stream_describes.describe);
                        _audioStreamIndex = i;
                    }
                }
            }

            _demuxerList.push_back(demuxer);
            _media_duration += demuxer->getDuration();
            _isLoaded = true;
        }
    }

    return _isLoaded;
}

void SoftwareSource::unload() {
    _media_duration = 0;
    std::memset(_stream_map, -1, sizeof(_stream_map));
    _mediaStreams.clear();
    _mediaDescs.clear();
    _videoStreamIndex = -1;
    _audioStreamIndex = -1;
    _isLoaded = false;
}

bool SoftwareSource::isInit() {
    return _isLoaded;
}

bool SoftwareSource::start(int64_t startMSec, int64_t endMSec) {
    if (_isLoaded) {
        if (_isStarted)
            return true;
        MediaSource::start(startMSec, endMSec);

        _demuxerIndex = getDemuxerIndex(startMSec, _duration_offset);
        _demuxerList[_demuxerIndex]->Seek(startMSec - _duration_offset, _videoStreamIndex != -1 ? 0 : 1);

        for (auto& stream : _mediaStreams) {
            stream->setMaxOutputFrameNum(2);
            stream->start();
        }
        _isStarted = true;
        _processThread = std::thread(&SoftwareSource::run, this);
    }
    return _isStarted;
}

void SoftwareSource::stop() {
    if (_isStarted) {
        _isStarted = false;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _condition.notify_one();
        }

        if (_processThread.joinable())
            _processThread.join();

        for (auto& stream : _mediaStreams) {
            stream->stop();
        }

        for (auto& demuxer : _demuxerList) {
            demuxer->Close();
        }
    }
}
bool SoftwareSource::seekTo(int64_t mSec) {
    if (! _isStarted)
        return false;
    if (mSec > _media_time_range._end) {
        return false;
    }
    int64_t duration_offset;
    int demuxerIndex = getDemuxerIndex(mSec, duration_offset);
    if (demuxerIndex < 0)
        return false;

    _media_time_range._start = mSec;
    _duration_offset = duration_offset;
    _demuxerIndex = demuxerIndex;

    return decodeTo(mSec);
}

bool SoftwareSource::decodeTo(int64_t mSec, bool precise) {

    //check if deocde finish
    auto check_frame_output = [this]()->bool{
        bool bRet = false;
        if (_videoStreamIndex >= 0) { //wait for first video decodeframe
            bRet = _mediaStreams[_stream_map[_videoStreamIndex]]->getDecodedFrameQueue().length() > 0;
        } else { //wait for first decodeframe
            for (auto& stream : _mediaStreams) {
                bRet |= stream->getDecodedFrameQueue().length() > 0;
            }
        }
        return bRet;
    };

    std::unique_lock<std::mutex> lock(_mutex);
    _condition.notify_one();
    //clear all stream cache
    for (auto& stream : _mediaStreams){
        stream->getDecodedFrameQueue().setAbort(false);
        stream->flush();
    }

    bool bRet = false;
    auto& demuxer_seek = _demuxerList[_demuxerIndex];
    demuxer_seek->Seek(mSec - _duration_offset, _videoStreamIndex != -1 ? 0 : 1);
    demuxer_seek->setIgnoreBFrame(true);

    for (auto& stream : _mediaStreams) {
        stream->waitForFlush();
        stream->setStartTimeLimit(precise? mSec : -1);
    }

    int flush_packet_count = 1;
    //decode and wait for frame output
    while (flush_packet_count > 0) {

        if (_demuxerIndex >= _demuxerList.size()) { //file lists end
            flush_packet_count --;

            //wait for decode finished
            if (_videoStreamIndex >= 0)
                _mediaStreams[_stream_map[_videoStreamIndex]]->getDecodedFrameQueue().peekFrame();
            else if (_audioStreamIndex >= 0)
                _mediaStreams[_stream_map[_audioStreamIndex]]->getDecodedFrameQueue().peekFrame();
            break;
        }

        //check break condition
        bRet = check_frame_output();
        if (bRet) break;

        //check Packet OverCache condition
        bool bPacketOverCache = true;
        for(auto& stream : _mediaStreams) {
            bPacketOverCache &= (stream->getEncodedPacketQueue().size() >= _packetCacheConut);
        }
        if (bPacketOverCache) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        //read new packet
        EncodedPacket packet(nullptr,0);
        auto& demuxer = _demuxerList[_demuxerIndex];
        if (demuxer->ReadPacket(packet) >= 0) {
            int media_stream_idx = _stream_map[packet.getEncodedBuffer()->stream_id()];
            if (media_stream_idx >= 0) {
                MediaStream *stream = _mediaStreams[media_stream_idx].get();
                if (stream) {
                    //packet pts add duration_offset
                    int64_t new_pts = packet.ntp_time_ms() + _duration_offset;
                    packet.set_ntp_time_ms(new_pts);
                    stream->getEncodedPacketQueue().addPacket(packet);
                    //check if ignore b frame
                    if (new_pts >= mSec - 10) demuxer_seek->setIgnoreBFrame(false);
                }
            }
        } else if(demuxer->IsEOF()) {
            //TODO: Demuxer read end
            _demuxerIndex ++;
            _duration_offset += demuxer->getDuration();
            if (_demuxerIndex < _demuxerList.size())
                _demuxerList[_demuxerIndex]->Seek(0, _videoStreamIndex != -1 ? 0 : 1);
            else {
                //send empty packet to finish decode
                for (auto&  stream : _mediaStreams) {
                    EncodedPacket empty_packet(nullptr,std::numeric_limits<int64_t >::max());
                    stream->getEncodedPacketQueue().addPacket(empty_packet);
                }
            }
        }
    }

    demuxer_seek->setIgnoreBFrame(false);
    return bRet;
}

bool SoftwareSource::isEOF() {
    return _demuxerIndex >= _demuxerList.size();
}

int SoftwareSource::getDemuxerIndex(int64_t time_ms, int64_t& time_offset) {
    int index = -1;
    int64_t duration = 0;
    for (int i = 0; i < _demuxerList.size(); ++i) {
        if (duration + _demuxerList[i]->getDuration() >= time_ms) {
            time_offset = duration;
            index = i;
            break;
        }
        duration += _demuxerList[i]->getDuration();
    }
    return index;
}

VideoFrame SoftwareSource::readNextVideoFrame(int& error) {
    error = -1;
    VideoFrame videoFrame(nullptr, 0);
    MediaStream* stream = _mediaStreams[_stream_map[_videoStreamIndex]].get();
    if (stream->getDecodedFrameQueue().length() <=0 && stream->isEnd()) {
        error = 1;
        return videoFrame;
    }
    DecodedFrame decodedFrame = stream->getDecodedFrameQueue().popFrame();
    if (decodedFrame.frame_buffer_) {
        videoFrame = VideoFrame(std::dynamic_pointer_cast<VideoFrameBuffer>(decodedFrame.frame_buffer_),decodedFrame.render_time_ms());
        error = 0;
    }
    return videoFrame;
}

AudioFrame SoftwareSource::readNextAudioFrame(int& error) {
    error = -1;
    AudioFrame audioFrame(nullptr, 0, 0);
    MediaStream* stream = _mediaStreams[_stream_map[_audioStreamIndex]].get();
    if (stream->getDecodedFrameQueue().length() <=0 && stream->isEnd()) {
        error = 1;
        return audioFrame;
    }
    DecodedFrame decodedFrame = stream->getDecodedFrameQueue().popFrame();
    if (decodedFrame.frame_buffer_) {
        audioFrame = AudioFrame(std::dynamic_pointer_cast<AudioFrameBuffer>(decodedFrame.frame_buffer_),decodedFrame.timestamp_ ,decodedFrame.render_time_ms());
        error = 0;
    }
    return audioFrame;
}

VideoFrameDrawer* SoftwareSource::createVideoFrameDrawer() {
    VideoFrameDrawer* videoFrameDrawer = nullptr;
    RawVideoFormat videoFormat = RawVideoFormat::kUnknown;
    if (_videoStreamIndex >= 0)
        videoFormat = _mediaStreams[_stream_map[_videoStreamIndex]]->getMediaDescribe().describe._videodesc.format;
    switch (videoFormat) {
        case RawVideoFormat::kI420:
        case RawVideoFormat::kYV12:
            videoFrameDrawer = new YUVFrameDrawer(_videoTarget);
            break;
        case RawVideoFormat::kARGB:
        case RawVideoFormat::kBGRA:
            videoFrameDrawer = new RGBAFrameDrawer(_videoTarget);
            break;
        default:
            break;
    }
    return videoFrameDrawer;
}