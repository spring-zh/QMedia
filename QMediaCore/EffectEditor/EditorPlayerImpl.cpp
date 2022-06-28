//
//  EditorPlayerImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EditorPlayerImpl.h"
#include "Utils/Logger.h"

using namespace QMedia::RenderEngine;

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
session_(new MediaSessionImpl()) {
    session_->setCallback(this);
}
EditorPlayerImpl::~EditorPlayerImpl() {
    
}

//implement EffectEditorCombiner::Callback
void EditorPlayerImpl::onPrepared(RetCode code) {
    session_->user_paused_ = true;
    session_->_playerClock.SetPaused(true);
    session_->_playerClock.SetClock(session_->getTotalTimeRange().start);
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
void EditorPlayerImpl::onStoped(RetCode code) {
    callback_->onStoped(code);
}
void EditorPlayerImpl::onStreamEnd(MediaType mediaType)
{
    if (mediaType == MediaType::Video) {
        session_->video_loop_->pause(true);
    }else if (mediaType == MediaType::Audio) {
        session_->audio_loop_->audioPause(true);
    }
}
void EditorPlayerImpl::onCompleted(RetCode code) {
    callback_->onCompleted(code);
}

void EditorPlayerImpl::onProgressUpdated(int64_t position) {
    callback_->onProgressUpdated(position);
}

int64_t EditorPlayerImpl::getPosition() {
    return session_->render_position_;
}

void EditorPlayerImpl::play() {
    session_->thread_task_.PostTask(CMD_START, [this](){
        callback_->onStarted(session_->__play());
    });
}

void EditorPlayerImpl::pause() {
    session_->thread_task_.PostTask(CMD_PAUSE, &MediaSessionImpl::__pause, session_, true);
}

void EditorPlayerImpl::seek(int64_t time_ms, int32_t flag) {
    session_->seek(time_ms, flag);
}

}
}
