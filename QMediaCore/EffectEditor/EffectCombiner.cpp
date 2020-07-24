//
//  EffectCombiner.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectCombiner.h"
#include "Utils/Logger.h"
#include "MediaCore/audiocore/AudioProcess.h"

#define STATE_ISRUNING(state) (state == CombinerState::Prepared || state == CombinerState::Started || state == CombinerState::Paused )

USING_GRAPHICCORE

GraphicCore::Rect calculateDisplayRegion(DisplayMode mode, int srcW, int srcH, int dstW, int dstH) {
    GraphicCore::Rect dstRegion;
        switch (mode) {
            case DisplayMode::Stretch:
                dstRegion.size.width = dstW;
                dstRegion.size.height = dstH;
                break;
            case DisplayMode::Adaptive:
            {
                float dstRatio = (float) dstW / dstH;
                float srcRatio = (float) srcW / srcH;
                if (srcRatio > dstRatio) {
                    dstRegion.origin.x = 0;
                    dstRegion.size.width = dstW;
                    int newH = (int) (dstW / srcRatio);
                    dstRegion.origin.y = (dstH - newH)/2;
                    dstRegion.size.height = newH;
                } else {
                    dstRegion.origin.y = 0;
                    dstRegion.size.height = dstH;
                    int newW = (int) (dstH * srcRatio);
                    dstRegion.origin.x = (dstW - newW) / 2;
                    dstRegion.size.width = newW;
                }
            }
                break;
            case DisplayMode::Clip:

                break;

        }
        return dstRegion;
    }


DisplayLayer::DisplayLayer(EffectCombiner *combiner):
GraphicCore::Layer(GraphicCore::Size(0,0)),
_combiner(combiner),
_displayMode(DisplayMode::Adaptive)
{
    _playerScene.setGLEngine(&_gle);
    _renderRange = Range<int64_t>(0, std::numeric_limits<int64_t>::max());
    _isInit = false;
}

const Range<int64_t> DisplayLayer::getRange()
{
    return _combiner->getValidTimeRange();
}

void DisplayLayer::releaseRes()
{
    _gle.releaseAll();
    RenderNode::releaseRes();
}

bool DisplayLayer::beginRender()
{
    _gle.useAsDefaultFrameBuffer();
    if (!_isInit) {
        _targetW = _combiner->getVideoTarget()->getWidth();
        _targetH = _combiner->getVideoTarget()->getHeight();
        _layerSize = GraphicCore::Size(_targetW, _targetH);

        createRes();
        _isInit = true;
    }

    return true;
}

void DisplayLayer::setDisplayMode(DisplayMode mode, int viewW, int viewH) {
    _displayMode = mode;
    _viewW = viewW;
    _viewH = viewH;
    GraphicCore::Rect vp(0, 0, _viewW, _viewH);
    _playerScene.setViewPort(vp);
    _playerScene.setProjection(Projection::_2D);
}

void DisplayLayer::render(int64_t timeStamp){
    _playerScene.setDelta(timeStamp);

    GraphicCore::Rect region = calculateDisplayRegion(_displayMode, _targetW, _targetH, _viewW, _viewH);
    setPosition(region.origin);
    setContentSize(region.size);

    Layer::visit(&_playerScene, _playerScene.getMatrix(MATRIX_STACK_MODELVIEW), 0);
}

EffectCombiner::EffectCombiner():
_videoTarget(nullptr),
_audioTarget(nullptr),
_hasAudio(false),
_bVideoCompleted(false),
_bAudioCompleted(false),
_callback(nullptr),
_state(CombinerState::Idle)
{
    _videoConfig.width = 640;
    _videoConfig.height = 480;
    _videoConfig.framerate = 25;
    _videoConfig.codectype = VideoCodecType::kH264;
    _audioConfig.samplerate = 44100;
    _audioConfig.nchannel = 2;
    _audioConfig.bitwidth = 16;
    _audioConfig.codectype = AudioCodecType::kAAC;
    _audioConfig.format = RawAudioFormat::kS16;
    _displayLayer = std::shared_ptr<DisplayLayer>(new DisplayLayer(this));
    _threadTask.setTaskCallback(&EffectCombiner::cmdCallback,this);
}
EffectCombiner::~EffectCombiner()
{
    
}

