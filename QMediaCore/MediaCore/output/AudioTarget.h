//
//  AudioTarget.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaCore/core/MediaComm.h"
#include "OutputTarget.h"

class AudioRender
{
public:
    virtual bool onAudioRender(uint8_t * const buffer, unsigned needBytes, int64_t wantTimeMs) = 0;
};

class AudioTarget :public OutputTarget{
public:
    
	virtual ~AudioTarget(){}

	virtual bool init(AudioDescribe audioDesc) = 0;
    virtual void unInit() = 0;
	virtual int getSampleRate() const = 0;
	virtual int getChannels() const = 0;
    virtual RawAudioFormat getFormat() const = 0;
    virtual void setVolume(float volume) = 0;
    virtual float getVolume() const = 0;

	virtual int getAudioDelayBytes() const = 0;
    
    virtual void setRender(AudioRender * render) { _render = render; }

protected:
	AudioRender * _render;
}; 

class AudioSampleCache
{
public:
	AudioSampleCache() { reset(); }
	~AudioSampleCache(){}

	void setAudioBuf(const uint8_t* audioBuf) { _audioBuf = audioBuf; }
	const uint8_t *getAudioBuf(int size) const{
		const uint8_t* buf_ptr = _audioBuf + _offset;
		_offset += size;
		return buf_ptr;
	}

	void setAudioLength(int length) { _lenght = length; }
	int getAudioLength() const { return _lenght - _offset; }

	void setAudioBufOffset(int offset) {
		_offset = offset;
	}

	void setReadEnd(bool readEnd) { _readEnd = readEnd; }
	bool isReadEnd() const { return _readEnd; }

	void reset() {
		_audioBuf = NULL;
		_lenght = 0;
		_offset = 0;
		_readEnd = false;
	}

private:

	const uint8_t * _audioBuf;
	int _lenght;
	mutable int _offset;
	bool _readEnd;
};
