#include "AudioFrame.h"

AudioFrame::AudioFrame(const std::shared_ptr<AudioFrameBuffer>& buffer, int64_t timestamp, int64_t render_time_ms)
	: audio_frame_buffer_(buffer),
	timestamp_(timestamp),
	ntp_time_ms_(0),
	timestamp_ms_(render_time_ms)
{
}

AudioFrame::~AudioFrame() = default;

AudioFrame::AudioFrame(const AudioFrame&) = default;
AudioFrame::AudioFrame(AudioFrame&&) = default;
AudioFrame& AudioFrame::operator=(const AudioFrame&) = default;
AudioFrame& AudioFrame::operator=(AudioFrame&&) = default;

// Get frame channels.
int AudioFrame::channels() const {
	return audio_frame_buffer_->Channels();
}
// Get frame samplerate.
int AudioFrame::samplerate() const {
	return audio_frame_buffer_->SampleRate();
}
// Get frame samplerate.
int AudioFrame::samplebits() const {
	return audio_frame_buffer_->SampleBits();
}
// Get frame size in buffer.
uint32_t AudioFrame::size() const {
	return audio_frame_buffer_->Size();
}

std::shared_ptr<AudioFrameBuffer> AudioFrame::audio_frame_buffer() const {
	return audio_frame_buffer_;
}

int64_t AudioFrame::render_time_ms() const {
	return timestamp_ms();
}