void EffectCombiner::setVideoTarget(VideoTarget* videoTarget)
{
    _videoTarget = videoTarget;
    _videoTarget->setRender(this);
    if (_videoTarget && _audioTarget) {
        _state = CombinerState::Initialized;
    }
}
void EffectCombiner::setAudioTarget(AudioTarget* audioTarget)
{
    _audioTarget = audioTarget;
    _hasAudio = true;
    _audioTarget->setRender(this);
    if (_videoTarget && _audioTarget) {
        _state = CombinerState::Initialized;
    }
}

void EffectCombiner::cmdCallback(RetCode errcode, int taskId)
{
//    LOGI("Cmd Code:%d Error Code%d",errcode, taskId);
    switch (taskId) {
        case CMD_PREPARE:
            _callback->onPrepared(errcode);
            break;
        case CMD_START:
            _callback->onStarted(errcode);
            break;
        case CMD_STOP:
            _callback->onStoped(errcode);
            break;
//        case CMD_SEEK:
//            break;
//        case CMD_ADDTRACK:
//            break;
//        case CMD_REMOVETRACK:
//            break;
        default:
            break;
    }
}

EffectCombiner::RetCode EffectCombiner::_prepare()
{
//    MPST_RET_IF_EQ(_state, CombinerState::Idle);
//    MPST_RET_IF_EQ(_state, CombinerState::Initialized);
//    MPST_RET_IF_EQ(_state, CombinerState::AsyncPreparing);
    MPST_RET_IF_EQ(_state, CombinerState::Prepared);
    MPST_RET_IF_EQ(_state, CombinerState::Started);
    MPST_RET_IF_EQ(_state, CombinerState::Paused);
    MPST_RET_IF_EQ(_state, CombinerState::Completed);
//    MPST_RET_IF_EQ(_state, CombinerState::Stopped);
    MPST_RET_IF_EQ(_state, CombinerState::Error);
    MPST_RET_IF_EQ(_state, CombinerState::Ended);
    
    //FIXME: defualt dispaylayer's contentsize will set by target' size
    if (_videoTarget) _videoTarget->init(_videoConfig);
    if (_audioTarget) _audioTarget->init(_audioConfig);
    _audioClock.init(_audioTarget);
    
    //TODO: start all media tracks
    _bAudioCompleted = _bVideoCompleted = false;

    MediaSupervisor::start();
    
    _state = CombinerState::Prepared;
    return RetCode::ok;
}

EffectCombiner::RetCode EffectCombiner::_start()
{
    MPST_RET_IF_EQ(_state, CombinerState::Idle);
//    MPST_RET_IF_EQ(_state, PlayerState::Initialized);
    MPST_RET_IF_EQ(_state, CombinerState::AsyncPreparing);
    // MPST_RET_IF_EQ(_state, PlayerState_Prepared);
    MPST_RET_IF_EQ(_state, CombinerState::Started);
    // MPST_RET_IF_EQ(_state, PlayerState_Paused);
    // MPST_RET_IF_EQ(_state, PlayerState_Completed);
//    MPST_RET_IF_EQ(_state, PlayerState::Stopped);
    MPST_RET_IF_EQ(_state, CombinerState::Error);
    MPST_RET_IF_EQ(_state, CombinerState::Ended);
    
    if (_videoTarget) _videoTarget->start();
    if (_audioTarget) _audioTarget->start();
    _state = CombinerState::Started;
    return RetCode::ok;
}
EffectCombiner::RetCode EffectCombiner::_stop()
{
    MPST_RET_IF_EQ(_state, CombinerState::Idle);
    MPST_RET_IF_EQ(_state, CombinerState::Initialized);
    MPST_RET_IF_EQ(_state, CombinerState::AsyncPreparing);
    // MPST_RET_IF_EQ(_state, PlayerState_Prepared);
//    MPST_RET_IF_EQ(_state, PlayerState::Started);
    // MPST_RET_IF_EQ(_state, PlayerState_Paused);
    // MPST_RET_IF_EQ(_state, PlayerState_Completed);
    MPST_RET_IF_EQ(_state, CombinerState::Stopped);
    MPST_RET_IF_EQ(_state, CombinerState::Error);
    MPST_RET_IF_EQ(_state, CombinerState::Ended);
    
    if (_videoTarget) _videoTarget->stop();
    if (_audioTarget) _audioTarget->stop();
    //stop all media tracks
    MediaSupervisor::stop();
    
    _state = CombinerState::Stopped;
    return RetCode::ok;
}

