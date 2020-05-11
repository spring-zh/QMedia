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
_resampler(nullptr),
_volume(1.0f),
_bEnable(true){
    _cacheBuffer.resize(8 * 1024);
}

const AudioSampleCache* MediaAudioChannel::getAudioSample(int64_t mSec) {
    if (_sampleCache.getAudioLength() <= 0) {
        bool isEnd = false;
        AudioFrame audioframe = _mediaTrack->getAudioFrame(mSec, isEnd);
        std::shared_ptr<AudioFrameBuffer> nativebuffer = audioframe.audio_frame_buffer();
        int cacheLen = 0;
        if (!isEnd) {
            if (nativebuffer) { //has audio buffer
                
                int out_samplerate = _audioTarget->getSampleRate();
                if(! FLOAT_ISEQUAL(_mediaTrack->getTimeScale() , 1.0f) && _mediaTrack->getTimeScale() > 0.f) {
                    out_samplerate = _audioTarget->getSampleRate() * _mediaTrack->getTimeScale();
                }
                
                if ((audioframe.channels() != _audioTarget->getChannels()) ||
                    (audioframe.samplerate() != out_samplerate) ||
                    (audioframe.get_audio_format() != _audioTarget->getFormat())) {
                    //TODO: need resample
                    if (_resampler == nullptr) {
                        _resampler = std::shared_ptr<ResamplerSpeex>(new ResamplerSpeex());
                        _resampler->init(audioToResamplerFormat(nativebuffer->SampleFormat()),
                                                                nativebuffer->Channels(),
                                                                nativebuffer->SampleRate(),
                                         audioToResamplerFormat(_audioTarget->getFormat()),
                                                                _audioTarget->getChannels(),
                                                                out_samplerate);
                    }
                    
                    cacheLen = _resampler->process((void*)nativebuffer->Data(), nativebuffer->Size(), _cacheBuffer);
                    
                }else {
                    cacheLen = nativebuffer->Size();
                    if (_cacheBuffer.size() < cacheLen) {
                        _cacheBuffer.resize(cacheLen);
                    }
                    std::memcpy(_cacheBuffer.data(), nativebuffer->Data(), cacheLen);
                }
            }else {
                //fill empty buffer
                cacheLen = MIN((int)_cacheBuffer.size(), 2048);
                std::memset(_cacheBuffer.data(), 0, cacheLen);
            }
            _sampleCache.setAudioBufOffset(0);
            _sampleCache.setAudioBuf(_cacheBuffer.data());
            _sampleCache.setAudioLength(cacheLen);
        }
        _sampleCache.setReadEnd(isEnd);
    }
    return &_sampleCache;
}
