//
//  MediaSessionPlayerImpl.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectEditor/EditorPlayerImpl.h"
#include "EffectEditor/EditorExporterImpl.h"
#import "QAudioPlayer.h"
#import <CoreVideo/CoreVideo.h>

namespace QMedia { namespace Api {

class AudioRenderConverter : public AudioRender {
public:
    AudioRenderConverter(MediaSessionImpl* session):
    session_(session) {}
    int32_t OnPlayBuffer(void* buffer, int32_t size_need, int64_t wantTime) override {
        NSData *inBuffer = (__bridge NSData*)buffer;
        return session_->onAudioRender((uint8_t * const)inBuffer.bytes, inBuffer.length, wantTime);
    }
    
    MediaSessionImpl* session_;
};

class VideoRenderConverter : public VideoRender {
public:
    VideoRenderConverter(MediaSessionImpl* session):
    session_(session) {}
    void setDisplayMode(int32_t mode, bool flip_v) override {
        session_->setDisplayMode((DisplayMode)mode, flip_v);
    }

    void onViewSizeChange(int32_t width, int32_t height) override {
        session_->OnViewSizeChange(width, height);
    }

    bool onDraw(int64_t time_ms, bool no_display) override {
        return session_->onVideoRender(time_ms, no_display);
    }

    void onViewDestroy() override {
        session_->onRenderDestroy();
    }
    
    bool readRGBA(void* buffer, int32_t width, int32_t height, int32_t format) override {
        CVPixelBufferRef renderTarget = (CVPixelBufferRef)buffer;
        CVPixelBufferLockBaseAddress(renderTarget, 0);
        GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(renderTarget);
        session_->GetDisplayLayer()->readRGB(pixelBufferData, width, height, format);
        CVPixelBufferUnlockBaseAddress(renderTarget, 0);
        return true;
    }
    
    MediaSessionImpl* session_;
};

std::shared_ptr<AudioRender> CreatePlayerAudioRender(MediaSessionImpl* session) {
    return std::shared_ptr<AudioRender>(new AudioRenderConverter(session));
}

std::shared_ptr<VideoRender> CreatePlayerVideoRender(MediaSessionImpl* session) {
    return std::shared_ptr<VideoRender>(new VideoRenderConverter(session));
}

}
}
