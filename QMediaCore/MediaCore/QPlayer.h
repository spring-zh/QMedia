//
//  QPlayer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef MEDIACORE_QPLAYER_H
#define MEDIACORE_QPLAYER_H

#include "core/SteadyClock.h"
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
    
    class Callback
    {
    public:
        virtual void onPrepare(RetCode code) = 0;
//        virtual void onPlayerStateChanged(PlayerState newState, PlayerState oldState) = 0;
        virtual void onProgressUpdated(int64_t mSec) = 0;
        virtual void onSeekTo(int64_t mSec) = 0;
        virtual void onCompleted(RetCode code) = 0;
    };
    
    QPlayer();
    ~QPlayer();
    
    void setCallBack(Callback *callback) { _callback = callback; }
    
    //player control
    void start() {}
    void pause(bool bPause = true) {}
    void seek(int64_t mSec, int flag) {}
    
protected:
    virtual bool onRenderCreate() override;
    virtual bool onVideoRender(int64_t wantTimeMs) override;
    virtual bool onRenderDestroy() override;
    virtual bool onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs) override;

private:

    Callback *_callback;
    SteadyClock<int64_t, scale_milliseconds> _playerClock;
};

#endif /* MEDIACORE_QPLAYER_H */
