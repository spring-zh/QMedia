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
#include "Utils/Comm.h"
#include "MediaCore/output/AudioTarget.h"
#include "MediaCore/audiocore/ResamplerSpeex.h"
#include "MediaCore/audiocore/AudioEffect.h"

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
    
    float getPitch() const { return _fPitch; }
    void setPitch(float fPitch) { _fPitch = fPitch; }
private:
    
    std::shared_ptr<ResamplerSpeex> _resampler;
    std::shared_ptr<AudioEffect> _audioEffect;
    
    AudioSampleCache _sampleCache;
    std::vector<uint8_t> _cacheBuffer;
    
    MediaTrack *_mediaTrack;
    const AudioTarget* _audioTarget;
    
    float _volume;
    bool _bEnable;
    float _fPitch;
};

CLASSREF(MediaAudioChannel)

#endif /* EFFECTEDITOR_MEDIAAUDIOCHANNEL_H */
