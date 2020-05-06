//
//  AudioFrame.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaComm.h"
#include <memory>

class AudioFrameBuffer {
public:
    
	virtual int Channels() const = 0;
	virtual int SampleRate() const = 0;
	virtual int SampleBits() const = 0;
    virtual int Samples() const = 0;

	virtual const uint8_t* Data() const = 0;
	virtual uint32_t Size() const = 0;

	virtual enum ChannelLayout ChannelLayout() const = 0;

	// Returns a new memory-backed frame buffer converted from this buffer's
	// native handle.
	virtual std::shared_ptr<AudioFrameBuffer> ReSampleBuffer(RawAudioFormat fmt, int samplerate, int channels) = 0;

	virtual RawAudioFormat SampleFormat() const { return sample_format_; }
protected:
	virtual ~AudioFrameBuffer() {}

	RawAudioFormat sample_format_;

};

class AudioFrame
{
public:

	enum : size_t {
		kMaxDataSizeSamples = 5760
	};

	AudioFrame(const std::shared_ptr<AudioFrameBuffer>& buffer, 
		int64_t timestamp,
		int64_t render_time_ms);

	~AudioFrame();

	// Support move and copy.
	AudioFrame(const AudioFrame&);
	AudioFrame(AudioFrame&&);
	AudioFrame& operator=(const AudioFrame&);
	AudioFrame& operator=(AudioFrame&&);

	// Get frame channels.
	int channels() const;
	// Get frame samplerate.
	int samplerate() const;
	// Get frame samplerate.
	int samplebits() const;
	// Get frame size in buffer.
	uint32_t size() const;

	// Set frame timestamp (90kHz).
	void set_timestamp(int64_t timestamp) { timestamp_ = timestamp; }

	// Get frame timestamp (90kHz).
	int64_t timestamp() const { return timestamp_; }

	// System monotonic clock, same timebase as rtc::TimeMicros().
	int64_t timestamp_ms() const { return timestamp_ms_; }
	void set_timestamp_ms(int64_t timestamp_us) { timestamp_ms_ = timestamp_us; }

	// Get render time in milliseconds.
	// TODO(nisse): Deprecated. Migrate all users to timestamp_us().
	int64_t render_time_ms() const;

	// Return the underlying buffer. Never nullptr for a properly
	// initialized AudioFrame.
	std::shared_ptr<AudioFrameBuffer> audio_frame_buffer() const;

	RawAudioFormat get_audio_format() const { return audio_frame_buffer_->SampleFormat(); }

private:

	std::shared_ptr<AudioFrameBuffer> audio_frame_buffer_;
	int64_t timestamp_;
	int64_t ntp_time_ms_;
	int64_t timestamp_ms_;
};


