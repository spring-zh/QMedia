//
//  EditorPlayerImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EditorPlayerImpl.h"
#include "Utils/Logger.h"

USING_GRAPHICCORE

#define MP_RET_IF_FAILED(ret) \
do { \
int retval = ret; \
if (retval != 0) return (retval); \
} while(0)

#define MPST_RET_IF_EQ_INT(real, expected, errcode) \
do { \
if ((real) == (expected)) return (errcode); \
} while(0)

#define MPST_RET_IF_EQ(real, expected) \
MPST_RET_IF_EQ_INT(real, expected, RetCode::e_state)

namespace QMedia { namespace Api {

EditorPlayerImpl::EditorPlayerImpl():
session_(new MediaSessionImpl(CreatePlayerVideoRender(this), CreatePlayerAudioRender(this))),
_playerPosition(0),
_userPaused(false),
_state(SessionState::Idle) {
    session_->setCallback(this);
    _bSeeking.store(false);
    _seekIdx.store(1);
}
EditorPlayerImpl::~EditorPlayerImpl() {
    
}

int32_t EditorPlayerImpl::OnPlayBuffer(uint8_t * const buffer, int32_t size_need, int64_t wantTime) {
    return session_->onAudioRender(buffer, size_need)? 1 : 0;
}

void EditorPlayerImpl::setDisplayMode(int mode, bool filp_v) {
    session_->setDisplayMode((DisplayMode)mode, filp_v);
}
void EditorPlayerImpl::OnViewSizeChange(int32_t width, int32_t height) {
    session_->OnViewSizeChange(width, height);
}
void EditorPlayerImpl::onViewCreate(int32_t width, int32_t height) {
    session_->onRenderCreate();
}
bool EditorPlayerImpl::onDraw(int64_t pirv) {
    t_lock_guard<ticket_lock> clk(_render_mutex);
    int64_t current_video_time = _playerClock.GetClock();
    if (! _bSeeking.load() ) {
        _playerPosition = current_video_time;
        int64_t currentRenderTime = SystemClock::getCurrentTime<int64_t,scale_milliseconds>();
        if (abs(_lastRenderTime - currentRenderTime) > 100 && _playerPosition > 0) {
            callback_->onProgressUpdated(_playerPosition);
            _lastRenderTime = currentRenderTime;
        }
    }

    return session_->onVideoRender(current_video_time);
}
void EditorPlayerImpl::onViewDestroy() {
    session_->onRenderDestroy();
}

//implement EffectEditorCombiner::Callback
void EditorPlayerImpl::onPrepared(RetCode code)
{
    _state = session_->_state;
    _userPaused = true;
    _playerClock.SetPaused(true);
    _playerClock.SetClock(session_->getTotalTimeRange().start);
    session_->video_loop_->forceUpdate();
    
    callback_->onPrepare(code);
}
//void EditorPlayerImpl::onStarted(RetCode code)
//{
//    _playerClock.SetPaused(false);
//    callback_->onStarted(code);
////    callback_->onPlayerStateChanged((int)session_->_state, (int)_state);
//    _state = session_->_state;
//}
void EditorPlayerImpl::onStoped(RetCode code)
{
    callback_->onStoped(code);
//    callback_->onPlayerStateChanged((int)session_->_state, (int)_state);
    _state = session_->_state;
}
void EditorPlayerImpl::onStreamEnd(MediaType mediaType)
{
    if (mediaType == MediaType::Video) {
        session_->video_loop_->pause(true);
    }else if (mediaType == MediaType::Audio) {
        session_->audio_loop_->audioPause(true);
    }
}
void EditorPlayerImpl::onCompleted(RetCode code)
{
    _state = session_->_state;
    callback_->onCompleted(code);
}

void EditorPlayerImpl::play() {
    session_->thread_task_.PostTask(CMD_START, &EditorPlayerImpl::_start, this);
}

void EditorPlayerImpl::pause() {
    session_->thread_task_.PostTask(CMD_PAUSE, &EditorPlayerImpl::_pause, this, true);
}

void EditorPlayerImpl::seek(int64_t time_ms, int32_t flag) {
    session_->thread_task_.RemoveTask(CMD_SEEK);
    _bSeeking.store(true);
    _seekIdx ++;
    _playerPosition = time_ms;
    session_->thread_task_.PostTask(CMD_SEEK, &EditorPlayerImpl::_seek, this, time_ms, _seekIdx.load(), flag);
}


#pragma mark Internal
RetCode EditorPlayerImpl::_start()
{
    RetCode ret = RetCode::e_state;
    if (_state == SessionState::Started || _state == SessionState::Paused) {
        ret = _pause(false);
    }else {
        
        if(RetCode::ok == ( ret = session_->__start())) {
            _playerClock.SetPaused(false);
            _userPaused = false;
            _state = session_->_state;
        }
    }
    callback_->onStarted(ret);
    return ret;
}

RetCode EditorPlayerImpl::_pause(bool bPause)
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
    _userPaused = bPause;
    _playerClock.SetPaused(bPause);
    session_->video_loop_->pause(bPause);
    session_->audio_loop_->audioPause(bPause);
    _state = bPause? SessionState::Paused : SessionState::Started;
    return RetCode::ok;
}
RetCode EditorPlayerImpl::_seek(int64_t mSec, int cnt, int flag)
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
    session_->audio_loop_->audioPause(true);
    session_->audio_loop_->audioFlush();
    session_->video_loop_->forceUpdate();
    
    _playerClock.SetClock(mSec);
    session_->_audioClock.setClock(mSec);
    session_->setPositionTo(mSec, flag);
    if(_state == SessionState::Completed) {
        _state = SessionState::Paused;
        _userPaused = true;
    }
    if (!_userPaused) {
        session_->video_loop_->pause(false);
        session_->audio_loop_->audioPause(false);
        _playerClock.SetPaused(false);
    }
    if (_seekIdx.load() == cnt) { //clear seeking state
        _bSeeking.store(false);
    }
//    _callback->onSeekTo(mSec);
    return RetCode::ok;
}


}
}
