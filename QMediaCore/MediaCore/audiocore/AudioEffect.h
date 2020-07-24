//
//  AudioEffect.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <stdint.h>
#include <vector>
#include "Utils/Comm.h"

namespace soundtouch{
    class SoundTouch;
}

class AudioEffect
{
public:
    
    AudioEffect();
    ~AudioEffect();
    
    bool init(int samplerate,int channels);
    void setTempo(float fTempo);
    void setRate(float fRate);
    void setPitch(float fPitch);
    void flush();
    void clear();

    /**
     *  return:  samples  processed
     */
    int process(void* indata, int inSamples,void *outdata,int maxOutSamples);
    
    /**
     *  return:  bytes  processed
     */
    int process(void*indata, int inBytes, std::vector<uint8_t>& outPut);
    
private:
    float _fTempo;
    float _fRate;
    float _fPitch;

    int _channels;
    int _samplerate;

    soundtouch::SoundTouch *_soundProcess;
};
