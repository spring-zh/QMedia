//
//  MediaAudioChannel.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIAAUDIOCHANNEL_H
#define EFFECTEDITOR_MEDIAAUDIOCHANNEL_H

#include <vector>
#include <memory>
#include "MediaCore/output/AudioTarget.h"
#include "MediaCore/core/ResamplerSpeex.h"

class MediaTrack;

class MediaAudioChannel {
public:
    explicit MediaAudioChannel(MediaTrack *mediaTrack);
    virtual ~MediaAudioChannel() {}
    
    const AudioSampleCache* getAudioSample(int64_t mSec);
    
    float getVolume() const { return _volume; }
    void setVolume(float volume) { _volume = volume; }
    
    bool isEnable() const { return _bEnable; }
    void setEnable(bool enable) { _bEnable = enable; }
private:
    
    std::shared_ptr<ResamplerSpeex> _resampler;
    
    AudioSampleCache _sampleCache;
    std::vector<uint8_t> _cacheBuffer;
    
    MediaTrack *_mediaTrack;
    const AudioTarget* _audioTarget;
    
    float _volume;
    bool _bEnable;
};

using MediaAudioChannelRef = std::shared_ptr<MediaAudioChannel>;

#endif /* EFFECTEDITOR_MEDIAAUDIOCHANNEL_H */
