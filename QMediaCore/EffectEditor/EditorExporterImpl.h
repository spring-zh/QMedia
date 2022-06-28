//
//  EditorExporterImpl.h
//  QMediaCore
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef MEDIA_EDITOR_EXPORTER_IMPL_H
#define MEDIA_EDITOR_EXPORTER_IMPL_H

#include "MediaSessionImpl.h"
#include "editor_exporter_internal.h"
#include "editor_exporter_callback.h"

namespace QMedia { namespace Api {

class EditorExporterImpl : public EditorExporterInternal, public MediaSessionImpl::Callback {
public:
    
    EditorExporterImpl();
    ~EditorExporterImpl();
    
    virtual std::shared_ptr<MediaSessionInternal> getSession() override { return session_;}

    virtual void setCallback(const std::shared_ptr<EditorExporterCallback> & callback) override {
        callback_ = callback;
    }
    
    virtual void setAudioOption(const AudioEncodeOption & option) override { }

    virtual void setVideoOption(const VideoEncodeOption & option) override { }

    //player control
    virtual int64_t getPosition() override { return session_->render_position_; }

    virtual void start() override;

    virtual void cancel() override;

protected:
    
    //TODO: MediaSession::Callback
    void onPrepared(RetCode code) override;
//    virtual void onStarted(RetCode code) override;
    void onStoped(RetCode code) override;
    void onStreamEnd(MediaType mediaType) override;
    void onCompleted(RetCode code) override;
    void onProgressUpdated(int64_t position) override;
private:
    
    std::shared_ptr<MediaSessionImpl> session_;
    //other
//    int64_t _lastRenderTime;
//    int64_t _renderPosition;
    std::shared_ptr<EditorExporterCallback> callback_;
    
    //control state
    SessionState _state{SessionState::Idle};
};


extern std::shared_ptr<AudioRender> CreateExporterAudioRender(EditorExporterImpl* player);

extern std::shared_ptr<VideoRender> CreateExporterVideoRender(EditorExporterImpl* player);

}
}

#endif /* MEDIA_EDITOR_EXPORTER_IMPL_H */
