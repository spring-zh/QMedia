//
//  AudioEffect.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "AudioEffect.h"
#include "Utils/Logger.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "sonic.h"
#ifdef __cplusplus
}
#endif

AudioEffect::AudioEffect():_channels(2),
_samplerate(44100) , _sonic(NULL){
    _fTempo = 1.0f;
    _fRate = 1.0f;
    _fPitch = 1.0f;
    _fVolume = 1.0f;

}

AudioEffect::~AudioEffect() {
    if (_sonic) {
        sonicDestroyStream(_sonic);
        _sonic = NULL;
    }
}

bool AudioEffect::init(int samplerate,int channels) {
    _samplerate = samplerate;
    _channels = channels;
    if (_sonic == NULL) {
        _sonic = sonicCreateStream(samplerate, channels);
    }else {
        sonicSetSampleRate(_sonic, samplerate);
        sonicSetNumChannels(_sonic, channels);
    }
    
//    sonicSetQuality(_sonic, 0.5);
    return _sonic != NULL;
}

void AudioEffect::setTempo(float fTempo) {
    if (! FLOAT_ISEQUAL(fTempo, _fTempo)){
        sonicSetSpeed(_sonic, fTempo);
        _fTempo = fTempo;
    }
}
void AudioEffect::setRate(float fRate) {
    if (! FLOAT_ISEQUAL(fRate, _fRate)){
        sonicSetRate(_sonic, fRate);
        _fRate = fRate;
    }
}
void AudioEffect::setPitch(float fPitch) {
    if (! FLOAT_ISEQUAL(fPitch, _fPitch)){
        sonicSetPitch(_sonic, fPitch);
        _fPitch = fPitch;
    }
}
void AudioEffect::setVolume(float fVolume) {
    if (! FLOAT_ISEQUAL(fVolume, _fVolume)){
        sonicSetVolume(_sonic, fVolume);
        _fVolume = fVolume;
    }
}

void AudioEffect::flush() {
    sonicFlushStream(_sonic);
}
void AudioEffect::clear() {
    if (_sonic) {
        sonicDestroyStream(_sonic);
        _sonic = sonicCreateStream(_samplerate, _channels);
    }
}

int AudioEffect::process(void* indata, int inSamples,void *outdata,int maxOutSamples) {
    if(indata && inSamples > 0)
        sonicWriteShortToStream(_sonic, static_cast<short *>(indata), inSamples);
    else
        sonicFlushStream(_sonic);

    int totalSamplse = 0;
    do{
        int samplse = sonicReadShortFromStream(_sonic, (short*)outdata + totalSamplse * _channels, maxOutSamples - totalSamplse);
        if (samplse > 0)
            totalSamplse += samplse;
        else
            break;
    }while (totalSamplse < maxOutSamples);
    return totalSamplse;
}

int AudioEffect::process(void* indata, int inBytes, std::vector<uint8_t>& outPut) {
    const int sample_addition = 512;
    const int bytesPreSample = _channels * sizeof(short);
    if(indata && inBytes > 0) {
        sonicWriteShortToStream(_sonic, static_cast<short *>(indata), inBytes/ bytesPreSample);
    }
    else /*if(sonicSamplesAvailable(_sonic) > 0)*/ {
        sonicFlushStream(_sonic);
    }
    
    int maxOutSamples = sample_addition;
    outPut.resize(maxOutSamples * bytesPreSample);
    
    int totalSamplse = 0;
    do{
        int samplse = sonicReadShortFromStream(_sonic, (short*)outPut.data() + totalSamplse * _channels, maxOutSamples - totalSamplse);
        if (samplse > 0)
            totalSamplse += samplse;
        else
            break;
        if (totalSamplse >= maxOutSamples) {
            maxOutSamples += sample_addition;
            outPut.resize(maxOutSamples * bytesPreSample);
        }
    }while (1);
    return totalSamplse * bytesPreSample;
}

