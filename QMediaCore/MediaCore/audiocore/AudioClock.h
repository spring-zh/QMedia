//
//  AudioClock.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef MEDIACORE_AUDIOCLOCK_H
#define MEDIACORE_AUDIOCLOCK_H

#include "Utils/spin_lock.h"
#include "Utils/Comm.h"
#include "AudioProcess.h"
#include "MediaCore/output/AudioTarget.h"

inline static int getBytesFromPcmFormat(RawAudioFormat audioFormat) {
    switch (audioFormat) {
        case RawAudioFormat::kU8:
            return 1;
        case RawAudioFormat::kS16:
        case RawAudioFormat::kU16:
            return 2;
        case RawAudioFormat::kS32:
        case RawAudioFormat::kU32:
        case RawAudioFormat::kFLOAT:
            return 4;
        default:
            return 2;
    }
}

class AudioClock {
public:
    AudioClock():_audioPosition(0),_writeAudioBytes(0) {}
    ~AudioClock() {}
    
    void init(AudioTarget *at) {
        _sampleRate = at->getSampleRate();
        _channels = at->getChannels();
        _sampleWidthBytes = getBytesFromPcmFormat(at->getFormat());
        _audoDelay = at->getAudioDelayBytes();
    }
    
    void init(int sampleRate, int channels, RawAudioFormat audioFmt, int audioDelay) {
        _sampleRate = sampleRate;
        _channels = channels;
        _sampleWidthBytes = getBytesFromPcmFormat(audioFmt);
        _audoDelay = audioDelay;
    }
    
    void increaseAudioData(unsigned int size) {
        t_lock_guard<spin_mutex> lck(_spin);
        _writeAudioBytes += size;
    }
    
    void setClock(int64_t clock) {
        t_lock_guard<spin_mutex> lck(_spin);
        _audioPosition = clock;
        _writeAudioBytes = 0;
    }
    int64_t getClock() const {
        int64_t play_duration = AudioProcess::calculateAudioDurationByBytes(_sampleRate,_channels,_sampleWidthBytes, _writeAudioBytes);
        return _audioPosition + play_duration;
    }
    //calculate clock with audio delay
    int64_t getPlayingClock() const {
        int64_t posBytes = MAX(0, _writeAudioBytes - _audoDelay);
        int64_t play_duration = AudioProcess::calculateAudioDurationByBytes(_sampleRate,_channels,_sampleWidthBytes, posBytes);
        return _audioPosition + play_duration;
    }
    
private:
    mutable spin_mutex _spin;
    int64_t _audioPosition;
    int64_t _writeAudioBytes;
    int _sampleRate;
    int _channels;
    int _sampleWidthBytes;
    int _audoDelay; //audio target delay bytes;l
};

#endif /* MEDIACORE_AUDIOCLOCK_H */
