//
//  EffectCombiner.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIASESSION_IMPL_H
#define EFFECTEDITOR_MEDIASESSION_IMPL_H

#include "Utils/ThreadTaskFuture.h"
#include "MediaCore/core/SteadyClock.h"
#include "MediaCore/audiocore/AudioClock.h"
#include "MediaSegmentManager.h"
#include "DisplayLayer.h"

#include "audio_runloop.h"
#include "video_runloop.h"

#include "size.h"
#include "vec4f.h"
#include "media_range.h"
#include "video_render.h"
#include "audio_render.h"
#include "media_session_internal.h"


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

enum AsyncCommand : int{
    CMD_UNKNOW = -1,
    CMD_PREPARE = 1,
    CMD_START,
    CMD_STOP,
    CMD_PAUSE,
    CMD_SEEK,
    CMD_ADD_SEGMENT,
    CMD_REMOVE_SEGMENT,
    CMD_COMPLETED
};

enum class SessionState : int{
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

class DisplayLayer;

class MediaSessionImpl : public MediaSegmentManager,
                         public MediaSessionInternal {
public:
    
    enum RetCode : int{
        unknow = -100,
        e_state = -2,
        e_system = -1,
        ok = 0
    };
    
    class Callback {
    public:
        virtual void onPrepared(RetCode code) = 0;
//        virtual void onStarted(RetCode code) = 0;
        virtual void onStoped(RetCode code) = 0;
        virtual void onStreamEnd(MediaType mediaType) = 0;
        virtual void onCompleted(RetCode code) = 0;
    };
    
    MediaSessionImpl();
    explicit MediaSessionImpl(std::shared_ptr<VideoRender> video_render, std::shared_ptr<AudioRender> audio_render);
    
    virtual ~MediaSessionImpl();
                             
    void setCallback(Callback* callback) { _callback = callback; }
    
    std::shared_ptr<MediaSegment> cresteMediaSegment(const std::string & filename, int32_t flag) override;

    bool addMediaSegment(const std::shared_ptr<MediaSegment> & segment) override;

    bool removeMediaSegment(const std::shared_ptr<MediaSegment> & segment) override;
                             
    std::vector<std::shared_ptr<MediaSegment>> getSegments() override { return source_segments_;}

    MediaRange getTotalTimeRange() override;

    std::shared_ptr<VideoRender> getVideoRender() override;

    std::shared_ptr<AudioRender> getAudioRender() override;
                             
    void setAudioRunLoop(const std::shared_ptr<AudioRunloop> & audio_loop) override;

    void setVideoRunLoop(const std::shared_ptr<VideoRunloop> & video_loop) override;

    void setDisplayLayerSize(const Size & size) override;

    Size getDisplayLayerSize() override;

    void setBkColor(const Vec4f & color) override;

    Vec4f getBkColor() override;

    /** control */
    virtual void prepare() override;

//    virtual void start() override;
//                             
    virtual void stop() override;
    
                             
    void setPositionTo(int64_t time_ms, int flag);
                             
    bool onVideoRender(int64_t wantTimeMs);
    bool onRenderDestroy();
    void OnViewSizeChange(int32_t width, int32_t height);
    void setDisplayMode(DisplayMode mode, bool filp_v);
                             
    DisplayLayer *GetDisplayLayer() const { return display_layer_.get(); }
protected:
                             
    friend class EditorPlayerImpl;
    friend class EditorExporterImpl;
    
    bool onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs);
    
    bool onAudioRender(uint8_t * const buffer, unsigned needBytes);
    RetCode onStreamCompleted(MediaType mediaType);
    
    int getCacheAudioSample(uint64_t timeMs, int wantLen, bool& hasData);
        
    void __addVideoRenderNode(std::shared_ptr<VideoRenderNode> video_render_node);
    void __removeVideoRenderNode(std::shared_ptr<VideoRenderNode> video_render_node);
    
    void __addAudioRenderNode(std::shared_ptr<AudioRenderNode> audio_render_node);
    void __removeAudioRenderNode(std::shared_ptr<AudioRenderNode> audio_render_node);
        
    RetCode __start();
    RetCode __stop();
    
    //output target pointer
    VideoDescribe _videoConfig;
    AudioDescribe _audioConfig;
    std::shared_ptr<VideoRender> video_render_;
    std::shared_ptr<AudioRender> audio_render_;
    std::shared_ptr<AudioRunloop> audio_loop_;
    std::shared_ptr<VideoRunloop> video_loop_;
    std::unique_ptr<DisplayLayer> display_layer_;
                             
    std::list<std::shared_ptr<AudioRenderNode>> audio_nodes_;
    std::vector<std::shared_ptr<MediaSegment>> source_segments_;
    
    bool _bVideoCompleted;
    bool _bAudioCompleted;
    int media_complete_flag_{0};
//    int64_t _playerPosition;
    Callback *_callback;
    
    SessionState _state;

    AudioClock _audioClock;
    bool _hasAudio;
    
    //push task to render thread
    template <typename Fn, class... Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    void postRenderTask(Fn fn, Args&&... args){
        std::unique_lock<std::mutex> lck(_renderCmdsMutex);
        _renderCmds.push_back(std::bind(fn, std::forward<Args>(args)...));
    }
    
    //push task to audio callback thread
    template <typename Fn, class... Args, typename RetT = typename std::result_of<Fn(Args...)>::type>
    void postAudioTask(Fn fn, Args&&... args){
        std::unique_lock<std::mutex> lck(_audioCmdsMutex);
        _audioCmds.push_back(std::bind(fn, std::forward<Args>(args)...));
    }
    
    void runRenderCmd();
    void runAudioCmd();
    
    //the thread for handle sync commands
    ThreadTaskFuture thread_task_;
    
    typedef std::function<void(void)> AsyncRenderCmd;
    std::mutex _renderCmdsMutex;
    std::mutex _audioCmdsMutex;
    std::list<AsyncRenderCmd> _renderCmds; //run on render thread
    std::list<AsyncRenderCmd> _audioCmds; //run on audio callback
};

using RetCode = MediaSessionImpl::RetCode;

}
}

#endif /* EFFECTEDITOR_MEDIASESSION_IMPL_H */
