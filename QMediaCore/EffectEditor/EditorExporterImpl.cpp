//
//  EditorExporterImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EditorExporterImpl.h"
#include "Utils/Logger.h"

namespace QMedia { namespace Api {

EditorExporterImpl::EditorExporterImpl():
session_(new MediaSessionImpl(CreateExporterVideoRender(this), CreateExporterAudioRender(this))),
_renderPosition(0),
_state(SessionState::Idle) {
    session_->setCallback(this);
}
EditorExporterImpl::~EditorExporterImpl() {
    
}

int32_t EditorExporterImpl::OnPlayBuffer(uint8_t * const buffer, int32_t size_need, int64_t wantTime) {
    return session_->onAudioRender(buffer, size_need, wantTime);
}

void EditorExporterImpl::setDisplayMode(int mode, bool filp_v) {
    session_->setDisplayMode((DisplayMode)mode, filp_v);
}
void EditorExporterImpl::OnViewSizeChange(int32_t width, int32_t height) {
    session_->OnViewSizeChange(width, height);
}
bool EditorExporterImpl::onDraw(int64_t pirv) {
//    t_lock_guard<ticket_lock> clk(_render_mutex);
    if (session_->_bVideoCompleted) return false;
    _renderPosition = pirv;
    int64_t currentRenderTime = SystemClock::getCurrentTime<int64_t,scale_milliseconds>();
    if (abs(_lastRenderTime - currentRenderTime) > 100) {
        callback_->onProgressUpdated(_renderPosition);
        _lastRenderTime = currentRenderTime;
    }
    return session_->onVideoRender(_renderPosition);
}
void EditorExporterImpl::onViewDestroy() {
    session_->onRenderDestroy();
}

//implement EffectEditorCombiner::Callback
void EditorExporterImpl::onPrepared(RetCode code) {
    if (code == RetCode::ok) {
        callback_->onStarted(session_->__start());
    }else
        callback_->onStarted(code);
}
//void EditorExporterImpl::onStarted(RetCode code) {
////    callback_->onStarted(code);
//    _state = session_->_state;
//}
void EditorExporterImpl::onStoped(RetCode code)
{
    callback_->onStoped();
    _state = session_->_state;
}
void EditorExporterImpl::onStreamEnd(MediaType mediaType)
{
    if (mediaType == MediaType::Video) {
        session_->video_loop_->stop();
    }else if (mediaType == MediaType::Audio) {
        session_->audio_loop_->audioStop();
    }
}
void EditorExporterImpl::onCompleted(RetCode code) {
    session_->_state = SessionState::Completed;
    _state = session_->_state;
    callback_->onCompleted(code);
}

void EditorExporterImpl::start() {
    session_->prepare();
}

void EditorExporterImpl::cancel() {
    session_->thread_task_.PostTask([this](){
        RetCode ret = session_->__stop();
        callback_->onCanceled();
        return ret;
    });
}

}
}
