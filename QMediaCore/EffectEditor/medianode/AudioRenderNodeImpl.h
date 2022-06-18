//
//  AudioRenderNodeImpl.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIA_API_AUDIO_RENDER_NODE_IMPL_H
#define QMEDIA_API_AUDIO_RENDER_NODE_IMPL_H

#include <list>
#include "Utils/Comm.h"
#include "MediaCore/core/DecodedFrame.h"
#include "MediaCore/output/AudioTarget.h"
#include "MediaCore/audiocore/ResamplerSpeex.h"
#include "MediaCore/audiocore/AudioEffect.h"
#include "media_range.h"
#include "audio_render_node.h"
#include "audio_description.h"

namespace QMedia { namespace Api {

class MediaSegmentImpl;

class AudioRenderNodeImpl : public AudioRenderNode {
public:
    AudioRenderNodeImpl(MediaSegmentImpl* segment);
    ~AudioRenderNodeImpl();
    
    virtual void setTempo(float tempo) override;

    virtual void setRate(float rate) override;

    virtual void setPitch(float pitch) override;

    virtual void setVolume(float volume) override;
    
    
    // other
    const AudioSampleCache* getAudioSample(int64_t time_ms);
    const MediaRange getRange() const;
    
    void PutAudioFrame(DecodedFrame audioframe);
    void ClearAudioFrame();
private:
    MediaSegmentImpl* segment_;
    
    std::shared_ptr<ResamplerSpeex> _resampler;
    std::shared_ptr<AudioEffect> _audioEffect;
    
    AudioSampleCache _sampleCache;
    std::vector<uint8_t> _cacheBuffer;
    float pitch_ = 1.0f;
    
    std::list<DecodedFrame> input_frames_;
};

}
}

#endif //QMEDIA_API_AUDIO_RENDER_NODE_IMPL_H
