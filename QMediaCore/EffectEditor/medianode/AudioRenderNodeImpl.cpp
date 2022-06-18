//
//  AudioRenderNodeImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "AudioRenderNodeImpl.h"
#include "MediaSegmentImpl.h"
#include "MediaEngineImpl.h"

namespace QMedia { namespace Api {

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

static RawAudioFormat audioToRawAudioFormat(int16_t desc_audioFormat){
    if (desc_audioFormat == AudioDescription::FORMAT_U8) {
        return RawAudioFormat::kU8;
    } else if (desc_audioFormat == AudioDescription::FORMAT_S16) {
        return RawAudioFormat::kS16;
    } else if (desc_audioFormat == AudioDescription::FORMAT_FLOAT) {
        return RawAudioFormat::kFLOAT;
    } {
        return RawAudioFormat::kS16;
    }
}

AudioRenderNodeImpl::AudioRenderNodeImpl(MediaSegmentImpl* segment): segment_(segment) {
    
}
AudioRenderNodeImpl::~AudioRenderNodeImpl() {
    
}
void AudioRenderNodeImpl::setTempo(float tempo) {
}

void AudioRenderNodeImpl::setRate(float rate) {
}

void AudioRenderNodeImpl::setPitch(float pitch) {
    pitch_ = pitch;
}

void AudioRenderNodeImpl::setVolume(float volume) {
    
}

const MediaRange AudioRenderNodeImpl::getRange() const {
    return segment_->getDisplayRange();
}

const AudioSampleCache* AudioRenderNodeImpl::getAudioSample(int64_t time_ms) {
    if (_sampleCache.getAudioLength() <= 0) {
        bool isEnd = false;
//        AudioFrame audioframe = _mediaTrack->getAudioFrame(mSec, isEnd);
        DecodedFrame audioframe(nullptr,  0);
        if (input_frames_.size() > 0) {
            audioframe = input_frames_.front();
            input_frames_.pop_front();
            _sampleCache.setReadEnd(false);
        } else {
            _sampleCache.setReadEnd(true);
            return &_sampleCache;
        }
        std::shared_ptr<DecodedFrameBuffer> nativebuffer = audioframe.frame_buffer_;
        int cacheLen = 0;
        auto& desc = MediaEngineImpl::instance()->get_audio_out();
        int target_samplerate = desc.sample_rate;
        int target_channels = desc.nchannel;
        RawAudioFormat target_format = audioToRawAudioFormat(desc.smaple_format);
        
        if (!isEnd) {
            if (nativebuffer) { //has audio buffer
                
                if(!FLOAT_ISEQUAL(segment_->getTimescale() , 1.0f) || !FLOAT_ISEQUAL(pitch_ , 1.0f)) {
                    //TODO: need process with audio effect
                    if (!_audioEffect) {
                        _audioEffect = std::shared_ptr<AudioEffect>(new AudioEffect());
                        _audioEffect->init(target_samplerate, target_channels);
                    }
                    _audioEffect->setTempo(1.0f / segment_->getTimescale());
                    _audioEffect->setPitch(pitch_);
                }else {
                    _audioEffect.reset();
                }
                
                if ((nativebuffer->Channels() != target_channels) ||
                    (nativebuffer->SampleRate() != target_samplerate) ||
                    (nativebuffer->SampleFormat() != target_format)) {
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
                    cacheLen = _audioEffect->process((void*)nativebuffer->Data(), nativebuffer->Size(), _cacheBuffer);
                }else {
                    cacheLen = nativebuffer->Size();
                    _cacheBuffer.assign(nativebuffer->Data(), nativebuffer->Data() + cacheLen);
                }
            }else {
                //fill empty buffer with 2048 bytes
                const int fill_size = 2048;
                int process_len = 0;
                if (_audioEffect) {
                    process_len = _audioEffect->process(NULL, 0, _cacheBuffer);
                }
                int need_fill_len = fill_size - process_len;
                if (need_fill_len > 0) {
                    std::memset(_cacheBuffer.data() + process_len, 0, need_fill_len);
                    cacheLen = fill_size;
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

void AudioRenderNodeImpl::PutAudioFrame(DecodedFrame audioframe) {
    input_frames_.push_back(audioframe);
}

void AudioRenderNodeImpl::ClearAudioFrame() {
    input_frames_.clear();
}

}
}
