//
//  MediaSessionImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaSessionImpl.h"
#include "MediaSegmentImpl.h"
#include "Utils/Logger.h"
#include "MediaCore/audiocore/AudioProcess.h"
#include "MediaEngineImpl.h"
#include "medianode/VideoGraphicNode.h"
#include "medianode/AudioRenderNodeImpl.h"

namespace QMedia { namespace Api {

extern std::shared_ptr<AudioRender> CreatePlayerAudioRender(MediaSessionImpl* session);

extern std::shared_ptr<VideoRender> CreatePlayerVideoRender(MediaSessionImpl* session);

#define STATE_ISRUNING(state) (state == CombinerState::Prepared || state == CombinerState::Started || state == CombinerState::Paused )

MediaSessionImpl::MediaSessionImpl(): thread_task_("[threadtask].mediasession_main") {
    video_render_ = CreatePlayerVideoRender(this);
    audio_render_ = CreatePlayerAudioRender(this);
    display_layer_ = std::unique_ptr<DisplayLayer>(new DisplayLayer(this));
}

MediaSessionImpl::~MediaSessionImpl() {
    
}

std::shared_ptr<MediaSegment> MediaSessionImpl::cresteMediaSegment(const std::string & filename, int32_t flag) {
    return std::shared_ptr<MediaSegment>(new MediaSegmentImpl(filename.c_str(), flag));
}

bool MediaSessionImpl::addMediaSegment(const std::shared_ptr<MediaSegment> & segment) {
    auto result = thread_task_.PostTask(CMD_ADD_SEGMENT, [this, &segment](){
        if (std::find(source_segments_.begin(), source_segments_.end(), segment) == source_segments_.end()) {
            source_segments_.push_back(segment);
            for (auto& stream : segment->getMediaStreamInfo()) {
                if (stream.media_type == MediaStreamInfo::MEDIA_VIDEO) {
                    MediaSegmentManager::addVideoSegment(segment);
                    __addVideoRenderNode(segment->getVideo());
                }
                if (stream.media_type == MediaStreamInfo::MEDIA_AUDIO) {
                    MediaSegmentManager::addAudioSegment(segment);
                    __addAudioRenderNode(segment->getAudio());
                }
            }
            return true;
        }
        return false;
    });
    return result.get();
}

bool MediaSessionImpl::removeMediaSegment(const std::shared_ptr<MediaSegment> & segment) {
    auto result = thread_task_.PostTask(CMD_REMOVE_SEGMENT, [this, &segment](){
        auto iter = std::find(source_segments_.begin(), source_segments_.end(), segment);
        if (iter != source_segments_.end()) {
            source_segments_.erase(iter);
            for (auto& stream : segment->getMediaStreamInfo()) {
                if (stream.media_type == MediaStreamInfo::MEDIA_VIDEO) {
                    MediaSegmentManager::removeVideoSegment(segment);
                    __addVideoRenderNode(segment->getVideo());
                }
                if (stream.media_type == MediaStreamInfo::MEDIA_VIDEO) {
                    MediaSegmentManager::removeAudioSegment(segment);
                    __addAudioRenderNode(segment->getAudio());
                }
            }
            return true;
        }
        return false;
    });
    return result.get();
}

MediaRange MediaSessionImpl::getTotalTimeRange() {
    return MediaSegmentManager::getMediaTimeRange();
}

std::shared_ptr<VideoRender> MediaSessionImpl::getVideoRender() {
    return video_render_;
}

std::shared_ptr<AudioRender> MediaSessionImpl::getAudioRender() {
    return audio_render_;
}

void MediaSessionImpl::setAudioRunLoop(const std::shared_ptr<AudioRunloop> & audio_loop) {
    audio_loop_ = audio_loop;
}

void MediaSessionImpl::setVideoRunLoop(const std::shared_ptr<VideoRunloop> & video_loop) {
    video_loop_ = video_loop;
}

void MediaSessionImpl::setDisplayLayerSize(const Size & size) {
    display_layer_->setLayerSize(RenderEngine::Size(size.width, size.height));
}

Size MediaSessionImpl::getDisplayLayerSize() {
    return Size(display_layer_->getContentSize().width, display_layer_->getContentSize().height);
}

void MediaSessionImpl::setBkColor(const Vec4f & color) {
    display_layer_->setBKColor(color);
}

Vec4f MediaSessionImpl::getBkColor() {
    return display_layer_->getBKColor();
}

/** control */
void MediaSessionImpl::prepare() {
    thread_task_.PostTask(CMD_PREPARE, [this]()->RetCode {
        //    MPST_RET_IF_EQ(_state, CombinerState::Idle);
        //    MPST_RET_IF_EQ(_state, CombinerState::Initialized);
        //    MPST_RET_IF_EQ(_state, CombinerState::AsyncPreparing);
        MPST_RET_IF_EQ(_state, SessionState::Prepared);
        MPST_RET_IF_EQ(_state, SessionState::Started);
        MPST_RET_IF_EQ(_state, SessionState::Paused);
        MPST_RET_IF_EQ(_state, SessionState::Completed);
    //    MPST_RET_IF_EQ(_state, CombinerState::Stopped);
        MPST_RET_IF_EQ(_state, SessionState::Error);
        MPST_RET_IF_EQ(_state, SessionState::Ended);
        
        //FIXME: defualt dispaylayer's contentsize will set by target' size
        auto &audio_out = MediaEngineImpl::instance()->get_audio_out();
        audio_loop_->audioInit(audio_out);
        _audioClock.init(audio_out.sample_rate, audio_out.nchannel, audio_out.sample_bits/8, 0);
        //TODO: start all media tracks
        _bAudioCompleted = _bVideoCompleted = false;
        // initial and start decoders
        MediaSegmentManager::start();

        _state = SessionState::Prepared;
        callback_->onPrepared(RetCode::ok);
        return RetCode::ok;
    });
}

RetCode MediaSessionImpl::__start() {
    assert(thread_task_.IsCurrent());
    MPST_RET_IF_EQ(_state, SessionState::Idle);
//    MPST_RET_IF_EQ(_state, SessionState::Initialized);
    MPST_RET_IF_EQ(_state, SessionState::AsyncPreparing);
    // MPST_RET_IF_EQ(_state, PlayerState_Prepared);
    MPST_RET_IF_EQ(_state, SessionState::Started);
    // MPST_RET_IF_EQ(_state, PlayerState_Paused);
    // MPST_RET_IF_EQ(_state, PlayerState_Completed);
//    MPST_RET_IF_EQ(_state, SessionState::Stopped);
    MPST_RET_IF_EQ(_state, SessionState::Error);
    MPST_RET_IF_EQ(_state, SessionState::Ended);
    
    if (video_loop_) video_loop_->start();
    if (audio_loop_) audio_loop_->audioStart();
    _state = SessionState::Started;
    return RetCode::ok;
}

RetCode MediaSessionImpl::__play()
{
    RetCode ret = RetCode::e_state;
    if (_state == SessionState::Started || _state == SessionState::Paused) {
        ret = __pause(false);
    }else {
        
        if(RetCode::ok == ( ret = __start())) {
            _playerClock.SetPaused(false);
            user_paused_ = false;
        }
    }
    return ret;
}

RetCode MediaSessionImpl::__seek(int64_t time_ms, int cnt, int flag)
{
    t_lock_guard<ticket_lock> clk(_render_mutex);
    MPST_RET_IF_EQ(_state, SessionState::Idle);
    MPST_RET_IF_EQ(_state, SessionState::Initialized);
    MPST_RET_IF_EQ(_state, SessionState::AsyncPreparing);
    // MPST_RET_IF_EQ(_state, PlayerState_Prepared);
    // MPST_RET_IF_EQ(_state, PlayerState_Started);
    // MPST_RET_IF_EQ(_state, PlayerState_Paused);
    // MPST_RET_IF_EQ(_state, PlayerState_Completed);
    MPST_RET_IF_EQ(_state, SessionState::Stopped);
    MPST_RET_IF_EQ(_state, SessionState::Error);
    MPST_RET_IF_EQ(_state, SessionState::Ended);
    
    _playerClock.SetPaused(true);
//    _videoTarget->pause(true);
    audio_loop_->audioPause(true);
    audio_loop_->audioFlush();
    video_loop_->forceUpdate();
    
    _playerClock.SetClock(time_ms);
    _audioClock.setClock(time_ms);
    //---------
//    setPositionTo(time_ms, flag);
    _bAudioCompleted = false;
    _bVideoCompleted = false;
    media_complete_flag_ = 0;
    
    MediaSegmentManager::setPositionTo(time_ms, flag == 1);
    for (auto& ac : audio_nodes_) {
        ((AudioRenderNodeImpl*)ac.get())->ClearAudioFrame();
    }
    //---------
    if(_state == SessionState::Completed) {
        _state = SessionState::Paused;
        user_paused_ = true;
    }
    if (!user_paused_) {
        video_loop_->pause(false);
        audio_loop_->audioPause(false);
        _playerClock.SetPaused(false);
    }
    if (_seekIdx.load() == cnt) { //clear seeking state
        _bSeeking.store(false);
    }
//    _callback->onSeekTo(mSec);
    return RetCode::ok;
}

RetCode MediaSessionImpl::__pause(bool bPause)
{
    MPST_RET_IF_EQ(_state, SessionState::Idle);
    MPST_RET_IF_EQ(_state, SessionState::Initialized);
    MPST_RET_IF_EQ(_state, SessionState::AsyncPreparing);
//    MPST_RET_IF_EQ(_state, PlayerState::Prepared);
    // MPST_RET_IF_EQ(_state, PlayerState_Started);
//    MPST_RET_IF_EQ(_state, PlayerState::Paused);
    MPST_RET_IF_EQ(_state, SessionState::Completed);
    MPST_RET_IF_EQ(_state, SessionState::Stopped);
    MPST_RET_IF_EQ(_state, SessionState::Error);
    MPST_RET_IF_EQ(_state, SessionState::Ended);
    
    if ((_state == SessionState::Paused && bPause) || (_state == SessionState::Started && !bPause)) {
        return RetCode::ok;
    }
    user_paused_ = bPause;
    _playerClock.SetPaused(bPause);
    video_loop_->pause(bPause);
    audio_loop_->audioPause(bPause);
    _state = bPause? SessionState::Paused : SessionState::Started;
    return RetCode::ok;
}

RetCode MediaSessionImpl::__stop() {
    MPST_RET_IF_EQ(_state, SessionState::Idle);
    MPST_RET_IF_EQ(_state, SessionState::Initialized);
    // MPST_RET_IF_EQ(_state, PlayerState_Prepared);
//    MPST_RET_IF_EQ(_state, PlayerState::Started);
    // MPST_RET_IF_EQ(_state, PlayerState_Paused);
    // MPST_RET_IF_EQ(_state, PlayerState_Completed);
    MPST_RET_IF_EQ(_state, SessionState::Stopped);
    MPST_RET_IF_EQ(_state, SessionState::Error);
    MPST_RET_IF_EQ(_state, SessionState::Ended);
    
    if (video_loop_) video_loop_->stop();
    if (audio_loop_) audio_loop_->audioStop();
    //stop all media tracks
    MediaSegmentManager::stop();
    
    _state = SessionState::Stopped;
    return RetCode::ok;
}

//void MediaSessionImpl::start() {
//    thread_task_.PostTask(CMD_START, [this](){
//        _callback->onStarted(__start());
//    });
//}
//
void MediaSessionImpl::stop() {
    thread_task_.PostTask(CMD_STOP, [this](){
        callback_->onStoped(__stop());
    });
}

void MediaSessionImpl::seek(int64_t time_ms, int32_t flag) {
    thread_task_.RemoveTask(CMD_SEEK);
    _bSeeking.store(true);
    _seekIdx ++;
    render_position_ = time_ms;
    thread_task_.PostTask(CMD_SEEK, &MediaSessionImpl::__seek, this, time_ms, _seekIdx.load(), flag);
}

void MediaSessionImpl::__addVideoRenderNode(std::shared_ptr<VideoRenderNode> video_render_node) {
    postRenderTask([this](std::shared_ptr<VideoRenderNode> video_render_node){
        display_layer_->addChildren((VideoGraphicNode*)video_render_node.get());
    }, video_render_node);
}
void MediaSessionImpl::__removeVideoRenderNode(std::shared_ptr<VideoRenderNode> video_render_node) {
    postRenderTask([this](std::shared_ptr<VideoRenderNode> video_render_node){
        display_layer_->removeChildren((VideoGraphicNode*)video_render_node.get());
    }, video_render_node);
}

void MediaSessionImpl::__addAudioRenderNode(std::shared_ptr<AudioRenderNode> audio_render_node) {
    postAudioTask([this](std::shared_ptr<AudioRenderNode> audio_render_node){
        audio_nodes_.push_back(audio_render_node);
    }, audio_render_node);
}
void MediaSessionImpl::__removeAudioRenderNode(std::shared_ptr<AudioRenderNode> audio_render_node) {
    postAudioTask([this](std::shared_ptr<AudioRenderNode> audio_render_node){
        auto iter = std::find(audio_nodes_.begin(), audio_nodes_.end(), audio_render_node);
        if (iter != audio_nodes_.end()) {
            audio_nodes_.erase(iter);
        }
    }, audio_render_node);
}

// run on render thread
bool MediaSessionImpl::onRenderDestroy()
{
    runRenderCmd();
    display_layer_->releaseRes();
    return true;
}

void MediaSessionImpl::OnViewSizeChange(int32_t width, int32_t height) {
    //TODO: init render resource
    display_layer_->setTargetSize(width, height);
    postRenderTask([this](){
        display_layer_->beginRender();
    });
    video_loop_->forceUpdate();
}

void MediaSessionImpl::setDisplayMode(DisplayMode mode, bool filp_v) {
    display_layer_->setDisplayMode(mode, filp_v);
}

bool MediaSessionImpl::onVideoRender(int64_t wantTimeMs, bool no_display) {
    runRenderCmd();
    
    if (_bVideoCompleted) {
        return false;
    }
    
    t_lock_guard<ticket_lock> clk(_render_mutex);
    int64_t position = wantTimeMs == -1? _playerClock.GetClock() : wantTimeMs;
    if (! _bSeeking.load() ) {
        render_position_ = position;
        int64_t currentRenderTime = SystemClock::getCurrentTime<int64_t,scale_milliseconds>();
        if (abs(last_renderTime_ - currentRenderTime) > 100 && render_position_ > 0) {
            callback_->onProgressUpdated(render_position_);
            last_renderTime_ = currentRenderTime;
        }
    }
     
     if (position >= getTotalTimeRange().end) {
         thread_task_.PostTask(&MediaSessionImpl::onStreamCompleted, this, MediaType::Video);
         _bVideoCompleted = true;
         position = getTotalTimeRange().end;
     }
     
     display_layer_->render(position, no_display);
     
     return true;
}

bool MediaSessionImpl::onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs) {
    runAudioCmd();
    //FIXME: must use TimeMapper to calculate real time stamp after.
    if (wantTimeMs > 0) {
        _audioClock.setClock(wantTimeMs);
    }
    if (_bAudioCompleted) {
        return false;
    }
    return onAudioRender( buffer, needBytes);
}

bool MediaSessionImpl::onAudioRender(uint8_t * const buffer, unsigned needBytes) {
        
    memset(buffer, 0, needBytes);
    
    int64_t audio_pts = _audioClock.getClock();
    int offset = 0;
    while (offset < needBytes) {
        ReadAudioFrames(audio_pts);
        bool hasData = true;
//        int64_t audio_pts = _audioClock.getClock(_audioTarget);
        int minCacheSize = getCacheAudioSample(audio_pts, needBytes - offset, hasData);
        if(minCacheSize > 0){
            //mix the cache audio data
            for (auto &node: audio_nodes_) {
                auto ac = dynamic_cast<AudioRenderNodeImpl*>(node.get());
                const AudioSampleCache* audioCache = ac->getAudioSample(audio_pts);
                if (!audioCache->isReadEnd()) {
                    AudioProcess::mixAudio(buffer + offset, audioCache->getAudioBuf(minCacheSize), minCacheSize, 1.0f);
                }
            }
        }else if (!hasData) {
            //no more audio data to read
            minCacheSize = needBytes - offset;
            memset(buffer + offset, 0, minCacheSize);
        }else {
            //wait for audio pcm data coming
            continue;
        }
        offset += minCacheSize;
        _audioClock.increaseAudioData(minCacheSize);
        audio_pts = _audioClock.getClock();
    }
    
    if (audio_pts >= getTotalTimeRange().end)
    {
        if (!_bAudioCompleted) {
            thread_task_.PostTask(&MediaSessionImpl::onStreamCompleted, this, MediaType::Audio);
            _bAudioCompleted = true;
        }
    }else
        _bAudioCompleted = false;

    return true;
}

MediaSessionImpl::RetCode MediaSessionImpl::onStreamCompleted(MediaType mediaType) {
    assert(thread_task_.IsCurrent());
    callback_->onStreamEnd(mediaType);
    media_complete_flag_ |= (int)mediaType;
    if ((media_complete_flag_ & (int)MediaType::Video) && ((media_complete_flag_ & (int)MediaType::Audio) || !_hasAudio )) {
        callback_->onCompleted(RetCode::ok);
    }
    
    return MediaSessionImpl::ok;
}

int MediaSessionImpl::getCacheAudioSample(uint64_t timeMs, int wantLen, bool& hasData)
{
    int minCacheSize = std::numeric_limits<int>::max();
    hasData = false;
    for (auto& node : audio_nodes_) {
        auto ac = dynamic_cast<AudioRenderNodeImpl*>(node.get());
         const AudioSampleCache* sampleCache = ac->getAudioSample(timeMs);
        if (! sampleCache->isReadEnd()) {
            minCacheSize = MIN(minCacheSize, sampleCache->getAudioLength());
            hasData = true;
        }
    }
    return MIN(minCacheSize, wantLen);
}

void MediaSessionImpl::runRenderCmd()
{
    std::unique_lock<std::mutex> lck(_renderCmdsMutex);
    for (const auto& cmd : _renderCmds) {
        cmd();
    }
    _renderCmds.clear();
}
void MediaSessionImpl::runAudioCmd()
{
    std::unique_lock<std::mutex> lck(_audioCmdsMutex);
    for (const auto& cmd : _audioCmds) {
        cmd();
    }
    _audioCmds.clear();
}

}
}
