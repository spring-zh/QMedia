//
//  AudioProcess.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <stdint.h>

class AudioProcess
{
public:
    AudioProcess() = default;
    ~AudioProcess() = default;

    static void mixAudio(uint8_t * dst, const uint8_t * src, int len, float volume);

    /**
     * ampfily audio PCM data,if volume is too big,it will be rescaled smaller to avoid clipping
     * */
    static void amplifyAudioVolume(const uint8_t* src,const uint8_t* dst,int len,float volume);

    static void shrinkAudioVolume(uint8_t * src, int len, float volume);

    static void adjustAudioVolume(uint8_t * src, int len, float volume);
    
    // millisecond
    static int64_t calculateAudioDurationByBytes(int samplerate, int channels, int bytesPerSample, int64_t byteSize);
};