//media manage
EffectCombiner::RetCode EffectCombiner::_addMediaTrack(MediaTrackRef mediaTrack)
{
    MPST_RET_IF_EQ(_state, CombinerState::Idle);
    //    MPST_RET_IF_EQ(_state, PlayerState::Initialized);
    //    MPST_RET_IF_EQ(_state, PlayerState::AsyncPreparing);
    // MPST_RET_IF_EQ(_state, PlayerState_Prepared);
//    MPST_RET_IF_EQ(_state, PlayerState::Started);
    // MPST_RET_IF_EQ(_state, PlayerState_Paused);
    // MPST_RET_IF_EQ(_state, PlayerState_Completed);
    //    MPST_RET_IF_EQ(_state, PlayerState::Stopped);
    MPST_RET_IF_EQ(_state, CombinerState::Error);
    MPST_RET_IF_EQ(_state, CombinerState::Ended);
    
    if ( !hasMediaTrack(mediaTrack) ) {
        if(mediaTrack->isPrepare()){
            _mediaTracks.insert(mediaTrack);

            if (STATE_ISRUNING(_state)) {
                mediaTrack->setPositionTo(0);
            }
        }else
            return RetCode::e_state;
    }
    
    return RetCode::ok;
}
EffectCombiner::RetCode EffectCombiner::_removeMediaTrack(MediaTrackRef mediaTrack)
{
    auto iter = _mediaTracks.find(mediaTrack);
    if( iter != _mediaTracks.end())
    {
        _mediaTracks.erase(iter);
//        postRenderTask(&EffectCombiner::_removeMediaGraphicChannel,this,
//                       mediaTrack->getMediaGraphicChannel());
//        postAudioTask(&EffectCombiner::_removeMediaAudioChannel,this,
//                      mediaTrack->getMediaAudioChannel());
    }
    return RetCode::ok;
}

void EffectCombiner::_addMediaGraphicChannel(MediaGraphicChannelRef graphicChannel)
{
    if (graphicChannel) {
        _graphicChannels.push_back(graphicChannel);
        //TODO: Render Node compose use by customer
//        _displayLayer->addChildren(graphicChannel.get());
    }
}
void EffectCombiner::_removeMediaGraphicChannel(MediaGraphicChannelRef graphicChannel)
{
    if (graphicChannel) {
        _displayLayer->removeChildren(graphicChannel.get());
        _graphicChannels.remove(graphicChannel);
    }
}
void EffectCombiner::_addMediaAudioChannel(MediaAudioChannelRef audioChannel)
{
    if (audioChannel) {
        _audioChannels.push_back(audioChannel);
    }
}
void EffectCombiner::_removeMediaAudioChannel(MediaAudioChannelRef audioChannel)
{
    if (audioChannel) {
        _audioChannels.remove(audioChannel);
    }
}

//make sure RenderNode operate run on render thread
void EffectCombiner::_attachRenderNode(GraphicCore::RenderNodeRef child, GraphicCore::RenderNodeRef parent) {
    child->removeFromParent();
    child->setParent(nullptr);
    parent->addChildren(child.get());
    if (_displayLayer->_isInit) {
        child->createRes();
    }
    //TODO: if need ??
//    _graphicChannels.push_back(child);
}

