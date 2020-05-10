//
//  EffectCombiner.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectCombiner.h"
#include "Utils/Logger.h"
#include "MediaCore/core/AudioProcess.h"

USING_GRAPHICCORE

EffectCombiner::RenderLayer::RenderLayer(EffectCombiner *combiner):_combiner(combiner){
    _playerScene.setGLEngine(&_gle);
    _isInit = false;
}

const Range<int64_t> EffectCombiner::RenderLayer::getRange()
{
    return _combiner->getMediaTimeRange();
}

void EffectCombiner::RenderLayer::releaseRes()
{
    _gle.releaseAll();
    RenderNode::releaseRes();
}

bool EffectCombiner::RenderLayer::beginRender()
{
    _gle.useAsDefaultFrameBuffer();
    
    if (!_isInit) {
        GraphicCore::Rect vp(0, 0, _combiner->getVideoTarget()->getWidth(), _combiner->getVideoTarget()->getHeight());
        _playerScene.setViewPort(vp);
        _playerScene.setProjection(Projection::_3D);
        
        createRes();
        _isInit = true;
    }
    return true;
}

void EffectCombiner::RenderLayer::render(int64_t timeStamp){
    _playerScene.setDelta(timeStamp);
    
    AnimaNode::updateAnimations(timeStamp);
    RenderNode::visit(&_playerScene, _playerScene.getMatrix(MATRIX_STACK_MODELVIEW), 0);
}

void EffectCombiner::RenderLayer::draw(GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, uint32_t /*flags*/){
    //draw background color
    //FIXME: ?????
    _gle.getCurrentFrameBuffer()->use();
    _gle.setClearColor(_realColor);
    _gle.clearByColor();
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
    _audioConfig.sampleFmt = RawAudioFormat::kS16;
    _displayLayer = std::shared_ptr<RenderLayer>(new RenderLayer(this));
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
//    _videoConfig._width = _displayLayer->getContentSize().width;
//    _videoConfig._height = _displayLayer->getContentSize().height;
//    videoResolution.width = _videoConfig._width;
//    videoResolution.height = _videoConfig._height;
    if (_videoTarget) _videoTarget->init(_videoConfig);
    if (_audioTarget) _audioTarget->init(_audioConfig);
    _audioClock.init(_audioTarget);
    
    //TODO: start all meida tracks
    _bAudioCompleted = _bVideoCompleted = false;
    _playerTimeRange = getMediaTimeRange();
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
    //stop all meida tracks
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
//            mediaTrack->setVideoTarget(_videoTarget);
//            mediaTrack->setAudioTarget(_audioTarget);
            _mediaTracks.insert(mediaTrack);
//            postRenderTask(&EffectCombiner::_addMediaGraphicChannel,this,
//                           mediaTrack->getMediaGraphicChannel());
            postAudioTask(&EffectCombiner::_addMediaAudioChannel,this,
                          mediaTrack->getMediaAudioChannel());
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
        postAudioTask(&EffectCombiner::_removeMediaAudioChannel,this,
                      mediaTrack->getMediaAudioChannel());
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
    
    if (position >= _playerTimeRange._end)
    {
        if( !_bVideoCompleted) {
            _bVideoCompleted = true;
            _threadTask.postTask(&EffectCombiner::onStreamCompleted, this, MediaType::Video);
        }
        position = _playerTimeRange._end;
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
    
    if (audio_pts >= _playerTimeRange._end)
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
