//
//  EditorPlayer.h
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_EDITORPLAYER_H
#define EFFECTEDITOR_EDITORPLAYER_H

#include "EffectCombiner.h"

typedef CombinerState PlayerState;

class EditorPlayer : public EffectCombiner, public EffectCombiner::Callback{
public:
    class Callback
    {
    public:
        virtual void onPrepare(RetCode code) = 0;
        virtual void onPlayerStateChanged(PlayerState newState, PlayerState oldState) = 0;
        virtual void onProgressUpdated(int64_t mSec) = 0;
        virtual void onSeekTo(int64_t mSec) = 0;
        virtual void onCompleted() = 0;
    };
    
    EditorPlayer();
    ~EditorPlayer();
    
    void setCallBack(Callback *callback) { _callback = callback; }
    
    //player control
    void start() override;
    void pause(bool bPause = true);
    void seek(int64_t mSec, int flag);
    int64_t getPosition() const override { return _playerPosition; }
    PlayerState getPlayerState() const {return _state; }
    
    bool getUserPaused() const { return _userPaused; }
    
protected:
//    virtual bool onRenderCreate() override;
    virtual bool onVideoRender(int64_t wantTimeMs) override;
//    virtual bool onRenderDestroy() override;
    virtual bool onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs) override;
    
    //implement EffectEditorCombiner::Callback
    virtual void onPrepared(RetCode code) override;
    virtual void onStarted(RetCode code) override;
    virtual void onStoped(RetCode code) override;
    virtual void onStreamEnd(MediaType mediaType) override;
    virtual void onCompleted() override;

private:
    RetCode _start();
    RetCode _pause(bool bPause = true);
    RetCode _seek(int64_t mSec, int flag);
    
    //other
    std::atomic_int _seekIdx;
    std::atomic_bool _bSeeking;
    int64_t _lastRenderTime;
    int64_t _playerPosition;
    Callback *_callback;
    
    //control state
    bool _userPaused;
    PlayerState _state;

    SteadyClock<int64_t, scale_milliseconds> _playerClock;
    
    std::mutex _render_mutex;
};

#endif /* EFFECTEDITOR_EDITORPLAYER_H */
