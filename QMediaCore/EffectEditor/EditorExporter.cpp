//
//  EditorExporter.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EditorExporter.h"

//#define CMD_CANCEL 100

EditorExporter::EditorExporter():_callback(nullptr),_renderPosition(0),_lastRenderTime(0)
{
    EffectCombiner::setCallBack(this);
}
EditorExporter::~EditorExporter()
{
    
}

void EditorExporter::start()
{
    EffectCombiner::prepare();
}
void EditorExporter::stop()
{
    EffectCombiner::stop();
}

void EditorExporter::cancel()
{
    _threadTask.PostTask(&EditorExporter::_cancel, this);
}
int64_t EditorExporter::getPosition() const {
    return _renderPosition;
}

RetCode EditorExporter::_complete()
{
    EffectCombiner::_stop();
    _state = CombinerState::Completed;
    _callback->onCompleted();
    return RetCode::ok;
}

bool EditorExporter::onVideoRender(int64_t wantTimeMs)
{
    if (_bVideoCompleted) return false;
    
    int64_t currentRenderTime = SystemClock::getCurrentTime<int64_t,scale_milliseconds>();
    if (abs(_lastRenderTime - currentRenderTime) > 100) {
        _callback->onProgressUpdated(_renderPosition);
        _lastRenderTime = currentRenderTime;
    }
    _renderPosition = wantTimeMs;
    return EffectCombiner::onVideoRender(wantTimeMs);
}

bool EditorExporter::onAudioRender(uint8_t *const buffer, unsigned int needBytes, int64_t wantTimeMs)
{
    if (_bAudioCompleted) return false;
    
    return EffectCombiner::onAudioRender(buffer, needBytes, wantTimeMs);
}
    
void EditorExporter::onPrepared(RetCode code)
{
    if (code == RetCode::ok) {
//        _displayLayer->setContentSize(GraphicCore::Size(_videoTarget->getWidth(), _videoTarget->getHeight()));
        _callback->onStarted(_start());
    }else
        _callback->onStarted(code);
}

void EditorExporter::onStarted(RetCode code)
{
    //TODO: onStarted id called in prepare
}

void EditorExporter::onStreamEnd(MediaType mediaType)
{
    if (mediaType == MediaType::Video) {
        _videoTarget->stop();
    }else if (mediaType == MediaType::Audio) {
        _audioTarget->stop();
    }
}

void EditorExporter::onStoped(RetCode code)
{
    _callback->onStoped(code);
}

void EditorExporter::onCompleted()
{
    _threadTask.PostTask(&EditorExporter::_complete, this);
}

EditorExporter::RetCode EditorExporter::_cancel()
{
    RetCode ret = EffectCombiner::_stop();
    _callback->onCanceled(ret);
    return ret;
}