void EffectCombiner::_detachRenderNode(GraphicCore::RenderNodeRef current)
{
    current->removeFromParent();
    current->releaseRes();
}

void EffectCombiner::_attachEffect(GraphicCore::RenderNodeRef renderNode, GraphicCore::EffectRef effect) {
    if (renderNode) {
        effect->load();
        renderNode->addEffect(effect);
    }
}
void EffectCombiner::_detachEffect(GraphicCore::RenderNodeRef renderNode, GraphicCore::EffectRef effect) {
    if (renderNode) {
        renderNode->removeEffect(effect);
        effect->unload();
    }
}

void EffectCombiner::_attachAudioNode(MediaAudioChannelRef child, MediaAudioChannelRef parent)
{
    _addMediaAudioChannel(child);
}
void EffectCombiner::_detachAudioNode(MediaAudioChannelRef current)
{
    _removeMediaAudioChannel(current);
}

bool EffectCombiner::onRenderCreate()
{
    //TODO: init render resource
    return true;
}

bool EffectCombiner::onVideoRender(int64_t wantTimeMs)
{
    //FIXME: must use TimeMapper to calculate real time stamp after.
    
    //TODO: wait for media tracks started ....

    _displayLayer->beginRender();
        
    runRenderCmd();
    
    int64_t position = wantTimeMs;
    
    if (position >= getValidTimeRange()._end)
    {
        if( !_bVideoCompleted) {
            _bVideoCompleted = true;
            _threadTask.postTask(&EffectCombiner::onStreamCompleted, this, MediaType::Video);
        }
        position = getValidTimeRange()._end;
    }else
        _bVideoCompleted = false;
    
    _displayLayer->render(position);
    
    return true;
}

bool EffectCombiner::onRenderDestroy()
{
    runRenderCmd();
    _displayLayer->releaseRes();
    return true;
}

void EffectCombiner::setDisplayMode(DisplayMode mode, int dstW, int dstH)
{
    _displayLayer->setDisplayMode(mode, dstW, dstH);
}

bool EffectCombiner::onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs)
{
    //FIXME: must use TimeMapper to calculate real time stamp after.
    if (wantTimeMs > 0) {
        _audioClock.setClock(wantTimeMs);
    }
    return onAudioRender( buffer, needBytes);
}

