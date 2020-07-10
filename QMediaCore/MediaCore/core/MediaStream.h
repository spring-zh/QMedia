//
//  MediaStream.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <queue>
#include <thread>
#include "MediaComm.h"
#include "EncodedPacket.h"
#include "DecodedFrame.h"
#include "DecodedFrameQueue.h"
#include "CodecSpecificInfo.h"
#include "MediaCore/decoder/Decoder.h"

class EncodedPacketQueue {
public:
	EncodedPacketQueue() ;
	~EncodedPacketQueue() ;

	void addPacket(EncodedPacket& packet) ;
	EncodedPacket removePacket() ;
	int64_t dropPackets(int64_t time_to, bool previous = true);

	inline size_t size() const { return _packet_list.size();}

	void clearAll();

protected:
	std::mutex _mutex;
    std::list<EncodedPacket> _packet_list;
	int64_t _max_time_ms;
};

/**
 * the description of MediaStream
 */
class MediaStreamDescribe {
public:
	MediaStreamDescribe() {
		std::memset(&describe, 0 , sizeof(describe));
		describe.mediatype_ = MediaType::Unkonw;
		duraion = 0;
	}
	MediaDescribe describe;
	CodecSpecificInfo specificInfo;
	int64_t duraion; //stream duration time millisecond
};

/**
 * hold and manage EncodedPacketQueue , DecodedFrameQueue and Decoder
 */
class MediaStream : public DecodedFrameCallback{
public:
	explicit MediaStream(const MediaStreamDescribe& streamDescribe, DecoderFactory* decoderFactory = nullptr);
	~MediaStream();

	const MediaStreamDescribe& getMediaDescribe() const { return _streamDescribe; }
    EncodedPacketQueue& getEncodedPacketQueue() { return _encodedPacketQueue; }
    DecodedFrameQueue<DecodedFrame>& getDecodedFrameQueue() { return _decodedFrameQueue; }

    //set max output frame number
 	void setMaxOutputFrameNum(int maxOutputFrameNum) { _maxOutputFrameNum = maxOutputFrameNum; }

 	//set the time to limit the pts of decode output frame
 	void setStartTimeLimit(int64_t timeLimit) { _timeLimit = timeLimit; }

    void start();
 	void stop();

    void flush() ;
    void waitForFlush();

    bool isEnd() const { return _isEnd; }
    void setIsEnd(bool isEnd) { _isEnd = isEnd; }

protected:
    virtual int32_t OnDecoded(DecodedFrame& decodedFrame) override ;
    virtual void OnDecodedEnd() override ;
	void run();
private:

	MediaStreamDescribe _streamDescribe;

    bool _isEnd;
    bool _isStarted;
    std::thread _decoded_thread;

    std::shared_ptr<Decoder> _decoder;

	bool _need_flush;
	std::mutex _flush_mutex;
	std::condition_variable _flush_condition;

    EncodedPacketQueue _encodedPacketQueue;
    DecodedFrameQueue<DecodedFrame> _decodedFrameQueue;

    int _maxOutputFrameNum;
    int64_t _timeLimit;
};

using MediaStreamRef = std::shared_ptr<MediaStream>;