//
//  EffectCombiner.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_EFFECTEDITORCOMBINER_H
#define EFFECTEDITOR_EFFECTEDITORCOMBINER_H

#include "Utils/ThreadTask.h"
#include "GraphicCore/GcLayer.h"
#include "MediaCore/core/SteadyClock.h"
#include "MediaCore/core/AudioProcess.h"
#include "MediaCore/output/VideoTarget.h"
#include "MediaCore/output/AudioTarget.h"
#include "MediaSupervisor.h"

class AudioClock {
public:
    AudioClock():_audioPosition(0),_writeAudioBytes(0) {}
    ~AudioClock() {}
    
    void init(AudioTarget *at) {
        _sampleRate = at->getSampleRate();
        _channels = at->getChannels();
        _sampleWidthBytes = getBytesFromPcmFormat(at->getFormat());
        _audoDelay = at->getAudioDelayBytes();
    }
    
    void increaseAudioData(unsigned int size) {
        t_lock_guard<spin_mutex> lck(_spin);
        _writeAudioBytes += size;
    }
    
    void setClock(int64_t clock) {
        t_lock_guard<spin_mutex> lck(_spin);
        _audioPosition = clock;
        _writeAudioBytes = 0;
    }
    int64_t getClock() const {
        int64_t play_duration = AudioProcess::calculateAudioDurationByBytes(_sampleRate,_channels,_sampleWidthBytes, _writeAudioBytes);
        return _audioPosition + play_duration;
    }
    
    int64_t getPlayingClock() const {
        int64_t posBytes = MAX(0, _writeAudioBytes - _audoDelay);
        int64_t play_duration = AudioProcess::calculateAudioDurationByBytes(_sampleRate,_channels,_sampleWidthBytes, posBytes);
        return _audioPosition + play_duration;
    }
    
private:
    mutable spin_mutex _spin;
    int64_t _audioPosition;
    int64_t _writeAudioBytes;
//    bool _bUpdate;
    int _sampleRate;
    int _channels;
    int _sampleWidthBytes;
    int _audoDelay;
};


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

enum AsyncCommand : int{
    CMD_UNKNOW = -1,
    CMD_PREPARE = 1,
    CMD_START,
    CMD_STOP,
    CMD_PAUSE,
    CMD_SEEK,
    CMD_ADDTRACK,
    CMD_REMOVETRACK,
    CMD_COMPLETED
};

enum class CombinerState : int{
    Idle = 0,
    Initialized,
    AsyncPreparing,
    Prepared,
    Started,
    Paused,
    Completed,
    Stopped,
    Error,
    Ended
};

class EffectCombiner : public MediaSupervisor, public VideoRender, public AudioRender{
public:
    
    enum RetCode : int{
        unknow = -100,
        e_state = -2,
        e_system = -1,
        ok = 0
    };
    
    class Callback
    {
    public:
        virtual void onPrepared(RetCode code) = 0;
        virtual void onStarted(RetCode code) = 0;
        virtual void onStoped(RetCode code) = 0;
        virtual void onStreamEnd(MediaType mediaType) = 0;
        virtual void onCompleted() = 0;
    };
    
    EffectCombiner();
    ~EffectCombiner();
    
    void setVideoTarget(VideoTarget* videoTarget);
    void setAudioTarget(AudioTarget* audioTarget);
    void setCallBack(Callback *callback) { _callback = callback; }
    
    GraphicCore::RenderNodeRef getRootNode() const { return _displayLayer; }

    /**
     * set the output traget config what you want .
     * after targets init call , targets will adjustive by the config what you set
     */
    void setvideoConfig(VideoDescribe vconfig) { _videoConfig = vconfig; }
    void setAudioConfig(AudioDescribe aconfig) { _audioConfig = aconfig; }
    VideoTarget* getVideoTarget() const { return _videoTarget; }
    AudioTarget* getAudioTarget() const { return _audioTarget; }
    
