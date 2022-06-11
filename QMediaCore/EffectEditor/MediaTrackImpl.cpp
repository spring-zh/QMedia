//
//  MediaTrackImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaTrackImpl.h"

#define MAX_FRAME_SKIP 5

MediaTrackImpl::MediaTrackImpl(MediaSourceRef source, MediaType media_type):
media_type_(media_type),
_sourceRef(source),
_isPrepare(false),
_timeScale(1.0f),
_repeatTimes(1),
_absoluteTrackRange(0,0),
_video_stream_idx(-1),_audio_stream_idx(-1)
{
    _after_seek_video = false;
    _after_seek_audio = false;
    _last_video_ms = -1;
    _last_audio_ms = -1;
    _media_position_ms = -1;
    
    sequentail_decoder_ = SequentailDecoder::UPtr(new SequentailDecoder(media_type, this));
}

MediaTrackImpl::~MediaTrackImpl()
{
    if (_isPrepare) {
        _sourceRef->unload();
        _isPrepare = false;
    }
}

bool MediaTrackImpl::setSourceRange(Range<int64_t> selectSourceRange) {
    if (selectSourceRange._end > _sourceRef->getMediaDuration()) {
        selectSourceRange._end = _sourceRef->getMediaDuration();
    }
    _sourceRange = selectSourceRange;
    return true;
}

bool MediaTrackImpl::mapTimeToMediaSource(int64_t mSec, int64_t& outmSec) const
{
    auto displayRange = getDisplayTrackRange();
    if (_sourceRange.isValid() && displayRange.isContain(mSec)) {
        int64_t distance = (mSec - displayRange._start) / _timeScale;
        int loop = _repeatTimes;
        while (distance > _sourceRange.getLength() && loop > 1) {
            distance -= _sourceRange.getLength();
            loop--;
        }
        if(distance <= _sourceRange.getLength()) {
            outmSec = _sourceRange._start + distance;
            return true;
        }
    }
    return false;
}

bool MediaTrackImpl::prepare()
{
//    wrlock_guard wrlock(_rwlock);
    if (!_isPrepare) {
        _isPrepare = _sourceRef->load();
        if (! _sourceRange.isValid()) {
            _sourceRange._start = 0;
            _sourceRange._end = _sourceRef->getMediaDuration();
        }

        for (int index = 0; index < _sourceRef->getMediaDescribe().size(); ++index) {
            if (_sourceRef->getMediaDescribe()[index].mediatype_ == MediaType::Video &&
                (_video_stream_idx < 0)) {
                _video_stream_idx = index;
//                _videoChannel = MediaGraphicChannelRef(new MediaGraphicChannel(this));
            }else if (_sourceRef->getMediaDescribe()[index].mediatype_ == MediaType::Audio &&
                (_audio_stream_idx < 0)) {
                _audio_stream_idx = index;
//                _audioChannel = MediaAudioChannelRef(new MediaAudioChannel(this));
            }
        }
        
        SequentailSlice slice(_sourceRef->filePath(), _sourceRef->getMediaDuration());
        Range<int64_t> src_range(0, _sourceRef->getMediaDuration()/2);
        slice.SetSourceRannge(src_range);
        slice.SetDisplayRannge(src_range);
        SequentailSlice slice2 = slice;
//        slice2.SetSourceRannge(src_range);
        slice2.SetDisplayRannge(slice.GetDisplayRannge()._end, slice.GetDisplayRannge()._end + src_range.getLength());
        slices.push_back(slice);
        slices.push_back(slice2);
        sequentail_decoder_->StartMedia(slices);
        
        _sourceRef->stop();
    }
    
    return _isPrepare;
}

std::future<bool> MediaTrackImpl::setPositionTo(int64_t mSec)
{
//    return thread_task_.PostTask([this](int64_t mSec)->bool {
//        wrlock_guard wrlock(_rwlock);
//        int64_t remap_time;
//        bool bRet = false;
//
//        if(mSec < getDisplayTrackRange()._start) {
//            mSec = getDisplayTrackRange()._start;
//        }
//
//        if(mapTimeToMediaSource(mSec, remap_time))
//        {
//            if (_sourceRange.isContain(remap_time)) {
//                if (_sourceRef->isStarted()) {
//                    if (remap_time < (_media_position_ms - 20) || remap_time > (_media_position_ms + 1000)) {
//                        bRet = _sourceRef->seekTo(remap_time);
//                        _media_position_ms = _last_video_ms = _last_audio_ms = remap_time - 1;
//                    }else
//                        bRet = true;
//                }else {
//                    bRet = _sourceRef->start(remap_time, _sourceRange._end);
//                    _media_position_ms = _last_video_ms = _last_audio_ms = remap_time - 1;
//                }
//            }
//        }
//        _after_seek_video = true;
//        _after_seek_audio = true;
//        return bRet;
//    }, mSec);
    
    return sequentail_decoder_->SetPositionTo(mSec);
}

