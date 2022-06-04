//
//  VideoFrame.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaComm.h"
#include <memory>

static const int64_t kNumMillisecsPerSec = INT64_C(1000);
static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);

static const int64_t kNumMicrosecsPerMillisec =
kNumMicrosecsPerSec / kNumMillisecsPerSec;
static const int64_t kNumNanosecsPerMillisec =
kNumNanosecsPerSec / kNumMillisecsPerSec;
static const int64_t kNumNanosecsPerMicrosec =
kNumNanosecsPerSec / kNumMicrosecsPerSec;

// Interface of a simple frame buffer containing pixel data. This interface does
// not contain any frame metadata such as rotation, timestamp, pixel_width, etc.
class VideoFrameBuffer {
public:
	// The resolution of the frame in pixels. For formats where some planes are
	// subsampled, this is the highest-resolution plane.
	virtual int width() const = 0;
	virtual int height() const = 0;

	// Returns pointer to the pixel data for a given plane. The memory is owned by
	// the VideoFrameBuffer object and must not be freed by the caller.
	virtual const uint8_t* DataY() const = 0;
	virtual const uint8_t* DataU() const = 0;
	virtual const uint8_t* DataV() const = 0;

	// Returns the number of bytes between successive rows for a given plane.
	virtual int StrideY() const = 0;
	virtual int StrideU() const = 0;
	virtual int StrideV() const = 0;

	// Return the handle of the underlying video frame. This is used when the
	// frame is backed by a texture.
	virtual void* native_handle() const = 0;

	// Returns a new memory-backed frame buffer converted from this buffer's
	// native handle.
	virtual std::shared_ptr<VideoFrameBuffer> NativeToI420Buffer() = 0;

	virtual RawVideoFormat PixelFormat() const { return pixel_format_; }

protected:
	virtual ~VideoFrameBuffer() {}

	RawVideoFormat pixel_format_;
};

class VideoFrame {
public:
	// TODO(nisse): This constructor is consistent with the now deleted
	// cricket::WebRtcVideoFrame. We should consider whether or not we
	// want to stick to this style and deprecate the other constructor.
	VideoFrame(const std::shared_ptr<VideoFrameBuffer>& buffer,
		int64_t timestamp_ms, 
		VideoRotation rotation = VideoRotation::kVideoRotation_0);

	// Preferred constructor.
	VideoFrame(const std::shared_ptr<VideoFrameBuffer>& buffer,
		int64_t timestamp,
		int64_t render_time_ms,
		VideoRotation rotation = VideoRotation::kVideoRotation_0);

	~VideoFrame();

	// Support move and copy.
	VideoFrame(const VideoFrame&);
	VideoFrame(VideoFrame&&);
	VideoFrame& operator=(const VideoFrame&);
	VideoFrame& operator=(VideoFrame&&);

	// Get frame width.
	int width() const;
	// Get frame height.
	int height() const;
	// Get frame size in pixels.
	uint32_t size() const;

	// System monotonic clock, same timebase as rtc::TimeMicros().
	int64_t timestamp_ms() const { return timestamp_ms_; }
	void set_timestamp_ms(int64_t timestamp_us) { timestamp_ms_ = timestamp_us; }

	// TODO(nisse): After the cricket::VideoFrame and webrtc::VideoFrame
	// merge, timestamps other than timestamp_us will likely be
	// deprecated.

	// Set frame timestamp (90kHz).
	void set_timestamp(int64_t timestamp) { timestamp_ = timestamp; }

	// Get frame timestamp (90kHz).
	int64_t timestamp() const { return timestamp_; }

	// For now, transport_frame_id and rtp timestamp are the same.
	// TODO(nisse): Must be handled differently for QUIC.
	uint32_t transport_frame_id() const { return (uint32_t)timestamp(); }

	// Set capture ntp time in milliseconds.
	// TODO(nisse): Deprecated. Migrate all users to timestamp_us().
	void set_ntp_time_ms(int64_t ntp_time_ms) { ntp_time_ms_ = ntp_time_ms; }

	// Get capture ntp time in milliseconds.
	// TODO(nisse): Deprecated. Migrate all users to timestamp_us().
	int64_t ntp_time_ms() const { return ntp_time_ms_; }

	// Naming convention for Coordination of Video Orientation. Please see
	// http://www.etsi.org/deliver/etsi_ts/126100_126199/126114/12.07.00_60/ts_126114v120700p.pdf
	//
	// "pending rotation" or "pending" = a frame that has a VideoRotation > 0.
	//
	// "not pending" = a frame that has a VideoRotation == 0.
	//
	// "apply rotation" = modify a frame from being "pending" to being "not
	//                    pending" rotation (a no-op for "unrotated").
	//
	VideoRotation rotation() const { return rotation_; }
	void set_rotation(VideoRotation rotation) { rotation_ = rotation; }

	// Get render time in milliseconds.
	// TODO(nisse): Deprecated. Migrate all users to timestamp_us().
	int64_t render_time_ms() const;

	// Return the underlying buffer. Never nullptr for a properly
	// initialized VideoFrame.
	std::shared_ptr<VideoFrameBuffer> video_frame_buffer() const;

	// TODO(nisse): Deprecated.
	// Return true if the frame is stored in a texture.
    bool is_texture() const ;

    RawVideoFormat get_video_format() const ;
    
    int pkt_flags = 0;

private:
	// An opaque reference counted handle that stores the pixel data.
	std::shared_ptr<VideoFrameBuffer> video_frame_buffer_;
	int64_t timestamp_;
	int64_t ntp_time_ms_;
	int64_t timestamp_ms_;
	VideoRotation rotation_;
};
