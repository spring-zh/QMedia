//
//  EditorPlayer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EditorPlayer.h"
#include "Utils/Logger.h"
#include "MediaCore/core/AudioProcess.h"

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

EditorPlayer::EditorPlayer():
_playerPosition(0),
_callback(nullptr),
_userPaused(false),
_state(PlayerState::Idle)
{
    EffectCombiner::setCallBack(this);
    _bSeeking.store(false);
    _seekIdx.store(1);
}
EditorPlayer::~EditorPlayer()
{
    
}

void EditorPlayer::start()
{
    _threadTask.postTask(&EditorPlayer::_start, this);
}

void EditorPlayer::pause(bool bPause)
{
    _threadTask.postTask(&EditorPlayer::_pause, this, bPause);
}
void EditorPlayer::seek(int64_t mSec, int flag)
{
    _threadTask.removeTaskById(CMD_SEEK);
    _bSeeking.store(true);
    _seekIdx ++;
    _playerPosition = mSec;
    _threadTask.postTask((int)CMD_SEEK, &EditorPlayer::_seek, this, mSec, _seekIdx.load());
}

bool EditorPlayer::onVideoRender(int64_t wantTimeMs)
{
    int64_t current_video_time = _playerClock.getClock();
    if (! _bSeeking.load() ) {
        _playerPosition = current_video_time;
        int64_t currentRenderTime = SystemClock::getCurrentTime<int64_t,scale_milliseconds>();
        if (abs(_lastRenderTime - currentRenderTime) > 100) {
            _callback->onProgressUpdated(_playerPosition);
            _lastRenderTime = currentRenderTime;
        }
    }
    return EffectCombiner::onVideoRender(current_video_time);
}

bool EditorPlayer::onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs)
{
    return EffectCombiner::onAudioRender(buffer, needBytes);
}

//implement EffectEditorCombiner::Callback
void EditorPlayer::onPrepared(RetCode code)
{
    _state = EffectCombiner::_state;
    _userPaused = true;
    _playerClock.setPaused(true);
    _playerClock.setClock(_playerTimeRange._start);
    _videoTarget->flush();
    _callback->onPrepare(code);
}
void EditorPlayer::onStarted(RetCode code)
{
    _playerClock.setPaused(false);
    _callback->onPlayerStateChanged(EffectCombiner::_state, _state);
    _state = EffectCombiner::_state;
}
void EditorPlayer::onStoped(RetCode code)
{
    _callback->onPlayerStateChanged(EffectCombiner::_state, _state);
    _state = EffectCombiner::_state;
}
void EditorPlayer::onStreamEnd(MediaType mediaType)
{
    if (mediaType == MediaType::Video) {
        _videoTarget->pause(true);
    }else if (mediaType == MediaType::Audio) {
        _audioTarget->pause(true);
    }
}
void EditorPlayer::onCompleted()
{
    _state = EffectCombiner::_state;
    _callback->onCompleted();
}

RetCode EditorPlayer::_start()
{
    RetCode ret = RetCode::e_state;
    if (_state == PlayerState::Started || _state == PlayerState::Paused) {
        ret = _pause(false);
    }else {
        
        if(RetCode::ok == ( ret = EffectCombiner::_start())) {
            _playerClock.setPaused(false);
            _userPaused = false;
            _state = EffectCombiner::_state;
        }
    }
    return ret;
}

RetCode EditorPlayer::_pause(bool bPause)
{
    MPST_RET_IF_EQ(_state, PlayerState::Idle);
    MPST_RET_IF_EQ(_state, PlayerState::Initialized);
    MPST_RET_IF_EQ(_state, PlayerState::AsyncPreparing);
//    MPST_RET_IF_EQ(_state, PlayerState::Prepared);
    // MPST_RET_IF_EQ(_state, PlayerState_Started);
//    MPST_RET_IF_EQ(_state, PlayerState::Paused);
    MPST_RET_IF_EQ(_state, PlayerState::Completed);
    MPST_RET_IF_EQ(_state, PlayerState::Stopped);
    MPST_RET_IF_EQ(_state, PlayerState::Error);
    MPST_RET_IF_EQ(_state, PlayerState::Ended);
    
    if ((_state == PlayerState::Paused && bPause) || (_state == PlayerState::Started && !bPause)) {
        return RetCode::ok;
    }
    _userPaused = bPause;
    _playerClock.setPaused(bPause);
    _videoTarget->pause(bPause);
    _audioTarget->pause(bPause);
    _state = bPause? PlayerState::Paused : PlayerState::Started;
    return RetCode::ok;
}
RetCode EditorPlayer::_seek(int64_t mSec, int flag)
{
    MPST_RET_IF_EQ(_state, PlayerState::Idle);
    MPST_RET_IF_EQ(_state, PlayerState::Initialized);
    MPST_RET_IF_EQ(_state, PlayerState::AsyncPreparing);
    // MPST_RET_IF_EQ(_state, PlayerState_Prepared);
    // MPST_RET_IF_EQ(_state, PlayerState_Started);
    // MPST_RET_IF_EQ(_state, PlayerState_Paused);
    // MPST_RET_IF_EQ(_state, PlayerState_Completed);
    MPST_RET_IF_EQ(_state, PlayerState::Stopped);
    MPST_RET_IF_EQ(_state, PlayerState::Error);
    MPST_RET_IF_EQ(_state, PlayerState::Ended);
    
    _bAudioCompleted = _bVideoCompleted = false;
    
    _playerClock.setPaused(true);
//    _videoTarget->pause(true);
    _audioTarget->pause(true);
    _audioTarget->flush();
    _videoTarget->flush();
    
    _playerClock.setClock(mSec);
    _audioClock.setClock(mSec);
    MediaSupervisor::setPositionTo(mSec);
    if(_state == PlayerState::Completed) {
        _state = PlayerState::Paused;
        _userPaused = true;
    }
    if (!_userPaused) {
        _videoTarget->pause(false);
        _audioTarget->pause(false);
        _playerClock.setPaused(false);
    }
    
    if (_seekIdx.load() == flag) { //clear seeking state
        _bSeeking.store(false);
    }

    return RetCode::ok;
}
