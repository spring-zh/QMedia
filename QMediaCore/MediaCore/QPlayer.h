//
//  QPlayer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef MEDIACORE_QPLAYER_H
#define MEDIACORE_QPLAYER_H

#include "Utils/ThreadTask.h"
#include "core/SteadyClock.h"
#include "MediaCore/audiocore/AudioClock.h"
#include "decoder/VideoDecoder.h"
#include "decoder/AudioDecoder.h"
#include "output/VideoTarget.h"
#include "output/AudioTarget.h"

class QPlayer : public VideoRender, public AudioRender{
public:
    
    enum RetCode : int{
        unknow = -100,
        e_state = -2,
        e_system = -1,
        ok = 0
    };
    
    enum State : int{
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
    
    class Callback
    {
    public:
        virtual void onPrepare(RetCode code) = 0;
//        virtual void onPlayerStateChanged(PlayerState newState, PlayerState oldState) = 0;
        virtual void onProgressUpdated(int64_t mSec) = 0;
        virtual void onSeekTo(int64_t mSec) = 0;
        virtual void onCompleted(RetCode code) = 0;
    };
    
    QPlayer(VideoDecoderFactory* vdFactory, AudioDecoderFactory* adFactory);
    ~QPlayer();
    
    void setCallBack(Callback *callback) { _callback = callback; }
    void setVideoTarget(VideoTarget* videoTarget);
    void setAudioTarget(AudioTarget* audioTarget);
    
    //player control
    RetCode open(std::string url);
    RetCode play();
    RetCode pause();
    RetCode seek(int64_t mSec, int flag);
    RetCode close();
    
protected:
    virtual bool onRenderCreate() override;
    virtual bool onVideoRender(int64_t wantTimeMs) override;
    virtual bool onRenderDestroy() override;
    virtual bool onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs) override;

private:

    State _state;
    Callback *_callback;
    //output target pointer
    VideoDescribe _videoConfig;
    AudioDescribe _audioConfig;
    VideoTarget* _videoTarget;
    AudioTarget* _audioTarget;
    
    SteadyClock<int64_t, scale_milliseconds> _playerClock;
    AudioClock _audioClock;
    
    //the thread for handle sync commands
    ThreadTask<RetCode> _threadTask;
};

#endif /* MEDIACORE_QPLAYER_H */
