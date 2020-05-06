#include "VideoFrame.h"

VideoFrame::VideoFrame(const std::shared_ptr<VideoFrameBuffer>& buffer,
	int64_t timestamp_ms,
	VideoRotation rotation)
	: video_frame_buffer_(buffer),
	timestamp_(0),
	ntp_time_ms_(0),
	timestamp_ms_(timestamp_ms),
	rotation_(rotation) {}

VideoFrame::VideoFrame(const std::shared_ptr<VideoFrameBuffer>& buffer,
	int64_t timestamp,
	int64_t render_time_ms,
	VideoRotation rotation)
	: video_frame_buffer_(buffer),
	timestamp_(timestamp),
	ntp_time_ms_(0),
	timestamp_ms_(render_time_ms),
	rotation_(rotation) {
	
}

VideoFrame::~VideoFrame() = default;

VideoFrame::VideoFrame(const VideoFrame&) = default;
VideoFrame::VideoFrame(VideoFrame&&) = default;
VideoFrame& VideoFrame::operator=(const VideoFrame&) = default;
VideoFrame& VideoFrame::operator=(VideoFrame&&) = default;

int VideoFrame::width() const {
	return video_frame_buffer_ ? video_frame_buffer_->width() : 0;
}

int VideoFrame::height() const {
	return video_frame_buffer_ ? video_frame_buffer_->height() : 0;
}

uint32_t VideoFrame::size() const {
	return width() * height();
}

std::shared_ptr<VideoFrameBuffer> VideoFrame::video_frame_buffer() const {
	return video_frame_buffer_;
}

int64_t VideoFrame::render_time_ms() const {
	return timestamp_ms();
}

bool VideoFrame::is_texture() const {
    return video_frame_buffer_? (video_frame_buffer_->native_handle() != nullptr) : false;
}

RawVideoFormat VideoFrame::get_video_format() const {
    return video_frame_buffer_? video_frame_buffer_->PixelFormat() : RawVideoFormat::kUnknown;
}