    //player control
    virtual void prepare();
    virtual void start();
    virtual void stop();
    
    //media manage
    void addMediaTrack(MediaTrackRef mediaTrack);
    void removeMediaTrack(MediaTrackRef mediaTrack);
    void attachRenderNode(GraphicCore::RenderNodeRef child, GraphicCore::RenderNodeRef parent);
    void detachRenderNode(GraphicCore::RenderNodeRef current);
    
protected:
    class RenderLayer : public GraphicCore::RenderNode {
    public:
        RenderLayer(EffectCombiner *combiner);
        ~RenderLayer(){}
        
        const Range<int64_t> getRange() override;
        
        void releaseRes() override;
        
        bool beginRender();
        
        void render(int64_t timeStamp);
        void draw(GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, uint32_t /*flags*/) override;
        
        EffectCombiner *_combiner;
        GraphicCore::Scene _playerScene;
        GraphicCore::GLEngine _gle;
        bool _isInit;
    };
    
    virtual bool onRenderCreate() override;
    virtual bool onVideoRender(int64_t wantTimeMs) override;
    virtual bool onRenderDestroy() override;
    virtual bool onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs) override;
    
    bool onAudioRender(uint8_t * const buffer, unsigned needBytes);
    RetCode onStreamCompleted(MediaType mediaType);
    
    int getCacheAudioSample(uint64_t timeMs, int wantLen, bool& hasData);
        
    RetCode _prepare();
    RetCode _start();
    RetCode _stop();
    //media manage
    RetCode _addMediaTrack(MediaTrackRef mediaTrack);
    RetCode _removeMediaTrack(MediaTrackRef mediaTrack);
        
    void _addMediaGraphicChannel(MediaGraphicChannelRef graphicChannel);
    void _removeMediaGraphicChannel(MediaGraphicChannelRef graphicChannel);
    void _addMediaAudioChannel(MediaAudioChannelRef audioChannel);
    void _removeMediaAudioChannel(MediaAudioChannelRef audioChannel);
    
    void _attachRenderNode(GraphicCore::RenderNodeRef child, GraphicCore::RenderNodeRef parent);
    void _detachRenderNode(GraphicCore::RenderNodeRef current);
        
    void cmdCallback(RetCode,int);
    
    //output target pointer
    VideoDescribe _videoConfig;
    AudioDescribe _audioConfig;
    VideoTarget* _videoTarget;
    AudioTarget* _audioTarget;
    std::shared_ptr<RenderLayer> _displayLayer;
//    VideoTarget::Resolution _videoResolution;
    
    bool _bVideoCompleted;
    bool _bAudioCompleted;
//    int64_t _playerPosition;
    Callback *_callback;
    
    CombinerState _state;

    AudioClock _audioClock;
    bool _hasAudio;
    
    //push task to render thread
    template <class func_ptr,class... T>
    void postRenderTask(func_ptr func, T... args){
        std::unique_lock<std::mutex> lck(_renderCmdsMutex);
        _renderCmds.push_back(std::bind(func, args...));
    }
    
    //push task to audio callback thread
    template <class func_ptr,class... T>
    void postAudioTask(func_ptr func, T... args){
        std::unique_lock<std::mutex> lck(_audioCmdsMutex);
        _audioCmds.push_back(std::bind(func, args...));
    }
    
    void runRenderCmd();
    void runAudioCmd();
    
    //the thread for handle sync commands
    ThreadTask<RetCode> _threadTask;
    
    typedef std::function<void(void)> AsyncRenderCmd;
    std::mutex _renderCmdsMutex;
    std::mutex _audioCmdsMutex;
    std::list<AsyncRenderCmd> _renderCmds; //run on render thread
    std::list<AsyncRenderCmd> _audioCmds; //run on audio callback
};

using RetCode = EffectCombiner::RetCode;

#endif /* EFFECTEDITOR_EFFECTEDITORCOMBINER_H */
