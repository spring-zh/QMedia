//
//  AudioEffect.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "AudioEffect.h"
#include "Utils/Logger.h"

#include "SoundTouch/SoundTouch.h"

AudioEffect::AudioEffect():_channels(2),
_samplerate(44100) {
    _fTempo = 1.0f;
    _fRate = 1.0f;
    _fPitch = 1.0f;

    _soundProcess = new soundtouch::SoundTouch();
}

AudioEffect::~AudioEffect() {
    _soundProcess->clear();
    delete _soundProcess;
}

bool AudioEffect::init(int samplerate,int channels) {
    _samplerate = samplerate;
    _channels = channels;
    _soundProcess->clear();
    
    _soundProcess->setChannels(channels);
    _soundProcess->setSampleRate(samplerate);
    if (/*speech*/true)
    {
        // use settings for speech processing
        _soundProcess->setSetting(SETTING_SEQUENCE_MS, 10);
        _soundProcess->setSetting(SETTING_SEEKWINDOW_MS, 10);
        _soundProcess->setSetting(SETTING_OVERLAP_MS, 10);
        _soundProcess->setSetting(SETTING_USE_QUICKSEEK, 1);
    }
    return true;
}

void AudioEffect::setTempo(float fTempo) {
    if (! FLOAT_ISEQUAL(fTempo, _fTempo)){
        _soundProcess->setTempo(fTempo);
        _fTempo = fTempo;
    }
}
void AudioEffect::setRate(float fRate) {
    if (! FLOAT_ISEQUAL(fRate, _fRate)){
        _soundProcess->setRate(fRate);
        _fRate = fRate;
    }
}
void AudioEffect::setPitch(float fPitch) {
    if (! FLOAT_ISEQUAL(fPitch, _fPitch)){
        _soundProcess->setPitch(fPitch);
        _fPitch = fPitch;
    }
}

void AudioEffect::flush() {
     _soundProcess->flush();
}
void AudioEffect::clear() {
    _soundProcess->clear();
}

int AudioEffect::process(void* indata, int inSamples,void *outdata,int maxOutSamples) {
    if(indata && inSamples > 0)
        _soundProcess->putSamples((soundtouch::SAMPLETYPE*)indata,inSamples);
    else
        _soundProcess->flush();

    int totalSamplse = 0;
    do{
        int samplse = _soundProcess->receiveSamples((soundtouch::SAMPLETYPE*)outdata + totalSamplse * _channels, maxOutSamples - totalSamplse);
        if (samplse > 0)
            totalSamplse += samplse;
        else
            break;
    }while (totalSamplse < maxOutSamples);
    return totalSamplse;
}

int AudioEffect::process(void*indata, int inBytes, std::vector<uint8_t>& outPut) {
    const int sample_addition = 512;
    const int bytesPreSample = _channels * sizeof(soundtouch::SAMPLETYPE);
    if(indata && inBytes > 0) {
        _soundProcess->putSamples((soundtouch::SAMPLETYPE*)indata, inBytes/ bytesPreSample);
    }
    else if(_soundProcess->numUnprocessedSamples() > 0) {
        //FIXME: SoundTouch flush will trigger assert(nominalSkip >= -skipFract) in TDStretch.cpp line 694
        _soundProcess->flush();
    }
    else
        return 0;
    
    int maxOutSamples = sample_addition;
    outPut.resize(maxOutSamples * bytesPreSample);
    
    int totalSamplse = 0;
    do{
        int samplse = _soundProcess->receiveSamples((soundtouch::SAMPLETYPE*)outPut.data() + totalSamplse * _channels, maxOutSamples - totalSamplse);
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