std::future<void> MediaTrackImpl::stopMedia()
{
//    return thread_task_.PostTask([this](){
//        wrlock_guard wrlock(_rwlock);
//        _sourceRef->stop();
//    });
    return sequentail_decoder_->StopMedia();
}

float MediaTrackImpl::getPlaySpeed() const
{
    return 1.0f / _timeScale;
}

const Range<int64_t> MediaTrackImpl::getDisplayTrackRange() const
{
    if (!_absoluteTrackRange.isValid()) {
        _absoluteTrackRange._start = 0;
        _absoluteTrackRange._end = _sourceRef->getMediaDuration() * _timeScale * _repeatTimes;
    }

    return _absoluteTrackRange;
}

VideoFrame MediaTrackImpl::getVideoFrame(int64_t mSec , bool& isEnd)
{
//    VideoFrame retFrame(nullptr,0);
//    int64_t remap_time = 0;
//    if (! mapTimeToMediaSource(mSec, remap_time)) {
//        return retFrame;
//    }
//
//    rdlock_guard rdlock(_rwlock);
//    isEnd = false;
//    int skip_count = 0;
//    if ( _sourceRef->isStarted()) {
//
//        while (remap_time > _last_video_ms) {
//            int errRet;
//            VideoFrame readframe = _sourceRef->readNextVideoFrame(errRet);
//            if (errRet == 0) {
//                retFrame = std::move(readframe);
//                _media_position_ms = _last_video_ms = retFrame.timestamp_ms();
//                //TODO:
//                if (_after_seek_video) {
//                    _after_seek_video = false;
//                    break;
//                }
//            }else if(errRet == 1){
//                isEnd = true;
//                break;
//            }
//
//            if (errRet == -1 || skip_count++ >= MAX_FRAME_SKIP) {
//                break;
//            }
//        }
//    }else
//        isEnd = true;
//
//    if(retFrame.video_frame_buffer() != nullptr)
//        isEnd = false;
//    return retFrame;
    if (media_type_ == MediaType::Video) {
        int index;
        auto frame = sequentail_decoder_->ReadFrame(mSec, index);
        return VideoFrame(frame.frame_buffer_, frame.timestamp_ms_);
    } else {
        isEnd = true;
        return VideoFrame(nullptr, 0);
    }
}
AudioFrame MediaTrackImpl::getAudioFrame(int64_t mSec , bool& isEnd)
{
//    int64_t remap_time = 0;
//    if (! mapTimeToMediaSource(mSec, remap_time)) {
//        return retFrame;
//    }
//
//    rdlock_guard rdlock(_rwlock);
//    isEnd = false;
//    int skip_count = 0;
//    if ( _sourceRef->isStarted()) {
//
//        while (remap_time > _last_audio_ms) {
//            int errRet;
//            AudioFrame readframe = _sourceRef->readNextAudioFrame(errRet);
//            if (errRet == 0) {
//                retFrame = std::move(readframe);
//                _last_audio_ms = retFrame.timestamp_ms();
//
//                //TODO:
//                if (_after_seek_audio) {
//                    _after_seek_audio = false;
//                    break;
//                }
//            }else if(errRet == 1) {
//                isEnd = true;
//                break;
//            }
//
//            if (errRet == -1 || skip_count++ >= MAX_FRAME_SKIP) {
//                break;
//            }
//        }
//    }else
//        isEnd = true;
//
//    if(retFrame.audio_frame_buffer() != nullptr)
//        isEnd = false;
    if (media_type_ == MediaType::Audio) {
        int index;
        auto frame = sequentail_decoder_->ReadFrame(mSec, index);
        return AudioFrame(frame.frame_buffer_, frame.timestamp_ms_, frame.timestamp_ms_);
    } else {
        isEnd = true;
        return AudioFrame(nullptr, 0, 0);
    }
}

void MediaTrackImpl::OnSequentailError(int error) {
    
}

const VideoDescribe *const MediaTrackImpl::getVideoDescribe() const
{
    if (_video_stream_idx >= 0) {
        return &_sourceRef->getMediaDescribe()[_video_stream_idx]._videodesc;
    }
    return nullptr;
}
const AudioDescribe *const MediaTrackImpl::getAudioDescribe() const
{
    if (_audio_stream_idx >= 0) {
        return &_sourceRef->getMediaDescribe()[_audio_stream_idx]._audiodesc;
    }
    return nullptr;
}
