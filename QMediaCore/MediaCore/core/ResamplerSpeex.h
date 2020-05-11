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
#if 0
    /**
     * @return out samples -1:error
     */
    int processAuto(void *indata, unsigned int inSamples, void *outdata, unsigned int outMaxSamples);

    int processAutoWithBytes(void *indata, unsigned int inBytes, void *outdata, unsigned int outMaxBytes);
#endif
    
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
    //merge channel 2 to 1
    template <typename T>
    void mergeChannel(const std::vector<uint8_t>& inPut, std::vector<uint8_t>& outPut, int samples) {
        outPut.resize(samples * sizeof(T));
        T* inPtr = (T*)inPut.data();
        T* outPtr = (T*)outPut.data();
        for (int i = 0; i < samples; i ++) {
            *outPtr = (*inPtr) / 2;
            *outPtr += (*(++inPtr)) / 2;
            ++outPtr;
            ++inPtr;
        }
    }
    //expand channel 1 to 2
    template <typename T>
    void expandChannel(const std::vector<uint8_t>& inPut, std::vector<uint8_t>& outPut, int samples) {
        outPut.resize(samples * sizeof(T) * 2);
        T* inPtr = (T*)inPut.data();
        T* outPtr = (T*)outPut.data();
        for (int i = 0; i < samples; i ++) {
            *outPtr = (*inPtr);
            *(++outPtr) += (*inPtr);
            ++outPtr;
            ++inPtr;
        }
    }
#if 0
    template <typename Ti, typename To>
    int convertBufferFormat(const Ti *indata, To *outdata, unsigned int samples)
    {
//        int min_samples = inSamples < outMaxSamples? inSamples : outMaxSamples;
        if (_inChannels == _outChannels){
            const Ti *inoffset = indata;
            To *outoffset = outdata;
            for (int i = 0; i < samples; ++i) {
                for (int i = 0; i < _inChannels; ++i) {
                    *(outoffset++) = valueMerge(*inoffset);
                    inoffset ++;
                }
            }
        } else if (_inChannels == 1){
            const Ti *inoffset = indata;
            To *outoffset = outdata;
            for (int i = 0; i < samples; ++i) {
                To value = valueMerge(*inoffset);
                inoffset ++;
                *(outoffset++) = value;
                *(outoffset++) = value;
            }
        } else if (_inChannels == 2){
            const Ti *inoffset = indata;
            To *outoffset = outdata;
            for (int i = 0; i < samples; ++i) {
                To value = valueMerge(inoffset,2);
                inoffset +=2;
                *(outoffset++) = value;
            }
        }

        return samples;
    }

    template <typename Ti>
    int convertBufferFormat<Ti,Ti>(const Ti *indata, Ti *outdata, unsigned int samples)
    {
        const Ti *inoffset = indata;
        Ti *outoffset = outdata;
//        int min_samples = inSamples < outMaxSamples? inSamples : outMaxSamples;
        for (int i = 0; i < samples; ++i) {
            Ti value = 0;
            for (int i = 0; i < _inChannels; ++i) {
                value += inoffset[i] / _inChannels;
            }
            for (int i = 0; i < _outChannels; ++i) {
                outoffset[i] = value;
            }
            inoffset += _inChannels;
            outoffset += _outChannels;
        }
        return samples;
    }

    inline float valueMerge(const int16_t* inv, int count){
        int value = 0;
        for (int i = 0; i < count; ++i) {
            value += inv[i];
        }
        return (float)value / (32767.0f * count);
    };
    inline float valueMerge(int16_t inv){
        return (float)inv / 32767.0f;
    };
    inline int16_t valueMerge(const float* inv, int count){
        float value = 0;
        for (int i = 0; i < count; ++i) {
            value += inv[i];
        }
        return (int16_t)(value * 32767 / count);
    };
    inline int16_t valueMerge(float inv){
        return (int16_t)(inv * 32767);
    };
#endif
    struct SpeexResamplerState_ *_resampler;
    bool _isinit;
    int _inSamplerate, _outSamplerate;
    int _inChannels,_outChannels;
    enum Foramt _inFormat,_outFormat;

    //
    std::vector<uint8_t> _tmpBuffer;
};

#endif //RESAMPLER_SPEEX_H
