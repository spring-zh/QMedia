//
//  MediaSessionPlayerImpl.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectEditor/MediaSessionPlayerImpl.h"
#import "QAudioPlayer.h"

namespace QMedia { namespace Api {

class AudioRenderConverter : public AudioRender {
public:
    AudioRenderConverter(MediaSessionPlayerImpl* player):
    player_(player) {}
    int32_t OnPlayBuffer(void* buffer, int32_t size_need, int64_t wantTime) override {
        AudioInputBuffer* inBuffer = (__bridge AudioInputBuffer*)buffer;
        return player_->OnPlayBuffer((uint8_t * const)inBuffer.audio_data, inBuffer.audio_size, wantTime);
    }
    
    MediaSessionPlayerImpl* player_;
};

class VideoRenderConverter : public VideoRender {
public:
    VideoRenderConverter(MediaSessionPlayerImpl* player):
    player_(player) {}
    void setDisplayMode(int32_t mode, int32_t width, int32_t height) override {
        player_->setDisplayMode(mode, width, height);
    }

    void OnViewSizeChange(int32_t width, int32_t height) override {
        player_->OnViewSizeChange(width, height);
    }

    void onViewCreate(int32_t width, int32_t height) override {
        player_->onViewCreate(width, height);
    }

    bool onDraw(int64_t pirv) override {
        return player_->onDraw(pirv);
    }

    void onViewDestroy() override {
        player_->onViewDestroy();
    }
    
    MediaSessionPlayerImpl* player_;
};

std::shared_ptr<AudioRender> CreatePlayerAudioRender(MediaSessionPlayerImpl* session_player) {
    return std::shared_ptr<AudioRender>(new AudioRenderConverter(session_player));
}

std::shared_ptr<VideoRender> CreatePlayerVideoRender(MediaSessionPlayerImpl* session_player) {
    return std::shared_ptr<VideoRender>(new VideoRenderConverter(session_player));
}

}
}
