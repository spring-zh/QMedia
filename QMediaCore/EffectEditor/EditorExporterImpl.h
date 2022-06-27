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
    
    virtual void setAudioOption(const AudioEncodeOption & option) override {
        
    }

    virtual void setVideoOption(const VideoEncodeOption & option) override {
        
    }

    //player control
    virtual int64_t getPosition() override { return _renderPosition; }

    virtual void start() override;

    virtual void cancel() override;
    
    //call by render
    int32_t OnPlayBuffer(uint8_t * const buffer, int32_t size_need, int64_t wantTime);
    
    void setDisplayMode(int mode, bool filp_v);
    void OnViewSizeChange(int32_t width, int32_t height);
    void onViewCreate(int32_t width, int32_t height);
    bool onDraw(int64_t pirv, bool no_display);
    void onViewDestroy();

protected:
    
    //implement EffectEditorCombiner::Callback
    virtual void onPrepared(RetCode code) override;
//    virtual void onStarted(RetCode code) override;
    virtual void onStoped(RetCode code) override;
    virtual void onStreamEnd(MediaType mediaType) override;
    virtual void onCompleted(RetCode code) override;

private:
    
    std::shared_ptr<MediaSessionImpl> session_;
    //other
    int64_t _lastRenderTime;
    int64_t _renderPosition;
    std::shared_ptr<EditorExporterCallback> callback_;
    
    //control state
    SessionState _state;
};


extern std::shared_ptr<AudioRender> CreateExporterAudioRender(EditorExporterImpl* player);

extern std::shared_ptr<VideoRender> CreateExporterVideoRender(EditorExporterImpl* player);

}
}

#endif /* MEDIA_EDITOR_EXPORTER_IMPL_H */
