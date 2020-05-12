//
//  ResamplerSpeex.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef RESAMPLER_SPEEX_H
#define RESAMPLER_SPEEX_H

#include <stdint.h>
#include <vector>

struct SpeexResamplerState_;

class ResamplerSpeex {
public:
    enum Foramt{
        FT_INT16 = 0,
        FT_UINT8 = 1,
        FT_FLOAT = 2
    };
    ResamplerSpeex();
    ~ResamplerSpeex();

    //only change samplerate
    bool init(int channels, unsigned int inSamplerate, unsigned int outSamplerate,Foramt foramt);

    bool init(Foramt inForamt, int inChannels, unsigned int inSamplerate, Foramt outForamt, int outChannels, unsigned int outSamplerate);
    void release();
    
    /**
     *process resample
     *return: output bytes
     */
    int process(void *indata, unsigned int inBytes, std::vector<uint8_t>& outPut);
    //only resample
    int resample(void *indata, unsigned int inSamples, void *outdata, unsigned int outMaxSamples);

    bool isInit(){ return _isinit; }

    int64_t getPredictedOutputByte(int64_t inbytes);
private:
    int64_t getPredictedResmapleByte(int64_t inbytes);
    //merge channel 2 to 1
    template <typename T>
    void mergeChannel(const std::vector<uint8_t>& inPut, std::vector<uint8_t>& outPut, int samples) {
        outPut.resize(samples * sizeof(T));
        T* inPtr = (T*)inPut.data();
        T* outPtr = (T*)outPut.data();
        T* outEndPtr = outPtr + samples;
        for (; outPtr < outEndPtr; ++outEndPtr ) {
            *outPtr = (*(inPtr++)) / 2;
            *outPtr += (*(inPtr++)) / 2;
        }
    }
    //expand channel 1 to 2
    template <typename T>
    void expandChannel(const std::vector<uint8_t>& inPut, std::vector<uint8_t>& outPut, int samples) {
        outPut.resize(samples * sizeof(T) * 2);
        T* inPtr = (T*)inPut.data();
        T* outPtr = (T*)outPut.data();
        T* inEndPtr = inPtr + samples;
        for (; inPtr < inEndPtr; ++inPtr ) {
            *(outPtr++) = *inPtr;
            *(outPtr++) = *inPtr;
        }
    }

    struct SpeexResamplerState_ *_resampler;
    bool _isinit;
    int _inSamplerate, _outSamplerate;
    int _inChannels,_outChannels;
    enum Foramt _inFormat,_outFormat;

    //
    std::vector<uint8_t> _tmpBuffer;
};

#endif //RESAMPLER_SPEEX_H
