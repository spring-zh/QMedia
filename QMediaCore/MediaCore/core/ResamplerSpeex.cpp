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

#define GET_FORMAT_BYTES(format) \
    ((format == FT_FLOAT)?4:2)

ResamplerSpeex::ResamplerSpeex():
        _isinit(false),
        _resampler(NULL),
        _inChannels(0),_outChannels(0),
        _inSamplerate(0),_outSamplerate(0)
{
    _output_buff.resize(1024 * 8);
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

int ResamplerSpeex::getPredictedOutputByte(int inbytes)
{
    if (_isinit && _resampler){
        unsigned int in_rate,out_rate;
        speex_resampler_get_rate(_resampler,&in_rate,&out_rate);
        int needbytes = inbytes *_outChannels * GET_FORMAT_BYTES(_outFormat) * in_rate /
                (out_rate *(GET_FORMAT_BYTES(_inFormat) * _inChannels));

        //stride by 16, then + 32
        return (needbytes & 0xfff0) + 0x0020;
    }
    return 0;
}

int ResamplerSpeex::processAutoWithBytes(void *indata, unsigned int inBytes, void *outdata, unsigned int outMaxBytes)
{
    if (!_isinit){
        return 0;
    }
    int inSamples = inBytes / (GET_FORMAT_BYTES(_inFormat) * _inChannels);
    int outMaxSamples = outMaxBytes / (GET_FORMAT_BYTES(_outFormat) * _outChannels);
    int outSamples = processAuto(indata,inSamples,outdata,outMaxSamples);
    return outSamples * (GET_FORMAT_BYTES(_outFormat) * _outChannels);
}

int ResamplerSpeex::processAuto(void *indata, unsigned int inSamples, void *outdata, unsigned int outMaxSamples)
{
    int iRet = -1;

    void * new_data = indata;
    int new_inSamples = inSamples;

    unsigned int in_rate,out_rate;
    speex_resampler_get_rate(_resampler,&in_rate,&out_rate);
    if (in_rate != out_rate){//rate change

        if (_inFormat == _outFormat && _inChannels == _outChannels){
            return process(indata,inSamples,outdata,outMaxSamples);
        } else{
            int expectation_samples = inSamples * in_rate / out_rate + 32;
            int out_buffer_len = GET_FORMAT_BYTES(_outFormat) * _inChannels* expectation_samples;
            if (out_buffer_len > _output_buff.size()){
                _output_buff.resize(out_buffer_len);
            }
            iRet = process(indata,inSamples,_output_buff.data(),expectation_samples);
            new_inSamples = iRet;
            new_data = _output_buff.data();
        }
    }

//    int min_smaples = (new_inSamples < outMaxSamples)?new_inSamples:outMaxSamples;
//    if (_inFormat == FT_FLOAT){
//        if (_outFormat == FT_FLOAT)
//            iRet = convertBufferFormat<float>((const float*)new_data,(float*)outdata,min_smaples);
//        else if(_outFormat == FT_INT16)
//            iRet = convertBufferFormat<float,int16_t>((const float*)new_data,(int16_t*)outdata,min_smaples);
//    }
//    else if (_inFormat == FT_INT16){
//        if (_outFormat == FT_FLOAT)
//            iRet = convertBufferFormat<int16_t,float>((const int16_t*)new_data,(float*)outdata,min_smaples);
//        else if(_outFormat == FT_INT16)
//            iRet = convertBufferFormat<int16_t>((const int16_t*)new_data,(int16_t*)outdata,min_smaples);
//    }

    return iRet;
}

int ResamplerSpeex::process(void *indata, unsigned int inSamples, void *outdata, unsigned int outMaxSamples)
{
    int nRet = -1;
    if (!_isinit)
        return -1;

    spx_uint32_t outputSamples = 0;
    if (_inChannels == 1){
        switch (_inFormat){
            case FT_FLOAT:
                nRet = speex_resampler_process_float(_resampler,1,(float*)indata,&inSamples, (float*)outdata,&outputSamples);
                break;
            case FT_INT16:
                nRet = speex_resampler_process_int(_resampler,1,(int16_t *)indata,&inSamples, (int16_t *)outdata,&outputSamples);
                break;
        }
    } else if (_inChannels == 2){
        switch (_inFormat) {
            case FT_FLOAT:
                nRet = speex_resampler_process_interleaved_float(_resampler,(float*)indata,&inSamples, (float*)outdata,&outputSamples);
                break;
            case FT_INT16:
                nRet = speex_resampler_process_interleaved_int(_resampler,(int16_t *)indata,&inSamples, (int16_t *)outdata,&outputSamples);
                break;
        }
    }

    if (nRet == 0) {
        if (outputSamples > outMaxSamples){
//            LOGW("speex resampler outputSamples > outMaxSamples ");
        }
        return outMaxSamples;
    }
    else
        return 0;
}
