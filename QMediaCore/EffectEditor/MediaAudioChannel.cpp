//
//  MediaAudioChannel.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaAudioChannel.h"
#include "MediaTrack.h"

static ResamplerSpeex::Foramt audioToResamplerFormat(RawAudioFormat audioFormat){
    switch (audioFormat) {
        case RawAudioFormat::kU8:
            return ResamplerSpeex::FT_UINT8;
        case RawAudioFormat::kS16:
        case RawAudioFormat::kU16:
            return ResamplerSpeex::FT_INT16;
        case RawAudioFormat::kFLOAT:
            return ResamplerSpeex::FT_FLOAT;
        default:
            return ResamplerSpeex::FT_INT16;
    }
}

MediaAudioChannel::MediaAudioChannel(MediaTrack *mediaTrack):
_mediaTrack(mediaTrack),
_audioTarget(mediaTrack->getAudioTarget()),
_audioEffect(nullptr),
_resampler(nullptr),
_volume(1.0f),
_bEnable(true),
_fPitch(1.0f){
    _cacheBuffer.resize(8 * 1024);
}

const AudioSampleCache* MediaAudioChannel::getAudioSample(int64_t mSec) {
    if (_sampleCache.getAudioLength() <= 0) {
        bool isEnd = false;
        AudioFrame audioframe = _mediaTrack->getAudioFrame(mSec, isEnd);
        std::shared_ptr<AudioFrameBuffer> nativebuffer = audioframe.audio_frame_buffer();
        int cacheLen = 0;
        
        int target_samplerate = _audioTarget->getSampleRate();
        int target_channels = _audioTarget->getChannels();
        RawAudioFormat target_format = _audioTarget->getFormat();
        
        if (!isEnd) {
            if (nativebuffer) { //has audio buffer
                
                if(!FLOAT_ISEQUAL(_mediaTrack->getTimeScale() , 1.0f) || !FLOAT_ISEQUAL(_fPitch , 1.0f)) {
                    //TODO: need process with audio effect
                    if (!_audioEffect) {
                        _audioEffect = std::shared_ptr<AudioEffect>(new AudioEffect());
                        _audioEffect->init(target_samplerate, target_channels);
                    }
                    _audioEffect->setTempo(1.0f / _mediaTrack->getTimeScale());
                    _audioEffect->setPitch(_fPitch);
                }else {
                    _audioEffect.reset();
                }
                
                if ((audioframe.channels() != target_channels) ||
                    (audioframe.samplerate() != target_samplerate) ||
                    (audioframe.get_audio_format() != target_format)) {
                    //TODO: need resample
                    if (!_resampler) {
                        _resampler = std::shared_ptr<ResamplerSpeex>(new ResamplerSpeex());
                    }
                    _resampler->init(audioToResamplerFormat(nativebuffer->SampleFormat()),
                                           nativebuffer->Channels(),
                                           nativebuffer->SampleRate(),
                                     audioToResamplerFormat(target_format),
                                           target_channels,
                                           target_samplerate);
                }
                
                if (_resampler && _audioEffect) {
                    std::vector<uint8_t> resample_buffer;
                    int resampleLen = _resampler->process((void*)nativebuffer->Data(), nativebuffer->Size(), resample_buffer);
                    cacheLen = _audioEffect->process(resample_buffer.data(), resampleLen, _cacheBuffer);
                }else if(_resampler) {
                    cacheLen = _resampler->process((void*)nativebuffer->Data(), nativebuffer->Size(), _cacheBuffer);
                }else if(_audioEffect) {
//                    int reqoutSamples = nativebuffer->Samples() * _mediaTrack->getTimeScale() + 512;
//                    _cacheBuffer.resize(reqoutSamples * target_channels * 2);
//                    int outSample = _audioEffect->process((void*)nativebuffer->Data(), nativebuffer->Samples(), _cacheBuffer.data(), reqoutSamples);
//                    cacheLen = outSample *target_channels * 2;
                    cacheLen = _audioEffect->process((void*)nativebuffer->Data(), nativebuffer->Size(), _cacheBuffer);
                }else {
                    cacheLen = nativebuffer->Size();
                    _cacheBuffer.assign(nativebuffer->Data(), nativebuffer->Data() + cacheLen);
                }
            }else {
                //fill empty buffer
                cacheLen = 0;
                if (_audioEffect) {
//                    uint8_t empty_buffer[2048] = {0};
//                    cacheLen = _audioEffect->process(empty_buffer, sizeof(empty_buffer), _cacheBuffer);
                    cacheLen = _audioEffect->process(NULL, 0, _cacheBuffer);
                }
                int fillLen = 2048 - cacheLen;
                if (fillLen > 0) {
                    cacheLen = 2048;
                    std::memset(_cacheBuffer.data() + cacheLen, 0, fillLen);
                }
            }
            _sampleCache.setAudioBufOffset(0);
            _sampleCache.setAudioBuf(_cacheBuffer.data());
            _sampleCache.setAudioLength(cacheLen);
        }

        _sampleCache.setReadEnd(isEnd);
    }
    return &_sampleCache;
}
