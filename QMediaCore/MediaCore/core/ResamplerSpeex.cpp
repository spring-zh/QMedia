//
//  ResamplerSpeex.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "ResamplerSpeex.h"
#include <stdlib.h>
#include <string.h>

#include "speex_resampler/speex_resampler.h"

static int FormatBytes[] = {2, 1, 4, 0};

//#define GET_FORMAT_BYTES(format) \
//    ((format == FT_FLOAT)?4:2)

ResamplerSpeex::ResamplerSpeex():
        _isinit(false),
        _resampler(NULL),
        _inChannels(0),_outChannels(0),
        _inSamplerate(0),_outSamplerate(0)
{
    _tmpBuffer.resize(1024 * 8);
}
ResamplerSpeex::~ResamplerSpeex()
{
    release();
}

bool ResamplerSpeex::init(int channels, unsigned int inSamplerate, unsigned int outSamplerate,Foramt foramt)
{
    return init(foramt,channels,inSamplerate,foramt,channels,outSamplerate);
}
bool ResamplerSpeex::init(Foramt inForamt, int inChannels, unsigned int inSamplerate, Foramt outForamt, int outChannels, unsigned int outSamplerate)
{
    int error;

    if (_isinit && _resampler){
        unsigned int in_rate,out_rate;
        speex_resampler_get_rate(_resampler,&in_rate,&out_rate);
        if ((in_rate == inSamplerate) && (out_rate == outSamplerate)
            && (inChannels == _inChannels)
            && (inForamt == _inFormat))
            return true;
        else {
//            release();
            if(speex_resampler_set_rate(_resampler,inSamplerate,outSamplerate) != 0){
                //update rate failed
            }
        }
    } else {
        _resampler = speex_resampler_init(inChannels,inSamplerate,outSamplerate,SPEEX_RESAMPLER_QUALITY_DEFAULT,&error);
        _isinit = (error == RESAMPLER_ERR_SUCCESS);
    }


    _inChannels = inChannels;
    _inFormat = inForamt;
    _outChannels = outChannels;
    _outFormat = outForamt;
    _inSamplerate = inSamplerate;
    _outSamplerate = outSamplerate;
    return _isinit;
}
void ResamplerSpeex::release()
{
    if (_isinit && _resampler){
        speex_resampler_destroy(_resampler);
    }
    _resampler = NULL;
    _isinit = false;
}

int64_t ResamplerSpeex::getPredictedOutputByte(int64_t inbytes)
{
    if (_isinit && _resampler){
        unsigned int in_rate,out_rate;
        speex_resampler_get_rate(_resampler,&in_rate,&out_rate);
        int64_t needbytes = inbytes *_outChannels * FormatBytes[_outFormat] * out_rate /
                (in_rate *(FormatBytes[_inFormat] * _inChannels));

        //stride by 16, then + 32
        return (needbytes & 0xfffffff0) + 0x0020;
    }
    return 0;
}
int64_t ResamplerSpeex::getPredictedResmapleByte(int64_t inbytes)
{
    if (_isinit && _resampler){
        unsigned int in_rate,out_rate;
        speex_resampler_get_rate(_resampler,&in_rate,&out_rate);
        int64_t needbytes = inbytes * out_rate / in_rate;

        //stride by 16, then + 32
        return (needbytes & 0xfffffff0) + 0x0020;
    }
    return 0;
}

int ResamplerSpeex::process(void *indata, unsigned int inBytes, std::vector<uint8_t>& outPut)
{
    int maxResampleBytes = getPredictedResmapleByte(inBytes);
    if (_tmpBuffer.size() < maxResampleBytes) {
        _tmpBuffer.resize(maxResampleBytes);
    }
    int inSamples = inBytes / (FormatBytes[_inFormat] * _inChannels);
    int maxResampleSamples = maxResampleBytes / (FormatBytes[_inFormat] * _inChannels);
    int outSamples = resample(indata, inSamples, _tmpBuffer.data(), maxResampleSamples);
    int outBytes = outSamples * _outChannels * FormatBytes[_outFormat];
    if (_inChannels != _outChannels) {
        if (_outChannels == 1) {
            mergeChannel<int16_t>(_tmpBuffer, outPut, outSamples);
        }else if(_outChannels == 2)
            expandChannel<int16_t>(_tmpBuffer, outPut, outSamples);
    }else {
        outPut.assign(_tmpBuffer.begin(), _tmpBuffer.begin() + outBytes);
    }
    return outBytes;
}

int ResamplerSpeex::resample(void *indata, unsigned int inSamples, void *outdata, unsigned int outMaxSamples)
{
    int nRet = -1;
    if (!_isinit)
        return -1;

    spx_uint32_t inputSamples = inSamples;
    spx_uint32_t outputSamples = outMaxSamples;
    if (_inChannels == 1){
        switch (_inFormat){
            case FT_FLOAT:
                nRet = speex_resampler_process_float(_resampler,0,(float*)indata,&inputSamples, (float*)outdata,&outputSamples);
                break;
            case FT_INT16:
                nRet = speex_resampler_process_int(_resampler,0,(int16_t *)indata,&inputSamples, (int16_t *)outdata,&outputSamples);
                break;
            case FT_UINT8:
                break;
        }
    } else if (_inChannels == 2){
        switch (_inFormat) {
            case FT_FLOAT:
                nRet = speex_resampler_process_interleaved_float(_resampler,(float*)indata,&inputSamples, (float*)outdata,&outputSamples);
                break;
            case FT_INT16:
                nRet = speex_resampler_process_interleaved_int(_resampler,(int16_t *)indata,&inputSamples, (int16_t *)outdata,&outputSamples);
                break;
            case FT_UINT8:
                break;
        }
    }

    if (nRet == 0) {
        if (inSamples != inputSamples) {
            printf("inSamples %d != inputSamples %d \n", inSamples, inputSamples);
        }
        if (outputSamples > outMaxSamples){
//            LOGW("speex resampler outputSamples > outMaxSamples ");
        }
        return outputSamples;
    }
    else
        return 0;
}