bool EffectCombiner::onAudioRender(uint8_t * const buffer, unsigned needBytes) {
    runAudioCmd();
        
    memset(buffer, 0, needBytes);
    int64_t audio_pts = _audioClock.getClock();
    int offset = 0;
    while (offset < needBytes) {
        bool hasData = true;
//        int64_t audio_pts = _audioClock.getClock(_audioTarget);
        int minCacheSize = getCacheAudioSample(audio_pts, needBytes - offset, hasData);
        if(minCacheSize > 0){
            //mix the cache audio data
            for (auto &ac: _audioChannels) {
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
    
    if (audio_pts >= getValidTimeRange()._end)
    {
        if (!_bAudioCompleted) {
            _bAudioCompleted = true;
            _threadTask.postTask(&EffectCombiner::onStreamCompleted, this, MediaType::Audio);
        }
    }else
        _bAudioCompleted = false;
    return true;
}

EffectCombiner::RetCode EffectCombiner::onStreamCompleted(MediaType mediaType) {
//    if (_bVideoCompleted && mediaType == MediaType::Video) {
//        _videoTarget->pause(true);
//        LOGI("onStreamCompleted MediaType Video");
//    }
//    if (_bAudioCompleted && mediaType == MediaType::Audio) {
//        _audioTarget->pause(true);
//        LOGI("onStreamCompleted MediaType Audio");
//    }
    _callback->onStreamEnd(mediaType);
    
    if (_bVideoCompleted && (_bAudioCompleted || !_hasAudio )) {
        _callback->onCompleted();
    }
    
    return EffectCombiner::ok;
}

int EffectCombiner::getCacheAudioSample(uint64_t timeMs, int wantLen, bool& hasData)
{
    int minCacheSize = std::numeric_limits<int>::max();
    hasData = false;
    for (auto& audioChannel : _audioChannels) {
         const AudioSampleCache* sampleCache = audioChannel->getAudioSample(timeMs);
        if (! sampleCache->isReadEnd()) {
            minCacheSize = MIN(minCacheSize, sampleCache->getAudioLength());
            hasData = true;
        }
    }
    return MIN(minCacheSize, wantLen);
}

void EffectCombiner::prepare()
{
    _threadTask.postTask((int)CMD_PREPARE ,&EffectCombiner::_prepare, this);
}

void EffectCombiner::start()
{
    _threadTask.removeTaskById(CMD_STOP);
    _threadTask.postTask((int)CMD_START ,&EffectCombiner::_start, this);
}
void EffectCombiner::stop()
{
    _threadTask.removeTaskById(CMD_START);
    _threadTask.postTask((int)CMD_STOP, &EffectCombiner::_stop, this);
}

Range<int64_t> EffectCombiner::getValidTimeRange() {
    if (! _validTimeRange.isValid())
        _validTimeRange = getMediaTimeRange();
    return _validTimeRange;
}

void EffectCombiner::addMediaTrack(MediaTrackRef mediaTrack)
{
//    _threadTask.removeTaskById(CMD_ADDTRACK);
    _threadTask.postTask((int)CMD_ADDTRACK, &EffectCombiner::_addMediaTrack, this, mediaTrack);
}
void EffectCombiner::removeMediaTrack(MediaTrackRef mediaTrack)
{
//    _threadTask.removeTaskById(CMD_ADDTRACK);
    _threadTask.postTask((int)CMD_REMOVETRACK, &EffectCombiner::_removeMediaTrack, this, mediaTrack);
}

void EffectCombiner::attachRenderNode(GraphicCore::RenderNodeRef child, GraphicCore::RenderNodeRef parent)
{
    postRenderTask(&EffectCombiner::_attachRenderNode, this, child, parent);
}

void EffectCombiner::detachRenderNode(GraphicCore::RenderNodeRef current)
{
    postRenderTask(&EffectCombiner::_detachRenderNode, this, current);
}

void EffectCombiner::topRenderNode(GraphicCore::RenderNodeRef current)
{
    postRenderTask([](GraphicCore::RenderNodeRef current){
        GraphicCore::Node *parent = current->getParent();
        if (parent) {
            current->removeFromParent();
            parent->addChildren(current.get());
        }
    }, current);
}

void EffectCombiner::attachAudioNode(MediaAudioChannelRef child, MediaAudioChannelRef parent) {
    postAudioTask(&EffectCombiner::_attachAudioNode, this, child, parent);
}

void EffectCombiner::detachAudioNode(MediaAudioChannelRef current) {
    postAudioTask(&EffectCombiner::_detachAudioNode, this, current);
}

void EffectCombiner::attachEffect(GraphicCore::RenderNodeRef renderNode, GraphicCore::EffectRef effect) {
    postRenderTask(&EffectCombiner::_attachEffect, this, renderNode, effect);
}
void EffectCombiner::detachEffect(GraphicCore::RenderNodeRef renderNode, GraphicCore::EffectRef effect) {
    postRenderTask(&EffectCombiner::_detachEffect, this, renderNode, effect);
}

void EffectCombiner::runRenderCmd()
{
    std::unique_lock<std::mutex> lck(_renderCmdsMutex);
    for (const auto& cmd : _renderCmds) {
        cmd();
    }
    _renderCmds.clear();
}
void EffectCombiner::runAudioCmd()
{
    std::unique_lock<std::mutex> lck(_audioCmdsMutex);
    for (const auto& cmd : _audioCmds) {
        cmd();
    }
    _audioCmds.clear();
}
