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

class AudioClock {
public:
    AudioClock():_audioPosition(0),_writeAudioBytes(0) {}
    ~AudioClock() {}
    
    void init(int sampleRate, int channels, int bytesPreSample, int audioDelay) {
        _sampleRate = sampleRate;
        _channels = channels;
        _sampleWidthBytes = bytesPreSample;
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
