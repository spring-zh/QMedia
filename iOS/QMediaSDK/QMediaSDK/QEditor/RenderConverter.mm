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
    AudioRenderConverter(EditorPlayerImpl* player):
    player_(player) {}
    int32_t OnPlayBuffer(void* buffer, int32_t size_need, int64_t wantTime) override {
        NSData *inBuffer = (__bridge NSData*)buffer;
        return player_->OnPlayBuffer((uint8_t * const)inBuffer.bytes, inBuffer.length, wantTime);
    }
    
    EditorPlayerImpl* player_;
};

class VideoRenderConverter : public VideoRender {
public:
    VideoRenderConverter(EditorPlayerImpl* player):
    player_(player) {}
    void setDisplayMode(int32_t mode, bool flip_v) override {
        player_->setDisplayMode(mode, flip_v);
    }

    void onViewSizeChange(int32_t width, int32_t height) override {
        setDisplayMode(0, true);
        player_->OnViewSizeChange(width, height);
    }

    bool onDraw(int64_t pirv, bool no_display) override {
        return player_->onDraw(pirv, no_display);
    }

    void onViewDestroy() override {
        player_->onViewDestroy();
    }
    
    void readRGBA(void* buffer, int32_t width, int32_t height, int32_t format) override {
        CVPixelBufferRef renderTarget = (CVPixelBufferRef)buffer;
        CVPixelBufferLockBaseAddress(renderTarget, 0);
        GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(renderTarget);
        MediaSessionImpl* session = (MediaSessionImpl*)(player_->getSession().get());
        session->GetDisplayLayer()->readRGB(buffer, width, height, format);
        CVPixelBufferUnlockBaseAddress(renderTarget, 0);
    }
    
    EditorPlayerImpl* player_;
};

class AudioRenderConverter2 : public AudioRender {
public:
    AudioRenderConverter2(EditorExporterImpl* exporter):
    exporter_(exporter) {}
    int32_t OnPlayBuffer(void* buffer, int32_t size_need, int64_t wantTime) override {
        NSData *inBuffer = (__bridge NSData*)buffer;
        return exporter_->OnPlayBuffer((uint8_t * const)inBuffer.bytes, inBuffer.length, wantTime);
    }
    
    EditorExporterImpl* exporter_;
};

class VideoRenderConverter2 : public VideoRender {
public:
    VideoRenderConverter2(EditorExporterImpl* exporter):
    exporter_(exporter) {}
    void setDisplayMode(int32_t mode, bool flip_v) override {
        exporter_->setDisplayMode(mode, flip_v);
    }

    void onViewSizeChange(int32_t width, int32_t height) override {
        setDisplayMode(0, false);
        exporter_->OnViewSizeChange(width, height);
    }

    bool onDraw(int64_t pirv, bool no_display) override {
        return exporter_->onDraw(pirv, no_display);
    }

    void onViewDestroy() override {
        exporter_->onViewDestroy();
    }
    
    void readRGBA(void* buffer, int32_t width, int32_t height, int32_t format) override {
        CVPixelBufferRef renderTarget = (CVPixelBufferRef)buffer;
        CVPixelBufferLockBaseAddress(renderTarget, 0);
        GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(renderTarget);
        MediaSessionImpl* session = (MediaSessionImpl*)(exporter_->getSession().get());
        session->GetDisplayLayer()->readRGB(pixelBufferData, width, height, format);
        CVPixelBufferUnlockBaseAddress(renderTarget, 0);
    }
    
    EditorExporterImpl* exporter_;
};

std::shared_ptr<AudioRender> CreatePlayerAudioRender(EditorPlayerImpl* player) {
    return std::shared_ptr<AudioRender>(new AudioRenderConverter(player));
}

std::shared_ptr<VideoRender> CreatePlayerVideoRender(EditorPlayerImpl* player) {
    return std::shared_ptr<VideoRender>(new VideoRenderConverter(player));
}


std::shared_ptr<AudioRender> CreateExporterAudioRender(EditorExporterImpl* exporter) {
    return std::shared_ptr<AudioRender>(new AudioRenderConverter2(exporter));
}

std::shared_ptr<VideoRender> CreateExporterVideoRender(EditorExporterImpl* exporter) {
    return std::shared_ptr<VideoRender>(new VideoRenderConverter2(exporter));
}

}
}
