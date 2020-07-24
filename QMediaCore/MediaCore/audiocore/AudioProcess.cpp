//
//  AudioProcess.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "AudioProcess.h"
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void AudioProcess::mixAudio(uint8_t * dst, const uint8_t * src, int len, float volume)
{
    if (volume < FLT_EPSILON) {
        // 0
        return;
    }
    
    // AUDIO_S16LSB
    int16_t src1, src2;
    int dst_sample;
    const int max_audioval = 32767;     // ((1 << (16 - 1)) - 1);
    const int min_audioval = -32768;    // -(1 << (16 - 1));
    adjustAudioVolume((uint8_t *)src,len,volume);
    len /= 2;
    while (len--) {
        src1 = ((src[1]) << 8 | src[0]);
        //ADJUST_VOLUME(src1, volume);
        src2 = ((dst[1]) << 8 | dst[0]);
        src += 2;
        //dst_sample = src1 + src2;
        //mix audio without clipping:A+B-A*B，A,B∈（0，1）
        //dst_sample = (src1 + src2 + 32767)/2.0 - src1*src2/65536.0;
        if( src1 < 0 && src2 < 0){
            dst_sample = src1+src2 - (src1 * src2 / -32767);
        }  else {
            dst_sample = src1+src2 - (src1 * src2 / 32767);
        }
        if (dst_sample > max_audioval) {
            dst_sample = max_audioval;
        } else if (dst_sample < min_audioval) {
            dst_sample = min_audioval;
        }
        dst[0] = dst_sample & 0xFF;
        dst_sample >>= 8;
        dst[1] = dst_sample & 0xFF;
        dst += 2;
    }
}

#define ADJUST_VOLUME(s, v) (s *= v)

/**
 * ampfily audio PCM data,if volume is too big,it will be rescaled smaller to avoid clipping
 * */
void AudioProcess::amplifyAudioVolume(const uint8_t* src,const uint8_t* dst,int len,float volume){
    int32_t data = 0;
    uint8_t *pSrc = (uint8_t *)src;
    int samples = len / 2;
    int maxAbs = 0;
    int clipPointCnt = 0;
    int *ampilyedData = (int*)malloc(samples*sizeof(int));
    int *temptr = ampilyedData;
    while (samples--) {
        int16_t s16 = pSrc[1] << 8 | pSrc[0];
        data = s16*volume;
        if (data > 32767 || data < -32767) {
            if (maxAbs < abs(data)) {
                maxAbs = abs(data);
            }
            clipPointCnt++;
        }
        *(temptr++) = data;
        pSrc += 2;
    }
    //float presentOfClip = 2.0*clipPointCnt/len;
    //clip概率大于5‰时才做调整,seems not work,even worse,ignore this
    if (/*presentOfClip > 0.005 && */maxAbs > 32767) {
        float weakenVal = 32760.0 / maxAbs;
        samples = len / 2;
        uint8_t *pDst = (uint8_t *)dst;
        int *temptr = ampilyedData;
        while (samples--) {
            *temptr *= weakenVal;
            pDst[0] = *(temptr) & 0xFF;
            *(temptr) >>= 8;
            pDst[1] = *(temptr) & 0xFF;
            temptr++;
            pDst += 2;
        }
    } else {
        samples = len / 2;
        uint8_t *pDst = (uint8_t *)dst;
        int *temptr = ampilyedData;
        if (*temptr  >= 32767) {
            *temptr  = 32767;
        } else if (*temptr  <= -32768) {
            *temptr  = -32768;
        }
        while (samples--) {
            pDst[0] = *(temptr) & 0xFF;
            *(temptr) >>= 8;
            pDst[1] = *(temptr) & 0xFF;
            temptr++;
            pDst += 2;
        }
    }
    free(ampilyedData);
    return ;
}

void AudioProcess::shrinkAudioVolume(uint8_t * src, int len, float volume){
    // AUDIO_S16LSB
    int16_t src1;
    len /= 2;
    uint8_t* srcPtr = src;
    while (len--) {
        src1 = ((srcPtr[1]) << 8 | srcPtr[0]);
        ADJUST_VOLUME(src1, volume);
        srcPtr[0] = src1 & 0xFF;
        src1 >>= 8;
        srcPtr[1] = src1 & 0xFF;
        srcPtr += 2;
    }
}

void AudioProcess::adjustAudioVolume(uint8_t * src, int len, float volume)
{
    //声音衰减很大时直接设为0
    if (volume < FLT_EPSILON) {
        // 0
        memset(src, 0, len);
        return;
    }
    //声音减小操作时不需要防止clipping，放大时才需要防止
    if(volume < 1.0001){
        shrinkAudioVolume(src, len, volume);
    } else {
        amplifyAudioVolume(src, src, len, volume);
    }

}

int64_t AudioProcess::calculateAudioDurationByBytes(int sampleRate, int channels, int bytesWidth, int64_t byteSize)
{
    return byteSize * 1000 / (sampleRate * channels * bytesWidth);
}
