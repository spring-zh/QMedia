//
//  Demuxer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaCore/core/MediaComm.h"
#include "MediaCore/core/MediaStream.h"

class Demuxer {
public:
    
    using UPtr = std::unique_ptr<Demuxer>;
    using SPtr = std::shared_ptr<Demuxer>;
    
	virtual ~Demuxer() {}

	virtual int32_t Open(const char* urlpath, int flags) = 0;

    virtual int32_t Close() = 0;

    /**
     * read packet form demuxer
     * @param packet: output packet
     * @return stream id: successed(>=0)  failed(<0)
     */
    virtual int32_t ReadPacket(EncodedPacket& packet) = 0;

    virtual int32_t Seek(int64_t seek_time, int flags) = 0;

	virtual int32_t Flush() = 0;

    virtual bool IsOpen() const = 0;

	virtual bool IsEOF() const = 0;

	virtual void setIgnoreBFrame(bool ignore) = 0;
    
    virtual void Select(int stream_id) = 0;

    virtual const std::vector<MediaStreamDescribe>& getMediaStreamDescribes() const = 0;

	/**
	 * get media total duration millisecond
	 * @return
	 */
    virtual int64_t getDuration() const = 0;
};

using DemuxerRef = std::shared_ptr<Demuxer>;
