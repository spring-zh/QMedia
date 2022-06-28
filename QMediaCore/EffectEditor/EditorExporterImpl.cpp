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
session_(new MediaSessionImpl()) {
    session_->setCallback(this);
}
EditorExporterImpl::~EditorExporterImpl() {
    
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

void EditorExporterImpl::onProgressUpdated(int64_t position) {
    callback_->onProgressUpdated(position);
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
