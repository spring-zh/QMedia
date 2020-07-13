//
//  MediaStream.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaCore/decoder/SoftwareDecoder.h"
#include "MediaStream.h"

EncodedPacketQueue::EncodedPacketQueue() :_max_time_ms(-1) {}
EncodedPacketQueue::~EncodedPacketQueue() {
    clearAll();
}

void EncodedPacketQueue::addPacket(EncodedPacket& packet) {
    std::unique_lock<std::mutex> lock(_mutex);
    int64_t current_pts = packet.ntp_time_ms();
    if (_max_time_ms < current_pts) _max_time_ms = current_pts;
    _packet_list.push_back(packet);
}

EncodedPacket EncodedPacketQueue::removePacket() {
    EncodedPacket encodedPacket = EncodedPacket(nullptr,0);
    std::unique_lock<std::mutex> lock(_mutex);
    if (!_packet_list.empty()) {
        encodedPacket = _packet_list.front();
        _packet_list.pop_front();
    }
    return encodedPacket;
}

int64_t EncodedPacketQueue::dropPackets(int64_t time_to, bool previous) {
    std::list<EncodedPacket>::iterator iter_finded = _packet_list.end();
    if (previous) {
        for (auto iter = _packet_list.end(); iter != _packet_list.begin(); --iter) {
            if (iter->flags() & (EncodedPacket::FLAG_SYNC)) {
                if (iter->ntp_time_ms() <= time_to) {
                    iter_finded = iter;
                    break;
                }
            }
        }
    } else {
        for (auto iter = _packet_list.begin(); iter != _packet_list.end(); ++iter) {
            if (iter->flags() & (EncodedPacket::FLAG_SYNC)) {
                if (iter->ntp_time_ms() >= time_to) {
                    iter_finded = iter;
                    break;
                }
            }
        }
    }

    if (iter_finded != _packet_list.begin() && iter_finded != _packet_list.end() ) {
        int64_t time_ms = iter_finded->ntp_time_ms();
        //found the position to drop
        do{
            --iter_finded;
            std::list<EncodedPacket>::iterator iter_prev = std::prev(iter_finded);
        }while (iter_finded->flags() & EncodedPacket::FLAG_CONFIG && iter_finded != _packet_list.begin());

        _packet_list.erase( _packet_list.begin(), iter_finded);
        return time_ms;
    }

    return -1;
}

void EncodedPacketQueue::clearAll() {
    std::unique_lock<std::mutex> lock(_mutex);
    _packet_list.clear();
    _max_time_ms = -1;
}

#pragma mark ---------- MediaStream --------------
MediaStream::MediaStream(const MediaStreamDescribe& streamDescribe, DecoderFactory* decoderFactory ):
_lastDropFrame(nullptr,0),
_streamDescribe(streamDescribe),
_isStarted(false),
_need_flush(false),
_maxOutputFrameNum(4),
_timeLimit(-1)
{
    Decoder *decoder = (decoderFactory != nullptr) ? decoderFactory->CreateDecoder(_streamDescribe.describe) : SoftwareDecoderFactory::instance()->CreateDecoder(_streamDescribe.describe);
    _decoder = std::shared_ptr<Decoder>(decoder);
    if (_decoder) _decoder->RegisterDecodedFrameCallback(this);
}
MediaStream::~MediaStream() {
}

void MediaStream::start() {
    if (! _isStarted) {
        _decodedFrameQueue.setAbort(false);

        _isStarted = true;
        _isEnd = false;
        _decoded_thread = std::thread(&MediaStream::run, this);
        pthread_setname_np(_decoded_thread.native_handle(), _streamDescribe.describe.mediatype_ == MediaType::Video? "VideoDecodeThread":"AudioDecodeThread");
    }
}
void MediaStream::stop() {
    flush();
    waitForFlush();
    _isStarted = false;
    if (_decoded_thread.joinable())
        _decoded_thread.join();

    _decodedFrameQueue.setAbort(true);
}

void MediaStream::flush() {
    std::unique_lock<std::mutex> lock(_flush_mutex);
    _need_flush = true;
}

void MediaStream::waitForFlush(){
    if (_need_flush){
        std::unique_lock<std::mutex> lock(_flush_mutex);
        while (_need_flush)
            _flush_condition.wait(lock);
    }
    _isEnd = false;
}


int32_t MediaStream::OnDecoded(DecodedFrame& decodedFrame) {
    if (decodedFrame.render_time_ms() >= _timeLimit - 20) {
        _lastDropFrame = DecodedFrame(nullptr, 0);
        _decodedFrameQueue.pushFrame(decodedFrame);
        return 0;
    } else {
        _lastDropFrame = decodedFrame;
    }
    return -1;
}

void MediaStream::OnDecodedEnd() {
    _isEnd = true;
    if (_lastDropFrame.frame_buffer_ != nullptr) {
        _decodedFrameQueue.pushFrame(_lastDropFrame);
        _lastDropFrame = DecodedFrame(nullptr, 0);
    }
    _decodedFrameQueue.setAbort(true);
}

void MediaStream::run() {
    _decoder->InitDecode(_streamDescribe.describe, 0, _streamDescribe.specificInfo);

    while (_isStarted) {
        {
            //TODO: ckeck flush flag
            std::unique_lock<std::mutex> lock(_flush_mutex);
            if (_need_flush)
            {
                _decodedFrameQueue.clearAll();
                _lastDropFrame = DecodedFrame(nullptr, 0);
                _encodedPacketQueue.clearAll();
                _decoder->Flush();
                _need_flush = false;
                _flush_condition.notify_one();
            }
        }

        // handle output mFrameQueue overload
        if ( _encodedPacketQueue.size() <=0 || _decodedFrameQueue.length() > _maxOutputFrameNum) {
            //TODO: use notify to replace it
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        if (! _isStarted) break;

        EncodedPacket encodedPacket = _encodedPacketQueue.removePacket();

        if (encodedPacket.flags() & EncodedPacket::FLAG_CONFIG) {
              //TODO: reconfigure decoder
//            CodecSpecificInfo* codecSpecificInfo = (CodecSpecificInfo *) encodedPacket.getEncodedBuffer()->data();
//            _decoder->InitDecode(_streamDescribe.describe, 0, *codecSpecificInfo);
        }

        int ret;
        do {
            ret = _decoder->Decode(encodedPacket);
        }while (ret == Decoder::busy);
    }

    _decoder->Release();
}
