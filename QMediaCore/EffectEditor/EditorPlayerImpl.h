//
//  EditorPlayerImpl.h
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef MEDIA_EDITOR_PLAYER_IMPL_H
#define MEDIA_EDITOR_PLAYER_IMPL_H

#include "MediaSessionImpl.h"
#include "editor_player_internal.h"
#include "editor_player_callback.h"

namespace QMedia { namespace Api {

class EditorPlayerImpl : public EditorPlayerInternal, public MediaSessionImpl::Callback {
public:
    
    EditorPlayerImpl();
    ~EditorPlayerImpl();
    
    virtual std::shared_ptr<MediaSessionInternal> getSession() override { return session_;}

    virtual void setCallback(const std::shared_ptr<EditorPlayerCallback> & callback) override {
        callback_ = callback;
    }

    //player control
    virtual int64_t getPosition() override;

    virtual void play() override;

    virtual void pause() override;

    virtual void seek(int64_t time_ms, int32_t flag) override;

    virtual bool isUserPaused() override { return session_->user_paused_; }
    
    virtual int32_t getState() override { return (int32_t)session_->_state;}

protected:
    
    //TODO: implement MediaSession::Callback
    void onPrepared(RetCode code) override;
//    virtual void onStarted(RetCode code) override;
    void onStoped(RetCode code) override;
    void onStreamEnd(MediaType mediaType) override;
    void onCompleted(RetCode code) override;
    void onProgressUpdated(int64_t position) override;
private:
    
    std::shared_ptr<MediaSessionImpl> session_;
    std::shared_ptr<EditorPlayerCallback> callback_;
};

}
}

#endif /* MEDIA_EDITOR_PLAYER_IMPL_H */
