//
//  MediaSessionPlayerImp.h
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef MEDIA_SESSION_PLAYER_IMPL_H
#define MEDIA_SESSION_PLAYER_IMPL_H

#include "MediaSessionImpl.h"
#include "editor_player_internal.h"
#include "editor_player_callback.h"

namespace QMedia { namespace Api {

class MediaSessionPlayerImpl : public EditorPlayerInternal, public MediaSessionImpl::Callback {
public:
    
    MediaSessionPlayerImpl();
    ~MediaSessionPlayerImpl();

    bool getUserPaused() const { return _userPaused; }
    
    virtual std::shared_ptr<MediaSessionInternal> getSession() override { return session_;}

    virtual void setCallback(const std::shared_ptr<EditorPlayerCallback> & callback) override {
        callback_ = callback;
    }

    //player control
    virtual int64_t getPosition() override { return _playerPosition; }

    virtual void play() override;

    virtual void pause() override;

    virtual void seek(int64_t time_ms, int32_t flag) override;

    virtual bool isUserPaused() override { return _userPaused; }
    
    virtual int32_t getState() override { return (int32_t)_state;}
    
    //call by render
    int32_t OnPlayBuffer(uint8_t * const buffer, int32_t size_need, int64_t wantTime);
    
    void setDisplayMode(int32_t mode, int32_t width, int32_t height);
    void OnViewSizeChange(int32_t width, int32_t height);
    void onViewCreate(int32_t width, int32_t height);
    bool onDraw(int64_t pirv);
    void onViewDestroy();
protected:
    
    //implement EffectEditorCombiner::Callback
    virtual void onPrepared(RetCode code) override;
    virtual void onStarted(RetCode code) override;
    virtual void onStoped(RetCode code) override;
    virtual void onStreamEnd(MediaType mediaType) override;
    virtual void onCompleted() override;

private:
    
    RetCode _start();
    RetCode _pause(bool bPause);
    RetCode _seek(int64_t mSec, int flag);
    
    std::shared_ptr<MediaSessionImpl> session_;
    //other
    std::atomic_int _seekIdx;
    std::atomic_bool _bSeeking;
    int64_t _lastRenderTime;
    int64_t _playerPosition;
    std::shared_ptr<EditorPlayerCallback> callback_;
    
    //control state
    bool _userPaused;
    SessionState _state;

    SteadyClock<int64_t, scale_milliseconds> _playerClock;

    ticket_lock _render_mutex;
};


extern std::shared_ptr<AudioRender> CreatePlayerAudioRender(MediaSessionPlayerImpl* session_player);

extern std::shared_ptr<VideoRender> CreatePlayerVideoRender(MediaSessionPlayerImpl* session_player);

}
}

#endif /* MEDIA_SESSION_PLAYER_IMPL_H */
