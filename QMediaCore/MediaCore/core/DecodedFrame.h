//
//  DecodedFrame.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "VideoFrame.h"
#include "AudioFrame.h"

class DecodedFrameBuffer : public VideoFrameBuffer, public AudioFrameBuffer {
public:
    virtual MediaType getMediaType() const = 0;
};

class DecodedFrame {
public:
    DecodedFrame(const std::shared_ptr<DecodedFrameBuffer>& buffer,
                 int64_t timestamp):DecodedFrame(buffer,timestamp,timestamp) {}
    DecodedFrame(const std::shared_ptr<DecodedFrameBuffer>& buffer,
               int64_t timestamp,
               int64_t render_time_ms) {
        frame_buffer_ = buffer;
        timestamp_ = timestamp;
        timestamp_ms_ = render_time_ms;
    }

    ~DecodedFrame() = default;

    // Support move and copy.
    DecodedFrame(const DecodedFrame&) = default;
    DecodedFrame(DecodedFrame&&) = default;
    DecodedFrame& operator=(const DecodedFrame&) = default;
    DecodedFrame& operator=(DecodedFrame&&) = default;

    int64_t render_time_ms() const { return timestamp_ms_; }

    //members
    std::shared_ptr<DecodedFrameBuffer> frame_buffer_;
    int64_t timestamp_;
    int64_t timestamp_ms_;
};